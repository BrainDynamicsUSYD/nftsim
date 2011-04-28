#!/usr/bin/perl
@file = `ls ../../neurofield.synaptout.?`;
print @file;
die "There must be exactly 2 neurofield.synaptout.*" if( (scalar @file)!=2 );

open FILE1, $file[0] or die $!;
open FILE2, $file[1] or die $!;
open OUT, '>../../neurofield.synaptout.total' or die $!;

while(<FILE1>)
{
	$G_1 = $_; $G_2 = <FILE2>;
	$sum = $G_1 + $G_2;
	print OUT "$sum\n";
}
