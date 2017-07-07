import _dfhack

_type_ids = _dfhack.all_type_ids()

class _DFObjectMeta(type):
    def __instancecheck__(self, instance):
        return self._id == instance._id

    def __subclasscheck__(self, subclass):
        return self._id == subclass._id

class DFObject:
    __metaclass__ = _DFObjectMeta
    _id = 0
    _address = 0

    def __repr__(self):
        return '<%s: 0x%x>' % (_dfhack.type_name(self._id), self._address)
    __str__ = __repr__

for type_id, type_name in _type_ids.items():
    globals()[type_name] = _DFObjectMeta(type_name, (DFObject,), {'_id': type_id})

def reinterpret_cast(obj_type, address):
    if not issubclass(obj_type, DFObject):
        raise TypeError("Not a DFObject subtype")
    obj = obj_type()
    obj._address = address
    return obj
