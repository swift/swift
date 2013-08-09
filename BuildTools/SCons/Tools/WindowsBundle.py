import SCons.Util, os

def generate(env) :
  def createWindowsBundle(env, bundle, resources = {}, qtplugins = {}, qtlibs = [], qtversion = '4') :
    all_files = []
    all_files += env.Install(bundle, bundle + ".exe")
    for lib in qtlibs :
      all_files += env.Install(bundle, os.path.join(env["QTDIR"], "bin", lib + ".dll"))
    plugins_suffix = '4'
    if qtversion == '5' :
      plugins_suffix = ''
    for plugin_type in qtplugins:
      all_files += env.Install(os.path.join(bundle, plugin_type), [os.path.join(env["QTDIR"], "plugins", plugin_type, "q" + plugin + plugins_suffix + ".dll") for plugin in qtplugins[plugin_type]])

    for dir, resourceFiles in resources.items() :
      for resource in resourceFiles :
	e = env.Entry(resource)
	if e.isdir() :
	  for subresource in env.Glob(str(e) + "/*") :
	    all_files += env.Install(os.path.join(bundle, dir, e.name), subresource)
	else :
	  all_files += env.Install(os.path.join(bundle, dir), resource)
    return all_files

  env.AddMethod(createWindowsBundle, "WindowsBundle")

def exists(env) :
  return env["PLATFORM"] == "win32"

