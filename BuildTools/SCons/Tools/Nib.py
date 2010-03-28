import SCons.Util

def generate(env) :
  env["IBTOOL"] = "ibtool"
  env["BUILDERS"]["Nib"] = SCons.Builder.Builder(
      action = SCons.Action.Action("$IBTOOL --errors --warnings --notices --output-format human-readable-text --compile $TARGET $SOURCE", cmdstr = "$NIBCOMSTR"),
      suffix = ".nib",
      src_suffix = ".xib",
      single_source = True)

def exists(env) :
  return env["PLATFORM"] == "darwin"
