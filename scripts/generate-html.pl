#!/usr/bin/perl
use strict;
use File::stat;
use warnings FATAL => 'all';
use Data::Dump qw(dump);
use File::stat;
use File::Find;
use DateTime;
use File::Basename;
use POSIX 'strftime';

sub all_files {
    my @ret= ();
    find({
        wanted => sub {
            if(/\.md$/) {
            push @ret, $File::Find::name;
            }
        }
         }, "blog");
    return @ret;
}
my @all_md = (all_files());
# print @all_md;
for my $file (@all_md) {
    #print "$file =>\n";
    my $html = $file;
    $html =~ s:blog/(.*)\.md$:deploy/$1.html:g;
    my @cmd = ('--filter', 'pandoc-include-code',
               '--from', 'markdown',
               '--template', './template.tmp',
               '--mathjax', '-c', 'writ.min.css',
               '--output', $html,
               $file);
    if( stat($html)->mtime > stat($file)->mtime) {
        # print "pandoc " , join(" ", @cmd);
    } else {
        system('pandoc', @cmd) == 0 or die("cannot convert $file to $html");
    }
}
