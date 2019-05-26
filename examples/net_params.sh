#!/bin/bash
sysctl -w net.core.rmem_default=8388608
sysctl -w net.core.rmem_max=8388608
sysctl -w net.core.wmem_default=8388608
sysctl -w net.core.wmem_max=8388608
#sysctl -w net.ipv4.tcp_mem="42303 56405 84606"
sysctl -w net.ipv4.tcp_rmem="4096 4194304 4194304"
sysctl -w net.ipv4.tcp_wmem="4096 4194304 4194304"
sysctl -w net.ipv4.tcp_timestamps=0
