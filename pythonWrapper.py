from ctypes import *
import code

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


    def CreateType(self,t_name, shape, index, flag):
        return self._CreateType(c_char_p(t_name), shape, index, flag)

    def StoreObjectPoint(self, t_name, sub_name, lat, lon, obj):
        return self._StoreObjectPoint(c_char_p(t_name), c_char_p(sub_name), lat, lon, c_char_p(obj))

    def QueryObjectRange(self, t_name, latmin, lonmin, latmax, lonmax):
        result = POINTER(c_char)()
        count = c_int(1)
        ret = self._QueryObjectRange(c_char_p(t_name), c_double(latmin), c_double(lonmin), c_double(latmax), c_double(lonmax), byref(result), byref(count))
        if ret >= 0:
            return (cast(result,c_char_p).value).split()
        else:
            return []
          

    def LoadObject(self, t_name, sub_name):
        result = POINTER(c_char)()
        ret = self._LoadObject(c_char_p(t_name), c_char_p(sub_name),byref(result))
        if ret >= 0:
            return (cast(result,c_char_p).value).split()
        else:
            return "NULL"
        


mPotato = Potato("/home/songtao/Mapmaking/Fluffy-Potato/pythonWrapper.so")
mPotato.CreateType("type1",1,1,0)
r = mPotato.QueryObjectRange("XXX",0,0,10,10)
print(r)

code.interact(local=locals())
