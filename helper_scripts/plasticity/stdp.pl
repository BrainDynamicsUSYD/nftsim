#!/usr/bin/perl
# reads output from current directory
# and prints the percentage change in synaptic strength wrt. ISI

@ls=`ls`;
foreach (@ls)
{
	chomp($_);
	next if(! -d $_);
	$isi = $1 if( /=(.*)/ );
	$start = `head -n1 "$_/neurofield.synaptout.2"`;
	$end = `tail -n1 "$_/neurofield.synaptout.2"`;
	$plast = ($end - $start)/$start*100;
	print "$isi\t$plast\n";
}
