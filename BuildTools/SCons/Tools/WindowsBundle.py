import SCons.Util, os

def generate(env) :
  def createWindowsBundle(env, bundle, resources = [], qtlibs = []) :
    env.Install(bundle, bundle + ".exe")
    for lib in qtlibs :
      env.Install(bundle, os.path.join(env["QTDIR"], "bin", lib + ".dll"))
    for resource in resources :
      env.Install(bundle, resource)

  env.AddMethod(createWindowsBundle, "WindowsBundle")

def exists(env) :
  return env["PLATFORM"] == "win32"

