from ctypes import *
import code
import cPickle as pickle
import socket

SHAPE_NULL  = 0
SHAPE_POINT = 1
SHAPE_RECT  = 2
SHAPE_GIRD  = 3
SHAPE_ARRAY = 4

INDEX_NULL  = 0
INDEX_LIST  = 1
INDEX_RTREE = 2
INDEX_QTREE = 3

OBJ_READONLY   = 1
OBJ_ATOM_FETCH = 8

class Potato(object):

    def __init__(self,soname):
        self.lib = cdll.LoadLibrary(soname)
        
        self._CreateType = self.lib.CreateType
        self._CreateType.restype = c_int
        self._CreateType.argtypes = [c_char_p, c_int, c_int, c_int]

        self._StoreObject = self.lib.StoreObject
        self._StoreObject.restype = c_int
        self._StoreObject.argtypes = [c_char_p, c_char_p, c_double, c_double, c_double, c_double, c_char_p]

        self._StoreArray = self.lib.StoreArray
        self._StoreArray.restype = c_int
        self._StoreArray.argtypes = [c_char_p, c_char_p, c_int, c_int, c_int, c_int, c_int, c_int, c_void_p, c_int]

        self._QueryObjectRange = self.lib.QueryObjectRange
        self._QueryObjectRange.restype = c_int
        self._QueryObjectRange.argtypes = [c_char_p, c_double, c_double, c_double, c_double, POINTER(POINTER(c_char)),POINTER(c_int)] 

        self._LoadObject = self.lib.LoadObject
        self._LoadObject.restype = c_int
        self._LoadObject.argtypes = [c_char_p, c_char_p, POINTER(POINTER(c_char))]

        self.ip = []
        self.port = []
        with open("Cluster.cfg") as f:
            content = f.readlines()
            for line in content:
                if len(line.split())>1: 
                    self.ip.append(line.split()[1])
                    self.port.append(int(line.split()[2])+1)

    def CreateType(self,t_name, shape = SHAPE_POINT, index = INDEX_LIST, flag = OBJ_READONLY):
        return self._CreateType(c_char_p(t_name), shape, index, flag)

    def StoreObject(self, t_name, obj, sub_name = None, shape = SHAPE_POINT, shape_data = [0,0,0,0]):
        string_obj = pickle.dumps(obj)
        if len(string_obj) > 16000 :
            print("Error, object size exceeds 16,000! We cannot support it so far.\n")
            return 0

        if sub_name is None:
            name = t_name.split('@')
            t_name = name[0]
            sub_name = name[1]

        shape = [0, 0, 0, 0]
        if len(shape_data) == 4:
            shape = shape_data
        if len(shape_data) == 2:
            shape[0] = shape_data[0]
            shape[1] = shape_data[1]
            shape[2] = shape_data[0]
            shape[3] = shape_data[1]

		
        #ONLY POINT NOW
        return self._StoreObject(c_char_p(t_name), c_char_p(sub_name), shape[0], shape[1], shape[2], shape[3], c_char_p(string_obj))

    def StoreArray(self, t_name, data, dim = [0,0,0,0], op = 1,  sub_name = None):
        if sub_name is None:
            name = t_name.split('@')
            t_name = name[0]
            sub_name = name[1]
        

        return self._StoreArray(c_char_p(t_name), c_char_p(sub_name),c_int(dim[0]), c_int(dim[1]), c_int(dim[2]), c_int(dim[3]), c_int(op), c_int(op), data.ravel().ctypes, c_int(dim[2]*dim[3]))






    def QueryObjectRect(self, t_name, rect):
        result = POINTER(c_char)()
        count = c_int(1)
        ret = self._QueryObjectRange(c_char_p(t_name), c_double(rect[0]), c_double(rect[1]), c_double(rect[2]), c_double(rect[3]), byref(result), byref(count))
        if ret > 0:
            return (cast(result,c_char_p).value).split()
        else:
            return None
          

    def LoadObject(self, t_name, sub_name = None):
        if sub_name is None:
            name = t_name.split('@')
            t_name = name[0]
            sub_name = name[1]
        
        result = POINTER(c_char)()
        ret = self._LoadObject(c_char_p(t_name), c_char_p(sub_name),byref(result))
        if ret >= 0:
            return pickle.loads((cast(result,c_char_p).value).strip())
        else:
            return None

    def Schedule(self,cmd,state,node):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print self.ip[node], self.port[node]
        s.connect((self.ip[node],self.port[node]))
        msg = cmd + " " + state
        s.send(msg)
        s.close()


#mPotato = Potato("/var/nfs/Fluffy-Potato/pythonWrapper.so")
#mPotato.CreateType("type1")

#A = [1,2,3]
#mPotato.StoreObject("XXX@k",A,shape_data=[1,1])
#r = mPotato.QueryObjectRect("XXX",[0,0,100,100])
#print(r)

#code.interact(local=locals())
