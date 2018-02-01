#!/usr/bin/env ruby

require 'optparse'

def usage
	"Usage: #{$0} <path/to/class>" +
	"\n (class must not have an extension)"
end

options = {}
OptionParser.new do |opts|
	opts.banner = usage
	opts.on('-f', '--force', 'Force overwrite') do |f|
		options[:force_overwrite] = f
	end
	opts.on('-d', '--derive-from=CLASS', 'Set parent class') do |c|
		options[:parent] = c
	end
	opts.on('-h', '--help', 'Generate this help') do
		puts opts
		exit
	end
end.parse!

if ARGV.length < 1
	puts usage
	exit 1
end
spl = ARGV[0].split '/'
clsname = spl[-1]
subdir = spl[-2]
path = spl[0..-2]

includes = ""
derived = ""
etc = ""
args = ""
constr = " "
func_impl = ""
constr_content = ""

case subdir
when 'entities'
	includes = "\n#include \"Entity.hpp\"\n"
	derived = ': public lif::Entity '
	args = 'const sf::Vector2f& pos'
	etc = "\n\n\tvoid update() override;"
	constr = "\n\t: lif::Entity(pos)\n"
	func_impl = "\n\nvoid #{clsname}::update() {\n\tlif::Entity::update();\n}"
when 'components'
	includes = "\n#include \"Component.hpp\"\n"
	derived = ': public lif::Component '
	args = 'lif::Entity& owner'
	constr = "\n\t: lif::Component(owner)\n"
	constr_content = "\n\t_declComponent<#{clsname}>();\n"
end

if options[:parent]
	derived = ": public lif::#{options[:parent]} "
	constr = "\n\t: lif::#{options[:parent]}()\n"
	includes = "\n#include \"#{options[:parent]}.hpp\"\n"
	func_impl = "\n\nvoid #{clsname}::update() {\n\tlif::#{options[:parent]}::update();\n}"
end

TEMPLATE_HPP = "#pragma once
#{includes}
namespace lif {

class #{clsname} #{derived}{
public:
	explicit #{clsname}(#{args});#{etc}
};

}"

TEMPLATE_CPP = "#include \"#{clsname}.hpp\"

using lif::#{clsname};

#{clsname}::#{clsname}(#{args})#{constr}{#{constr_content}}#{func_impl}"


hppfname = "#{ARGV[0]}.hpp"
cppfname = "#{ARGV[0]}.cpp"
if !options[:force_overwrite] and (File.exists? hppfname or File.exists? cppfname)
	STDERR.puts "Error: file #{hppfname} or #{cppfname} already exists: use -f to force overwrite."
	exit 2
end

begin
	File.open(hppfname, 'w') do |f|
		f.write TEMPLATE_HPP
	end
	STDERR.puts "Written file #{hppfname}"
	File.open(cppfname, 'w') do |f|
		f.write TEMPLATE_CPP
	end
	STDERR.puts "Written file #{cppfname}"
rescue ex
	STDERR.puts "Errors writing files: #{ex}"
end
