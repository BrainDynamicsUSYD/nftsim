#!/usr/bin/perl
# prints a particular node for a particular field and plots with gnuplot

#die("quickplot arguments: input file, field name, and node number") if( scalar(@ARGV)<3);

my $file = $ARGV[0];
my $name = $ARGV[1];
my $node = $ARGV[2];

open(F,$file) or die("Could not open $file");

my @fieldname;
my $column;

# if argument is "All", then plot all fields
if( $name =~ /all/xi ) {
	@fieldname = split( / +/, <F> );
	<F>; # throw away second line;
	foreach my $i (2..$#fieldname) {
		system "gnuplot -e 'set terminal wxt size 300,240; set nokey; set title \"$fieldname[$i]\"; set xlabel \"Time (s)\"; plot \"$file\" using 1:$i with line lt rgb \"black\"' -p";
	}
	exit;
}

# read first line, which indicates field name
@fieldname = split( / +/, <F> );

for( my $i=0; $i<scalar(@fieldname); $i++ ) {
	if( $fieldname[$i] =~ /$name/xi ) {
		$column = $i;
		last;
	}
}

# use the second line to check node number
@fieldname = split( / +/, <F> );
die("Node number incorrect!") if( $fieldname[$column+$node-2] !~ /$node/ );

# plot appropriate column
my $output = $column+$node-1;
exec "gnuplot -e 'set terminal wxt size 300,240; set nokey; set title \"$name\"; set xlabel \"Time (s)\"; plot \"$file\" using 1:$output with line lt rgb \"black\"' -p";
