import SCons.Util, os

def generate(env) :
  def modify_targets(target, source, env) :
    target = [env.File("html/index.html")]
    return target, source

  def generate_actions(source, target, env, for_signature) :
    if env.WhereIs("$DOXYGEN") and env.WhereIs("$DOT") :
      return [SCons.Action.Action("$DOXYGEN $SOURCE", cmdstr = "$DOXYCOMSTR")]
    else :
      return []

  env["DOXYGEN"] = "doxygen"
  # FIXME: For some reason, things go incredibly slow (at least on OS X)
  # when not doing this. Some environment flag is having an effect on
  # this; find out which
  env["ENV"] = os.environ
  env["DOT"] = "dot"
  env["BUILDERS"]["DoxyGen"] = SCons.Builder.Builder(
    emitter = modify_targets,
    generator = generate_actions,
    single_source = True)

def exists(env) :
  return True
