import sys, os
sys.path.append(Dir("BuildTools/SCons").abspath)

################################################################################
# Build variables
################################################################################

vars = Variables("config.py")
vars.Add(EnumVariable("test", "Compile and run tests", "none", ["none", "all", "unit", "system"]))
vars.Add(BoolVariable("optimize", "Compile with optimizations turned on", "no"))
vars.Add(BoolVariable("debug", "Compile with debug information", "yes" if os.name != "nt" else "no"))
vars.Add(BoolVariable("warnings", "Compile with warnings turned on", 
    "yes" if os.name != "nt" else "no"))
if os.name != "nt" :
  vars.Add(BoolVariable("coverage", "Compile with coverage information", "no"))
if os.name == "posix" :
  vars.Add(BoolVariable("valgrind", "Run tests with valgrind", "no"))
if os.name == "mac" :
  vars.Add(BoolVariable("universal", "Create universal binaries", "no"))
if os.name == "nt" :
  vars.Add(PathVariable("vcredist", "MSVC redistributable dir", "", PathVariable.PathAccept))
if os.name == "nt" :
  vars.Add(PackageVariable("bonjour", "Bonjour SDK location", "yes"))
vars.Add(PackageVariable("openssl", "OpenSSL location", "yes"))
vars.Add(PathVariable("qt", "Qt location", "", PathVariable.PathAccept))

################################################################################
# Set up default build & configure environment
################################################################################

env = Environment(CPPPATH = "#", ENV = {'PATH' : os.environ['PATH']}, variables = vars)

Help(vars.GenerateHelpText(env))

env.Alias("dist", ["."])

# Default custom tools
env.Tool("Test", toolpath = ["#/BuildTools/SCons/Tools"])
env.Tool("WriteVal", toolpath = ["#/BuildTools/SCons/Tools"])
env.Tool("BuildVersion", toolpath = ["#/BuildTools/SCons/Tools"])
if env["PLATFORM"] == "darwin" :
	env.Tool("Nib", toolpath = ["#/BuildTools/SCons/Tools"])
	env.Tool("AppBundle", toolpath = ["#/BuildTools/SCons/Tools"])
if env["PLATFORM"] == "win32" :
	env.Tool("WindowsBundle", toolpath = ["#/BuildTools/SCons/Tools"])

# Default compiler flags
if env["optimize"] :
	env.Append(CCFLAGS = "-O2")
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = ["GL"])
		env.Append(LINKFLAGS = ["/INCREMENTAL:NO", "/LTCG"])

if env["debug"] :
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = ["/Zi", "/MDd"])
		env.Append(LINKFLAGS = ["/DEBUG"])
	else :
		env.Append(CCFLAGS = "-g")
elif env["PLATFORM"] == "win32" :
	env.Append(CCFLAGS = ["/MD"])

if env.get("universal", 0) :
	assert(env["PLATFORM"] == "darwin")
	env.Append(CCFLAGS = [
			"-isysroot", "/Developer/SDKs/MacOSX10.4u.sdk", 
			"-arch", "i386", 
			"-arch", "ppc"])
	env.Append(LINKFLAGS = [
			"-mmacosx-version-min=10.4", 
			"-Wl", "-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk", 
			"-arch", "i386", 
			"-arch", "ppc"])

if env["warnings"] :
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = ["/Wall"])
	else :
		env.Append(CCFLAGS = ["-W", "-Wall"])
		#env.Append(CCFLAGS = ["-W", "-Wall", "-Wredundant-decls", "-pedantic", "-Wno-long-long", "-Woverloaded-virtual", "-Wundef", "-Wfloat-equal", "-Wold-style-cast"])

if env.get("coverage", 0) :
	assert(env["PLATFORM"] != "win32")
	env.Append(CCFLAGS = ["-fprofile-arcs", "-ftest-coverage"])
	env.Append(LINKFLAGS = ["-fprofile-arcs", "-ftest-coverage"])

if env["PLATFORM"] == "win32" :
	env.Append(LIBS = ["dnsapi", "ws2_32", "wsock32"])
	env.Append(CCFLAGS = "/EHsc")
	env["LINKCOM"] = [env["LINKCOM"], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;1']
	env["SHLINKCOM"] = [env["SHLINKCOM"], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2']

if env["PLATFORM"] == "darwin" :
	env.Append(FRAMEWORKS = "AppKit")

# Testing
env["TEST_TYPE"] = env["test"]
env.Alias("check", ".")
if "check" in ARGUMENTS or "check" in COMMAND_LINE_TARGETS :
	env["TEST_TYPE"] = "unit"
env["TEST"] = (env["TEST_TYPE"] != "none") or env.GetOption("clean")
if env.get("valgrind", 0) :
  env["TEST_RUNNER"] = "valgrind --suppressions=QA/valgrind.supp -q --leak-check=full --track-origins=yes "

# Packaging
if ARGUMENTS.get("SWIFT_INSTALLDIR", "") :
	env["SWIFT_INSTALLDIR"] = Dir(ARGUMENTS["SWIFT_INSTALLDIR"]).abspath

conf_env = env.Clone()

Export("env")
Export("conf_env")


################################################################################
# Extend the default build environment (not affecting the configure env)
#
# Keeping both environments separated mostly because of SCons Issue 2391,
# although it doesn't hurt to separate them (e.g. not have pretty printed
# strings in config.log)
################################################################################

#if env["PLATFORM"] == "win32" :
#	env["MSVC_BATCH"] = 1

# Pretty output
def colorize(command, target, color) :
	colors = { "red": "31", "green": "32", "yellow": "33", "blue": "34" }
	prefix = ""
	suffix = ""
	if sys.stdout.isatty() and env["PLATFORM"] != "win32":
		prefix = "\033[0;" + colors[color] + ";140m"
		suffix = "\033[0m"
	return "  " + prefix + command + suffix + " " + target

if int(ARGUMENTS.get("V", 0)) == 0:
	env["CCCOMSTR"] = colorize("CC", "$TARGET", "green")
	env["CXXCOMSTR"] = colorize("CXX", "$TARGET", "green")
	env["LINKCOMSTR"] = colorize("LINK", "$TARGET", "red")
	env["ARCOMSTR"] = colorize("AR", "$TARGET", "red")
	env["RANLIBCOMSTR"] = colorize("RANLIB", "$TARGET", "red")
	env["QT4_RCCCOMSTR"] = colorize("RCC", "$TARGET", "blue")
	env["QT4_UICCOMSTR"] = colorize("UIC", "$TARGET", "blue")
	env["QT4_MOCFROMHCOMSTR"] = colorize("MOC", "$TARGET", "blue")
	env["QT4_MOCFROMCXXCOMSTR"] = colorize("MOC", "$TARGET", "blue")
	env["GENCOMSTR"] = colorize("GEN", "$TARGET", "blue")
	env["RCCOMSTR"] = colorize("RC", "$TARGET", "blue")
	env["BUNDLECOMSTR"] = colorize("BUNDLE", "$TARGET", "blue")
	env["NIBCOMSTR"] = colorize("NIB", "$TARGET", "blue")
	env["NSISCOMSTR"] = colorize("NSIS", "$TARGET", "blue")
	env["TESTCOMSTR"] = colorize("TEST", "$SOURCE", "yellow")
	#Progress(colorize("DEP", "$TARGET", "red")

################################################################################
# Platform configuration
################################################################################

conf = Configure(conf_env)

if conf.CheckLib("z") :
	env.Append(LIBS = "z")
	env["ZLIB_FLAGS"] = ""
else :
	SConscript("3rdParty/ZLib/SConscript")

if conf.CheckLib("dl") :
	env.Append(LIBS = ["dl"])

if conf.CheckLib("c") :
	env.Append(LIBS = ["c"])

if conf.CheckLib("resolv") :
	env.Append(LIBS = ["resolv"])

# Expat
if conf.CheckCHeader("expat.h") and conf.CheckLib("expat") :
	env["HAVE_EXPAT"] = 1
	env["EXPAT_FLAGS"] = { "LIBS": ["expat"] }

conf.Finish()

# LibXML
conf = Configure(conf_env)
if conf.CheckCHeader("libxml/parser.h") and conf.CheckLib("xml2") :
	env["HAVE_LIBXML"] = 1
	env["LIBXML_FLAGS"] = { "LIBS": ["xml2"] }
conf.Finish()

if not env.get("HAVE_LIBXML", 0) :
	libxml_env = conf_env.Clone()
	libxml_env.Append(CPPPATH = ["/usr/include/libxml2"])
	conf = Configure(libxml_env)
	if conf.CheckCHeader("libxml/parser.h") and conf.CheckLib("xml2") :
		env["HAVE_LIBXML"] = 1
		env["LIBXML_FLAGS"] = { "CPPPATH": ["/usr/include/libxml2"], "LIBS": ["xml2"] }
	conf.Finish()

# Bundled expat
bundledExpat = False
if not env.get("HAVE_EXPAT", 0) :
	print "Expat or LibXML not found. Using bundled Expat"
	SConscript("3rdParty/Expat/SConscript")
	env["HAVE_EXPAT"] = 1
	bundledExpat = True

# Qt
if env["qt"] :
	env["QTDIR"] = env["qt"]

# OpenSSL
openssl_env = conf_env.Clone()
use_openssl = bool(env["openssl"])
openssl_prefix = env["openssl"] if isinstance(env["openssl"], str) else ""
openssl_flags = {}
if openssl_prefix :
	openssl_flags = { "CPPPATH": [os.path.join(openssl_prefix, "include")] }
	if env["PLATFORM"] == "win32" : 
		openssl_flags["LIBPATH"] = [os.path.join(openssl_prefix, "lib", "VC")]
		env["OPENSSL_DIR"] = openssl_prefix
	else :
		openssl_flags["LIBPATH"] = [os.path.join(openssl_prefix, "lib")]
	openssl_env.MergeFlags(openssl_flags)

openssl_conf = Configure(openssl_env)
if use_openssl and openssl_conf.CheckCHeader("openssl/ssl.h") :
	env["HAVE_OPENSSL"] = 1
	env["OPENSSL_FLAGS"] = openssl_flags
	if env["PLATFORM"] == "win32" : 
		env["OPENSSL_FLAGS"]["LIBS"] = ["libeay32MT", "ssleay32MT"]
	else:
		env["OPENSSL_FLAGS"]["LIBS"] = ["ssl", "crypto"]
else :
	env["OPENSSL_FLAGS"] = ""

openssl_conf.Finish()

# Bonjour
if env["PLATFORM"] == "darwin" :
	env["HAVE_BONJOUR"] = 1
elif env.get("bonjour", False) :
	bonjour_env = conf_env.Clone()
	bonjour_conf = Configure(bonjour_env)
	bonjour_flags = {}
	if env.get("bonjour") != True :		
		bonjour_prefix = env["bonjour"]
		bonjour_flags["CPPPATH"] = [os.path.join(bonjour_prefix, "include")]
		bonjour_flags["LIBPATH"] = [os.path.join(bonjour_prefix, "lib", "win32")]
	bonjour_env.MergeFlags(bonjour_flags)
	if bonjour_conf.CheckCHeader("dns_sd.h") and bonjour_conf.CheckLib("dnssd") :
		env["HAVE_BONJOUR"] = 1
		env["BONJOUR_FLAGS"] = bonjour_flags
		env["BONJOUR_FLAGS"]["LIBS"] = ["dnssd"]
	bonjour_conf.Finish()

################################################################################
# Project files
# FIXME: We need to explicitly list the order of libraries here, because of
# the exported FLAGS. We should put FLAGS in separate SConscript files, and
# read these in before anything else, such that we don't need to manually
# list modules in order.
################################################################################

# Third-party modules
SConscript(dirs = [
		"3rdParty/CppUnit",
		"3rdParty/Boost",
		"3rdParty/LibIDN",
		"3rdParty/SQLite"])

# Checker
SConscript(dirs = ["QA/Checker"])

# Libraries
SConscript(dirs = [
		"Swiften",
		"SwifTools"
	])

# Projects
for dir in os.listdir(".") :
	if dir in ["QA", "Swiften", "SwifTools"] :
		continue 
	sconscript = os.path.join(dir, "SConscript")
	if os.path.isfile(sconscript) :
		SConscript(sconscript)

# Unit test runner
SConscript(dirs = ["QA/UnitTest"])

################################################################################
# Print summary
################################################################################

print
print "  Build Configuration"
print "  -------------------"

parsers = []
if env.get("HAVE_LIBXML", 0):
	parsers.append("LibXML")
if env.get("HAVE_EXPAT", 0):
	parsers.append("Expat")
	if bundledExpat:
		parsers.append("(Bundled)")
print "  XML Parsers: " + ' '.join(parsers)

print "  TLS Support: " + ("OpenSSL" if env.get("HAVE_OPENSSL",0) else "Disabled")
print "  DNSSD Support: " + ("Bonjour" if env.get("HAVE_BONJOUR") else ("Avahi" if env.get("HAVE_AVAHI") else "Disabled"))
print
