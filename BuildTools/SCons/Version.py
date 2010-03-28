import subprocess, os, datetime

def getGitBuildVersion() :
  p = subprocess.Popen("git rev-parse HEAD", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=(os.name != "nt"))
  gitVersion = p.stdout.read().rstrip()[0:7]
  p.stdin.close()
  return gitVersion if p.wait() == 0 else None

def getBuildVersion(version = None) :
  if version :
    return version
  gitVersion = getGitBuildVersion() 
  if gitVersion :
    return gitVersion
  return datetime.date.today().strftime("%Y%m%d")
