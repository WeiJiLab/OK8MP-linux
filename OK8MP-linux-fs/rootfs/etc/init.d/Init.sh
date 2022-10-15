#! /bin/sh

## add forlinx user 
useradd -k /etc/skel -m -U forlinx -s /bin/bash
passwd forlinx <<EOM
forlinx
forlinx
EOM

## 

resize2fs /dev/mmcblk2p2
/usr/bin/fltest_runRefreshMatrix.sh


## delete Init.sh
sed -i "s/\/etc\/init.d\/Init.sh//g" /etc/rc.local
rm -f /etc/init.d/Init.sh
sync

