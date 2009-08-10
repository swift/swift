import sys, os

################################################################################
# Set up default build environment
################################################################################

env = Environment(CPPPATH = "#", ENV = {'PATH' : os.environ['PATH']})

# Pretty output
if ARGUMENTS.get("V") != "1":
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
    env["GENCOMSTR"]            = "  GEN    $TGENGET"
    #Progress('  DEP $TARGET\n')

# Default compiler flags
if int(ARGUMENTS.get("optimize", 0)) == 1 :
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = "/O2")
	else :
		env.Append(CCFLAGS = "-O2")

if int(ARGUMENTS.get("debug", 1)) == 1 :
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = ["/Zi", "/MDd"])
	else :
		env.Append(CCFLAGS = "-g")

if int(ARGUMENTS.get("universal", 0)) == 1 :
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

if int(ARGUMENTS.get("warnings", 1)) == 1 :
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = ["/Wall"])
	else :
		env.Append(CCFLAGS = ["-W", "-Wall"])
		#env.Append(CCFLAGS = ["-W", "-Wall", "-Wredundant-decls", "-pedantic", "-Wno-long-long", "-Woverloaded-virtual", "-Wundef", "-Wfloat-equal", "-Wold-style-cast"])

if int(ARGUMENTS.get("coverage", 0)) == 1 :
	assert(env["PLATFORM"] != "win32")
	env.Append(CCFLAGS = ["-fprofile-arcs", "-ftest-coverage"])
	env.Append(LINKFLAGS = ["-fprofile-arcs", "-ftest-coverage"])

if env["PLATFORM"] == "win32" :
	env.Append(CCFLAGS = "/EHsc")

if env["PLATFORM"] == "darwin" :
	env.Append(FRAMEWORKS = "AppKit")

Export("env")


################################################################################
# Platform configuration
################################################################################

conf = Configure(env)

if conf.CheckCHeader("pthread.h") :
	env["HAVE_PTHREAD"] = 1

if conf.CheckCHeader("openssl/ssl.h") :
	env["HAVE_OPENSSL"] = 1
	env["OPENSSL_FLAGS"] = {
			"LIBS": ["ssl", "crypto"]
		}
else :
	env["OPENSSL_FLAGS"] = ""

if conf.CheckLib("z") :
	env.Append(LIBS = "z")
	env["ZLIB_FLAGS"] = ""
else :
	SConscript("3rdParty/ZLib/SConscript")

if conf.CheckLib("dl") :
	env.Append(LIBS = "dl")

if conf.CheckLib("resolv") :
	env.Append(LIBS = "resolv")

if conf.CheckCHeader("expat.h") and conf.CheckLib("expat") :
	env["HAVE_EXPAT"] = 1
	env["EXPAT_FLAGS"] = ""
	env.Append(LIBS = "expat")

conf.Finish()

bundledExpat = False
if not env.get("HAVE_EXPAT", 0) :
	print "Expat or LibXML not found. Using bundled Expat"
	SConscript("3rdParty/Expat/SConscript")
	env["HAVE_EXPAT"] = 1
	bundledExpat = True


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
print "		 Build Configuration"
print "		 -------------------"

parsers = []
if env.get("HAVE_LIBXML", 0):
	parsers.append("LibXML")
if env.get("HAVE_EXPAT", 0):
	parsers.append("Expat")
	if bundledExpat:
		parsers.append("(Bundled)")
print "		 XML Parsers: " + ' '.join(parsers)

print "		 TLS Support: " + ("OpenSSL" if env["HAVE_OPENSSL"] else "Disabled")
print
