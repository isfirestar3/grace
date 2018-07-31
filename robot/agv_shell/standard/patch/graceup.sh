#!/bin/bash

patch_name="patch"
update_file_path="/agvshell/standard/"$patch_name"/"
if [ $# -gt 0 ]; then
	update_file_path=$1
fi
dest_grobot_path="/gzrobot/"
dest_agvshell_path="/agvshell/"
dest_lib_path="/usr/local/lib/"

##create directory
mkdir -p $dest_grobot_path
mkdir -p $dest_lib_path

head_agvshell=agv_shell
head_mt=motion_template
head_loc=Loc
head_devition=Deviation
head_picking=custom_picking
head_cart=custom_cart
head_nshost=nshost.so
head_motion_net=motion_net.so
head_camera=libgzcamera.so
head_fts=libgzfts.so
file_clear_log=clear_log.sh
file_autostart=autostart.sh
file_agvshell_config=agv_shell.xml
file_version=version.txt
file_custom_setting_pickingagv=custom_setting_pickingagv_fork_protect.xml


#kill processes
pkill $head_cart
pkill $head_picking
pkill $head_devition
pkill $head_loc
pkill $head_mt
pkill $head_agvshell

sleep 3
date_today_format=`date "+%Y%m%d_%H%M%S"`

function cp_to_gzrobot()
{
    echo "update file:"$update_file_path$1", file_head:"$2 >>/agvshell/log/update_$date_today_format.log
    readelf -h $update_file_path$1 >>/dev/null
	#echo $?
	if [ $? -eq 0 ]; then
		mv $update_file_path$1 $dest_grobot_path
		if [[ $dest_grobot_path$1 != $dest_grobot_path$2 ]]; then
			ln -sf $dest_grobot_path$1 $dest_grobot_path$2
		#else
			#echo $1" not include version info"
		fi
	fi
}

function cp_to_agvshell()
{
    echo "update file:"$update_file_path$1", file_head:"$2 >>/agvshell/log/update_$date_today_format.log
    readelf -h $update_file_path$1 >>/dev/null
	#echo $?
	if [ $? -eq 0 ]; then
		mv $update_file_path$1 $dest_agvshell_path
		if [[ $dest_agvshell_path$1 != $dest_agvshell_path$2 ]]; then
			ln -sf $dest_agvshell_path$1 $dest_agvshell_path$2
		#else
			#echo $1" not include version info"
		fi
	fi
}

function cp_to_lib()
{
    echo "update file:"$update_file_path$1", file_head:"$2 >>/agvshell/log/update_$date_today_format.log
    readelf -h $update_file_path$1 >>/dev/null
	#echo $?
	if [ $? -eq 0 ]; then
		mv $update_file_path$1 $dest_lib_path
		if [[ $dest_lib_path$1 != $dest_lib_path$2 ]]; then
			ln -sf $dest_lib_path$1 $dest_lib_path$2
		#else
			#echo $1" not include version info"
		fi
	fi
}



all_files=`ls $update_file_path`
for s in ${all_files[@]}; do
	#echo $s
    #sleep 3;
	if [[ $s == $head_cart* ]]; then
		cp_to_gzrobot $s $head_cart
	fi
    
	if [[ $s == $head_picking* ]]; then
		cp_to_gzrobot $s $head_picking
	fi
    
    if [[ $s == $head_devition* ]]; then
		cp_to_gzrobot $s $head_devition
	fi
    
    if [[ $s == $head_loc* ]]; then
		cp_to_gzrobot $s $head_loc
	fi
    
    if [[ $s == $head_mt* ]]; then
		cp_to_gzrobot $s $head_mt
	fi
    
    if [[ $s == $head_camera* ]]; then
		cp_to_lib $s $head_camera
	fi
    
    if [[ $s == $head_motion_net* ]]; then
		cp_to_lib $s $head_motion_net
	fi
    
    if [[ $s == $head_nshost* ]]; then
		cp_to_lib $s $head_nshost
	fi
    
    if [[ $s == $head_agvshell* ]]; then
		cp_to_agvshell $s $head_agvshell
	fi
    
    if [[ $s == $head_fts* ]]; then
		cp_to_agvshell $s $head_fts
	fi
    
done;

#copy scripts
if [ -f $update_file_path$file_clear_log ]; then 
	mv $update_file_path$file_clear_log $dest_grobot_path
fi
if [ -f $update_file_path$file_autostart ]; then
	mv $update_file_path$file_autostart $dest_agvshell_path"/standard/"
fi
if [ -f $update_file_path$file_agvshell_config ]; then
	mv $update_file_path$file_agvshell_config $dest_agvshell_path"/standard/"
fi
if [ -f $update_file_path$file_version ]; then
	mv $update_file_path$file_version $dest_grobot_path
fi
if [ -f $update_file_path$file_custom_setting_pickingagv ]; then
	mv $update_file_path$file_custom_setting_pickingagv /etc/agv/
fi

#exit;
reboot

