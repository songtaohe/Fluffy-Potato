from MyPythonWrapper import Potato
import code

class State(object):
    myid = 2
    node = 0
    x = 1
    y = 1


#mPotato = Potato("/var/nfs/Fluffy-Potato/pythonWrapper.so")
mPotato = Potato("/home/songtao/Mapmaking/Fluffy-Potato/pythonWrapper.so")

state = State();

mPotato.CreateType("potatoA")

mPotato.StoreObject("potatoA@2", state, shape_data = [1,1])

mPotato.Schedule("HotPotato.py","potatoA@2",0)


code.interact(local=locals())



