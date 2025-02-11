## Language

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
您可以通过以下链接下载“.ova”文件：`https://pan.baidu.com/s/14-slbUZkwORLGymKFIZ04g?pwd=5ryi`. 然后多次导入该文件，并将其重命名为hosta到hostd。
2）构建网络拓扑  
为每台机器下载“01 networkmanager all.yaml”文件，并替换“/etc/netplan”文件夹中的文件。然后在命令行中输入“sudo netplan apply”以启用该设置。  
最终形成从“hostA”到“hostD”的线性拓扑  
## 使用方法  
这里提供了一个示例来了解如何使用它。hostB中的服务会将指定目的地址的TCP数据包前5位循环左移一次。
1）设置SRV6转发动作
运行为每个主机准备的`route.sh`文件。
2）启用左移服务
使用`hostB`中的`make`命令编译准备好的代码，使用`sudo insmod left_shift_data.ko`运行它，最后使用`sudo rmmod left_shift_data`停止服务。
3）发送消息
从“hostA”向“hostD”发送一些消息。你可以看到前几个字符顺序发生了改变。
