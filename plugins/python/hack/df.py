import _dfhack

class DFObject(object):
    _address = 0
    def __repr__(self):
        return '<%s: 0x%x>' % (self.__class__.__name__, self._address)
    __str__ = __repr__

for type_id, type_name in _dfhack.all_type_ids().items():
    print(type_id, type_name)
    globals()[type_name] = type(type_name, (DFObject,), {'_id': type_id})

def reinterpret_cast(obj_type, address):
    if not issubclass(obj_type, DFObject):
        raise TypeError("Not a DFObject subtype")
    obj = obj_type()
    obj._address = address
    return obj
