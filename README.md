
## Language/语言

- [English](#english)
- [中文](#中文)

### English

# SRv6-vritual-machine
This project is a SRv6 Sandbox.

## Install Guide
1) Download Virtual Machine  
You can download the `.ova` file at this link: `https://pan.baidu.com/s/14-slbUZkwORLGymKFIZ04g?pwd=5ryi`. Then import the file multiple times in VMware and use it as `Host A` to `Host E`. The username and password are both `kll`.   
2) Modify Network Adapter Configuration  
Before starting the virtual machine, You should configure the network adapter in the order corresponding to the `yaml` file in 3). Do not modify separately.  
- Host A is configured with two network adapter: LAN segment (ab) and NET  
- Host B is configured with four network adapter: namely LAN segment (ab), LAN segment (bc), LAN segment (be), and NET  
- Host C is configured with four network adapter: namely LAN segment (bc), LAN segment (cd), LAN segment (ec), and NET  
- Host D is configured with two network adapter: namely LAN segment (cd) and NET  
- Host E is configured with three network adapter: namely LAN segment (be), LAN segment (ec), and NET  

3) Configure Network Address and Static Routing Table  
Download the `01-networkmanager-all.ymal` file for each machine and replace the files in the `/etc/netplan` folder. Then use the following sentence in the command line to enable the setting.
```bash
sudo netplan apply
```
The final topology from Host A to Host E is shown in the following figure.  

<img width="772" alt="image" src="https://github.com/user-attachments/assets/fe1949e1-3d30-40d4-a583-48cb3258e73a" />

## Usage

Here are several examples provided to understand and use SRv6.

### 1. SRv6 is not enabled  
1) Send Message  
Use server.c in `Host D` to receive information.  
Use client.c in `Host A` to send messages.  
Running Wireshark at `Host C` shows that packets are sent directly from `Host B` to `Host C`(Wireshark captures the network card as eth0).  
 
<img width="400" alt="image" src="https://github.com/user-attachments/assets/661c14eb-cb01-4cc0-ad68-92629b301680" />
<img width="400" alt="image" src="https://github.com/user-attachments/assets/b9601e8a-60a5-4883-b0fd-11701e959645" />

### 2. Enable SRv6  
1) Set up ip tables  
Run at `Host A`  
```bash
sudo ip -6 route add 2024:cd::2/128 encap seg6 mode encap segs fc00:2::a0,fc00:5::a0,fc00:3::a0 dev eth0
```
Run at `Host B`  
```bash
sudo ip -6 route add fc00:2::a/64 encap seg6local action End dev eth1 metric 200
sudo ip -6 route add fc00:2::a/64 encap seg6local action End dev eth2 metric 300
```
Run at `Host E`  
```bash
sudo ip -6 route add fc00:5::a0/64 encap seg6local action End dev eth1 metric 200
```
Run at `Host C`  
```bash
sudo ip -6 route add fc00:3::a0/64 encap seg6local action End.DX6 nh6 2024:cd::2 dev eth1
```
2) Send Message  
Use server.c in `Host D` to receive information.  
Use client.c in `Host A` to send messages.  
Running Wireshark at `Host C` shows that packets are sent from `Host E` to `Host C` and there is an additional Source Routring Header (Wireshark captures the network card as eth2).  
<img width="555" alt="image" src="https://github.com/user-attachments/assets/452154e8-573e-447f-b1c4-aad268a18628" />

### 3. Add SRv6 Service  
1) A simple network service is provided in `Host B`, which can cyclically shift the first 5 bits of the sent TCP packet to the left once.  
The usage method is as follows.  
Copy the left_sthift_data folder to `Host B` and use the `make` command to compile the prepared code in the folder.  
Under this folder, use the following command to start the service
```bash
sudo insmod left_shift_data.ko
```
Finally, use the following command to stop the service   
```bash
sudo rmmod left_shift_data
```
2） Refer to [2](#Enable) for seting up ip tables  
3）发送消息  
在`hostD`中使用server.c来接收信息  
在`hostA`中使用client.c来发送消息  
在`hostC`处运行wireshark可以看到数据包是从`Host E`处发往`Host C`，并且有一个额外的SRH，同时数据部分已经向左循环位移了一次。  
<img width="450" alt="image" src="https://github.com/user-attachments/assets/14cf7edd-05cf-445c-9a41-9f78f535af54" />
<img width="450" alt="image" src="https://github.com/user-attachments/assets/1e0be680-b13b-4438-bf9a-2813da9c7688" />


### 中文  
# SRv6虚拟机  
该项目是一个SRv6沙盒。  
## 安装指南  
1）下载虚拟机文件  
您可以通过以下链接下载`.ova`文件：`https://pan.baidu.com/s/14-slbUZkwORLGymKFIZ04g?pwd=5ryi`. 然后在VMware中多次导入该文件，并将其作为`Host A`到`Host E`。用户名和密码均为`kll`。   
2）修改网卡配置  
启动虚拟机前首先进行网卡配置，顺序与3）中的yaml文件对应，请勿单独修改  
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

### 1、未启用SRv6  
1）发送消息  
在`Host D`中使用server.c来接收信息  
在`Host A`中使用client.c来发送消息  
在`Host C`处运行wireshark可以看到数据包从`Host B`处直接发往`Host C`（wireshark捕获网卡为eth0）  
<img width="400" alt="image" src="https://github.com/user-attachments/assets/661c14eb-cb01-4cc0-ad68-92629b301680" />
<img width="400" alt="image" src="https://github.com/user-attachments/assets/b9601e8a-60a5-4883-b0fd-11701e959645" />

### 2、启用SRv6  
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

### 3、添加SRv6服务  
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
