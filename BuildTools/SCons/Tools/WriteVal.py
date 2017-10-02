import SCons.Util

def generate(env) :
    def replacePragmaOnce(env, target, source) :
        f = open(str(target[0]), 'wb')
        f.write(source[0].get_contents())
        f.close()

    env["BUILDERS"]["WriteVal"] = SCons.Builder.Builder(
        action = SCons.Action.Action(replacePragmaOnce, cmdstr = "$GENCOMSTR"),
        single_source = True)

def exists(env) :
    return True

