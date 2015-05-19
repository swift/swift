import SCons.Util

def generate(env) :
	def useFlags(env, flags) :
		for flag in flags :
			if flag in env :
				env[flag] = env[flag] + flags[flag]
			else :
				env[flag] = flags[flag]
	env.AddMethod(useFlags, "UseFlags")

def exists(env) :
	return True
