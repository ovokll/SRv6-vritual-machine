# SRv6-vritual-machine
This project is a SRv6 Sandbox.
## Install Guide
1) Download Machine  
You can download the `.ova` file at this link: https://pan.baidu.com/s/14-slbUZkwORLGymKFIZ04g?pwd=5ryi  
2) Building Network Topology  
Download the `01-network-manager-all.yaml` file for each machine, and replace the file at `/etc/netplan` folder. Then enter `sudo netplan apply` in the command line to enable the setting.  
Ultimately forming a linear topology from `hostA` to `hostD`  
## Usage  
Here is a small example provided to understand how to use it. The service at host B will reverse the case of the first five English letters of the packets passing through.  
1) Set up SRV6 forwarding  
Run the `route.sh` file prepared for each host.  
2) Enable the Reverse Letter Service  
Use the `make` command in `hostB` to compile the prepared code, run it with `sudo insmod reverse_letter.ko`, and finally stop the service using `sudo rmmod reverse_letter`.  
3) Sending Message  
Sending some messages from `hostA` to `hostD`. You can see that the capitalization of the first few letters has been reversed。
