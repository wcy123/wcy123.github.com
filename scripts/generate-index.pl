#!/usr/bin/perl
use strict;
use warnings FATAL => 'all';
use Data::Dump qw(dump);
use File::stat;
use DateTime;
use POSIX 'strftime';
my @args = @ARGV;
sub read_info {
    my $file = shift(@_);
    return {
        'filename' => $file,
        'attributes' => read_info_attr($file)
    };
}
sub read_info_attr {
    my $file = shift(@_);
    my $sb = stat($file);
    my %ret = (
        size => $sb->size,
        mtime => DateTime->from_epoch(epoch => $sb->mtime),
        pubtime => DateTime->from_epoch(epoch => $sb->mtime),
    );
    if($file =~ /([0-9]{4})-([0-9]{2})-([0-9]{2})-([^.]*)..*/){
        $ret{pubtime} = DateTime->new(
            year => $1,
            month => $2,
            day  => $3
            );
        my $title = $4;
        $title =~ s/-/ /;
        $ret{title} = $title ;
    }
    open(my $fh, "<", $file )
        or die "cannot open < $file : $!";
    my $flag=0;
    while(<$fh>) {
        if(/^---\s*$/ and $flag == 0){
            $flag = 1;
            next;
        }
        if(/^---\s*$/ and $flag == 1){
            last;
        }
        if($flag == 1) {
            chomp;
            if(/^\s*([^[:space:]]*)\s*:\s*(.*)\s*$/) {
                $ret{$1} = $2;
            }
        }
    }
    if(exists $ret{date} &&  $ret{date} =~ m{([0-9]{4})/([0-9]{2})/([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})}){
        $ret{pubtime} = DateTime->new(
            year => $1,
            month => $2,
            day  => $3,
            hour => $4,
            minute => $5,
            second => $6
            );
    }
    close($fh);
    return \%ret;
}
sub generate_output {
    my ($info)= @_;
    #print dump($info);
    my $title = $info->{attributes}{title};
    my $date = $info->{attributes}{pubtime}->strftime("%Y-%m-%d");
    $title = "无主题" if !defined($title);
    my $file = $info->{filename};
    $file =~ s/\..*$/.html/;
    print "<h3><a href=\"$file\">$date $title</a></h3>\n";
}
my @info =
    sort { DateTime->compare( $b->{attributes}{pubtime}, $a->{attributes}{pubtime} )
    } grep {
        not (exists $_->{attributes}{draft} && $_->{attributes}{draft} eq "true")
    } map {read_info $_} @args;

print "## wcy123 的主页 \n";
map {generate_output($_)} @info;

#print dump($info);
