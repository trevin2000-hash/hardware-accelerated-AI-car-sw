# First flush the existing configuration
sudo ip addr flush dev eth0

# Then add the IPv4 address again
sudo ip addr add 192.168.100.1/24 dev eth0

# Verify the configuration
ip addr show eth0