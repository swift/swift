import SCons.Util, os

def generate(env) :
  def registerTest(env, target, type = "unit", is_checker = False) :
    if env["TEST_TYPE"] == "all" or env["TEST_TYPE"] == type :
      cmd = target[0].abspath if SCons.Util.is_List(target) else target.abspath
      params = ""

      # Special support for unittest checker
      if is_checker and env.get("checker_report", False) :
        params = " --xml > " + os.path.join(target[0].dir.path, "checker-report.xml")

      env.Command("**dummy**", target, 
          SCons.Action.Action(env.get("TEST_RUNNER", "") + cmd + " " + params, cmdstr = "$TESTCOMSTR"))

  env.AddMethod(registerTest, "Test")

def exists(env) :
  return True
