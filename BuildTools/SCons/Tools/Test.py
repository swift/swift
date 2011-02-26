import SCons.Util, os

def generate(env) :
  def registerTest(env, target, type = "unit", is_checker = False) :
    if env["TEST_TYPE"] == "all" or env["TEST_TYPE"] == type :
      cmd = target[0].abspath if SCons.Util.is_List(target) else target.abspath
      params = ""

      # Special support for unittest checker
      if is_checker and env.get("checker_report", False) :
        params = " --xml > " + os.path.join(target[0].dir.path, "checker-report.xml")
	
      ignore_prefix = ""
      if env.get("TEST_IGNORE_RESULT", False) :
        ignore_prefix = "-"
      env.Command("**dummy**", target, 
          SCons.Action.Action(ignore_prefix + env.get("TEST_RUNNER", "") + cmd + " " + params, cmdstr = "$TESTCOMSTR"))

  def registerScriptTests(env, scripts, name, type) :
    if env["TEST_TYPE"] == "all" or env["TEST_TYPE"] == type :
      pass

  env.AddMethod(registerTest, "Test")
  env.AddMethod(registerScriptTests, "ScriptTests")

def exists(env) :
  return True
