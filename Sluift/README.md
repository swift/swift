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


## Examples

Example scripts can be found in `Examples/`, and in the Swift tree under
`Swiften/QA/ScriptedTests`.

## Extending

### Adding support for payloads

To add support for a specific Swiften payload, create a new `LuaElementConvertor`
in `ElementConvertors/` by inheriting from `GenericLuaElementConvertor`, and register
it it `LuaElementConvertors.cpp`. The name of the convertor passed in the constructor
of `GenericLuaElementConvertor` should correspond to the snake case version of the 
Swiften element it converts. For examples, see the existing convertors in
`ElementConvertors/`.

When the convertor is registered, you can use it in message bodies, or in get/set
queries. For example, for the `command` convertor:
  
    client:set{to = 'alice@wonderland.lit', query = {_type = 'command', 
      type = 'execute', node = 'uptime'}}

Optionally, you can add a convenience `set_<type>` shortcut on the client object by
adding it to the list of `get_set_shortcuts` in `boot.lua`. With such a shortcut, the
command above can be rewritten as:

    client:set_command{to = 'alice@wonderland.lit', command = {
      type = 'execute', node = 'uptime'}}

