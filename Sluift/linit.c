#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "sluift.h"

// A built-in table print function
// From: http://lua-users.org/wiki/TableSerialization
static const char tprint[] = 
	"function tprint (tt, indent, done)\n"
	"  done = done or {}\n"
	"  indent = indent or 0\n"
	"  if type(tt) == \"table\" then\n"
	"    for key, value in pairs (tt) do\n"
	"      io.write(string.rep (\" \", indent)) -- indent it\n"
	"      if type (value) == \"table\" and not done [value] then\n"
	"        done [value] = true\n"
	"        io.write(string.format(\"[%s] => table\\n\", tostring (key)));\n"
	"        io.write(string.rep (\" \", indent+4)) -- indent it\n"
	"        io.write(\"(\\n\");\n"
	"        tprint (value, indent + 7, done)\n"
	"        io.write(string.rep (\" \", indent+4)) -- indent it\n"
	"        io.write(\")\\n\");\n"
	"      else\n"
	"        io.write(string.format(\"[%s] => %s\\n\",\n"
	"            tostring (key), tostring(value)))\n"
	"      end\n"
	"    end\n"
	"  elseif type(tt) == \"nil\" then\n"
	"    io.write(\"nil\\n\")\n"
	"  else\n"
	"    io.write(tt .. \"\\n\")\n"
	"  end\n"
	"end\n";

static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_IOLIBNAME, luaopen_io},
  {LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_DBLIBNAME, luaopen_debug},
  {"sluift", luaopen_sluift},
  {NULL, NULL}
};


LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
	if (luaL_dostring(L, tprint) != 0) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);	
	}
}
