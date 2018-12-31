#!/usr/bin/perl
use strict;
use warnings FATAL => 'all';
use Data::Dump qw(dump);
use File::stat;
use DateTime;
use File::Slurp;
use POSIX 'strftime';
my @args = @ARGV;

for my $file (@args) {
    my $c = read_file($file);
    $c =~ s:```include\nquote (.*) (.*):```{.$1 include=$2}:g;
#    $c =~ s/title:\s+"(.*)"/title: $1/g;
    $c =~ s:blog/src/:cpp_src/:g;
    write_file($file, $c);
}
#print dump($info);
