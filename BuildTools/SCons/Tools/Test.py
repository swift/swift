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

			# Set environment variables for running the test
			test_env = env.Clone()
			for i in ["HOME", "USERPROFILE", "APPDATA"]:
				if os.environ.get(i, "") :
					test_env["ENV"][i] = os.environ[i]
			if test_env["PLATFORM"] == "darwin" :
				test_env["ENV"]["DYLD_FALLBACK_LIBRARY_PATH"] = ":".join(map(lambda x : str(x), test_env.get("LIBPATH", [])))
			elif test_env["PLATFORM"] == "win32" :
				test_env["ENV"]["PATH"] = ";".join(map(lambda x : str(x), test_env.get("LIBRUNPATH", []))) + ";" + test_env["ENV"]["PATH"]

			# Run the test
			test_env.Command("**dummy**", target, 
					SCons.Action.Action(ignore_prefix + env.get("TEST_RUNNER", "") + cmd + " " + params, cmdstr = "$TESTCOMSTR"))

	def registerScriptTests(env, scripts, name, type) :
		if env["TEST_TYPE"] == "all" or env["TEST_TYPE"] == type :
			pass

	env.AddMethod(registerTest, "Test")
	env.AddMethod(registerScriptTests, "ScriptTests")

def exists(env) :
	return True
