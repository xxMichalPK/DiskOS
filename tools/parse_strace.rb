#!/usr/bin/ruby

num = ARGV[0]
dbs = ARGV[1]
dcount = ARGV[2]
STDIN.each { |line|
  if (line.match(/write\(#{ num },\s*"(.*?)"/)) then
    text = $1.split('\x').map { |s| s.to_i(16).chr }.join()
    text = text.split.first.to_s  
    text.gsub(/\s+/, "").scan(/\d+/) do |x|
      system("./progress #{x.to_f / dbs.to_i / dcount.to_i * 100}")
    end
  end
}
