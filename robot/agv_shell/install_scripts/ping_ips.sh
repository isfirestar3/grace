#!/bin/bash

##iptables need to ping, change it before use "ping_ips.sh"!
ips=("10.10.100.151"
"10.10.100.152"
"10.10.100.155"
"10.10.100.160")

sleep_sec=2


mkdir -p /gzrobot/log/
date_today=`date "+%Y%m%d_%H%M%S"`
while [ true ]
do
    for s in ${ips[@]}; do
        ping -c 1 -w 1 $s > /dev/null
        if [ $? -eq 1 ]; then
            echo `date`", "$s" not connected!!!" >>/gzrobot/log/ping_$date_today".log"
        else
            echo `date`", "$s" is connected" >>/gzrobot/log/ping_$date_today".log"
        fi
    done;
    sleep $sleep_sec;
done

