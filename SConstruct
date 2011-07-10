variant_dir = SConscript("BuildTools/SCons/SConscript.boot")
if variant_dir :
	SConscript("BuildTools/SCons/SConstruct", variant_dir = variant_dir, src_dir = ".")
else :
	SConscript("BuildTools/SCons/SConstruct")
