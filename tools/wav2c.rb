#!/usr/bin/env ruby
# convert a wav file into a C struct
require 'rubygems'
require 'wavefile'

MAX_WIDTH = 100

def headerfile_top(n)
  sprintf(<<EOF, n)
#ifndef __included_voices_h
#define __included_voices_h

typedef int16_t audioBuf_t;

typedef struct {
    audioBuf_t const *voice_data;
    uint16_t voice_nsamples;
    const char *voice_name;
};

extern voice_t allVoices[%d];

EOF
end

$voices = []
# $\ = $/ = "\r\n"

File.open("voices.h", 'w') do |headerfile|
  headerfile.print headerfile_top(ARGV.size)
  File.open("voices.c", 'w') do |srcfile|
    srcfile.puts("#include \"voices.h\"")
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

      $voices << [ "voice_" + basename, w.sample_data.size, arg ]

      headerfile.puts("// File:            #{arg}")
      headerfile.puts(w.inspect.gsub(/^/, '// '))
      headerfile.puts("extern audioBuf_t const voice_#{basename}[#{w.sample_data.size}];")
      headerfile.puts

      srcfile.puts
      srcfile.puts("audioBuf_t const voice_#{basename}[#{w.sample_data.size}] = {")

      startofline = srcfile.tell
      w.sample_data.each do |s|
        bytes += 2
        srcfile.printf("%d, ", s)
        if srcfile.tell - startofline >= MAX_WIDTH
          srcfile.puts
          startofline = srcfile.tell
        end
      end
      srcfile.seek(-2, IO::SEEK_CUR)
      srcfile.puts(" };")
    end
    srcfile.puts("\nvoice_t allVoices[#{ARGV.size}] = {")
    $voices.each { |v| srcfile.puts("    { #{v[0]}, #{v[1]}, \"#{v[2]}\" },") }
    srcfile.puts("};")
    srcfile.puts
    headerfile.puts("// Total bytes: #{bytes}")
    puts "\nTotal bytes: #{bytes}"
  end
  headerfile.puts("#endif")
end
