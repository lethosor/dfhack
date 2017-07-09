import _dfhack

_type_ids = _dfhack.all_type_ids()

class _DFObjectMeta(type):
    def __instancecheck__(self, instance):
        return hasattr(instance, '_id') and _dfhack.type_is_subclass(self._id, instance._id)

    # identical implementations - this takes "subclass" instead of "instance",
    # but both have "_id" fields
    __subclasscheck__ = __instancecheck__

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
    if isinstance(obj_type, int):
        if obj_type in _type_ids:
            obj_type = globals()[_type_ids[obj_type]]
        else:
            raise TypeError("Invalid type ID: %i" % obj_type)
    if not issubclass(obj_type, DFObject):
        raise TypeError("Not a DFObject subtype")
    obj = obj_type()
    obj._address = address
    return obj
