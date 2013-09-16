# Sluift

## Building/Installing

If you have a Lua installed on your system, edit `config.py` to point the build
to it using the `lua_libdir`, `lua_includedir`, and `lua_libname` (usually 
only needed on Windows) variables.

In case no Lua installation is found, a bundled Lua version will be used.
Note that there are several limitations when using the bundled Lua:

- The standalone Sluift binary will not support dynamic loading of modules.
- The Sluift module will not be built

The standalone executable will be placed into `Sluift/exe`, and the loadable
Lua module in `Sluift/dll`.

In order to use the Sluift Lua module, make sure it is copied to a location in the
Lua search path (`LUA_PATH`, or the built-in path). On Linux and Mac OS X, this is
typically `<LUA_INSTALLPREFIX>/lib/lua/<LUA_VERSION>/`. If `require 'sluift'` fails,
the error message will give an indication what this path is.
