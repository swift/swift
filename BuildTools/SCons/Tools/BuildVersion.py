import SCons.Util

import Version

def generate(env) :
  def createBuildVersion(env, target, project) :
    buildVersion = """#pragma once

static const char* buildVersion = \"%(buildVersion)s\";\n
""" % { "buildVersion" : Version.getBuildVersion(project) }
    env.WriteVal(target, env.Value(buildVersion))

  env.AddMethod(createBuildVersion, "BuildVersion")


def exists(env) :
  return true
