#!/usr/bin/env python

import SimpleHTTPServer
import SocketServer
import os
import transmit
import time

global transmitter

# sudo lsof -i:8888 

class server(SimpleHTTPServer.SimpleHTTPRequestHandler):
    
    def do_POST(self):
        if self.path=="/ping":
            self.send_response(200)
            return
                        
        robot_id = -1
        try:
            robot_id = int(self.path.split("-")[1])
        except:
            pass
        print(robot_id)

        if robot_id!=-1:
            data_string = self.rfile.read(int(self.headers['Content-Length']))
            print("data_string is: "+data_string)

            if transmitter.program(robot_id,data_string):
                self.send_response(200)
            else:
                self.send_response(500)
        else:
            self.send_response(501)
        
    def do_GET(self):
        return SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)


os.chdir("/home/pi/code/penelopean-robotics/yarn-blocks")
os.system("pkill yarnc")

PORT = 8888
SocketServer.TCPServer.allow_reuse_address = True
httpd = SocketServer.TCPServer(("", PORT), server)
transmitter = transmit.transmit()
httpd.serve_forever()

