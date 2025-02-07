sudo ip -6 route add fc00:2::a/64 encap seg6local action End dev eth1 metric 200
sudo ip -6 route add fc00:2::a/64 encap seg6local action End dev eth2 metric 300
