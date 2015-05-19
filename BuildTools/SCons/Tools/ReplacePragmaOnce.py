import SCons.Util, os.path

def generate(env) :
	root = env.Dir("#").abspath
	def relpath(path, start) :
		i = len(os.path.commonprefix([path, start]))
		return path[i+1:]

	def replacePragmaOnce(env, target, source) :
		guard = relpath(source[0].abspath, root).replace("/", "_").replace(".", "_").upper()
		data = source[0].get_contents()
		f = open(str(target[0]), 'wb')
		if "#pragma once" in data :
			f.write(data.replace("#pragma once", "#ifndef %(guard)s\n#define %(guard)s" % {"guard": guard}))
			f.write("\n#endif\n")
		else :
			f.write(data)
		f.close()

	env["BUILDERS"]["ReplacePragmaOnce"] = SCons.Builder.Builder(
		action = SCons.Action.Action(replacePragmaOnce, cmdstr = "$GENCOMSTR"),
		single_source = True)

def exists(env) :
	return True
