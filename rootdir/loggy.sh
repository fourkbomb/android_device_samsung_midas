#!/system/bin/sh
#loggy
find /dev > /dev/kmsg
/system/bin/sh /data/debug.sh

date=`date +%F_%H-%M-%S`
logcat -v time -f  /data/media/cm14logcat_${date}.txt

