#import gym
import numpy as np
from random import randint
#from gym.spaces import Box, Discrete
from keras.models import Model
from keras.layers import Input, Dense, Lambda
from keras.layers.normalization import BatchNormalization
from keras import backend as K
from keras.models import Sequential
from numpy.random import *
import argparse
import OSC
#import matplotlib.pyplot as plt


class QLearn:
    __BATCH_SIZE=300
    __EXPLORATION=0.01
    __NUM_INPUT=9
    __NUM_OUTPUT=625
    __MIN_TRAIN=300
    __TRAIN_REPEAT=300
    __GANMMA=0.90
    __DISPLY=True

    prestates = []
    actions = []
    rewards = []
    poststates = []
    terminals = []
    total_reward = 0
    episode_reward=0
    observation = np.array([0,0,0,0,0,0,0,0,0])
    action =[0]
    #env = gym.make('Pendulum-v0')


    def __init__(self):
        x, z = self.createLayers(self.__NUM_INPUT,self.__NUM_OUTPUT)
        ###########make caluculation Model#################
        self.model = Model(input=x, output=z)

        self.model.summary()
        #x, z = self.createLayers(self.__NUM_INPUT,self.__NUM_OUTPUT)
        #target_model = Model(input=x, output=z)
        #target_model.set_weights(self.model.get_weights())
        self.model.compile(optimizer='adam', loss='mse')


    def createLayers(self,num_input,num_output):
        #x = Input(shape=env.observation_space.shape)
        #i=3
        x = Input(shape=(num_input,))#########input_NUM
        h = x
        h = Dense(640, activation='tanh')(h)
        h = Dense(640, activation='tanh')(h)
        h = Dense(640, activation='tanh')(h)
        h = Dense(640, activation='tanh')(h)
        y = Dense(640,input_shape=(640,))(h)

        z = Dense(num_output,input_shape=(640,))(y)
        #print('z:',z)
        #return x, z
        return x,z

###########################################################
###########################################################

    def resetParametors(self):
        ######reset per repetition
        #self.env.reset()
        self.episode_reward = 0

    def getAction(self):

        if np.random.random() < self.__EXPLORATION:
            self.action=self.__getRandomAction()
            ###########random
        else:
            s = np.array([self.observation])
            q = self.model.predict(s, batch_size=100)
            self.action = [np.argmax(q[0])]
            print("Action:",self.action)

        return self.action


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

        #self.printStatus(observation,reward,done,info)

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
