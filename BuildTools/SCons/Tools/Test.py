import SCons.Util, os

def generate(env) :
    def registerTest(env, target, type = "unit", is_checker = False) :
        if env["TEST_TYPE"] == "all" or env["TEST_TYPE"] == type :
            if SCons.Util.is_List(target) :
                cmd = target[0].abspath
            else :
                cmd = target.abspath
            params = ""

            # Special support for unittest checker
            if is_checker and env.get("checker_report", False) :
                params = " --xml"

            ignore_prefix = ""
            if env.get("TEST_IGNORE_RESULT", False) :
                ignore_prefix = "-"

            # Set environment variables for running the test
            test_env = env.Clone()
            for i in ["HOME", "PATH", "USERPROFILE", "APPDATA", "GTEST_FILTER", "ASAN_OPTIONS", "LSAN_OPTIONS", "SWIFT_NETWORK_TEST_IPV4", "SWIFT_NETWORK_TEST_IPV6"]:
                if os.environ.get(i, "") :
                    test_env["ENV"][i] = os.environ[i]
            if env["target"] == "android" :
                test_env["ENV"]["PATH"] = env["android_sdk_bin"] + ";" + test_env["ENV"]["PATH"]
            else :
                if test_env["PLATFORM"] == "darwin" :
                    test_env["ENV"]["DYLD_FALLBACK_LIBRARY_PATH"] = ":".join(map(lambda x : str(x), test_env.get("LIBPATH", [])))
                elif test_env["PLATFORM"] == "win32" :
                    test_env["ENV"]["PATH"] = ";".join(map(lambda x : str(x), test_env.get("LIBRUNPATH", []))) + ";" + test_env["ENV"]["PATH"]


            # Run the test
            if env["target"] == "android":
                exec_name = os.path.basename(cmd)
                test_env.Command("**dummy**", target, SCons.Action.Action(
                    ["adb shell mount -o rw,remount /system",
                    "adb push " + cmd + " /system/bin/" + exec_name,
                    "adb shell SWIFT_CLIENTTEST_JID=\"" + os.getenv("SWIFT_CLIENTTEST_JID") + "\" SWIFT_CLIENTTEST_PASS=\"" + os.getenv("SWIFT_CLIENTTEST_PASS") + "\" " + env.get("TEST_RUNNER", "") + "/system/bin/" + exec_name], cmdstr = "$TESTCOMSTR"))
            else :
                test_env.Command("**dummy**", target,
                    SCons.Action.Action(ignore_prefix + env.get("TEST_RUNNER", "") + cmd + " " + params, cmdstr = "$TESTCOMSTR"))

    def registerScriptTests(env, scripts, name, type) :
        if env["TEST_TYPE"] == "all" or env["TEST_TYPE"] == type :
            pass

    env.AddMethod(registerTest, "Test")
    env.AddMethod(registerScriptTests, "ScriptTests")

def exists(env) :
    return True
