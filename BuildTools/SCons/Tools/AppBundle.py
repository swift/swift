import SCons.Util, os.path

def generate(env) :
  def createAppBundle(env, bundle, version = "1.0", resources = [], frameworks = [], info = {}, handlesXMPPURIs = False) :
    bundleDir = bundle + ".app"
    bundleContentsDir = bundleDir + "/Contents"
    resourcesDir = bundleContentsDir + "/Resources"
    frameworksDir = bundleContentsDir + "/Frameworks"
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
        "NSHumanReadableCopyright" : unichr(0xA9) + " 2010 Swift Development Team.\nAll Rights Reserved."
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
    if handlesXMPPURIs :
      plist += """<key>CFBundleURLTypes</key>
<array>
    <dict>
        <key>CFBundleURLName</key>
        <string>XMPP URL</string>
        <key>CFBundleURLSchemes</key>
        <array>
            <string>xmpp</string>
        </array>
    </dict>
</array>\n"""
    plist += """</dict>
  </plist>
  """
    env.WriteVal(bundleContentsDir + "/Info.plist", env.Value(plist))

    for (target, resource) in resources.items() :
      env.Install(os.path.join(resourcesDir, target), resource)

    for framework in frameworks :
      env.Install(frameworksDir, framework)

    return env.Dir(bundleDir)

  env.AddMethod(createAppBundle, "AppBundle")


def exists(env) :
  return env["PLATFORM"] == "darwin"
