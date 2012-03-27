import sys, os, re, platform, hashlib
sys.path.append(Dir("#/BuildTools/SCons").abspath)

################################################################################
# Build variables
################################################################################

vars = Variables(os.path.join(Dir("#").abspath, "config.py"))
vars.Add('cc', "C compiler")
vars.Add('cxx', "C++ compiler")
vars.Add('ccflags', "Extra C(++) compiler flags")
vars.Add('link', "Linker")
vars.Add('linkflags', "Extra linker flags")
vars.Add(BoolVariable("ccache", "Use CCache", "no"))
vars.Add(BoolVariable("distcc", "Use DistCC", "no"))
vars.Add('distcc_hosts', "DistCC hosts (overrides DISTCC_HOSTS)")
vars.Add(EnumVariable("test", "Compile and run tests", "none", ["none", "all", "unit", "system"]))
vars.Add(BoolVariable("optimize", "Compile with optimizations turned on", "no"))
vars.Add(BoolVariable("debug", "Compile with debug information", "yes"))
vars.Add(BoolVariable("allow_warnings", "Allow compilation warnings during compilation", "yes"))
vars.Add(BoolVariable("assertions", "Compile with assertions", "yes"))
vars.Add(BoolVariable("max_jobs", "Build with maximum number of parallel jobs", "no"))
vars.Add(EnumVariable("target", "Choose a target platform for compilation", "native", ["native", "iphone-simulator", "iphone-device", "xcode"]))
vars.Add(BoolVariable("swift_mobile", "Build mobile Swift", "no"))
if os.name != "nt" :
	vars.Add(BoolVariable("coverage", "Compile with coverage information", "no"))
if os.name == "posix" :
	vars.Add(BoolVariable("valgrind", "Run tests with valgrind", "no"))
if os.name == "mac" or (os.name == "posix" and os.uname()[0] == "Darwin"):
	vars.Add(BoolVariable("universal", "Create universal binaries", "no"))
	vars.Add(BoolVariable("mac105", "Link against the 10.5 frameworks", "no"))
if os.name == "nt" :
	vars.Add(PathVariable("vcredist", "MSVC redistributable dir", "", PathVariable.PathAccept))
if os.name == "nt" :
	vars.Add(PathVariable("wix_bindir", "Path to WiX binaries", "", PathVariable.PathAccept))
if os.name == "nt" :
	vars.Add(PackageVariable("bonjour", "Bonjour SDK location", "yes"))
vars.Add(PackageVariable("openssl", "OpenSSL location", "yes"))
vars.Add(PathVariable("boost_includedir", "Boost headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("boost_libdir", "Boost library location", None, PathVariable.PathAccept))
vars.Add(PathVariable("expat_includedir", "Expat headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("expat_libdir", "Expat library location", None, PathVariable.PathAccept))
vars.Add("expat_libname", "Expat library name", "libexpat" if os.name == "nt" else "expat")
vars.Add(PathVariable("libidn_includedir", "LibIDN headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("libidn_libdir", "LibIDN library location", None, PathVariable.PathAccept))
vars.Add("libidn_libname", "LibIDN library name", "libidn" if os.name == "nt" else "idn")
vars.Add(PathVariable("sqlite_includedir", "SQLite headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("sqlite_libdir", "SQLite library location", None, PathVariable.PathAccept))
vars.Add("sqlite_libname", "SQLite library name", "libsqlite3" if os.name == "nt" else "sqlite3")
vars.Add(PathVariable("avahi_includedir", "Avahi headers location", None, PathVariable.PathAccept))
vars.Add(PathVariable("avahi_libdir", "Avahi library location", None, PathVariable.PathAccept))
vars.Add(PathVariable("qt", "Qt location", "", PathVariable.PathAccept))
vars.Add(PathVariable("docbook_xml", "DocBook XML", None, PathVariable.PathAccept))
vars.Add(PathVariable("docbook_xsl", "DocBook XSL", None, PathVariable.PathAccept))
vars.Add(BoolVariable("build_examples", "Build example programs", "yes"))
vars.Add(BoolVariable("enable_variants", "Build in a separate dir under build/, depending on compile flags", "no"))
vars.Add(BoolVariable("experimental", "Build experimental features", "no"))
vars.Add(BoolVariable("set_iterator_debug_level", "Set _ITERATOR_DEBUG_LEVEL=0", "yes"))

################################################################################
# Set up default build & configure environment
################################################################################

env = Environment(CPPPATH = ["#"], ENV = {
		'PATH' : os.environ['PATH'], 
		'LD_LIBRARY_PATH' : os.environ.get("LD_LIBRARY_PATH", ""),
	}, variables = vars)

Help(vars.GenerateHelpText(env))

# Default environment variables
env["PLATFORM_FLAGS"] = {}

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

# Default compiler flags
if env.get("distcc", False) :
	env["ENV"]["HOME"] = os.environ["HOME"]
	env["ENV"]["DISTCC_HOSTS"] = os.environ.get("DISTCC_HOSTS", "")
	if "distcc_hosts" in env :
		env["ENV"]["DISTCC_HOSTS"] = env["distcc_hosts"]
	env["CC"] = "distcc gcc"
	env["CXX"] = "distcc g++"
if "cc" in env :
	env["CC"] = env["cc"]
if "cxx" in env :
	env["CXX"] = env["cxx"]
ccflags = env.get("ccflags", [])
if isinstance(ccflags, str) :
	# FIXME: Make the splitting more robust
	env["CCFLAGS"] = ccflags.split(" ")
else :
	env["CCFLAGS"] = ccflags
if "link" in env :
	env["SHLINK"] = env["link"]
	env["LINK"] = env["link"]
linkflags = env.get("linkflags", [])
if isinstance(linkflags, str) :
	# FIXME: Make the splitting more robust
	env["LINKFLAGS"] = linkflags.split(" ")
else :
	env["LINKFLAGS"] = linkflags
# This isn't a real flag (yet) AFAIK. Be sure to append it to the CXXFLAGS
# where you need it
env["OBJCCFLAGS"] = []
if env["optimize"] :
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = ["/O2", "/GL"])
		env.Append(LINKFLAGS = ["/INCREMENTAL:NO", "/LTCG"])
	else :
		env.Append(CCFLAGS = ["-O2"])

if env["target"] == "xcode" and os.environ["CONFIGURATION"] == "Release" :
	env.Append(CCFLAGS = ["-Os"])

if env["debug"] :
	if env["PLATFORM"] == "win32" :
		env.Append(CCFLAGS = ["/Zi", "/MDd"])
		env.Append(LINKFLAGS = ["/DEBUG"])
		if env["set_iterator_debug_level"] :
			env.Append(CPPDEFINES = ["_ITERATOR_DEBUG_LEVEL=0"])
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

if env.get("mac105", 0) :
	assert(env["PLATFORM"] == "darwin")
	env.Append(CCFLAGS = [
			"-isysroot", "/Developer/SDKs/MacOSX10.5.sdk", 
			"-arch", "i386"])
	env.Append(LINKFLAGS = [
			"-mmacosx-version-min=10.5", 
			"-isysroot", "/Developer/SDKs/MacOSX10.5.sdk", 
			"-arch", "i386"])
	env.Append(FRAMEWORKS = ["Security"])

if not env["assertions"] :
	env.Append(CPPDEFINES = ["NDEBUG"])

if env["experimental"] :
	env.Append(CPPDEFINES = ["SWIFT_EXPERIMENTAL_FT"])

# If we build shared libs on AMD64, we need -fPIC.
# This should have no performance impact om AMD64
if env["PLATFORM"] == "posix" and platform.machine() == "x86_64" :
	env.Append(CCFLAGS = ["-fPIC"])

# Warnings
if env["PLATFORM"] == "win32" :
	# TODO: Find the ideal set of warnings
	#env.Append(CCFLAGS = ["/Wall"])
	pass
else :
	env.Append(CXXFLAGS = ["-Wextra", "-Wall", "-Wnon-virtual-dtor", "-Wundef", "-Wold-style-cast", "-Wno-long-long", "-Woverloaded-virtual", "-Wfloat-equal", "-Wredundant-decls"])
	if not env.get("allow_warnings", False) :
		env.Append(CXXFLAGS = ["-Werror"])
	gccVersion = env.get("CCVERSION", "0.0.0").split(".")
	if gccVersion >= ["4", "5", "0"] and not "clang" in env["CC"] :
		env.Append(CXXFLAGS = ["-Wlogical-op"])
	if "clang" in env["CC"] :
		env.Append(CXXFLAGS = ["-W#warnings", "-Wc++0x-compat", "-Waddress-of-temporary", "-Wambiguous-member-template", "-Warray-bounds", "-Watomic-properties", "-Wbind-to-temporary-copy", "-Wbuiltin-macro-redefined", "-Wc++-compat", "-Wc++0x-extensions", "-Wcomments", "-Wconditional-uninitialized", "-Wconstant-logical-operand", "-Wdeclaration-after-statement", "-Wdeprecated", "-Wdeprecated-implementations", "-Wdeprecated-writable-strings", "-Wduplicate-method-arg", "-Wempty-body", "-Wendif-labels", "-Wenum-compare", "-Wformat=2", "-Wfour-char-constants", "-Wgnu", "-Wincomplete-implementation", "-Winvalid-noreturn", "-Winvalid-offsetof", "-Winvalid-token-paste", "-Wlocal-type-template-args", "-Wmethod-signatures", "-Wmicrosoft", "-Wmissing-declarations", "-Wnon-pod-varargs", "-Wnonfragile-abi2", "-Wnull-dereference", "-Wout-of-line-declaration", "-Woverlength-strings", "-Wpacked", "-Wpointer-arith", "-Wpointer-sign", "-Wprotocol", "-Wreadonly-setter-attrs", "-Wselector", "-Wshift-overflow", "-Wshift-sign-overflow", "-Wstrict-selector-match", "-Wsuper-class-method-mismatch", "-Wtautological-compare", "-Wtypedef-redefinition", "-Wundeclared-selector", "-Wunknown-warning-option", "-Wunnamed-type-template-args", "-Wunused-exception-parameter", "-Wunused-member-function", "-Wused-but-marked-unused", "-Wvariadic-macros"])
# To enable: 
# "-Wheader-hygiene"
#	"-Wnon-gcc",
# "-Wweak-vtables",
# "-Wlarge-by-value-copy",

if env.get("coverage", 0) :
	assert(env["PLATFORM"] != "win32")
	env.Append(CCFLAGS = ["-fprofile-arcs", "-ftest-coverage"])
	env.Append(LINKFLAGS = ["-fprofile-arcs", "-ftest-coverage"])

if env["PLATFORM"] == "win32" :
	env.Append(LIBS = ["user32", "crypt32", "dnsapi", "iphlpapi", "ws2_32", "wsock32", "Advapi32"])
	env.Append(CCFLAGS = ["/EHsc", "/nologo"])
	# FIXME: We should find a decent solution for MSVS 10
	if int(env["MSVS_VERSION"].split(".")[0]) < 10 :
		env["LINKCOM"] = [env["LINKCOM"], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;1']
		env["SHLINKCOM"] = [env["SHLINKCOM"], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2']

if env["PLATFORM"] == "darwin" and not env["target"] in ["iphone-device", "iphone-simulator", "xcode"] :
	env.Append(FRAMEWORKS = ["IOKit", "AppKit", "SystemConfiguration"])

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
for path in ["SWIFT_INSTALLDIR", "SWIFTEN_INSTALLDIR"] :
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
		env["XCODE_PLATFORM_DEVELOPER_BIN_DIR"] = "/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin"
		if target == "iphone-device":
			env["XCODE_ARCH_FLAGS"] = ["-arch", "armv6", "-arch", "armv7"]
			sdkPart = "iPhoneOS"
		else :
			env["XCODE_ARCH_FLAGS"] = ["-arch", "i386"]
			sdkPart = "iPhoneSimulator"
		sdkVer = "4.3"
		env["XCODE_SDKROOT"] = "/Developer/Platforms/" + sdkPart + ".platform/Developer/SDKs/" + sdkPart + sdkVer + ".sdk"
		env["IPHONEOS_DEPLOYMENT_TARGET"] = "4.1"

	# Set the build flags
	env["CC"] = "$XCODE_PLATFORM_DEVELOPER_BIN_DIR/gcc"
	env["CXX"] = "$XCODE_PLATFORM_DEVELOPER_BIN_DIR/g++"
	env["OBJCCFLAGS"] = ["-fobjc-abi-version=2", "-fobjc-legacy-dispatch"]
	env["LD"] = env["CC"]
	env.Append(CCFLAGS = env["XCODE_ARCH_FLAGS"] + ["-fvisibility=hidden", "-miphoneos-version-min=" + env["IPHONEOS_DEPLOYMENT_TARGET"]])
	if os.environ.get("GCC_THUMB_SUPPORT", False) :
		env.Append(CCFLAGS = ["-mthumb"])
	env.Append(LINKFLAGS = env["XCODE_ARCH_FLAGS"])
	env.Append(CPPFLAGS = ["-isysroot", "$XCODE_SDKROOT"])
	env.Append(FRAMEWORKS = ["CoreFoundation", "Foundation", "UIKit", "CoreGraphics"])
	env.Append(LINKFLAGS = env["XCODE_ARCH_FLAGS"] + ["-isysroot", "$XCODE_SDKROOT", "-L\"$XCODE_SDKROOT/usr/lib\"", "-F\"$XCODE_SDKROOT/System/Library/Frameworks\"", "-F\"$XCODE_SDKROOT/System/Library/PrivateFrameworks\""])
	# Bit of a hack, because BOOST doesn't know the endianness for ARM
	env.Append(CPPDEFINES = ["_LITTLE_ENDIAN"]) 

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
