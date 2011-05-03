#!/usr/bin/perl
$file1 = `ls ../../neurofield.synaptout.$ARGV[0]`;
$file2 = `ls ../../neurofield.synaptout.$ARGV[1]`;
print "$file1$file2";

open FILE1, $file1 or die $!;
open FILE2, $file2 or die $!;
open OUT, '>../../neurofield.synaptout.total' or die $!;

while(<FILE1>)
{
	$G_1 = $_; $G_2 = <FILE2>;
	$sum = $G_1 + $G_2;
	print OUT "$sum\n";
}
