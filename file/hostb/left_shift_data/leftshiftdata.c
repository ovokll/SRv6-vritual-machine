/*
Copyright (c) 2015 Radoslav Gerganov <rgerganov@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ipv6.h>
#include <linux/socket.h>/*PF_INET*/
#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>/*NF_IP_PRE_FIRST*/
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/version.h>
#include <linux/slab.h>

#include <net/ip.h>
#include <net/ipv6.h>
#include <net/tcp.h>

#define IPT_TCP (6)
#define IPT_IPIP6 (41)
#define IPT_SRV6 (43)
#define ETHALEN 14
#define DEST_PORT 12345

struct ipv6len_ipip{
    int inner_iphlen;
    char ipip;
};

int check_ipv6_function(struct ipv6hdr *ip6h){
    struct in6_addr daddr = ip6h->daddr;
    if(daddr.in6_u.u6_addr8[15] == 0xa0)
        return 1;
    return 0;
}

unsigned int packet_filter(unsigned int hooknum, struct sk_buff *skb,
               const struct net_device *in, const struct net_device *out,
               int (*okfn)(struct sk_buff *));

static struct nf_hook_ops packet_simple_nf_opt = {
    .hook = (nf_hookfn *)packet_filter,
    .pf = NFPROTO_IPV6,
    .hooknum = NF_INET_PRE_ROUTING,
    .priority = NF_IP6_PRI_FIRST,
};

int check_inner_protocol(uint8_t *payload, struct ipv6len_ipip *iii){
    struct ipv6hdr *ip6h = (struct ipv6hdr *)payload;
    struct ipv6_opt_hdr *opthdr = (struct ipv6_opt_hdr *)(payload + sizeof(struct ipv6hdr));
    int ret = 40;
    switch(ip6h->nexthdr)
    {
        case IPT_TCP:
            break;
        case IPT_SRV6://40+opt.hdrlen
            if(opthdr->nexthdr == IPT_TCP){
                ret += ((opthdr->hdrlen+1)<<3);
            }else if(opthdr->nexthdr == IPT_IPIP6){
                ret += ((opthdr->hdrlen+1)<<3);
                iii->inner_iphlen = check_inner_protocol((uint8_t *)(payload + ret), iii);
                ret += iii->inner_iphlen;
                iii->ipip = 1;
            }else
                ret = -1;
            break;
        default :
            ret = -1;
    }
    return ret;
}

//大小端转换
int to_opposite_endian(unsigned int data, int size){
    unsigned int copy = data;
    unsigned int mask = 0xff;
    unsigned int temp;
    int i;

    data = 0;
    for(i = 0; i < size; i++){
        data = data << 8;
        temp = copy & mask;
        temp = temp >> (i * 8);
        data |= temp;
        mask = mask << 8;
    }
    return data;
}

unsigned int packet_filter(unsigned int hooknum, struct sk_buff *skb,
               const struct net_device *in, const struct net_device *out,
               int (*okfn)(struct sk_buff *))
{
            //printk("get a packet\n");
    __u8 *payload = NULL;
    struct ipv6hdr *ip6h;
    struct tcphdr *tcph;
    int ip6h_len;//总的网络层头部长度
    int data_len;
    int ret;

    if(skb == NULL)
        return NF_ACCEPT;

    ip6h = ipv6_hdr(skb);
    payload = (__u8 *)ipv6_hdr(skb);
    if(ip6h == NULL)
        return NF_ACCEPT;
    struct ipv6len_ipip *iii = (struct ipv6len_ipip *)kmalloc(5, GFP_KERNEL);
    ip6h_len = check_inner_protocol(payload, iii);
    if(ip6h_len == -1)
        return NF_ACCEPT;

    skb_pull(skb, ip6h_len);
    skb_reset_transport_header(skb);
    tcph = tcp_hdr(skb);
    skb_push(skb, ip6h_len);
    if(tcph->dest == htons(DEST_PORT) && check_ipv6_function(ip6h)) //根据自己得需求来进行过滤数据包
    {
        data_len = to_opposite_endian(ip6h->payload_len, 2) + 40 - ip6h_len - (tcph->doff<<2);

            printk("packet len: %d\n", data_len);
        if(data_len <= 0)
            return NF_ACCEPT;

        payload = (__u8*)(payload + ip6h_len + (tcph->doff<<2));
        __u8 *pos = payload;
        __u8 *next;
        __u8 temp = *payload;
        int i = 0;
        for(; i < data_len-1 && i < 4; i++){
            next = pos + 1;
            *pos = *next;
            pos++;
        }
        *pos = temp;
	
        int bflen = ip6h_len;//TCP紧挨的ipv6头部长度
        if(iii->ipip){
            skb_pull(skb, ip6h_len - iii->inner_iphlen);
            skb_reset_network_header(skb);
            ip6h = ipv6_hdr(skb);
            bflen = iii->inner_iphlen;
        }

        //full check
        tcph->check = 0;
        skb->csum = skb_checksum(skb, bflen, skb->len - bflen, 0);
        tcph->check = csum_ipv6_magic(&ip6h->saddr, &ip6h->daddr, skb->len-bflen, IPT_TCP, skb->csum);

        if(iii->ipip){
            skb_push(skb, ip6h_len - iii->inner_iphlen);
            skb_reset_network_header(skb);
        }

        skb->ip_summed = CHECKSUM_UNNECESSARY;

    }
    return NF_ACCEPT;
}


static int simple_nf_init(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,3,0)
        nf_register_net_hook(&init_net, &packet_simple_nf_opt);
#else
        nf_register_hook(&packet_simple_nf_opt);
#endif
 
        printk("[left_shift_data] network hooks success.\n");
 
        return 0;
 
}

static void simple_nf_exit(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,3,0)
        nf_unregister_net_hook(&init_net, &packet_simple_nf_opt);
#else
        nf_unregister_hook(&packet_simple_nf_opt);
#endif
    printk("[left_shift_data] remove hook lkm success!\n");
}

module_init(simple_nf_init)
module_exit(simple_nf_exit)
MODULE_LICENSE("GPL");
