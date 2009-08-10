import sys

# Default build environment
env = Environment(CPPPATH = "#")
env.Dump()

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
		#Progress('  \033[0;35;140mDEP\033[0m $TARGET\n', overwrite = True)
	else :
		env["CCCOMSTR"]     = "  CC     $TARGET"
		env["CXXCOMSTR"]    = "  CXX    $TARGET"
		env["LINKCOMSTR"]   = "  LINK   $TARGET"
		env["ARCOMSTR"]     = "  AR     $TARGET"
		env["RANLIBCOMSTR"] = "  RANLIB $TARGET"
		#Progress('  \033[0;35;140mDEP\033[0m $TARGET\n', overwrite = True)

# FIXME: Should check compiler
if env["PLATFORM"] != "win32" :
	if ARGUMENTS.get("debug", 1) :
		env.Append(CCFLAGS = "-g")
else :
	if ARGUMENTS.get("debug", 1) :
		env.Append(CCFLAGS = ["/Zi", "/MDd"])
	env.Append(CCFLAGS = "/EHsc")

if env["PLATFORM"] == "darwin" :
	env.Append(FRAMEWORKS = "AppKit")

print "Platform: " + env["PLATFORM"]
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

if not env.get("HAVE_EXPAT", 0) :
	print "Expat or LibXML not found. Using bundled Expat"
	SConscript("3rdParty/Expat/SConscript")
	env["HAVE_EXPAT"] = 1


################################################################################
# Project files
################################################################################

SConscript([
		"3rdParty/CppUnit/SConscript",
		"3rdParty/Boost/SConscript",
		"3rdParty/LibIDN/SConscript",
		"3rdParty/SQLite/SConscript",
		"Swiften/SConscript",
		"Swift/SConscript",
		"QA/UnitTest/SConscript",
	])
