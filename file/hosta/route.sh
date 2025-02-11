sudo ip -6 route del 2024:cd::2
sudo ip -6 route add 2024:cd::2/128 encap seg6 mode encap segs fc00:2::a0,fc00:2::a0,fc00:3::a0 dev eth0
