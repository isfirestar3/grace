# !/bin/usr/bash

set -e # exit immediately on error
set -x # display all commands

N_CORE=$(cat /proc/cpuinfo | grep processor | wc -l)

if [ $# -lt 1 ]; then
	echo 'usage: /usr/bin/bash @your-version'
	exit
fi

patch_dir=./$1/

if [ -d ${patch_dir} ]; then
	rm ${patch_dir} -fr
fi

mkdir ${patch_dir}
mkdir ${patch_dir}arm-linux
mkdir ${patch_dir}sdk
mkdir ${patch_dir}sdk/arm-linux-lib/
mkdir ${patch_dir}sdk/win32-lib/
mkdir ${patch_dir}sdk/x64-linux-lib/
mkdir ${patch_dir}tool
mkdir ${patch_dir}win32
mkdir ${patch_dir}win32-debug
mkdir ${patch_dir}x64-linux

arm_target_dir=("./robot/motion_template/" "./gzfts/" "./robot/agv_shell/" "./robot/motion_interface/" "./motion_net/" "./robot/gzcamera/")
arm_target_file=("motion_template" "libgzfts.so" "agv_shell" "libmotion_interface.a" "motion_net.so" "libgzcamera.so")
x64_target_dir=("./motion_net/" "./path_search/" "./manage/agv_interface/" "./agvinfo/agvinfo_server/" "./agvinfo/libagvinfo/prj_win32/")
x64_target_file=("motion_net.so" "PathSearch.so" "libagv_interface.a" "agvinfo_server" "libagvinfo.so")
ARM_MAKE_ARG='arch=arm'
DEBUG_ARG='build=debug'

sdk_target_dir=${patch_dir}sdk/

# clear *.o files that are already existed in sdk directory
for ((i=0;i<${#arm_target_dir[@]};i++)) do
	make -C ${arm_target_dir[i]} clean
done

# storage all head/library files
cp -rf sdk/misc $sdk_target_dir
cp -rf sdk/nsp $sdk_target_dir
cp -rf sdk/var $sdk_target_dir
cp -rf sdk/rapidxml $sdk_target_dir

# build all project for arm-linux runtime
for ((i=0;i<${#arm_target_dir[@]};i++)) do
	make -C ${arm_target_dir[i]} clean
	make -C ${arm_target_dir[i]} ${ARM_MAKE_ARG} -j${N_CORE}
	cp -f ${arm_target_dir[i]}${arm_target_file[i]} ${patch_dir}arm-linux/
	make -C ${arm_target_dir[i]} clean
	make -C ${arm_target_dir[i]} ${ARM_MAKE_ARG} ${DEBUG_ARG} -j${N_CORE}
	cp -f ${arm_target_dir[i]}${arm_target_file[i]} ${patch_dir}arm-linux/${arm_target_file[i]}.debug
done

# copy newest nshost library to arm linux package
# insure symbolic link of nshost.so has full path
newest_nshost_for_arm=$(readlink /usr/local/lib/nshost.so)
cp -f ${newest_nshost_for_arm} ${patch_dir}arm-linux/

# adjust library file location from app output to sdk
mv ${patch_dir}arm-linux/libmotion_interface.* ${patch_dir}sdk/arm-linux-lib/

# build all project for x86-64 linux runtime
for ((i=0;i<${#x64_target_dir[@]};i++)) do
	make -C ${x64_target_dir[i]} clean
	make -C ${x64_target_dir[i]} -j${N_CORE}
	cp -f ${x64_target_dir[i]}${x64_target_file[i]} ${patch_dir}x64-linux/
	make -C ${x64_target_dir[i]} clean
	make -C ${x64_target_dir[i]} ${DEBUG_ARG} -j${N_CORE}
	cp -f ${x64_target_dir[i]}${x64_target_file[i]} ${patch_dir}x64-linux/${x64_target_file[i]}.debug
done

# copy newest nshost libraray to x86-64 linux package
# insure symbolic link of nshost.so has full path
newest_nshost_for_x8664=$(readlink /usr/local/lib64/nshost.so)
cp -f ${newest_nshost_for_x8664} ${patch_dir}x64-linux/

# adjust library file location from app output to sdk
mv ${patch_dir}x64-linux/libagv_interface.* ${patch_dir}sdk/x64-linux-lib/

#for i in ${filepath[@]}; do
#	for arg in ${build_args[@]}; do
#		make clean -C $i
#		make -C $i $arg $1 -j ${N_CORE}
#	done
#done

# ok.let's mark the versions of this patch.
version_file=${patch_dir}arm-linux/version.txt

if [ -f ${version_file} ]; then
	rm -f ${version_file}
fi

echo $1 >> ${version_file}

svn_Relative=$(svn info | grep Relative | cut -d: -f2)
echo ${svn_Relative} >> ${version_file}

find ${patch_dir}arm-linux/* -type f -exec md5sum {} \; >> ${version_file}
