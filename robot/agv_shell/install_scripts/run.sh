#!/bin/bash
echo 'core-%t-%e' > /proc/sys/kernel/core_pattern
ulimit -c unlimited

#graceup
cd /agvshell/standard/patch/
. /agvshell/standard/patch/graceup.sh

#start ahv_shell
cd /agvshell/
/bin/chmod +x /agvshell/agv_shell
/agvshell/agv_shell -s &
