from ctypes import *
import code
import cPickle as pickle


SHAPE_NULL  = 0
SHAPE_POINT = 1
SHAPE_RECT  = 2
SHAPE_GIRD  = 3

INDEX_NULL  = 0
INDEX_LIST  = 1
INDEX_QTREE = 2
INDEX_RTREE = 3

OBJ_READONLY   = 1
OBJ_ATOM_FETCH = 8




class Potato(object):
    def __init__(self,soname):
        self.lib = cdll.LoadLibrary(soname)
        
        self._CreateType = self.lib.CreateType
        self._CreateType.restype = c_int
        self._CreateType.argtypes = [c_char_p, c_int, c_int, c_int]

        self._StoreObjectPoint = self.lib.StoreObjectPoint
        self._StoreObjectPoint.restype = c_int
        self._StoreObjectPoint.argtypes = [c_char_p, c_char_p, c_double, c_double, c_char_p]

        self._QueryObjectRange = self.lib.QueryObjectRange
        self._QueryObjectRange.restype = c_int
        self._QueryObjectRange.argtypes = [c_char_p, c_double, c_double, c_double, c_double, POINTER(POINTER(c_char)),POINTER(c_int)] 

        self._LoadObject = self.lib.LoadObject
        self._LoadObject.restype = c_int
        self._LoadObject.argtypes = [c_char_p, c_char_p, POINTER(POINTER(c_char))]



    def CreateType(self,t_name, shape = SHAPE_POINT, index = INDEX_LIST, flag = OBJ_READONLY):
        return self._CreateType(c_char_p(t_name), shape, index, flag)

    def StoreObject(self, t_name, obj, sub_name = None, shape = SHAPE_POINT, shape_data = [0,0,0,0]):
        string_obj = pickle.dumps(obj)
        if len(string_obj) > 16000 :
            print("Error, object size exceeds 16,000! We cannot support so far.\n")
            return 0

        if sub_name is None:
            name = t_name.split('@')
            t_name = name[0]
            sub_name = name[1]

        #ONLY POINT NOW
        return self._StoreObjectPoint(c_char_p(t_name), c_char_p(sub_name), shape_data[0], shape_data[1], c_char_p(string_obj))



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
        


mPotato = Potato("/var/nfs/Fluffy-Potato/pythonWrapper.so")
mPotato.CreateType("type1")
#A = [1,2,3]
#mPotato.StoreObject("XXX@k",A,shape_data=[1,1])
#r = mPotato.QueryObjectRect("XXX",[0,0,100,100])
#print(r)

code.interact(local=locals())
