import subprocess, os

def getGitBuildVersion() :
  p = subprocess.Popen("git rev-parse HEAD", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=(os.name != "nt"))
  return p.stdout.read().rstrip()[0:7]

def getBuildVersion(version = None) :
  if version :
    return version
  return getGitBuildVersion() 
