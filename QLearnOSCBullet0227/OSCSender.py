#/usr/bin/env python3
import argparse
import random
import time

from pythonosc import osc_message_builder
from pythonosc import udp_client

class OSCSender:
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default="127.0.0.1",help="The ip of the OSC server")
    parser.add_argument("--port", type=int, default=7115,help="The port the OSC server is listening on")
    args = parser.parse_args()

    client = udp_client.SimpleUDPClient(args.ip, args.port)


    def __init__(self):
        print()
        #parser = argparse.ArgumentParser()
        #parser.add_argument("--ip", default="127.0.0.1",help="The ip of the OSC server")
        #parser.add_argument("--port", type=int, default=7115,help="The port the OSC server is listening on")
        #args = parser.parse_args()
        #self.client = udp_client.SimpleUDPClient("127.0.0.1", 7115)

    def sendAction(self,action):
        self.client.send_message("/action", action)




""""

from OSC import OSCClient, OSCMessage

class OSCSender:
    client = OSCClient()


    def __init__(self):
        self.client.connect( ("localhost", 7110) )



    def sendAction(self,action):
        s='/action'
        s+=str(action)
        self.client.send( OSCMessage(s) )




#client.send( OSCMessage("/quit") )

#client.send( OSCMessage("/user/2", [2.0, 3.0, 4.0 ] ) )
#client.send( OSCMessage("/user/3", [2.0, 3.0, 3.1 ] ) )
#client.send( OSCMessage("/user/4", [3.2, 3.4, 6.0 ] ) )

"""
