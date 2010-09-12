import SCons.Util, os.path, os

def generate(env) :
  def createSLOCCount(env, source) :
    myenv = env.Clone()
    myenv["ENV"]["HOME"] = os.environ["HOME"]
    source = myenv.Dir(source)
    target = myenv.File("#/" + source.path + ".sloccount")
    # FIXME: There's probably a better way to force building the .sc
    if os.path.exists(target.abspath) :
      os.unlink(target.abspath)
    return myenv.Command(target, source, [SCons.Action.Action("sloccount --duplicates --wide --details " + source.path + " | grep -v qrc_ > $TARGET", cmdstr = "$GENCOMSTR")])

  env.AddMethod(createSLOCCount, "SLOCCount")

def exists(env) :
  return True
