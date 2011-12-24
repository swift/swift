import re, os
import SCons.Util
from subprocess import call

def generate(env) :
	wixPath = env.get("wix_bindir", "")
	if len(wixPath) > 0 and wixPath[-1] != "\\":
		wixPath += "\\"
	env['WIX_HEAT'] = wixPath + 'heat.exe'
	env['WIX_HEAT_OPTIONS'] = '-gg -sfrag -suid -template fragment -dr ProgramFilesFolder'
	env['WIX_CANDLE'] = wixPath + 'candle.exe'
	env['WIX_CANDLE_OPTIONS'] = ''
	env['WIX_LIGHT'] = wixPath + 'light.exe'
	env['WIX_LIGHT_OPTIONS'] = '-ext WixUIExtension'

	def WiX_IncludeScanner(source, env, path, arg):
		wixIncludeRegexp = re.compile(r'^\s*\<\?include (\S+.wxs)\s*\?\>\S*', re.M)
		contents = source.get_contents()
		includes = wixIncludeRegexp.findall(contents)
		return [ "" + include for include in includes ]

	heat_builder = SCons.Builder.Builder(
 		action = '"$WIX_HEAT" dir Swift\\QtUI\\Swift -cg Files $WIX_HEAT_OPTIONS -o ${TARGET} -t Swift\\Packaging\\WiX\\include.xslt',
		suffix = '.wxi')


	candle_scanner = env.Scanner(name = 'wixincludefile',
		function = WiX_IncludeScanner,
		argument = None,
		skeys = ['.wxs'])

	candle_builder = SCons.Builder.Builder(
 		action = '"$WIX_CANDLE" $WIX_CANDLE_OPTIONS ${SOURCES} -o ${TARGET}',
		src_suffix = '.wxs',
		suffix = '.wixobj',
		source_scanner = candle_scanner,
 		src_builder = heat_builder)


	light_builder = SCons.Builder.Builder(
 		action = '"$WIX_LIGHT" $WIX_LIGHT_OPTIONS -b Swift\\QtUI\\Swift ${SOURCES} -o ${TARGET}',
		src_suffix = '.wixobj',
 		src_builder = candle_builder)

	env['BUILDERS']['WiX_Heat'] = heat_builder
	env['BUILDERS']['WiX_Candle'] = candle_builder
	env['BUILDERS']['WiX_Light'] = light_builder

def exists(env) :
	return True

