import os, os.path

projects = [("Swift", "Swift\QtUI\Swift.exe"), ("Slimber", "Slimber\Qt\Slimber.exe")]

for (project, outputbin) in projects :
	if not os.path.exists(project) :
		os.mkdir(project)
	output = open(os.path.join(project, project + ".vcproj"), "w")

	headers = []
	sources = []
	for root, dirs, files in os.walk(os.path.join("..", "..", project)) :
		for file in files :
			if file.endswith(".h") :
				headers.append('<File RelativePath="' + os.path.join("..", root, file) + '" />')
			elif file.endswith(".cpp") :	
				sources.append('<File RelativePath="' + os.path.join("..", root, file) + '" />')

	output.write("""<?xml version="1.0" encoding="Windows-1252"?>
<VisualStudioProject
	ProjectType="Visual C++"
	Version="9.00"
	Name="%(project)s"
	Keyword="MakeFileProj"
	TargetFrameworkVersion="196613"
	>
	<Platforms>
		<Platform
			Name="Win32"
		/>
	</Platforms>
	<ToolFiles>
	</ToolFiles>
	<Configurations>
		<Configuration
			Name="Debug|Win32"
			OutputDirectory="$(ConfigurationName)"
			IntermediateDirectory="$(ConfigurationName)"
			ConfigurationType="0"
			>
			<Tool
				Name="VCNMakeTool"
				BuildCommandLine="cd ..\..\..\ &amp;&amp; scons debug=1 %(project)s"
				ReBuildCommandLine=""
				CleanCommandLine="cd ..\..\..\ &amp;&amp; scons -c debug=1 %(project)s"
				Output="..\..\..\%(output)s"
				PreprocessorDefinitions="WIN32;_DEBUG"
				IncludeSearchPath=""
				ForcedIncludes=""
				AssemblySearchPath=""
				ForcedUsingAssemblies=""
				CompileAsManaged=""
			/>
		</Configuration>
		<Configuration
			Name="Release|Win32"
			OutputDirectory="$(ConfigurationName)"
			IntermediateDirectory="$(ConfigurationName)"
			ConfigurationType="0"
			>
			<Tool
				Name="VCNMakeTool"
				BuildCommandLine="cd ..\..\..\ &amp;&amp; scons %(project)s"
				ReBuildCommandLine=""
				CleanCommandLine="cd ..\..\..\ &amp;&amp; scons -c %(project)s"
				Output="..\..\..\%(output)s"
				PreprocessorDefinitions="WIN32;NDEBUG"
				IncludeSearchPath=""
				ForcedIncludes=""
				AssemblySearchPath=""
				ForcedUsingAssemblies=""
				CompileAsManaged=""
			/>
		</Configuration>
	</Configurations>
	<References>
	</References>
	<Files>
		<Filter
			Name="Source Files"
			Filter="cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
			>
			%(sources)s
		</Filter>
		<Filter
			Name="Header Files"
			Filter="h;hpp;hxx;hm;inl;inc;xsd"
			>
			%(headers)s
		</Filter>
		<Filter
			Name="Resource Files"
			Filter="rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav"
			>
		</Filter>
	</Files>
	<Globals>
	</Globals>
</VisualStudioProject>""" % { "project": project, "output" : outputbin, "headers" : '\n'.join(headers), "sources": '\n'.join(sources) })
	output.close()
