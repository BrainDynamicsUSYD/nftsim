#!/usr/bin/perl
# reads output from directory
# and prints the percentage change in synaptic strength wrt. ISI

$dir = $ARGV[0];
$steady = $ARGV[1];

@ls=`ls $dir`;
foreach (@ls)
{
	chomp($_);
	next if(! -d "$dir$_");
	$isi = $1 if( /=(.*)/ );
	$start = `head -n$steady "$dir$_/neurofield.synaptout.2" | tail -n1`;
	$end = `tail -n1 "$dir$_/neurofield.synaptout.2"`;
	$plast = ($end - $start)/$start*100;
	print "$isi\t$plast\n";
}
