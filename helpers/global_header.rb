#!/usr/bin/env ruby

# This file allow project wide file header definition
require 'find'
require 'fileutils'
require 'rubygems'
require 'rainbow'

Dir.chdir File.join(File.dirname(__FILE__), '..', 'client')

license = File.open('../helpers/license.txt').read

puts license

class Source < String
  def initialize file_path
    @file_path = file_path
    replace File.open(file_path).read
  end
  
  def remove_header!
    sub! /\A\s*\/\*.+?\*\/\s*/m, ''
  end
  
  def insert_header! header
    insert 0, header
  end
  
  def write_back!
    File.open(@file_path, 'w').write self
  end
end

Find.find('.') do |file|
  # Skip svn & external files
  next if file.include? '.svn' or file.include? '/libraries/'
  # Select source & header files
  if ['.h', '.hpp', '.c', '.cpp'].include? File.extname(file)
    puts file.color(:green)
    source = Source.new file
    source.remove_header!
    source.insert_header! license
    source.write_back!
  end
end
