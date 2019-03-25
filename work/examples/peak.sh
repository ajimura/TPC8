#!/bin/bash
../utils/get_data 1 <<EOF
5
1
2
1
EOF
../utils/analpeak output.dat > peak.txt
cat peak.txt


