import sys, os
sys.path.append(Dir("BuildTools/SCons").abspath)

################################################################################
# Build variables
################################################################################

vars = Variables("config.py")
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

if env.get("valgrind", 0) :
  env["TEST_RUNNER"] = "valgrind --suppressions=QA/valgrind.supp -q --leak-check=full --track-origins=yes "

if env["PLATFORM"] == "win32" :
	env.Append(LIBS = ["dnsapi", "ws2_32", "wsock32"])
	env.Append(CCFLAGS = "/EHsc")
	env["LINKCOM"] = [env["LINKCOM"], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;1']
	env["SHLINKCOM"] = [env["SHLINKCOM"], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2']

if env["PLATFORM"] == "darwin" :
	env.Append(FRAMEWORKS = "AppKit")

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

# Pretty output
if int(ARGUMENTS.get("V", 0)) == 0:
  if sys.stdout.isatty() and env["PLATFORM"] != "win32":
    env["CCCOMSTR"]             = "  \033[0;33;140mCC\033[0m     $TARGET"
    env["CXXCOMSTR"]            = "  \033[0;32;140mCXX\033[0m    $TARGET"
    env["LINKCOMSTR"]           = "  \033[0;31;140mLINK\033[0m   $TARGET"
    env["ARCOMSTR"]             = "  \033[0;31;140mAR\033[0m     $TARGET"
    env["RANLIBCOMSTR"]         = "  \033[0;31;140mRANLIB\033[0m $TARGET"
    env["QT4_RCCCOMSTR"]        = "  \033[0;34;140mRCC\033[0m    $TARGET"
    env["QT4_UICCOMSTR"]        = "  \033[0;34;140mUIC\033[0m    $TARGET"
    env["QT4_MOCFROMHCOMSTR"]   = "  \033[0;34;140mMOC\033[0m    $TARGET"
    env["QT4_MOCFROMCXXCOMSTR"] = "  \033[0;34;140mMOC\033[0m    $TARGET"
    env["GENCOMSTR"]            = "  \033[0;34;140mGEN\033[0m    $TARGET"
    env["RCCOMSTR"]             = "  \033[0;34;140mRC\033[0m     $TARGET"
    env["BUNDLECOMSTR"]         = "  \033[0;34;140mBUNDLE\033[0m $TARGET"
    env["NIBCOMSTR"]            = "  \033[0;34;140mNIB\033[0m    $TARGET"
    env["NSISCOMSTR"]           = "  \033[0;34;140mNSIS\033[0m   $TARGET"
    #Progress(                     "  \033[0;35;140mDEP\033[0m    $TARGET\n")
  else :
    env["CCCOMSTR"]             = "  CC     $TARGET"
    env["CXXCOMSTR"]            = "  CXX    $TARGET"
    env["LINKCOMSTR"]           = "  LINK   $TARGET"
    env["ARCOMSTR"]             = "  AR     $TARGET"
    env["RANLIBCOMSTR"]         = "  RANLIB $TARGET"
    env["QT4_RCCCOMSTR"]        = "  RCC    $TARGET"
    env["QT4_UICCOMSTR"]        = "  UIC    $TARGET"
    env["QT4_MOCFROMHCOMSTR"]   = "  MOC    $TARGET"
    env["QT4_MOCFROMCXXCOMSTR"] = "  MOC    $TARGET"
    env["GENCOMSTR"]            = "  GEN    $TARGET"
    env["RCCOMSTR"]             = "  RC     $TARGET"
    env["BUNDLECOMSTR"]         = "  BUNDLE $TARGET"
    env["NIBCOMSTR"]            = "  NIB    $TARGET"
    env["NSISCOMSTR"]           = "  NSIS   $TARGET"
    #Progress('  DEP $TARGET\n')

if env["PLATFORM"] == "win32" :
	env["MSVC_BATCH"] = 1

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
################################################################################

# Third-party modules
SConscript([
		"3rdParty/CppUnit/SConscript",
		"3rdParty/Boost/SConscript",
		"3rdParty/LibIDN/SConscript",
		"3rdParty/SQLite/SConscript"])

# Swiften
SConscript("Swiften/SConscript")

# Projects
for dir in os.listdir(".") :
	if dir in ["QA", "Swiften"] :
		continue 
	sconscript = os.path.join(dir, "SConscript")
	if os.path.isfile(sconscript) :
		SConscript(sconscript)

# QA
SConscript("QA/SConscript")


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
