import gym
import numpy as np
from gym.spaces import Box, Discrete
from keras.models import Model
from keras.layers import Input, Dense, Lambda
from keras.layers.normalization import BatchNormalization
from keras import backend as K
from keras.models import Sequential
from numpy.random import *
import argparse
import OSC

parser = argparse.ArgumentParser()
parser.add_argument('--BATCH_SIZE', type=int, default=100)
parser.add_argument('--EXPLORATION', type=float, default=0.02)
parser.add_argument('--NUM_OUTPUT', type=int, default=5)

parser.add_argument('--MIN_TRAIN', type=int, default=10)
parser.add_argument('--TRAIN_REPEAT', type=int, default=100)
parser.add_argument('--GANMMA', type=float, default=0.90)
args = parser.parse_args()


DISPLY=True

action =[]
env = gym.make('Pendulum-v0')
import matplotlib.pyplot as plt

def createLayers():
  #x = Input(shape=env.observation_space.shape)
  x = Input(shape=(3,))#########input_NUM
  #print ('x:',x)
  h = x
  h = Dense(32, activation='relu')(h)
  h = Dense(32, activation='relu')(h)
  h = Dense(32, activation='relu')(h)

  y = Dense(32,input_shape=(32,))(h)
  z = Dense(args.NUM_OUTPUT,input_shape=(32,))(y)
  #print('z:',z)
  return x, z

x, z = createLayers()
###########make caluculation Model#################
model = Model(input=x, output=z)
model.summary()
loss=0

x, z = createLayers()
target_model = Model(input=x, output=z)
target_model.set_weights(model.get_weights())
model.compile(optimizer='adam', loss='mse')



prestates = []
actions = []
rewards = []
poststates = []
terminals = []
total_reward = 0
episode_reward=0
observation = [0,0,0]
action =[0]


def getActionFromNum(_num):
    _action=[_num*1-2]
    return _action


def getIndexFromAction(_action):
    _num=int((_action[0]+2))
    return _num


def getAction():
    if np.random.random() < args.EXPLORATION:
        action = env.action_space.sample()
    else:
        s = np.array([observation])
        q = model.predict(s, batch_size=1)
        argMax = np.argmax(q[0])
        action = getActionFromNum(argMax)
    return action


def getIndexes(prestates):
    for k in range(args.TRAIN_REPEAT):
        if len(prestates) > args.BATCH_SIZE:
            indexes = np.random.choice(len(prestates), size=args.BATCH_SIZE)
        else:
            indexes = range(len(prestates))
    return indexes

def postProcces(indexes,prestates,terminals):
    #########prediction per repitation################
    qpre = model.predict(np.array(prestates)[indexes])
    qpost = model.predict(np.array(poststates)[indexes])
    #print(qpre)
    for i in range(len(indexes)):
        actionIndex=getIndexFromAction(actions[indexes[i]])
        if terminals[indexes[i]]:
            qpre[i, actionIndex] = rewards[indexes[i]]
        else:
            qpre[i, actionIndex] = rewards[indexes[i]] + args.GANMMA * np.amax(qpost[i])

    ##########Single gradient update over one batch of samples.###############
    #print (qpre)
    loss=model.train_on_batch(np.array(prestates)[indexes], qpre)
    print (loss)


for i_episode in range(20):
    observation = [0,0,0]
    env.reset()  ######reset per repetition
    #observation = np.array([0, 0, 0])

    episode_reward = 0

    for t in range(200):
        #if args.DISPLY:
        env.render()
        action=getAction()

        prestates.append(observation)
        actions.append(action)

        print ("action:", action)

        #while True :
        #    alive_flag = False
        #    for p in processes :
        #        if p.is_alive() :
        #            alive_flag = True
        #        break
        #        if alive_flag :
        #            time.sleep(0.1)
        #        continue
        #break

        ###########NextAction and getInfo##########################
        ###########NextAction and getInfo##########################
        observation, reward, done, info = env.step(action)
        print("observation",observation)
        print("reward",reward)
        print("done",done)
        print("info",info)

        episode_reward += reward
        #print ("reward:" ,reward)

        ###########add elements#############
        rewards.append(reward)
        poststates.append(observation)
        terminals.append(done)

        ############presets  >>>>>> env.reset()#############
        if len(prestates) > args.MIN_TRAIN:
            indexes=getIndexes(prestates)
            postProcces(indexes,prestates,terminals)
