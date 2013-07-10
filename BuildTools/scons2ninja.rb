#!/usr/bin/env ruby

################################################################################
#
# scons2ninja: A script to create a Ninja build file from SCons.
#
# Copyright (c) 2013 Remko Tronçon
# Licensed under the simplified BSD license.
# See COPYING for details.
#
################################################################################

require 'pathname'
require 'open3'


################################################################################
# Helper for building Ninja files
################################################################################

class NinjaBuilder
  attr_reader :targets

  def initialize
    @header = ""
    @variables = ""
    @rules = ""
    @build = ""
    @pools = ""
    @flags = Hash.new{ |h,k| h[k] = Hash.new() }
    @targets = []
  end

  def header(text)
    @header << text << "\n"
  end

  def rule(name, opts = {})
    @rules << "rule #{name}\n"
    opts.each { |k, v| @rules << "  " << k.to_s << " = " << v.to_s << "\n" }
    @rules << "\n"
  end

  def pool(name, opts = {})
    @pools << "pool #{name}\n"
    opts.each { |k, v| @pools << "  " << k.to_s << " = " << v.to_s << "\n" }
    @pools << "\n"
  end

  def variable(name, value)
    @variables << "#{name} = #{value}\n"
  end

  def build(target, rule, sources = nil, opts = {})
    @build << "build " << str(target) << ": " << rule
    @build << " " << str(sources) if sources
    @build << " | " << str(opts[:deps]) if opts[:deps]
    @build << " || " << str(opts[:order_deps]) if opts[:order_deps]
    @build << "\n"
    opts.each do |var, value| 
      next if [:deps, :order_deps].include? var
      var = var.to_s
      value = str(value)
      value = get_flags_variable(var, value) if var.end_with? "flags"
      @build << "  #{var} = #{value}\n"
    end
    @targets += list(target)
  end

  def header_targets
    @targets.select { |target| target.end_with? '.h' or target.end_with? '.hh' }
  end

  def to_s
    result = ""
    result << @header << "\n"
    result << @variables << "\n"
    @flags.each { |_, prefix| prefix.each { |k, v| result << "#{v} = #{k}\n" } }
    result << "\n"
    result << @pools << "\n"
    result << @rules << "\n"
    result << @build << "\n"
    result
  end

  private
    def str(list) 
      return list.map{ |x| escape(x) }.join(' ') if list.is_a? Enumerable
      return @targets.select { |x| list.match(x) }.map { |x| escape(x) }.join(' ') if list.is_a? Regexp
      list
    end

    def escape(s)
      s.gsub(/ /, '$ ')
    end

    def get_flags_variable(type, flags)
      return '' if flags.empty?
      type_flags = @flags[type]
      unless id = type_flags[flags]
        id = "#{type}_#{type_flags.size()}"
        type_flags[flags] = id
      end
      "$#{id}"
    end
end

################################################################################
# Helper methods & variables
################################################################################
      
if RUBY_PLATFORM =~ /(win32|mingw32)/
  LIB_PREFIX = ""
  LIB_SUFFIX = ""
  EXE_SUFFIX = ".exe"
else
  LIB_PREFIX = "lib"
  LIB_SUFFIX = ".a"
  EXE_SUFFIX = ""
end

def list(l)
  return [] if nil
  return l if l.is_a? Enumerable
  [l]
end

def get_unary_flags(prefix, flags)
  flags.select {|x| /^#{prefix}/i.match(x)}.map { |x| x[prefix.size .. -1] }
end

def extract_unary_flags(prefix, flags)
  flag, flags = flags.partition { |x| /^#{prefix}/i.match(x) }
  [flag.map { |x| x[prefix.size .. -1] }, flags]
end

def extract_unary_flag(prefix, flags)
  flag, flags = extract_unary_flags(prefix, flags)
  [flag[0], flags]
end

def extract_binary_flag(prefix, flags)
  i = flags.index(prefix)
  flag = flags[i + 1]
  flags.delete_at(i)
  flags.delete_at(i)
  [flag, flags]
end

BINARY_FLAGS = ["-framework", "-arch", "-x", "--output-format", "-isystem", "-include"]

def get_non_flags(flags)
  skip = false
  result = []
  flags.each do |f|
    if skip
      skip = false
    elsif BINARY_FLAGS.include? f
      skip = true
    elsif not f.start_with? "/" and not f.start_with? "-"
      result << f
    end
  end
  result
end

def extract_non_flags(flags)
  non_flags = get_non_flags(flags)
  [non_flags, flags - non_flags]
end

def to_native_path(path) 
  path.gsub(File::SEPARATOR, File::ALT_SEPARATOR || File::SEPARATOR)
end

def from_native_path(path) 
  path.gsub(File::ALT_SEPARATOR || File::SEPARATOR, File::SEPARATOR)
end

def get_dependencies(target, build_targets)
  result = []
  queue = $dependencies[target].dup
  while queue.size > 0
    n = queue.pop
    result << n
    queue += $dependencies[n].dup
  end
  # Filter out Value() results
  result.select {|x| build_targets.include? x or File.exists? x }
end

def get_built_libs(libs, libpaths, outputs)
  canonical_outputs = outputs.map {|p| File.expand_path(p) }
  result = []
  libpaths.each do |libpath| 
    libs.each do |lib|
      lib_libpath = Pathname.new(libpath) + "#{LIB_PREFIX}#{lib}#{LIB_SUFFIX}"
      if canonical_outputs.include? lib_libpath.expand_path.to_s
        result << to_native_path(lib_libpath.to_s)
      end
    end
  end
  result
end

script = to_native_path($0)

################################################################################
# Configuration
################################################################################

$ninja_post = []
$scons_cmd = "scons"
$scons_dependencies = Dir['SConstruct'] + Dir['**/SConscript']

def ninja_post (&block)
  $ninja_post << block
end


CONFIGURATION_FILE = '.scons2ninja.conf'

load CONFIGURATION_FILE

$scons_dependencies = $scons_dependencies.map {|x| to_native_path(x) }

################################################################################
# Rules
################################################################################

ninja = NinjaBuilder.new

ninja.pool 'scons_pool', depth: 1

if RUBY_PLATFORM =~ /(win32|mingw32)/
  ninja.rule 'cl', 
    deps: 'msvc', 
    command: '$cl /showIncludes $clflags -c $in /Fo$out',
    description: 'CXX $out'

  ninja.rule 'link',
    command: '$link $in $linkflags $libs /out:$out',
    description: 'LINK $out'

  ninja.rule 'lib',
    command: '$lib $libflags /out:$out $in',
    description: 'AR $out'

  ninja.rule 'rc',
    command: '$rc $rcflags /Fo$out $in',
    description: 'RC $out'

  # SCons doesn't touch files if they didn't change, which makes
  # ninja rebuild the file over and over again. There's no touch on Windows :(
  # Could implement it with a script, but for now, delete the file if
  # this problem occurs. I'll fix it if it occurs too much.
  ninja.rule 'scons',
    command: "#{$scons_cmd} $out",
    pool: 'scons_pool',
    description: 'GEN $out'

  ninja.rule 'install', command: 'cmd /c copy $in $out'
  ninja.rule 'run', command: '$in'
else
  ninja.rule 'cxx',
    deps: 'gcc',
    depfile: '$out.d',
    command: '$cxx -MMD -MF $out.d $cxxflags -c $in -o $out',
    description: 'CXX $out'

  ninja.rule 'cc',
    deps: 'gcc',
    depfile: '$out.d',
    command: '$cc -MMD -MF $out.d $ccflags -c $in -o $out',
    description: 'CC $out'

  ninja.rule 'link',
    command: '$glink -o $out $in $linkflags',
    description: 'LINK $out'

  ninja.rule 'ar',
    command: 'ar $arflags $out $in && ranlib $out',
    description: 'AR $out'

  # SCons doesn't touch files if they didn't change, which makes
  # ninja rebuild the file over and over again. Touching solves this.
  ninja.rule 'scons',
    command: "#{$scons_cmd} $out && touch $out",
    pool: 'scons_pool',
    description: 'GEN $out'

  ninja.rule 'install', command: 'install $in $out'
  ninja.rule 'run', command: './$in'
end

ninja.rule 'moc',
  command: '$moc $mocflags -o $out $in',
  description: 'MOC $out'

ninja.rule 'rcc',
  command: '$rcc $rccflags -name $name -o $out $in',
  description: 'RCC $out'

ninja.rule 'uic',
  command: '$uic $uicflags -o $out $in',
  description: 'UIC $out'

ninja.rule 'lrelease',
  command: '$lrelease $lreleaseflags $in -qm $out',
  description: 'LRELEASE $out'

ninja.rule 'ibtool',
  command: '$ibtool $ibtoolflags --compile $out $in',
  description: 'IBTOOL $out'

ninja.rule 'generator',
  command: "ruby #{script} ${generator_args}",
  pool: 'scons_pool',
  generator: '1',
  description: 'Regenerating build.ninja'


################################################################################
# Build Statements
################################################################################

generator_args = ARGV.join(' ')
scons_generate_cmd = "#{$scons_cmd} #{generator_args} --tree=all,prune dump_trace=1"
#scons_generate_cmd = 'cmd /c type scons2ninja.in'
#scons_generate_cmd = 'cat scons2ninja.in'

# Pass 1: Parse dependencies (and prefilter some build rules)
build_lines = []
$dependencies = Hash.new {|h, k| h[k] = [] }
previous_file = nil
Open3.popen3(scons_generate_cmd) do |stdin, f, stderr, thread|
  stage = :preamble
  skip_nth_line = -1
  stack = ['.']
  f.each_line do |line|
    # Skip lines if requested from previous command
    skip_nth_line -= 1 if skip_nth_line >= 0
    next if skip_nth_line == 0

    line.chop!

    break if line.start_with? 'scons: done building targets'

    case stage
      # Pass all lines from the SCons configuration step to output
      when :preamble
        if /^scons: Building targets .../.match(line)
          stage = :build
        else
          puts line
        end

      when :build
        if line.start_with? '+-'
          stage = :dependencies
        # Ignore response files from MSVS
        elsif /^Using tempfile/.match(line)
          skip_nth_line = 2
        else
          build_lines << line
        end

      when :dependencies
        # Work around bug in SCons that splits output over multiple lines
        next unless /^[\s|]+\+\-/.match(line)

        level = line.index('+-') / 2
        file = line[level*2+2..-1]
        file = file[1..-2] if file.start_with? '['

        # Check if we use the 'fixed' format which escapes filenames
        file = eval('"' + file[1..-2].gsub('"', '\\"') + '"') if file.start_with? '\''

        if level < stack.length
          stack = stack[0..level-1]
        elsif level > stack.length
          raise "Internal Error" if level != stack.length + 1
          stack << previous_file
        end
        # Skip absolute paths
        $dependencies[stack[-1]] << file unless Pathname.new(file).absolute?
        previous_file = file
    end
  end

  unless thread.value.success?
    print "Error calling '#{scons_generate_cmd}': "
    print stderr.read
    exit(-1)
  end
end

# Pass 2: Parse build rules
tools = {}
build_lines.each do |line|
  # Custom python function
  if m = /^(\w+)\(\[([^\]]*)\]/.match(line)
    out = m[2].split(',').map { |x| x[1..-2] }
    out.each do |x| 
      # Note: To be more correct, deps should also include $scons_dependencies,
      # but this regenerates a bit too often, so leaving it out for now.
      ninja.build x, 'scons', nil, deps: get_dependencies(x, ninja.targets)
    end

  # TextFile
  elsif m = /^Creating '([^']+)'/.match(line)
    out = m[1]
    # Note: To be more correct, deps should also include $scons_dependencies,
    # but this regenerates a bit too often, so leaving it out for now.
    ninja.build out, 'scons', nil, deps: get_dependencies(out, ninja.targets)

  # Install
  elsif m = /^Install file: "(.*)" as "(.*)"/.match(line)
    ninja.build m[2], 'install', m[1]

  elsif m = /^Install directory: "(.*)" as "(.*)"/.match(line)
    Dir["#{m[1]}/**"].each do |file|
      source = Pathname.new(file)
      native_source = to_native_path(source.to_s)
      target = Pathname.new(m[2]) + source.relative_path_from(Pathname.new(m[1]))
      native_target = to_native_path(target.to_s)
      ninja.build native_target, 'install', native_source
    end

  # Tools
  else
    command = line.split
    flags = command[1..-1]
    tool = File.basename(command[0], File.extname(command[0]))
    tool = "cxx" if ["clang++", "g++"].include? tool
    tool = "cc" if ["clang", "gcc"].include? tool
    tool = "glink" if ["cc", "cxx"].include? tool and not flags.include? "-c"
    tool.gsub!(/-qt4$/, '')
    tools[tool] = command[0]

    case tool

      ############################################################
      # clang/gcc tools
      ############################################################

      when 'cc'
        out, flags = extract_binary_flag("-o", flags)
        files, flags = extract_non_flags(flags)
        ninja.build out, 'cc', files, order_deps: '_generated_headers', ccflags: flags

      when 'cxx'
        out, flags = extract_binary_flag("-o", flags)
        files, flags = extract_non_flags(flags)
        ninja.build out, 'cxx', files, order_deps: '_generated_headers', cxxflags: flags

      when 'glink'
        out, flags = extract_binary_flag("-o", flags)
        files, flags = extract_non_flags(flags)
        libs = get_unary_flags('-l', flags)
        libpaths = get_unary_flags("-L", flags)
        dependencies = get_built_libs(libs, libpaths, ninja.targets)
        ninja.build out, 'link', files, deps: dependencies, linkflags: flags

      when 'ar'
        objects, flags = flags.partition { |x| x.end_with? ".o" }
        libs, flags = flags.partition { |x| x.end_with? ".a" }
        out = libs[0]
        ninja.build out, 'ar', objects, arflags: flags

      when 'ranlib'


      ############################################################
      # MSVC tools
      ############################################################
      
      when 'cl'
        out, flags = extract_unary_flag("/Fo", flags)
        files, flags = extract_non_flags(flags)
        ninja.build out, 'cl', files, order_deps: '_generated_headers', clflags: flags

      when 'lib'
        out, flags = extract_unary_flag("/out:", flags)
        files, flags = extract_non_flags(flags)
        ninja.build out, 'lib', files, libflags: flags

      when 'link'
        objects, flags = flags.partition { |x| x.end_with? ".obj" }
        out, flags = extract_unary_flag("/out:", flags)
        libs, flags = flags.partition { |x| not x.start_with? "/" and x.end_with? ".lib" }
        libpaths = get_unary_flags("/libpath:", flags)
        dependencies = get_built_libs(libs, libpaths, ninja.targets)
        ninja.build out, 'link', objects, deps: dependencies, 
          libs: libs, linkflags: flags

      when 'rc'
        out, flags = extract_unary_flag("/fo", flags)
        files, flags = extract_non_flags(flags)
        ninja.build out, 'rc', files[0], order_deps: '_generated_headers', rcflags: flags

      ############################################################
      # Qt tools
      ############################################################
      
      when 'moc'
        out, flags = extract_binary_flag("-o", flags)
        files, flags = extract_non_flags(flags)
        ninja.build out, 'moc', files, mocflags: flags

      when 'uic'
        out, flags = extract_binary_flag("-o", flags)
        files, flags = extract_non_flags(flags)
        ninja.build out, 'uic', files, uicflags: flags

      when 'lrelease'
        out, flags = extract_binary_flag("-qm", flags)
        files, flags = extract_non_flags(flags)
        ninja.build out, 'lrelease', files, lreleaseflags: flags

      when 'rcc'
        out, flags = extract_binary_flag("-o", flags)
        name, flags = extract_binary_flag("-name", flags)
        files, flags = extract_non_flags(flags)
        deps = get_dependencies(out, ninja.targets) - files
        ninja.build out, 'rcc', files, deps: deps, name: name, rccflags: flags

      ############################################################
      # OS X tools
      ############################################################

      when 'ibtool'
        out, flags = extract_binary_flag("--compile", flags)
        files, flags = extract_non_flags(flags)
        ninja.build out, 'ibtool', files, ibtoolflags: flags

      else
        raise "Unknown tool: '#{line}'"
    end
  end
end

# Phony target for all generated headers, used as an order-only dependency from all C/C++ sources
ninja.build '_generated_headers', 'phony', ninja.header_targets

# Regenerate build.ninja file
ninja.build 'build.ninja', 'generator', [], deps: [script, CONFIGURATION_FILE] + $scons_dependencies

# Header & variables
ninja.header "# This file is generated by #{script}"
ninja.variable "ninja_required_version", "1.3"
ninja.variable "generator_args", generator_args
tools.each { |k, v| ninja.variable k, v }

# Extra customizations
$ninja_post.each { |p| p.call(ninja) }

################################################################################
# Result
################################################################################

File.open('build.ninja', 'w') { |f| f.write ninja }
