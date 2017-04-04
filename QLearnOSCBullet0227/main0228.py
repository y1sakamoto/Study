#import gym
#from gym.spaces import Box, Discrete
import sys
import numpy as np



#!/usr/bin/env python3
from OSC import OSCServer
import sys
from time import sleep
import types

recieveServer = OSCServer( ("localhost", 7116) )
recieveServer.timeout = 0
run = True
def handle_timeout(self):
    self.timed_out = True

recieveServer.handle_timeout = types.MethodType(handle_timeout, recieveServer)


from OSCSender import OSCSender
sender=OSCSender()


observation=np.array([0,0,0,0])

client = OSCSender()
#client.connect( ("localhost", 7115) )

from QLearn4Out0228 import QLearn
QL=QLearn()


def episodeStart(path, tags, args, source):
    print("start")
    global observation
    QL.resetParametors()
    action=QL.getAction()

    #env.reset()
    observation=[0,0,0,0]####FirstStatus
    QL.preProcces(action,observation)


def sendAction():
    #env.render()
    global observation
    action=QL.getAction()
    print(action[0])
    QL.preProcces(action,observation)
    i=int(action[0])
    client.sendAction(i)
    #print ("preobservation:",observation)



def getStatus(path, tags, args, soursce):
    print(args[0],args[1],args[2],args[3],args[4],args[5],args[6])

    global observation
    #observation, reward, done, info = env.step(QL.action)
    observation=[args[0],args[1],args[2],args[3]]#################
    reward=args[4]
    done=args[5]
    info=args[6]
    QL.setStatus(observation,reward,done,info)
    QL.postProcces()
    #print ("postobservation:",observation)
    #print ("reward:",reward)
    #print ("done:",done)
    #print ("info:",info)
    sendAction()


def quit_callback(path, tags, args, source):
    global run
    run = False
    print("quit")
    sys.exit()

recieveServer.addMsgHandler( "/start", episodeStart )
recieveServer.addMsgHandler( "/status", getStatus )
recieveServer.addMsgHandler( "/quit", quit_callback )


def each_frame():
    recieveServer.timed_out = False
    while not recieveServer.timed_out:
        recieveServer.handle_request()
while run:
    each_frame()

recieveServer.close()
print("close")
