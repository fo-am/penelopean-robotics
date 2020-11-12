import SimpleHTTPServer
import SocketServer
import os
import transmit

global transmitter
transmitter = transmit.transmit()


class server(SimpleHTTPServer.SimpleHTTPRequestHandler):
    
    def do_POST(self):
        print("POST",self.path)
        data_string = self.rfile.read(int(self.headers['Content-Length']))
        print(data_string)
        with open("robot.lisp", "w") as outfile:
            outfile.write(data_string)
        transmitter.program(3,"robot.lisp")
        #os.system("../compiler/compile.sh robot.lisp")
        self.send_response(200)
        
    def do_GET(self):
        return SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)

os.chdir("/home/pi/code/penelopean-robotics/yarn-blocks")
PORT = 8888
httpd = SocketServer.TCPServer(("", PORT), server)
httpd.serve_forever()
