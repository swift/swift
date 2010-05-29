import subprocess, os, datetime

def getGitBuildVersion(project) :
  headVersion = git("rev-parse HEAD")
  if headVersion :
    tags = git("tag --contains HEAD -l " + project + "-*")
    if len(tags) > 0 :
      for tag in tags.split("\n") :
        tagVersion = git("rev-parse " + tag + "^{commit}")
        if tagVersion == headVersion :
          return tag[len(project)+1:]
  return None

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
