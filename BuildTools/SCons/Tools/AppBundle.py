import SCons.Util

def generate(env) :
  def createAppBundle(env, bundle, version = "1.0", resources = [], info = {}) :
    bundleContentsDir = bundle + ".app" + "/Contents"
    resourcesDir = bundleContentsDir + "/Resources"
    env.Install(bundleContentsDir + "/MacOS", bundle)
    env.WriteVal(bundleContentsDir + "/PkgInfo", env.Value("APPL\77\77\77\77"))

    infoDict = {
        "CFBundleDevelopmentRegion" : "English",
        "CFBundleExecutable" : bundle,
        "CFBundleIdentifier" : "im.swift." + bundle,
        "CFBundleInfoDictionaryVersion" : "6.0",
        "CFBundleName" : bundle,
        "CFBundlePackageType" : "APPL",
        "CFBundleSignature": "\77\77\77\77",
        "CFBundleVersion" : version,
        "CFBundleIconFile" : bundle,
        "NSPrincipalClass" : "NSApplication",
        "NSHumanReadableCopyright" : unichr(0xA9) + " 2009 Swift Development Team.\nAll Rights Reserved."
      }
    infoDict.update(info)
    
    plist = """<?xml version="1.0" encoding="UTF-8"?>
  <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
  <plist version="1.0">
  <dict>
  """
    for key, value in infoDict.items() :
      plist += "<key>" + key + "</key>\n"
      plist += "<string>" + value.encode("utf-8") + "</string>\n"
    plist += """</dict>
  </plist>
  """
    env.WriteVal(bundleContentsDir + "/Info.plist", env.Value(plist))

    for resource in resources :
      env.Install(resourcesDir, resource)

  env.AddMethod(createAppBundle, "AppBundle")


def exists(env) :
  return env["PLATFORM"] == "darwin"
