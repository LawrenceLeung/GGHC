#!/usr/bin/env ruby
# convert a wav file into a C struct
require 'rubygems'
require 'wavefile'

MAX_WIDTH = 100

File.open("voices.h", 'w') do |headerfile|
  headerfile.print("#ifndef __included_voices_h\r\n#define __included_voices_h\r\n\r\n")
  headerfile.print("typedef int16_t audioBuf_t;\r\n\r\n")
  File.open("voices.c", 'w') do |srcfile|
    srcfile.print("#include \"voices.h\"\r\n");
    bytes = 0
    ARGV.each do |arg|
      dir, fn = File.split(arg)
      basename = fn.sub(/\.[^.]+$/, '')

      puts "\nFILE: #{arg}"

      w = WaveFile.open(arg)
      puts "INPUT: " + w.inspect

      w.bits_per_sample = 16
      w.num_channels = 1

      puts "OUTPUT: " + w.inspect

      headerfile.print("\r\n// File:            #{arg}\r\n" +
                       w.inspect.gsub(/^/, '// ').gsub("\n", "\r\n"))
      headerfile.print("extern audioBuf_t const voice_#{basename}[#{w.sample_data.size}];\r\n")
      srcfile.print("audioBuf_t const voice_#{basename}[#{w.sample_data.size}] = {\r\n")

      startofline = srcfile.tell
      w.sample_data.each do |s|
        bytes += 2
        srcfile.printf("%d, ", s)
        if srcfile.tell - startofline >= MAX_WIDTH
          srcfile.print("\r\n")
          startofline = srcfile.tell
        end
      end
      srcfile.seek(-2, IO::SEEK_CUR)
      srcfile.print("\r\n};\r\n")
      end
      headerfile.print("\r\n// Total bytes: #{bytes}\r\n\r\n")
      puts "\nTotal bytes: #{bytes}"
    end
  headerfile.print("#endif\r\n")
end
