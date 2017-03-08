#!/usr/bin/perl
use strict;
use warnings FATAL => 'all';
use File::Spec;

my $f = File::Spec->catfile("hello/a/b/c");
print $f;
