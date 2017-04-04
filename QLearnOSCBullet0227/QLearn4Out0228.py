#import gym
import numpy as np
from random import randint
#from gym.spaces import Box, Discrete
from keras.models import Model
from keras.layers import Input, Dense, Lambda,merge
from keras.layers.normalization import BatchNormalization
from keras import backend as K
from keras.models import Sequential
from numpy.random import *
import argparse
import OSC
import matplotlib.pyplot as plt


class QLearn:
    __BATCH_SIZE=100
    __EXPLORATION=0.001
    __NUM_INPUT=4
    __NUM_OUTPUT=10
    __MIN_TRAIN=100
    __TRAIN_REPEAT=100
    __GANMMA=0.90
    __DISPLY=True
    __NUM_ACTIONS=5
    prestates = []
    actions = []
    rewards = []
    poststates = []
    terminals = []
    total_reward = 0
    episode_reward=0
    observation = np.array([0,0,0,0])
    action =[0]


    #env = gym.make('Pendulum-v0')


    def __init__(self):
        status,action_0,action_1,action_2,action_3,action_4,out = self.createLayers(self.__NUM_INPUT,self.__NUM_OUTPUT)
        ###########make caluculation Model#################
        self.model = Model(input=[x,a0], output=z)

        self.model.summary()
        status,action_0,action_1,action_2,action_3,action_4,out = self.createLayers(self.__NUM_INPUT,self.__NUM_OUTPUT)
        #target_model = Model(input=x, output=z)
        #target_model.set_weights(self.model.get_weights())
        self.model.compile(optimizer='adam', loss='mse')


    def createLayers(self,num_input,num_output):
        #x = Input(shape=env.observation_space.shape)
        #i=3
        status  = Input(shape=(num_input,),name='x')
        action_0 = Input(shape=(5,), name='a0')
        action_1 = Input(shape=(5,), name='a1')
        action_2 = Input(shape=(5,), name='a2')
        action_3 = Input(shape=(5,), name='a3')

        x = merge([status,action_0,action_1,action_2,action_3], mode='concat')

        #x = Input(shape=(num_input,))#########input_NUM

        h = x
        h = Dense(64, activation='tanh')(h)
        h = Dense(64, activation='tanh')(h)
        h = Dense(64, activation='tanh')(h)
        h = Dense(64, activation='tanh')(h)

        y = Dense(64,input_shape=(64,))(h)
        out = Dense(num_output,input_shape=(64,))(y)


        #z = Dense(num_output,input_shape=(64,))(y)
        #print('z:',z)
        #return x, z
        return status,action_0,action_1,action_2,action_3,action_4,out

###########################################################
###########################################################

    def resetParametors(self):
        ######reset per repetition
        #self.env.reset()
        self.episode_reward = 0

    def getAction(self):

        if np.random.random() < self.__EXPLORATION:
            for i in range(self.__NUM_ACTIONS):
                self.action[i]=elf.__getRandomAction()
        else:
            s = np.array([self.observation])
            q = self.model.predict(s, batch_size=10)##########
            self.action = [np.argmax(q[0])]
            print("Action:",self.action)
            print(q)

        return self.action


    def __setQ(self,NUM_ACTION,NUM_INPUT):
        allCase=int(numInput**numAction)
        for i in range(allCase):
            a0,a1,a2,a3=self.__getActionFromNum(i,NUM_ACTION,NUM_INPUT)
            __q = self.model.predict(s,a0,a1,a2,a3, batch_size=10)
            q.append(__q)
        self.action = [np.argmax(q[0])]


    def __getActionFromNum(self,num,NUM_ACTION,NUM_INPUT):
        a0= num%    NUM_INPUT
        num=int(num/NUM_INPUT)
        a1=num%     NUM_INPUT
        num=int(num/NUM_INPUT)
        a2=num%     NUM_INPUT
        num=int(num/NUM_INPUT)
        a3=num%     NUM_INPUT
        return a0,a1,a2,a0



    def __getRandomAction(self):
        action=[randint(0,self.__NUM_OUTPUT)]
        print(action)
        return action


    def preProcces(self,action,observation):
        self.prestates.append(observation)
        self.actions.append(action)

###########################################################
###########################################################

    def setStatus(self,observation,reward,done,info):
        self.rewards.append(reward)
        self.poststates.append(observation)
        self.terminals.append(done)
        self.episode_reward += reward
        self.observation=observation
        #print(type(self.poststates))

        self.printStatus(observation,reward,done,info)

    def printStatus(self,observation,reward,done,info):
        print("observation:",observation)
        print("reward:",reward)
        print("done:",done)
        print("info:",info)
        print ("rewards:",self.episode_reward)


    def postProcces(self):
        if len(self.prestates) > self.__MIN_TRAIN:
            indexes=self.__getIndexes()
            self.__calculateQLerning(indexes)


    def __getIndexes(self):
        for k in range(self.__TRAIN_REPEAT):
            if len(self.prestates) > self.__BATCH_SIZE:
                indexes = np.random.choice(len(self.prestates), size=self.__BATCH_SIZE)
            else:
                indexes = range(len(self.prestates))
        return indexes


    def __calculateQLerning(self,indexes):
        #########prediction per repitation################
        qpre = self.model.predict(np.array(self.prestates)[indexes])
        qpost = self.model.predict(np.array(self.poststates)[indexes])

        #print("prestates:",self.prestates)
        #print("poststates:",self.poststates)

        for i in range(len(indexes)):
            actionIndex=self.actions[indexes[i]]
            if self.terminals[indexes[i]]:
                qpre[i, actionIndex] = self.rewards[indexes[i]]
            else:
                qpre[i, actionIndex] = self.rewards[indexes[i]] + self.__GANMMA * np.amax(qpost[i])

        ##########Single gradient update over one batch of samples.###############
        #print (qpre)
        loss=self.model.train_on_batch(np.array(self.prestates)[indexes], qpre)
        print ("loss:",loss)

#############################################################
##############################################################
    #def render(self):
        #self.env.render()
