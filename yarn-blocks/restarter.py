#!/usr/bin/env python

import psutil
import urllib2
import datetime
import signal
import time
import socket

def killproc(procname):
    for proc in psutil.process_iter():
        # check whether the process name matches
        if procname in proc.cmdline():            
            print(str(datetime.datetime.now())+" restarted")
            proc.kill()

def running(url):
    try:
        req = urllib2.Request(url, "")
        response = urllib2.urlopen(req)
        if response.code==200:
            return True
        else:
            print("not the expected response")
            return False
    except Exception, e:
        print("not there",e)
        return False
    return False

def timeout(signum, frame):
    print("timeout")
    raise Exception("end of time")
    
url = "http://127.0.0.1:8888/ping"

signal.signal(signal.SIGALRM, timeout)
signal.alarm(10)
socket.setdefaulttimeout(1.0)

while True:
    try:
        time.sleep(5)
        if not running(url):
            print("killing")
            killproc("./robot-server.py")
    except Exception, exc: 
        print exc
