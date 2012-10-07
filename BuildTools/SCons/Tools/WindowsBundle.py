import SCons.Util, os

def generate(env) :
  def createWindowsBundle(env, bundle, resources = {}, qtimageformats = [], qtlibs = []) :
    all_files = []
    all_files += env.Install(bundle, bundle + ".exe")
    for lib in qtlibs :
      all_files += env.Install(bundle, os.path.join(env["QTDIR"], "bin", lib + ".dll"))
    all_files += env.Install(os.path.join(bundle, "imageformats"), [os.path.join(env["QTDIR"], "plugins", "imageformats", "q" + codec + "4.dll") for codec in qtimageformats])

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

