#!/bin/bash
../utils/flush
../utils/nodeini 1
../utils/adcini 1
#sleep 1
../utils/port3read <<EOF
0
0
EOF
../utils/get_ped 1 <<EOF
5
1
EOF
../utils/analped output.dat
cat pedestal.txt
