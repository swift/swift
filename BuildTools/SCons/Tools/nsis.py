import re, os
import SCons.Util
nsisFiles_re = re.compile(r'^\s*File "([^"]*)"', re.M)

"""
TODO:
	- Extract the target from the nsis file
	- When a target is provided use the output function
"""

def generate(env) :
	"""Add Builders and construction variables for qt to an Environment."""
	print "Loading nsis tool..."

	Builder = SCons.Builder.Builder

	env['NSIS_MAKENSIS'] = 'makensis'
	env['NSIS_OPTIONS'] = ''
	def winToLocalReformat(path) :
		return os.path.join(*path.split("\\"))
	def scanNsisContent(node, env, path, arg):
		contents = node.get_contents()
		includes = nsisFiles_re.findall(contents)
		includes = [ winToLocalReformat(include) for include in includes ]
		return filter(lambda x: x.rfind('*')==-1, includes)
	nsisscanner = env.Scanner(name = 'nsisfile',
		function = scanNsisContent,
		argument = None,
		skeys = ['.nsi'])
	nsisbuilder = Builder(
		action = '$NSIS_MAKENSIS $NSIS_OPTIONS $SOURCE',
		source_scanner = nsisscanner,
		single_source = True
		)
	env.Append( BUILDERS={'Nsis' : nsisbuilder} )

def exists(env) :
	return True

