import subprocess, os, datetime, re, os.path

def getGitBuildVersion(project) :
  tag = git("describe --tags --exact --match \"" + project + "-*\"")
  if tag :
    return tag.rstrip()[len(project)+1:]
  tag = git("describe --tags --match \"" + project + "-*\"")
  if tag :
    m = re.match(project + "-(.*)-(.*)-(.*)", tag)
    if m :
      return m.group(1) + "-dev" + m.group(2)
  return None
    
def git(cmd) :
  p = subprocess.Popen("git " + cmd, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=(os.name != "nt"))
  gitVersion = p.stdout.read()
  p.stdin.close()
  return gitVersion if p.wait() == 0 else None

def getBuildVersion(root, project) :
  versionFilename = os.path.join(root, "VERSION." + project)
  if os.path.isfile(versionFilename) :
    f = open(versionFilename)
    version = f.read().strip()
    f.close()
    return version

  gitVersion = getGitBuildVersion(project) 
  if gitVersion :
    return gitVersion

  return datetime.date.today().strftime("%Y%m%d")

def convertToWindowsVersion(version) :
  version_match = re.match("(\d+)\.(\d+)(.*)", version)
  major = int(version_match.group(1)) if version_match else 0
  minor = int(version_match.group(2)) if version_match else 0
  if version_match and len(version_match.group(3)) == 0 :
    patch = 99999
  else :
    match = re.match("^beta(\d+)(.*)", version_match.group(3))
    build_string = ""
    if match :
      patch = 1000*int(match.group(1))
      build_string = match.group(2)
    else :
      rc_match = re.match("^rc(\d+)(.*)", version_match.group(3))
      if rc_match :
        patch = 10000*int(rc_match.group(1))
        build_string = rc_match.group(2)
      else :
        patch = 0
        alpha_match = re.match("^alpha(.*)", version_match.group(3))
        if alpha_match :
          build_string = alpha_match.group(1)

    if len(build_string) > 0 :
      build_match = re.match("^-dev(\d+)", build_string) 
      if build_match :
        patch += int(build_match.group(1))

    return (major, minor, patch)

