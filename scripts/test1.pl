#!/usr/bin/perl
use strict;
use warnings FATAL => 'all';
use File::Spec;
use File::Find;

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


my @ret = all_files();
#@print join("\n", @ret);
dirname(
