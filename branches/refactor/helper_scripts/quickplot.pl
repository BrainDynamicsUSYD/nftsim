#!/usr/bin/perl
# prints a particular node for a particular field and plots with gnuplot

die("quickplot arguments: input file, field name, and node number") if( scalar(@ARGV)<3);

my $file = $ARGV[0];
my $name = $ARGV[1];
my $node = $ARGV[2];

open(F,$file) or die("Could not open $file");
open(O,">.".$name) or die("Could not open .quickplot.output");

my @fieldname;
my $column;

# read first line, which indicates field name
@fieldname = split( /\|/, <F> );

for( my $i=0; $i<scalar(@fieldname); $i++ ) {
	if( $fieldname[$i] =~ /$name/xi ) {
		$column = $i;
		break;
	}
}

# use the second line to check node number
@fieldname = split( /\|/, <F> );
print $node.",".$fieldname[$column+$node-1]."\n";
die("Node number incorrect!") if( $fieldname[$column+$node-1] !~ /$node/ );

# plot appropriate column
my $output = 2*$column+$node;
exec "gnuplot -e 'set nokey; plot \"$file\" using 1:$output with line' -p";
