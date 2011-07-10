variant_dir = SConscript("BuildTools/SCons/SConscript.boot")
print "Variant dir: " + str(variant_dir)
if variant_dir :
	variant_dir = SConscript("BuildTools/SCons/SConstruct", variant_dir = variant_dir, src_dir = ".")
else :
	variant_dir = SConscript("BuildTools/SCons/SConstruct")
