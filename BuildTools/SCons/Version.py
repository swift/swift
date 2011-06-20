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
