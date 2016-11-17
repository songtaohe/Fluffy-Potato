from MyPythonWrapper import *
import code
import numpy as np
#mPotato = Potato("/var/nfs/Fluffy-Potato/pythonWrapper.so")
P = Potato("/home/songtao/Mapmaking/Fluffy-Potato/pythonWrapper.so")

P.CreateType("A",shape=4, index=INDEX_LIST)
a = np.ones(8, dtype=np.float32)
P.StoreArray("A@test", a,dim = [0,0,2,2], op = 2)

code.interact(local=locals())

