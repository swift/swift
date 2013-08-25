t = sluift.from_xml([[
	<foo xmlns='http://swift.im/test' myattr1='myval1' myattr2='myval2'>
		<bar myattr3='myval3'>
			<baz/>
		</bar>
		Some Text
		<bam myattr4='myval4'>
			Some other text
		</bam>
	</foo>
]])

print(t)
print(sluift.to_xml(t))
