import SCons.Util, os

def generate(env) :
  def createWindowsBundle(env, bundle, resources = [], qtimageformats = [], qtlibs = []) :
    env.Install(bundle, bundle + ".exe")
    for lib in qtlibs :
      env.Install(bundle, os.path.join(env["QTDIR"], "bin", lib + ".dll"))
    env.Install(os.path.join(bundle, "imageformats"), [os.path.join(env["QTDIR"], "plugins", "imageformats", "q" + codec + "4.dll") for codec in qtimageformats])

    for resource in resources :
      e = env.Entry(resource)
      if e.isdir() :
	for subresource in env.Glob(str(e) + "/*") :
      	  env.Install(os.path.join(bundle, e.name), subresource)
      else :
      	env.Install(bundle, resource)

  env.AddMethod(createWindowsBundle, "WindowsBundle")

def exists(env) :
  return env["PLATFORM"] == "win32"

