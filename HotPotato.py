from MyPythonWrapper import Potato
import sys

class State(object):
    myid = 2
    node = 0
    x = 1
    y = 1


#mPotato = Potato("/var/nfs/Fluffy-Potato/pythonWrapper.so")
mPotato = Potato("/home/songtao/Mapmaking/Fluffy-Potato/pythonWrapper.so")

print(str(sys.argv[1]))

state = mPotato.LoadObject(str(sys.argv[1]))

state.myid = state.myid + 1
state.node = state.node + 1
#if state.node > 3 : 
state.node = 0

state.x = state.x + 1
state.y = state.y + 1

if(state.myid > 1000):
    quit()

mPotato.StoreObject("potatoA@"+str(state.myid),state,shape_data = [state.x,state.y])

mPotato.Schedule("HotPotato.py","potatoA@"+str(state.myid),state.node)




