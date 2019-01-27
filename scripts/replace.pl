#!/usr/bin/perl
use strict;
use warnings FATAL => 'all';
use Data::Dump qw(dump);
use File::stat;
use DateTime;
use File::Slurp;
use POSIX 'strftime';
use warnings FATAL => 'all';
use File::Spec;
use File::Find;
use File::Basename;

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

for my $f (@ret) {
    # next if not $f =~ /.*2017-06-18.*/;
    my $dir = dirname($f);
    my $file = basename($f);
    my $workspace = $dir . "/" . substr($file, 0, 10);
    if ( not  -d $workspace ) {
        system("mkdir -p $workspace");
    }
    my $tmpfile = "/tmp/a.txt";
    rename($f , $tmpfile);
    open my $fh1 , $tmpfile or die "Could not open $tmpfile: $!";
    open my $fh2 , ">", $f  or die "Could not open $f: $!";
    while (<$fh1>) {
        if( /```{\.(.*) include=(.*)}/) {
            my $type = $1;
            my $include = $2;
            my $newname =  $workspace . "/" . basename($include);
            if (not $newname eq $include ) {
                if ( -f $include ) {
                    rename($include, $newname) or die "Could not rename $include to $newname: $!";
                }
                print $fh2 "```{.$type include=$newname}\n";
            }else {
                print $fh2 $_;
            }
        }else {
            print $fh2 $_;
        }
    }
    close($fh1);
    close($fh2);
}
