import SCons.Util, os
import subprocess
import re
import shutil

def which(program_name):
	if hasattr(shutil, "which"):
		return shutil.which(program_name)
	else:
		path = os.getenv('PATH')
		for p in path.split(os.path.pathsep):
			p = os.path.join(p,program_name)
			if os.path.exists(p) and os.access(p,os.X_OK):
				return p

def generate(env) :
	def captureWinDeployQtMapping(release = True):
		p = False

		qt_bin_folder = ""
		if "QTDIR" in env:
			qt_bin_folder = env["QTDIR"] + "\\bin;"

		environ = {"PATH": qt_bin_folder + os.getenv("PATH"), "TEMP": os.getenv("TEMP"), "TMP": os.getenv("TMP")}

		if release:
			p = subprocess.Popen(['windeployqt', '--release', '--dry-run', '--list', 'mapping', 'Swift.exe'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=environ)
		else:
			p = subprocess.Popen(['windeployqt', '--debug', '--dry-run', '--list', 'mapping', 'Swift.exe'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=environ)

		if p:
			stdout, stderr = p.communicate()

			mappings = []

			p = re.compile(ur'"([^\"]*)" "([^\"]*)"')

			matches = re.findall(p, stdout)
			for match in matches:
				mappings.append(match)
			return mappings
		else:
			return False

	def createWindowsBundleManual(env, bundle, resources = {}, qtplugins = {}, qtlibs = [], qtversion = '4') :
		all_files = []
		all_files += env.Install(bundle, bundle + ".exe")
		for lib in qtlibs :
			all_files += env.Install(bundle, os.path.join(env["QTDIR"], "bin", lib + ".dll"))
		plugins_suffix = '4'
		if qtversion == '5' :
			plugins_suffix = ''
		for plugin_type in qtplugins:
			all_files += env.Install(os.path.join(bundle, plugin_type), [os.path.join(env["QTDIR"], "plugins", plugin_type, "q" + plugin + plugins_suffix + ".dll") for plugin in qtplugins[plugin_type]])
		for dir, resourceFiles in resources.items() :
			for resource in resourceFiles :
				e = env.Entry(resource)
				if e.isdir() :
					for subresource in env.Glob(str(e) + "/*") :
						all_files += env.Install(os.path.join(bundle, dir, e.name), subresource)
				else :
					all_files += env.Install(os.path.join(bundle, dir), resource)
		return all_files

	# This version of uses windeployqt tool
	def createWindowsBundleWithWinDeployQt(env, bundle, resources = {}, qtplugins = {},  qtlibs = [],  qtversion = '4') :
		assert(qtversion == '5')
		all_files = []

		# add swift executable
		all_files += env.Install(bundle, bundle + ".exe")

		# adding resources (swift sounds/images/translations)
		for dir, resourceFiles in resources.items() :
			for resource in resourceFiles :
				e = env.Entry(resource)
				if e.isdir() :
					for subresource in env.Glob(str(e) + "/*") :
						all_files += env.Install(os.path.join(bundle, dir, e.name), subresource)
				else :
					all_files += env.Install(os.path.join(bundle, dir), resource)

		qtmappings = captureWinDeployQtMapping()
		assert(qtmappings)

		# handle core DLLs
		qt_corelib_regex = re.compile(ur".*bin.*\\(.*)\.dll")

		for qtlib in qtlibs:
			if qtlib.startswith("Qt5"):
				(src_path, target_path) = next(((src_path, target_path) for (src_path, target_path) in qtmappings if  qt_corelib_regex.match(src_path) and qt_corelib_regex.match(src_path).group(1) == qtlib), (None, None))
				if src_path != None:
					all_files += env.Install(bundle, src_path)

		# handle core dependencies
		for (src_path, target_path) in qtmappings:
			if qt_corelib_regex.match(src_path) and not qt_corelib_regex.match(src_path).group(1).startswith("Qt5"):
				all_files += env.Install(bundle, src_path)

		# handle plugins
		qt_plugin_regex = re.compile(ur".*plugins.*\\(.*)\\(.*)\.dll")
		for (src_path, target_path) in qtmappings:
			if qt_plugin_regex.match(src_path):
				plugin_folder, filename = qt_plugin_regex.match(src_path).groups()
				try:
					if filename[1:] in qtplugins[plugin_folder]:
						all_files += env.Install(os.path.join(bundle, plugin_folder), src_path)
				except:
						pass
		return all_files

	def createWindowsBundle(env, bundle, resources = {}, qtplugins = {}, qtlibs = [], qtversion = '4'):
		if which("windeployqt.exe"):
			return createWindowsBundleWithWinDeployQt(env, bundle, resources, qtplugins, qtlibs, qtversion)
		else:
			return createWindowsBundleManual(env, bundle, resources, qtplugins, qtlibs, qtversion)

	env.AddMethod(createWindowsBundle, "WindowsBundle")

def exists(env) :
	return env["PLATFORM"] == "win32"

