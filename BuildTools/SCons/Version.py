import subprocess, os, datetime, re

def getGitBuildVersion(project) :
  tag = git("describe --tags --exact --match \"" + project + "-*\"")
  if tag :
    return tag.rstrip()[len(project)+1:]
  tag = git("describe --tags --match \"" + project + "-*\"")
  if tag :
    m = re.match(project + "-(.*)-(.*)-(.*)", tag)
    if m :
      return m.group(1) + "-dev" + m.group(2)
  log = git("log --oneline")
  if log :
    return project + "-0.1-dev" + str(len(log.split("\n")))
  else :
    return project + "-0.0-dev"
    
def git(cmd) :
  p = subprocess.Popen("git " + cmd, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=(os.name != "nt"))
  gitVersion = p.stdout.read()
  p.stdin.close()
  return gitVersion if p.wait() == 0 else None

def getBuildVersion(project) :
  gitVersion = getGitBuildVersion(project) 
  if gitVersion :
    return gitVersion
  # TODO: Add the current branch
  # TODO: Pick up a version number from a file (for distributing)
  return datetime.date.today().strftime("%Y%m%d")
