import sys, os, re, platform, hashlib
sys.path.append(Dir("#/BuildTools/SCons").abspath)

################################################################################
# Build variables
################################################################################

vars = Variables(os.path.join(Dir("#").abspath, "config.py"))
vars.Add('cc', "C compiler")
vars.Add('cxx', "C++ compiler")
vars.Add('ccflags', "Extra C/C++/ObjC compiler flags")
vars.Add('cxxflags', "Extra C++ compiler flags")
vars.Add('link', "Linker")
vars.Add('linkflags', "Extra linker flags")
vars.Add('ar', "Archiver (ar or lib)")
if os.name == "nt":
	vars.Add('mt', "manifest tool")
vars.Add(BoolVariable("ccache", "Use CCache", "no"))
vars.Add(EnumVariable("test", "Compile and run tests", "none", ["none", "all", "unit", "system"]))
vars.Add(BoolVariable("optimize", "Compile with optimizations turned on", "no"))
vars.Add(BoolVariable("debug", "Compile with debug information", "yes"))
vars.Add(BoolVariable("allow_warnings", "Allow compilation warnings during compilation", "yes"))
vars.Add(BoolVariable("assertions", "Compile with assertions", "yes"))
vars.Add(BoolVariable("max_jobs", "Build with maximum number of parallel jobs", "no"))
vars.Add(EnumVariable("target", "Choose a target platform for compilation", "native", ["native", "iphone-simulator", "iphone-device", "xcode", "android"]))
vars.Add('android_toolchain', "Path to Android toolchain")
vars.Add('android_sdk_bin', "Path to Android SDK's tools directory")
vars.Add(BoolVariable("swift_mobile", "Build mobile Swift", "no"))
vars.Add(BoolVariable("swiften_dll", "Build Swiften as dynamically linked library", "no"))
if os.name != "nt" :
	vars.Add(BoolVariable("coverage", "Compile with coverage information", "no"))
if os.name == "posix" :
	vars.Add(BoolVariable("valgrind", "Run tests with valgrind", "no"))
if os.name == "mac" or (os.name == "posix" and os.uname()[0] == "Darwin"):
	vars.Add(BoolVariable("universal", "Create universal binaries", "no"))
	vars.Add(BoolVariable("mac105", "Link against the 10.5 frameworks", "no"))
	vars.Add(BoolVariable("mac106", "Link against the 10.6 frameworks", "no"))
if os.name == "nt" :
	vars.Add(PathVariable("vcredist", "MSVC redistributable dir", None, PathVariable.PathAccept))
if os.name == "nt" :
	vars.Add(PathVariable("wix_bindir", "Path to WiX binaries", "", PathVariable.PathAccept))
if os.name == "nt" :
	vars.Add(PackageVariable("bonjour", "Bonjour SDK location", "yes"))
vars.Add(PackageVariable("openssl", "OpenSSL location", "yes"))
vars.Add("openssl_libnames", "Comma-separated openssl library names to override defaults", None)
vars.Add(BoolVariable("openssl_force_bundled", "Force use of the bundled OpenSSL", "no"))
vars.Add("openssl_include", "Location of OpenSSL include files (if not under (openssl)/include)", None)
vars.Add("openssl_libdir", "Location of OpenSSL library files (if not under (openssl)/lib)", None)
vars.Add(PackageVariable("hunspell", "Hunspell location", False))
vars.Add(PathVariable("boost_includedir", "Boost headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("boost_libdir", "Boost library location", None, PathVariable.PathAccept))
vars.Add(BoolVariable("boost_bundled_enable", "Allow use of bundled Boost as last resort", "true"))
vars.Add(PathVariable("zlib_includedir", "Zlib headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("zlib_libdir", "Zlib library location", None, PathVariable.PathAccept))
vars.Add(PathVariable("zlib_libfile", "Zlib library file (full path to file)", None, PathVariable.PathAccept))
vars.Add(BoolVariable("zlib_bundled_enable", "Allow use of bundled Zlib as last resort", "true"))
vars.Add(BoolVariable("try_gconf", "Try configuring for GConf?", "true"))
vars.Add(BoolVariable("try_libxml", "Try configuring for libXML?", "true"))
vars.Add(BoolVariable("try_expat", "Try configuring for expat?", "true"))
vars.Add(PathVariable("expat_includedir", "Expat headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("expat_libdir", "Expat library location", None, PathVariable.PathAccept))
vars.Add("expat_libname", "Expat library name", os.name == "nt" and "libexpat" or "expat")
vars.Add(PackageVariable("icu", "ICU library location", "no"))
vars.Add(BoolVariable("libidn_bundled_enable", "Allow use of bundled LibIDN", "true"))
vars.Add(BoolVariable("try_libidn", "Try configuring for LibIDN?", "true"))
vars.Add(PathVariable("libidn_includedir", "LibIDN headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("libidn_libdir", "LibIDN library location", None, PathVariable.PathAccept))
vars.Add("libidn_libname", "LibIDN library name",  os.name == "nt" and "libidn" or "idn")
vars.Add(BoolVariable("need_idn", "Whether an IDN library is required. Without this, most internal binaries will fail", "true"))
vars.Add(PathVariable("libminiupnpc_includedir", "LibMiniUPNPC headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("libminiupnpc_libdir", "LibMiniUPNPC library location", None, PathVariable.PathAccept))
vars.Add("libminiupnpc_libname", "LibMiniUPNPC library name", os.name == "nt" and "libminiupnpc" or "miniupnpc")
vars.Add(PathVariable("libnatpmp_includedir", "LibNATPMP headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("libnatpmp_libdir", "LibNATPMP library location", None, PathVariable.PathAccept))
vars.Add("libnatpmp_libname", "LibNATPMP library name", os.name == "nt" and "libnatpmp" or "natpmp")
vars.Add(PathVariable("sqlite_includedir", "SQLite headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("sqlite_libdir", "SQLite library location", None, PathVariable.PathAccept))
vars.Add("sqlite_libname", "SQLite library name", os.name == "nt" and "libsqlite3" or "sqlite3")
vars.Add("sqlite_force_bundled", "Force use of the bundled SQLite", None)
vars.Add(PathVariable("lua_includedir", "Lua headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("lua_libdir", "Lua library location", None, PathVariable.PathAccept))
vars.Add("lua_libname", "Lua library name", os.name == "nt" and "liblua" or "lua")
vars.Add("lua_force_bundled", "Force use of the bundled Lua", None)

vars.Add(PathVariable("editline_includedir", "Readline headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("editline_libdir", "Readline library location", None, PathVariable.PathAccept))
vars.Add("editline_libname", "Readline library name", os.name == "nt" and "libedit" or "edit")

vars.Add(BoolVariable("try_avahi", "Try configuring for avahi?", "true"))
vars.Add(PathVariable("avahi_includedir", "Avahi headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("avahi_libdir", "Avahi library location", None, PathVariable.PathAccept))
vars.Add(PathVariable("qt", "Qt location", "", PathVariable.PathAccept))
vars.Add(BoolVariable("qt5", "Compile in Qt5 mode", "no")) # TODO: auto-detect this
vars.Add(PathVariable("docbook_xml", "DocBook XML", None, PathVariable.PathAccept))
vars.Add(PathVariable("docbook_xsl", "DocBook XSL", None, PathVariable.PathAccept))
vars.Add(BoolVariable("build_examples", "Build example programs", "yes"))
vars.Add(BoolVariable("enable_variants", "Build in a separate dir under build/, depending on compile flags", "no"))
vars.Add(BoolVariable("experimental_ft", "Build experimental file transfer", "yes"))
vars.Add(BoolVariable("experimental", "Build experimental features", "no"))
vars.Add(BoolVariable("set_iterator_debug_level", "Set _ITERATOR_DEBUG_LEVEL=0", "yes"))
vars.Add(BoolVariable("unbound", "Build bundled ldns and unbound. Use them for DNS lookup.", "no"))

vars.Add(BoolVariable("install_git_hooks", "Install git hooks", "true"))

################################################################################
# Set up default build & configure environment
################################################################################

env_ENV = {
	'PATH' : os.environ['PATH'],
	'LD_LIBRARY_PATH' : os.environ.get("LD_LIBRARY_PATH", ""),
	'TERM' : os.environ.get("TERM", ""),
}

if "MSVC_VERSION" in ARGUMENTS :
	env = Environment(ENV = env_ENV, variables = vars, MSVC_VERSION = ARGUMENTS["MSVC_VERSION"], platform = ARGUMENTS.get("PLATFORM", None))
else :
	env = Environment(ENV = env_ENV, variables = vars, platform = ARGUMENTS.get("PLATFORM", None))

Help(vars.GenerateHelpText(env))

# Workaround for missing Visual Studio 2012 support in SCons
# Requires scons to be run from a VS2012 console
if env.get("MSVC_VERSION", "").startswith("11.0") :
	env["ENV"]["LIB"] = os.environ["LIB"]
	env["ENV"]["INCLUDE"] = os.environ["INCLUDE"]

# Default environment variables
env["PLATFORM_FLAGS"] = {
		"LIBPATH": [],
		"LIBS": [],
		"FRAMEWORKS": [],
}

# Default custom tools
env.Tool("Test", toolpath = ["#/BuildTools/SCons/Tools"])
env.Tool("WriteVal", toolpath = ["#/BuildTools/SCons/Tools"])
env.Tool("BuildVersion", toolpath = ["#/BuildTools/SCons/Tools"])
env.Tool("Flags", toolpath = ["#/BuildTools/SCons/Tools"])
if env["PLATFORM"] == "darwin" :
	env.Tool("Nib", toolpath = ["#/BuildTools/SCons/Tools"])
	env.Tool("AppBundle", toolpath = ["#/BuildTools/SCons/Tools"])
if env["PLATFORM"] == "win32" :
	env.Tool("WindowsBundle", toolpath = ["#/BuildTools/SCons/Tools"])
	#So we don't need to escalate with UAC
	if "TMP" in os.environ.keys() :
		env['ENV']['TMP'] = os.environ['TMP']
env.Tool("SLOCCount", toolpath = ["#/BuildTools/SCons/Tools"])

# Max out the number of jobs
if env["max_jobs"] :
	try :
		import multiprocessing
		SetOption("num_jobs", multiprocessing.cpu_count())
	except NotImplementedError :
		pass
	except ImportError :
		pass

# Set speed options
env.Decider("MD5-timestamp")
env.SetOption("max_drift", 1)
env.SetOption("implicit_cache", True)

# Set the default compiler to CLang on OS X, and set the necessary flags
if env["PLATFORM"] == "darwin" and env["target"] == "native" :
	if "cc" not in env :
		env["CC"] = "clang"
		if platform.machine() == "x86_64" :
			env["CCFLAGS"] = ["-arch", "x86_64"]
	if "cxx" not in env :
		env["CXX"] = "clang++"
		# Compiling Qt5 in C++0x mode includes headers that we don't have
		if not env["qt5"] :
			env["CXXFLAGS"] = ["-std=c++11"]
	if "link" not in env :
		env["LINK"] = "clang"
		if platform.machine() == "x86_64" :
			env.Append(LINKFLAGS = ["-arch", "x86_64"])

# Check whether we are running inside scan-build, and override compiler if so
if "CCC_ANALYZER_HTML" in os.environ :
	for key, value in os.environ.items() :
		if key.startswith("CCC_") or key.startswith("CLANG") :
			env["ENV"][key] = value
	env["CC"] = os.environ["CC"]
	env["CXX"] = os.environ["CXX"]

# Override the compiler with custom variables set at config time
if "cc" in env :
	env["CC"] = env["cc"]
if "cxx" in env :
	env["CXX"] = env["cxx"]
if "ar" in env :
	env["AR"] = env["ar"]

if "link" in env :
	env["SHLINK"] = env["link"]
	env["LINK"] = env["link"]
for flags_type in ["ccflags", "cxxflags", "linkflags"] :
	if flags_type in env :
		if isinstance(env[flags_type], str) :
			# FIXME: Make the splitting more robust
			env[flags_type.upper()] = env[flags_type].split(" ")
		else :
			env[flags_type.upper()] = env[flags_type]
# This isn't a real flag (yet) AFAIK. Be sure to append it to the CXXFLAGS
# where you need it
env["OBJCCFLAGS"] = []

if env["optimize"] :
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = ["/O2"])
	else :
		env.Append(CCFLAGS = ["-O2"])

if env["target"] == "xcode" and os.environ["CONFIGURATION"] == "Release" :
	env.Append(CCFLAGS = ["-Os"])

if env["debug"] :
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = ["/Zi"])
		env.Append(LINKFLAGS = ["/DEBUG"])
		if GetOption("num_jobs") > 1 :
			env["CCPDBFLAGS"] = '/Fd${TARGET}.pdb'
			env["PDB"] = '${TARGET.base}.pdb'
		if env["set_iterator_debug_level"] :
			env.Append(CPPDEFINES = ["_ITERATOR_DEBUG_LEVEL=0"])
		if env["optimize"] :
			env.Append(LINKFLAGS = ["/OPT:NOREF"])
			env.Append(CCFLAGS = ["/MD"])
		else :
			env.Append(CCFLAGS = ["/MDd"])
	else :
		env.Append(CCFLAGS = ["-g"])
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
			"-isysroot", "/Developer/SDKs/MacOSX10.4u.sdk",
			"-arch", "i386",
			"-arch", "ppc"])


# Link against other versions of the OS X SDKs.
# FIXME: This method does not work anymore, we need to set deployment targets.
if env.get("mac105", 0) :
	assert(env["PLATFORM"] == "darwin")
	env.Append(CCFLAGS = [
			"-isysroot", "/Developer/SDKs/MacOSX10.5.sdk",
			"-arch", "i386"])
	env.Append(LINKFLAGS = [
			"-mmacosx-version-min=10.5",
			"-isysroot", "/Developer/SDKs/MacOSX10.5.sdk",
			"-arch", "i386"])
if env.get("mac106", 0) :
	assert(env["PLATFORM"] == "darwin")
	env.Append(CCFLAGS = [
			"-isysroot", "/Developer/SDKs/MacOSX10.6.sdk",
			"-arch", "i386"])
	env.Append(LINKFLAGS = [
			"-mmacosx-version-min=10.6",
			"-isysroot", "/Developer/SDKs/MacOSX10.6.sdk",
			"-arch", "i386"])

if not env["assertions"] :
	env.Append(CPPDEFINES = ["NDEBUG"])

# disable file-transfer support on iOS
if env["target"] in ["iphone-device", "iphone-simulator", "xcode"] :
	env["experimental_ft"] = False

if env["experimental_ft"] :
	env.Append(CPPDEFINES = ["SWIFT_EXPERIMENTAL_FT"])

if env["experimental"] :
	env.Append(CPPDEFINES = ["SWIFT_EXPERIMENTAL_HISTORY", "SWIFT_EXPERIMENTAL_WB"])

# If we build shared libs on AMD64, we need -fPIC.
# This should have no performance impact om AMD64
if env["PLATFORM"] == "posix" and platform.machine() in ["x86_64", "amd64"] :
	env.Append(CCFLAGS = ["-fPIC"])

# Warnings
if env["PLATFORM"] == "win32" :
	env.Append(CXXFLAGS = ["/wd4068"])
elif env["PLATFORM"] == "hpux" :
	# HP-UX gives a flood of minor warnings if this is enabled
	#env.Append(CXXFLAGS = ["+w"])
	pass
elif env["PLATFORM"] == "sunos" :
	#env.Append(CXXFLAGS = ["-z verbose"])
	pass
else :
	if "clang" in env["CXX"] :
		env.Append(CXXFLAGS = [
			"-Weverything",
			"-Wno-unknown-warning-option", # To stay compatible between CLang versions
			"-Wno-unknown-pragmas", # To stay compatible between CLang versions
			"-Wno-weak-vtables", # Virtually none of our elements have outlined methods. This also seems to affect classes in .cpp files, which in turn affects all our tests, which may need fixing in CLang
			"-Wno-shadow", # Also warns for shadowing on constructor arguments, which we do a lot
			"-Wno-documentation", # We don't care about documentation warnings
			"-Wno-documentation-unknown-command", # We don't care about documentation warnings
			"-Wno-exit-time-destructors", # Used a lot in e.g. CPPUnit
			"-Wno-c++98-compat-pedantic", # We do different things that violate this, but they could be fixed
			"-Wno-global-constructors", # We depend on this for e.g. string constants
			"-Wno-disabled-macro-expansion", # Caused due to system headers
			"-Wno-c++11-extensions", # We use C++11; turn this off when we use -std=c++11
			"-Wno-long-long", # We use long long
			"-Wno-padded",
			"-Wno-missing-variable-declarations", # Getting rid of CPPUnit warnings
			"-Wno-direct-ivar-access", # Obj-C code warning
			"-Wno-potentially-evaluated-expression", # Caused due to calling shared_ptr::get() inside typeid()
			])
	else :
		env.Append(CXXFLAGS = ["-Wextra", "-Wall", "-Wnon-virtual-dtor", "-Wundef", "-Wold-style-cast", "-Wno-long-long", "-Woverloaded-virtual", "-Wfloat-equal", "-Wredundant-decls", "-Wno-unknown-pragmas"])
		gccVersion = env.get("CCVERSION", "0.0.0").split(".")
		if gccVersion >= ["4", "5", "0"] and not "clang" in env["CC"] :
			env.Append(CXXFLAGS = ["-Wlogical-op"])
	if not env.get("allow_warnings", False) :
		env.Append(CXXFLAGS = ["-Werror"])

if env.get("coverage", 0) :
	assert(env["PLATFORM"] != "win32")
	env.Append(CCFLAGS = ["-fprofile-arcs", "-ftest-coverage"])
	env.Append(LINKFLAGS = ["-fprofile-arcs", "-ftest-coverage"])

if env["PLATFORM"] == "win32" :
	env.Append(LIBS = ["user32", "crypt32", "dnsapi", "iphlpapi", "ws2_32", "wsock32", "Advapi32"])
	env.Append(CCFLAGS = ["/EHsc", "/nologo", "/Zm256"])
	env.Append(LINKFLAGS = ["/INCREMENTAL:no", "/NOLOGO"])
	if int(env["MSVS_VERSION"].split(".")[0]) < 10 :
		mt = env.get('mt')
		if not mt:
			mt = 'mt.exe'
		env["LINKCOM"] = [env["LINKCOM"], '%s  -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;1' % mt]
		env["SHLINKCOM"] = [env["SHLINKCOM"], '%s  -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2' % mt]

if env["PLATFORM"] == "darwin" and not env["target"] in ["iphone-device", "iphone-simulator", "xcode", "android"] :
	env["PLATFORM_FLAGS"]["FRAMEWORKS"] += ["IOKit", "AppKit", "SystemConfiguration", "Security", "SecurityInterface"]

# Required by boost headers on HP-UX
if env["PLATFORM"] == "hpux" :
	env.Append(CXXFLAGS = ["+hpxstd98", "-mt", "-AA"])
	# FIXME: Need -AA for linking C++ but not C
	#env.Append(LINKFLAGS = ["-AA"])


# Testing
env["TEST_TYPE"] = env["test"]
if "check" in ARGUMENTS :
	env["TEST_TYPE"] = "unit"
env["checker_report"] = ARGUMENTS.get("checker_report", False)
env["TEST"] = (env["TEST_TYPE"] != "none") or env.GetOption("clean")
if env.get("valgrind", 0) :
	env["TEST_RUNNER"] = "valgrind --suppressions=QA/valgrind.supp -q --leak-check=full --track-origins=yes "
env["TEST_IGNORE_RESULT"] = "ignore_test_result" in ARGUMENTS
env["TEST_CREATE_LIBRARIES"] = "create_test_libraries" in ARGUMENTS

# Packaging
env["DIST"] = "dist" in ARGUMENTS or env.GetOption("clean")
for path in ["SWIFT_INSTALLDIR", "SWIFTEN_INSTALLDIR", "SLUIFT_INSTALLDIR"] :
	if ARGUMENTS.get(path, "") :
		if os.path.isabs(ARGUMENTS[path]) :
			env[path] = Dir(ARGUMENTS[path]).abspath
		else :
			env[path] = Dir("#/" + ARGUMENTS[path]).abspath


################################################################################
# XCode / iPhone / ...
################################################################################

target = env["target"]
if target in ["iphone-device", "iphone-simulator", "xcode"] :
	# Extract/initialize all the information we need
	if target == "xcode" :
		# Get the information from the XCode environment
		env["XCODE_PLATFORM_DEVELOPER_BIN_DIR"] = os.environ["PLATFORM_DEVELOPER_BIN_DIR"]
		env["XCODE_SDKROOT"] = os.environ["SDKROOT"]
		env["XCODE_ARCH_FLAGS"] = sum([["-arch", arch] for arch in os.environ["ARCHS"].split(" ")], [])
		env["IPHONEOS_DEPLOYMENT_TARGET"] = os.environ["IPHONEOS_DEPLOYMENT_TARGET"]
		# Use absolute path sources so Xcode can highlight compilation errors in swiften
		env['CXXCOM'] = '$CXX -o $TARGET -c $CXXFLAGS $CCFLAGS $_CCCOMCOM ${SOURCES.abspath}'
	else :
		# Hard code values
		env["XCODE_PLATFORM_DEVELOPER_BIN_DIR"] = "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin"
		if target == "iphone-device":
			env["XCODE_ARCH_FLAGS"] = ["-arch", "armv6", "-arch", "armv7"]
			sdkPart = "iPhoneOS"
		else :
			env["XCODE_ARCH_FLAGS"] = ["-arch", "i386"]
			sdkPart = "iPhoneSimulator"
		sdkVer = "6.0"
		env["XCODE_SDKROOT"] = "/Applications/Xcode.app/Contents/Developer/Platforms/" + sdkPart + ".platform/Developer/SDKs/" + sdkPart + sdkVer + ".sdk"
		env["IPHONEOS_DEPLOYMENT_TARGET"] = "4.1"

	# Set the build flags
	env["CC"] = os.environ["DEVELOPER_BIN_DIR"] + "/gcc"
	env["CXX"] = os.environ["DEVELOPER_BIN_DIR"] + "/g++"
	env["OBJCCFLAGS"] = ["-fobjc-abi-version=2", "-fobjc-legacy-dispatch"]
	env["LD"] = env["CC"]
	env.Append(CCFLAGS = env["XCODE_ARCH_FLAGS"] + ["-fvisibility=hidden", "-miphoneos-version-min=" + env["IPHONEOS_DEPLOYMENT_TARGET"]])
	env.Append(LINKFLAGS = "-miphoneos-version-min=" + env["IPHONEOS_DEPLOYMENT_TARGET"])
	if os.environ.get("GCC_THUMB_SUPPORT", False) :
		env.Append(CCFLAGS = ["-mthumb"])
	env.Append(LINKFLAGS = env["XCODE_ARCH_FLAGS"])
	env.Append(CPPFLAGS = ["-isysroot", "$XCODE_SDKROOT"])
	env.Append(FRAMEWORKS = ["CoreFoundation", "Foundation", "UIKit", "CoreGraphics"])
	env.Append(LINKFLAGS = env["XCODE_ARCH_FLAGS"] + ["-isysroot", "$XCODE_SDKROOT", "-L\"$XCODE_SDKROOT/usr/lib\"", "-F\"$XCODE_SDKROOT/System/Library/Frameworks\"", "-F\"$XCODE_SDKROOT/System/Library/PrivateFrameworks\""])
	# Bit of a hack, because BOOST doesn't know the endianness for ARM
	env.Append(CPPDEFINES = ["_LITTLE_ENDIAN"])

################################################################################
# Android
################################################################################
if target in ["android"] :
	env["ENV"]["PATH"] = env["android_toolchain"] + "/bin:" + env["ENV"]["PATH"]
	env["CC"] = "arm-linux-androideabi-gcc"
	env["CXX"] = "arm-linux-androideabi-g++"
	env["AR"] = "arm-linux-androideabi-ar"
	env["RANLIB"] = "arm-linux-androideabi-ranlib"
	env.Append(CPPDEFINES = ["ANDROID"])
	env.Append(CPPDEFINES = ["_REENTRANT", "_GLIBCXX__PTHREADS"])

# CCache
if env.get("ccache", False) :
	env["ENV"]["HOME"] = os.environ["HOME"]
	for var in os.environ :
		if var.startswith("CCACHE_") :
			env["ENV"][var] = os.environ[var]
	if env.get("CC", "") != "" :
		env["CC"] = "ccache " + env["CC"]
	else :
		env["CC"] = "ccache gcc"
	if env.get("CXX", "") != "" :
		env["CXX"] = "ccache " + env["CXX"]
	else :
		env["CC"] = "ccache g++"

conf_env = env.Clone()

Export("env")
Export("conf_env")

variant = ""
if env["enable_variants"] :
	fingerprint = ",".join([flag for flag in env["CXXFLAGS"] + env["CCFLAGS"] if not flag.startswith("-W") and not flag.startswith("-fvisibility")])
	variant = "build/" + fingerprint
	if not os.path.exists(Dir("#/build").abspath) :
		os.mkdir(Dir("#/build").abspath)
	if os.path.exists(Dir("#/build/current").abspath) :
		os.unlink(Dir("#/build/current").abspath)
	os.symlink(os.path.basename(variant), Dir("#/build/current").abspath)

Return("variant")
