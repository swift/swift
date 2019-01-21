from __future__ import print_function
import subprocess, os, datetime, re, os.path, sys, unittest

def gitDescribeToVersion(tag, tagPrefix):
    m = re.match(r'^' + tagPrefix + r'-(.+?)(?:-(.+?)-(.+?))?$', tag)
    if not m:
        return None
    result = m.group(1)
    if m.group(2):
        result += "-dev" + m.group(2)
    return result

def getGitBuildVersion(root, tagPrefix) :
    tag = git("describe --tags --match \"" + tagPrefix + "-*\"", root)
    if tag:
        return gitDescribeToVersion(tag, tagPrefix)
    return None

def git(cmd, root):
    full_cmd = "git " + cmd
    # Would've used with .. as p, but that's not supported by Python 2.7
    p = subprocess.Popen(full_cmd, cwd=root, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=(os.name != "nt"))
    gitVersion = p.stdout.read()
    try:
        p.stdin.close()
        p.stdout.close()
        p.stderr.close()
    except:
        pass
    if p.wait() == 0:
        return bytes(gitVersion).decode('utf-8')
    return None

def getBuildVersion(root, project) :
    versionFilename = os.path.join(root, "VERSION." + project)
    if os.path.isfile(versionFilename) :
        f = open(versionFilename)
        version = f.read().strip()
        f.close()
        return version

    gitVersion = getGitBuildVersion(root, project)
    if gitVersion :
        return gitVersion

    return datetime.date.today().strftime("%Y%m%d")

# The following conversion allows us to use version tags the format:
#   major.0
#   major.0.(0 to 9)
#
#   Either from above followed by:
#   alpha(0 to 4)
#   beta(0 to 6)
#   rc(1 to 11)
#
#   Followed by an optional -dev(1-65535) for off tag builds.
def convertToWindowsVersion(version):
    match = re.match(r"(?P<major>\d+)\.(?P<minor>\d+)\.?(?P<patch>\d+)?(?:(?P<stage>rc|beta|alpha)(?P<stage_number>\d+)?)?(?:-dev(?P<dev>\d+))?", version)
    assert(match)
    major, minor, patch = (0, 0, 0)

    groups = match.groupdict()
    assert(groups['major'])
    major = int(groups['major'])

    if groups['minor']:
        assert(int(groups['minor']) == 0)

    if groups['patch']:
        assert(0 <= int(groups['patch']) <= 9)
        minor = int(groups['patch']) * 25

    stage = groups["stage"]
    if stage:
        stageNumber = groups['stage_number']
        if not stageNumber or stageNumber == "":
            stageNumber = 0
        else:
            stageNumber = int(stageNumber)

        if stage == "alpha":
            assert(0 <= stageNumber <= 4)
            minor = 1 + stageNumber
        elif stage == "beta":
            assert(0 <= stageNumber <= 6)
            minor = 6 + stageNumber
        elif stage == "rc":
            assert(1 <= stageNumber <= 11)
            minor = 12 + stageNumber
        else:
            assert(False)
    else:
        minor = minor + 24

    if groups['dev']:
        patch = 1 + int(groups['dev'])

    # The following constraints are set by Windows Installer framework
    assert(0 <= major <= 255)
    assert(0 <= minor <= 255)
    assert(0 <= patch <= 65535)
    return (major, minor, patch)

# Test Windows version mapping scheme
class TestCases(unittest.TestCase):
    def testWindowsVersionsAreDescending(self):
        versionStringsWithOldVersions = [
            ("5.0rc11", None),
            ("5.0rc1", None),
            ("5.0beta6", None),
            ("5.0alpha4", None),
            ("5.0alpha2", None),
            ("5.0alpha", None),
            ("4.0.9", None),
            ("4.0.1", None),
            ("4.0", (4, 0, 60000)),
            ("4.0rc6", (4, 0, 60000)),
            ("4.0rc5", (4, 0, 50000)),
            ("4.0rc4", (4, 0, 40000)),
            ("4.0rc3", (4, 0, 30000)),
            ('4.0rc2-dev39', (4, 0, 20039)),
            ('4.0rc2-dev34', (4, 0, 20034)),
            ('4.0rc2-dev33', (4, 0, 20033)),
            ('4.0rc2-dev31', (4, 0, 20031)),
            ('4.0rc2-dev30', (4, 0, 20030)),
            ('4.0rc2-dev29', (4, 0, 20029)),
            ('4.0rc2-dev27', (4, 0, 20027)),
            ('4.0rc2', (4, 0, 20000)),
            ('4.0rc1', (4, 0, 10000)),
            ('4.0beta2-dev203', (4, 0, 2203)),
            ('4.0beta2-dev195', (4, 0, 2195)),
            ('4.0beta2-dev177', (4, 0, 2177)),
            ('4.0beta2-dev171', (4, 0, 2171)),
            ('4.0beta2-dev154', (4, 0, 2154)),
            ('4.0beta2-dev150', (4, 0, 2150)),
            ('4.0beta2-dev142', (4, 0, 2142)),
            ('4.0beta2-dev140', (4, 0, 2140)),
            ('4.0beta2-dev133', (4, 0, 2133)),
            ('4.0beta2-dev118', (4, 0, 2118)),
            ('4.0beta2-dev112', (4, 0, 2112)),
            ('4.0beta2-dev93', (4, 0, 2093)),
            ('4.0beta2-dev80', (4, 0, 2080)),
            ('4.0beta2-dev72', (4, 0, 2072)),
            ('4.0beta2-dev57', (4, 0, 2057)),
            ('4.0beta2-dev44', (4, 0, 2044)),
            ('4.0beta2-dev38', (4, 0, 2038)),
            ('4.0beta2-dev29', (4, 0, 2029)),
            ('4.0beta2-dev15', (4, 0, 2015)),
            ('4.0beta2', (4, 0, 2000)),
            ('4.0beta1', (4, 0, 1000)),
            ('4.0alpha-dev80', (4, 0, 80)),
            ('4.0alpha-dev50', (4, 0, 50)),
            ('4.0alpha-dev43', (4, 0, 43)),
            ('4.0alpha-dev21', (4, 0, 21)),
            ('3.0', (3, 0, 60000)),
            ('3.0rc3', (3, 0, 30000)),
            ('3.0rc2', (3, 0, 20000)),
            ('3.0rc1', (3, 0, 10000)),
            ('3.0beta2-dev124', (3, 0, 2124)),
            ('3.0beta2-dev81', (3, 0, 2081)),
            ('3.0beta2-dev50', (3, 0, 2050)),
            ('3.0beta2-dev44', (3, 0, 2044)),
            ('3.0beta2-dev40', (3, 0, 2040)),
            ('3.0beta2-dev26', (3, 0, 2026)),
            ('3.0beta2', (3, 0, 2000)),
            ('3.0beta1', (3, 0, 1000)),
            ('3.0alpha-dev529', (3, 0, 529)),
            ('3.0alpha-dev528', (3, 0, 528)),
            ('3.0alpha-dev526', (3, 0, 526)),
            ('3.0alpha-dev524', (3, 0, 524)),
            ('3.0alpha-dev515', (3, 0, 515)),
        ]
        prevParsed = None
        prevOldVersion = None
        for string, oldVersion in versionStringsWithOldVersions:
            parsed = convertToWindowsVersion(string)
            if prevOldVersion and oldVersion:
                self.assertTrue(oldVersion <= prevOldVersion, "Old version %r must come before %r" % (oldVersion, prevOldVersion))
            if prevParsed:
                self.assertTrue(parsed < prevParsed, "%s => %r must be smaller than %s => %r" % (string, parsed, prevString, prevParsed))
            prevString = string
            prevParsed = parsed
            prevOldVersion = oldVersion

    def testThatBetaIsHigherThanAlpha(self):
        self.assertTrue(convertToWindowsVersion("3.0beta0") > convertToWindowsVersion("3.0alpha0"))
        self.assertTrue(convertToWindowsVersion("3.0beta6") > convertToWindowsVersion("3.0alpha1"))
        self.assertTrue(convertToWindowsVersion("3.0beta6") > convertToWindowsVersion("3.0alpha4"))

    def testThatRcIsHigherThanAlphaAndBeta(self):
        self.assertTrue(convertToWindowsVersion("3.0rc11") > convertToWindowsVersion("3.0alpha0"))
        self.assertTrue(convertToWindowsVersion("3.0rc11") > convertToWindowsVersion("3.0alpha4"))
        self.assertTrue(convertToWindowsVersion("3.0rc1") > convertToWindowsVersion("3.0alpha0"))
        self.assertTrue(convertToWindowsVersion("3.0rc1") > convertToWindowsVersion("3.0alpha4"))
        self.assertTrue(convertToWindowsVersion("3.0rc11") > convertToWindowsVersion("3.0beta0"))
        self.assertTrue(convertToWindowsVersion("3.0rc11") > convertToWindowsVersion("3.0beta6"))
        self.assertTrue(convertToWindowsVersion("3.0rc1") > convertToWindowsVersion("3.0beta0"))
        self.assertTrue(convertToWindowsVersion("3.0rc1") > convertToWindowsVersion("3.0beta6"))

    def testThatStableIsHigherThanAlphaAndBetaAndRc(self):
        self.assertTrue(convertToWindowsVersion("3.0") > convertToWindowsVersion("3.0alpha0"))
        self.assertTrue(convertToWindowsVersion("3.0") > convertToWindowsVersion("3.0alpha4"))
        self.assertTrue(convertToWindowsVersion("3.0") > convertToWindowsVersion("3.0alpha0"))
        self.assertTrue(convertToWindowsVersion("3.0") > convertToWindowsVersion("3.0alpha4"))
        self.assertTrue(convertToWindowsVersion("3.0") > convertToWindowsVersion("3.0beta0"))
        self.assertTrue(convertToWindowsVersion("3.0") > convertToWindowsVersion("3.0beta6"))
        self.assertTrue(convertToWindowsVersion("3.0") > convertToWindowsVersion("3.0rc1"))
        self.assertTrue(convertToWindowsVersion("3.0") > convertToWindowsVersion("3.0rc11"))

    def testGitDescribeToVersion(self):
        self.assertEqual("5.0alpha2-dev100", gitDescribeToVersion("swift-5.0alpha2-100-g33d5f6571", "swift"))
        self.assertEqual("5.0alpha2", gitDescribeToVersion("swift-5.0alpha2", "swift"))
        self.assertEqual("5.0-dev1", gitDescribeToVersion("swift-5.0-1-g012312312", "swift"))
        self.assertIsNone(gitDescribeToVersion("swiften-5.0-1-g012312312", "swift"))
        self.assertIsNone(gitDescribeToVersion("quickly-5.0-1-g012312312", "swift"))
        self.assertIsNone(gitDescribeToVersion("swift-", "swift"))
        self.assertIsNone(gitDescribeToVersion("swift", "swift"))

if __name__ == '__main__':
    if len(sys.argv) == 1:
        unittest.main()
    elif len(sys.argv) == 2:
        if sys.argv[1] == "--git-build-version":
            print(getGitBuildVersion(os.path.dirname(os.path.dirname(__file__)), "swift"))
        else:
            print(convertToWindowsVersion(sys.argv[1]))
        sys.exit(0)
    else:
        print("Error: Simply run the script without arguments or pass a single argument.")
        sys.exit(-1)
