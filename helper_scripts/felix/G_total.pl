#!/usr/bin/perl
$dir = $ARGV[0];
$file1 = `ls $dir/neurofield.synaptout.$ARGV[1]`;
$file2 = `ls $dir/neurofield.synaptout.$ARGV[2]`;

open FILE1, $file1 or die $!;
open FILE2, $file2 or die $!;
open OUT, ">$dir/neurofield.synaptout.total" or die $!;

while(<FILE1>)
{
	$G_1 = $_; $G_2 = <FILE2>;
	$sum = $G_1 + $G_2;
	print OUT "$sum\n";
}

close FILE1, FILE2, OUT;
