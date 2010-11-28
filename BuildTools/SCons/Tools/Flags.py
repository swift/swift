import SCons.Util

def generate(env) :
  def useFlags(env, flags) :
    for flag in flags :
      env[flag] = env.get(flag, []) + flags[flag]
  env.AddMethod(useFlags, "UseFlags")

def exists(env) :
  return True
