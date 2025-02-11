
## Language/语言

- [English](#english)
- [中文](#中文)

### English
# SRv6-vritual-machine
This project is a SRv6 Sandbox.
## Install Guide
1) Download Machine  
You can download the `.ova` file at this link: `https://pan.baidu.com/s/14-slbUZkwORLGymKFIZ04g?pwd=5ryi`. Then import the file multiple times and rename it as hosta to hostd.  
2) Building Network Topology  
Download the `01-network-manager-all.yaml` file for each machine, and replace the file at `/etc/netplan` folder. Then enter `sudo netplan apply` in the command line to enable the setting.  
Ultimately forming a linear topology from `hostA` to `hostD`  
<img width="772" alt="image" src="https://github.com/user-attachments/assets/fe1949e1-3d30-40d4-a583-48cb3258e73a" />

## Usage  
Here is a small example provided to understand how to use it. The hostB service will cyclically left shift the first five English letters of the data packet.  
1) Set up SRV6 forwarding  
Run the `route.sh` file prepared for each host.  
2) Enable the Left Shift Service  
Use the `make` command in `hostB` to compile the prepared code, run it with `sudo insmod left_shift_data.ko`, and finally stop the service using `sudo rmmod left_shift_data`.  
3) Sending Message  
Sending some messages from `hostA` to `hostD`. You can see that the order of the first five few characters has changed.


### 中文  
# SRv6虚拟机  
该项目是一个SRv6沙盒。  
## 安装指南  
1）下载虚拟机文件  
您可以通过以下链接下载`.ova`文件：`https://pan.baidu.com/s/14-slbUZkwORLGymKFIZ04g?pwd=5ryi`. 然后在VMware中多次导入该文件，并将其作为`Host A`到`Host E`。用户名和密码均为`kll`。   
2）修改网卡配置  
启动虚拟机前首先进行网卡配置，顺序与3）中的yaml文件对应，请勿随意修改  
- Host A配置2个网卡，分别为：LAN区段（ab）、NET  
- Host B配置4个网卡，分别为：LAN区段（ab）、LAN区段（bc）、LAN区段（be）、NET  
- Host C配置4个网卡，分别为：LAN区段（bc）、LAN区段（cd）、LAN区段（ec）、NET  
- Host D配置2个网卡，分别为：LAN区段（cd）、NET  
- Host E配置3个网卡，分别为：LAN区段（be）、LAN区段（ec）、NET

3）配置网络地址及静态路由  
为每台机器下载`01-networkmanager-all.yaml`文件，并替换`/etc/netplan`文件夹中的文件。然后在命令行中输入`sudo netplan apply`以启用该设置。  
最终形成从`Host A`到`Host E`的拓扑，如下图所示  
<img width="772" alt="image" src="https://github.com/user-attachments/assets/fe1949e1-3d30-40d4-a583-48cb3258e73a" />

## 使用方法  
这里提供了几个示例来了解并使用SRv6。  

# 1、未启用SRv6  
1）发送消息  
在`Host D`中使用server.c来接收信息  
在`Host A`中使用client.c来发送消息  
在`Host C`处运行wireshark可以看到数据包从`Host B`处直接发往`Host C`（wireshark捕获网卡为eth0）  
<img width="400" alt="image" src="https://github.com/user-attachments/assets/661c14eb-cb01-4cc0-ad68-92629b301680" />
<img width="400" alt="image" src="https://github.com/user-attachments/assets/b9601e8a-60a5-4883-b0fd-11701e959645" />

# 2、启用SRv6  
1）设置SRV6相关动作  
在`Host A`处运行
```bash
sudo ip -6 route add 2024:cd::2/128 encap seg6 mode encap segs fc00:2::a0,fc00:5::a0,fc00:3::a0 dev eth0
```
在`host B`处运行
```bash
sudo ip -6 route add fc00:2::a/64 encap seg6local action End dev eth1 metric 200
sudo ip -6 route add fc00:2::a/64 encap seg6local action End dev eth2 metric 300
```
在`host E`处运行
```bash
sudo ip -6 route add fc00:5::a0/64 encap seg6local action End dev eth1 metric 200
```
在`host C`处运行
```bash
sudo ip -6 route add fc00:3::a0/64 encap seg6local action End.DX6 nh6 2024:cd::2 dev eth1
```
2）发送消息  
在`Host D`中使用server.c来接收信息  
在`Host A`中使用client.c来发送消息  
在`Host C`处运行wireshark可以看到数据包是从`Host E`处发往`Host C`，并且有一个额外的SRH（wireshark捕获网卡为eth2）  
<img width="555" alt="image" src="https://github.com/user-attachments/assets/452154e8-573e-447f-b1c4-aad268a18628" />

3、添加SRv6服务  
1）在`Host B`中提供了一个简单的功能服务，可以将发送的TCP数据包前5位向左循环移动一次。  
使用方法如下。  
复制left_shift_data文件夹到`Host B`中，在文件夹中使用`make`命令编译准备好的代码，  
在该文件夹下，使用如下命令启动该服务  
```bash
sudo insmod left_shift_data.ko
```
最后使用如下命令停止服务  
```bash
sudo rmmod left_shift_data
```
2）参考2中步骤配置SRv6相关动作  
3）发送消息  
在`hostD`中使用server.c来接收信息  
在`hostA`中使用client.c来发送消息  
在`hostC`处运行wireshark可以看到数据包是从`Host E`处发往`Host C`，并且有一个额外的SRH，同时数据部分已经向左循环位移了一次。  
<img width="450" alt="image" src="https://github.com/user-attachments/assets/14cf7edd-05cf-445c-9a41-9f78f535af54" />
<img width="450" alt="image" src="https://github.com/user-attachments/assets/1e0be680-b13b-4438-bf9a-2813da9c7688" />
