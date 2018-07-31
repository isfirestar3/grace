# -*- coding:utf-8 -*-
from multiprocessing import Process,Pool
import subprocess
import os,time

#python3 ping_ips.py
##iptables need to ping, change it before use "ping_ips.py"!
g_ip_table=[
"10.10.100.151",
"10.10.100.152",
"10.10.100.151",
"10.10.100.253",
"10.10.102.74",
"10.10.102.102",
]

g_exit_flag = False
g_sleep_time = 2
log_name = "./ping_" + time.strftime("%Y%m%d_%H%M%S", time.localtime()) + ".log"
log_detail = "./ping_detail_" + time.strftime("%Y%m%d_%H%M%S", time.localtime()) + ".log"
log_name_hl = open(log_name, 'w+')
log_detail_hl = open(log_detail, 'w+')

def grep(srcstr, keyword):
    lines = srcstr.split('\n')
    if 0 == len(lines):
        return list()
    r = list()
    for l in lines:
        if keyword in l:
            r.append(l)
    return r

class c_ping(Process):
    def __init__(self,ip):
        Process.__init__(self)
        self.__ip = ip
        
    def run(self):
        global g_exit_flag
        global g_sleep_time
        log = ""
        fresh_count = 0
        try:
            while g_exit_flag == False:
                p = subprocess.Popen([r'/bin/ping', '-c1', '-w1', self.__ip],stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                out = p.stdout.read()
                s = out.decode()
                log_detail_hl.write(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + "\n" + s)
                lines = grep(s, '100% packet loss')
                if len(lines) > 0:
                    print('%s connection lost.' % self.__ip)
                    log = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + " ip[" + self.__ip + "] unconnected"
                    log_name_hl.write(log + "\n")
                else:
                    print('%s connected.' % self.__ip)
                    log = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + " ip[" + self.__ip + "] connected"
                    log_name_hl.write(log + "\n")
                
                fresh_count += 1
                if 5 == fresh_count:
                    log_name_hl.flush()
                    log_detail_hl.flush()
                    fresh_count = 0
                time.sleep(g_sleep_time)
        except(Exception):
            g_exit_flag = True
            print("except happen, ip[%s]" % self.__ip)
            log_name_hl.flush()
            log_name_hl.close()
            log_detail_hl.close()
        else:
            g_exit_flag = True
            print("close file")
            log_name_hl.flush()
            log_name_hl.close()
            log_detail_hl.close()


if __name__ =='__main__': 
    print('Run the main process (%s).' % (os.getpid()))
    ping_th = []
    for i in range(len(g_ip_table)):
        print("ip:", g_ip_table[i])
        th = c_ping(g_ip_table[i])
        th.daemon = True
        th.start()
        ping_th.append(th)
    
    for i in range(len(ping_th)):
        ping_th[i].join()
    
    log_name_hl.flush()
    log_name_hl.close()
    log_detail_hl.close()
    
    