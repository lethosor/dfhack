""" Main DFHack Python module """

import _dfhack
import dfhack
import sys

__all__ = ['print', 'printerr', 'run_script']

def print(*objects, sep=' ', end='\n', color=None, callback=_dfhack.print):
    msg = sep.join(map(str, objects)) + end
    if color is None:
        callback(msg)
    else:
        callback(msg, color)

def printerr(*args, **kwargs):
    kwargs['callback'] = _dfhack.printerr
    print(*args, **kwargs)

class StdioWrapper:
    def __init__(self, file, write_fn):
        self.file = file
        self.write_fn = write_fn

    def write(self, msg):
        self.write_fn(msg)

    def __getattr__(self, attr):
        return getattr(self.file, attr)

sys.stdout = StdioWrapper(sys.stdout, _dfhack.print)
sys.stderr = StdioWrapper(sys.stdout, _dfhack.printerr)

def run_script(path, *args):
    with open(path) as f:
        contents = f.read()

    code = compile(contents, path, 'exec')

    env = {
        'dfhack': dfhack,
        'sys': sys,
        '__file__': path,
        'argv': (path,) + args,
    }

    for k in __all__:
        env[k] = globals()[k]

    exec(code, env)

class LuaFunction:
    def __init__(self, path):
        self.path = path

    def __call__(self, *args):
        return _dfhack.lua_call_func(self.path, *args)

class LuaModule:
    def __init__(self, name):
        self.name = name

    def __getattr__(self, name):
        path = ('dfhack', self.name, name)
        if _dfhack.lua_can_call(path):
            func = LuaFunction(path)
            setattr(self, name, func)
            return func
        else:
            raise AttributeError("%r module has no attribute %r" % (self.name, name))

internal = LuaModule("internal")
world = LuaModule("world")
