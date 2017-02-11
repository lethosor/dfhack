#include "df/item_trapcompst.h"
#include "df/item_trappartsst.h"
#include "df/item_weaponst.h"

struct trap_wear_hook_trapcomp : df::item_trapcompst {
    typedef df::item_trapcompst interpose_base;
    DEFINE_VMETHOD_INTERPOSE(bool, checkWearDestroy, (bool simple, bool lose_masterwork))
    {
        bool res = INTERPOSE_NEXT(checkWearDestroy)(simple, lose_masterwork);
        Core::printerr("item_trapcompst::checkWearDestroy: %i\n", int(res));
        return false;
    }
};

struct trap_wear_hook_trappart : df::item_trappartsst {
    typedef df::item_trappartsst interpose_base;
    DEFINE_VMETHOD_INTERPOSE(bool, checkWearDestroy, (bool simple, bool lose_masterwork))
    {
        bool res = INTERPOSE_NEXT(checkWearDestroy)(simple, lose_masterwork);
        Core::printerr("item_trappartsst::checkWearDestroy: %i\n", int(res));
        return false;
    }
};


struct trap_wear_hook_weapon : df::item_weaponst {
    typedef df::item_weaponst interpose_base;
    DEFINE_VMETHOD_INTERPOSE(bool, checkWearDestroy, (bool simple, bool lose_masterwork))
    {
        bool res = INTERPOSE_NEXT(checkWearDestroy)(simple, lose_masterwork);
        Core::printerr("item_weaponst::checkWearDestroy: %i\n", int(res));
        return false;
    }
};


IMPLEMENT_VMETHOD_INTERPOSE(trap_wear_hook_trapcomp, checkWearDestroy);
IMPLEMENT_VMETHOD_INTERPOSE(trap_wear_hook_trappart, checkWearDestroy);
IMPLEMENT_VMETHOD_INTERPOSE(trap_wear_hook_weapon, checkWearDestroy);
