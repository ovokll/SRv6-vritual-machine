# SRv6-vritual-machine
This project is a SRv6 Sandbox.
## Install Guide
1) Download Machine  
You can download the `.ova` file at this link: https://pan.baidu.com/s/14-slbUZkwORLGymKFIZ04g?pwd=5ryi  
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
Sending some messages from `hostA` to `hostD`. You can see that the capitalization of the first few letters has been reversed.

