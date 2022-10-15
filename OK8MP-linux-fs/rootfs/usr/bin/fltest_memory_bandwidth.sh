#!/bin/bash
for opt in rd wr rdwr cp frd fwr fcp bzero bcopy
do
echo "L1 cache bandwidth $opt test with #$proc process"
#8k is fit for all platform
for idx in `seq 1 5`
do
bw_mem -P 1 8k $opt
done
echo "L2 cache bandwidth $opt test"
# For Layerscape platform, each platform has more than 256K L2 cache, so chose 128k as L2 cache size.
for idx in `seq 1 5`
do
bw_mem -P 1 128k $opt
done
echo "Main mem bandwidth $opt test"
for idx in `seq 1 5`
do
bw_mem -P 1 50m $opt
done
done
