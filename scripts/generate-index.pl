#!/usr/bin/perl
use strict;
use warnings FATAL => 'all';
use Data::Dump qw(dump);
use File::stat;
use File::Find;
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
# 更新日期：
#  1. 首先看文件内容，里面有没有 date 的属性。
#  2. 然后看文件名
#  3. 最后看文件的修改日期
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
                my $att = $1;
                my $val = $2;
                if ($att eq "title") {
                    $val =~ s/"//g;
                    $ret{$att} = $val;
                }else {
                    $ret{$att} = $val;
                }
                # print "$att => $val\n";
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

my %header = (
    "index.md" =>
    "---\n" .
    "title:  \"wcy123 的个人主页\"\n" .
    "---\n" .
    "\n" .
    "\n"
    ,
    "daughter.md" =>
    "---\n" .
    "title:  \"随机数学笔记\"\n" .
    "---\n" .
    "\n" .
    "\n"
    );

my %opened_file = ();
sub generate_output {
    my ($info)= @_;
    return 0  if exists $info->{attributes}{draft} and $info->{attributes}{draft} eq "true";
    #print dump($info);
    my $title = $info->{attributes}{title};
    my $index_file = $info->{attributes}{index} || "index.md";
    my $date = $info->{attributes}{pubtime}->strftime("%Y-%m-%d");
    $title = "无主题" if !defined($title);
    my $file = $info->{filename};
    $file =~ s/(.*)\.md$/$1.html/;
    my $fh = $opened_file{$index_file};
    if(!$fh) {
        open($fh, '>', $index_file);
        $opened_file{$index_file} = $fh;
        print $fh $header{$index_file}||"";
    }
    print $fh "<h3><a href=\"$file\">$date $title</a></h3>\n";
}

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

my @info =
    sort { DateTime->compare( $b->{attributes}{pubtime}, $a->{attributes}{pubtime} )
    } grep {
        not (exists $_->{attributes}{draft} && $_->{attributes}{draft} eq "true")
} map {read_info $_} all_files();


#print "## wcy123 的主页 \n";
map {generate_output($_)} @info;

#print dump($info);
