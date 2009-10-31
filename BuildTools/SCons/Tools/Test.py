import SCons.Util

def generate(env) :
  def registerTest(env, target, type = "unit") :
    if env["TEST_TYPE"] == "all" or env["TEST_TYPE"] == type :
      cmd = target[0].abspath if SCons.Util.is_List(target) else target.abspath
      env.Command("**dummy**", target, 
          SCons.Action.Action(env.get("TEST_RUNNER", "") + cmd, cmdstr = "$TESTCOMSTR"))

  env.AddMethod(registerTest, "Test")

def exists(env) :
  return True
