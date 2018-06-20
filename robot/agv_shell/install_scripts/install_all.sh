#!/bin/bash

cd /install/
seccuss_flag=0

##bak ori dirs
rm -f /install/bak.tar.gz
rm -rf /agvshell/log
rm -rf /gzrobot/log
if [ -d "/usr/local/lib64" ]; then
    tar -czvf /install/bak.tar.gz /agvshell/ /gzrobot/ /etc/agv/ /usr/local/lib64/nshost.so /usr/local/lib64/libgzcamera.so /usr/local/lib64/motion_net.so /usr/local/lib64/libjpeg.so.62
else
    tar -czvf /install/bak.tar.gz /agvshell/ /gzrobot/ /etc/agv/ /usr/local/lib/nshost.so /usr/local/lib/libgzcamera.so /usr/local/lib/motion_net.so /usr/local/lib/libjpeg.so.62
fi


##delete ori dirs
rm -rf /agvshell/
rm -rf /gzrobot/
rm -rf /etc/agv/

##create directory and softlink
mkdir -p /agvshell/standard/
mkdir -p /gzrobot/
mkdir -p /etc/agv/

if [ -d "/usr/local/lib64" ]; then
	mkdir -p /usr/local/lib64/
	cp -fp /install/lib/* /usr/local/lib64/
else
	mkdir -p /usr/local/lib/
    cp -fp /install/lib/* /usr/local/lib/
fi
cp -rfp /install/agvshell/* /agvshell/
if [ $? -eq 1 ]; then
    seccuss_flag=1
    echo "/agvshell/ install failure, please check the directory!!!"
fi
cp -rfp /install/gzrobot/* /gzrobot/
if [ $? -eq 1 ]; then
    seccuss_flag=1
    echo "/gzrobot/ install failure, please check the directory!!!"
fi 
cp -rfp /install/agv/* /etc/agv/
if [ $? -eq 1 ]; then
    seccuss_flag=1
    echo "/etc/agv/ install failure, please check the directory!!!"
fi


if [ ! -d "/agvshell/standard/gzrobot" ]; then
    ln -sf /gzrobot /agvshell/standard/gzrobot
fi

if [ ! -d "/agvshell/standard/agv" ]; then
    ln -sf /etc/agv /agvshell/standard/agv
fi

if [ ! -d "/agvshell/standard/lib" ]; then
    mkdir /agvshell/standard/lib
fi

if [ -d "/usr/local/lib64" ]; then
    ln -sf /usr/local/lib64/libgzcamera.so /agvshell/standard/lib/libgzcamera.so
    ln -sf /usr/local/lib64/libjpeg.so.62 /agvshell/standard/lib/libjpeg.so.62
    ln -sf /usr/local/lib64/motion_net.so /agvshell/standard/lib/motion_net.so
    ln -sf /usr/local/lib64/nshost.so /agvshell/standard/lib/nshost.so
else
    ln -sf /usr/local/lib/libgzcamera.so /agvshell/standard/lib/libgzcamera.so
    ln -sf /usr/local/lib/libjpeg.so.62 /agvshell/standard/lib/libjpeg.so.62
    ln -sf /usr/local/lib/motion_net.so /agvshell/standard/lib/motion_net.so
    ln -sf /usr/local/lib/nshost.so /agvshell/standard/lib/nshost.so
fi
echo "create directory finished"

#copy files
cp -p /install/rc.local /etc/rc.local
echo "copy rc.local finished"

cp -p /install/ntp.conf /etc/ntp.conf
echo "copy ntp.conf finished"

if [ ! -f "/usr/sbin/ntpdate" ]; then
    cp -p /install/gzrobot/ntpdate /usr/sbin/ntpdate
    chmod +x /usr/sbin/ntpdate
fi
echo "copy /usr/sbin/ntpdate finished"

if [ ! -f "/usr/sbin/ntpd" ]; then
    cp -p /install/gzrobot/ntpd /usr/sbin/ntpd
    chmod +x /usr/sbin/ntpd
fi
echo "copy /usr/sbin/ntpd finished"


#start gzrobot.service
cd /agvshell/
chmod +x /agvshell/agv_shell
systemctl stop gzrobot.service
systemctl disable gzrobot.service
systemctl enable /agvshell/gzrobot.service

if [ $seccuss_flag -eq 1 ]; then
    echo "auto restart, please reconnect later!"
    sleep 1
    reboot
fi

#exit 0

