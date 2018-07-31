#!/bin/bash

sleep 3

patch_name="patch"
update_file_path="/agvshell/standard/"${patch_name}"/"
if [ $# -gt 0 ]; then
	update_file_path=$1
fi

date_today_format=`date "+%Y%m%d_%H%M%S"`
log_file=/agvshell/log/update_${date_today_format}.log
mkdir -p "/agvshell/log/"
echo upgrade begin ---------`date`------------------ >> ${log_file}

function fn_update_file()
{
	src_file_array=$1
	dest_path=$2
	
	for item in ${src_file_array[*]};do
		file_name=${update_file_path}${item}
		link_name=""
		
		if [ ! -f "${file_name}" ]; then
			if [ "${file_name##*.}" != "so" ]; then
				#is not a so file
				continue
			fi
			#find so file whith version, example: nshost.so -> nshost.so.x.y.z
			file_name=`ls ${update_file_path}${item}* 2>/dev/null | tail -1`
			if [ ! -f "${file_name}" ]; then
				continue
			fi
			item="${file_name##*/}"
		fi
		file_type=`file -b --mime-type ${file_name}`
		if [ "${file_type}" == "application/x-executable" ]; then
			#bin/elf files
			pkill ${file_name}
			chmod 770 ${file_name} 
		elif [ "${file_type}" == "text/x-shellscript" ]; then
			#shell/scirpts files
			pkill ${file_name}
			chmod 770 ${file_name} 
		elif [ "${file_type}" == "application/x-sharedlib" ]; then
			#lib/so files
			link_name=${item%%.so*}.so
		fi
		
		err_str=`mv -f ${file_name} ${dest_path} 2>&1`
		if [ $? -eq 0 ]; then
			echo "update file:"${file_name}" to:"${dest_path} >> ${log_file}
		else
			echo "${err_str}" >> ${log_file}
		fi
		
		if [ "${link_name}" != "" ]; then
			#creat so link, example: nshost.so.9.6.3 >>> nshost.so
			if [ ${item} != ${link_name} ]; then
				ln -sf ${dest_path}${item} ${dest_path}${link_name}
				echo "create a link file:"${dest_path}${link_name} >> ${log_file}
			fi
		fi
	done
}

#set copy dest directory
dest_gzrobot="/gzrobot/"
dest_agvshell="/agvshell/"
dest_standard="/agvshell/standard/"
dest_lib="/usr/local/lib/"
dest_etc="/etc/agv/"

#set create directory
mk_path=(
	${dest_gzrobot}
	${dest_agvshell}
	${dest_standard}
	${dest_lib}
	${dest_etc}
)
#create directory
for item in ${mk_path[*]};do
	if [ -d ${item} ]; then
		continue
	fi
	mkdir -p ${item}
	echo "mkdir:"${item} >>${log_file}
done

#set gzorobot file name
src_gzrobot=(
	#bin/elf files
	'motion_template'
	'Loc'
	'Deviation'
	'custom_picking'
	'custom_cart'
	#shell/scirpts files
	'clear_log.sh'
	'ping_ips.sh'
	#config/xml/txt files
	'docks.xml'
	'Deviation_common.ini'
	'Visual2dCodeLoc_common.ini'
	'axis_table.txt'
)
#update gzrobot files -> ${dest_gzrobot}
fn_update_file "${src_gzrobot[*]}" "${dest_gzrobot}"

#set agvshell file name
src_agvshell=(
	#bin/elf files
	'agv_shell'
	#shell/scirpts files
	'run.sh'
	#config/xml/txt files
	'version.txt'
	'version_config.txt'
	'gzrobot.service'
)
#update agvshell files -> ${dest_agvshell}
fn_update_file "${src_agvshell[*]}" "${dest_agvshell}"

#set standard file name
src_standard=(
	#shell/scirpts files
	'autostart.sh'
	#config/xml/txt files
	'agv_shell.xml'
)
#update agvshell files -> ${dest_standard}
fn_update_file "${src_standard[*]}" "${dest_standard}"

#set config etc file name
src_config_etc=(
	'misc.xml'
	'mnt.xml'
	'navigation.xml'
	'banks.xml'
	'safety.xml'
	'layout.xml'
	'layout.bin'
	'nsp.xml'
	'motion_net.xml'
	'custom_setting.xml'
	'custom_setting_cartagv.xml'
	'custom_setting_forkagv.xml'
	'custom_setting_pickingagv.xml'
	'custom_setting_pickingagv_fork_protect.xml'
)
#update etc files -> ${dest_etc}
fn_update_file "${src_config_etc[*]}" "${dest_etc}"

#set lib file name
src_lib=(
	'nshost.so'
	'libxml2.so'
	'motion_net.so'
	'libgzcamera.so'
	'libgzfts.so'
)
#update lib files -> ${dest_lib}
fn_update_file "${src_lib[*]}" "${dest_lib}"

#services enable
if [ -f "/agvshell/gzrobot.service" ]; then
	systemctl stop gzrobot.service
	systemctl disable gzrobot.service
	rm -f /etc/systemd/system/gzrobot.service
	systemctl enable /agvshell/gzrobot.service
fi

#update done.
sleep 0.2
echo upgrade end   ---------`date`------------------ >> ${log_file}
#Synchronize os's cached writes 
sync
sync
#reboot os
reboot

