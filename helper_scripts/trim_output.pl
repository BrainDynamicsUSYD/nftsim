#!/usr/bin/perl
# if the output in NeuroField is too long,
# shorten it to the length specified by the argument

$length=$ARGV[0];

@file = `ls neurofield.*`;
chomp(@file);

foreach(@file) {
	next if( /.proper$/ );
	next if( /^neurofield.conf$/ );
	next if( /^neurofield.dump$/ );
	`mv $_ "$_.proper"` unless( -e "$_.proper" );
	my $orig_length = `wc -l "$_.proper"`;
	my $space = $orig_length/$length;
	my $line = 0;
	open IN, "<$_.proper" or die $!;
	open OUT, ">$_" or die $!;
	if( /^neurofield.output$/ ) {
		while( $read = <IN> ) {
			if( $read =~ /^Deltat:\s*(.*)$/ ) {
				$deltat = $1;
			} elsif( $read =~ /^Number of integration steps:\s*(.*)$/ ) {
				$space = $1/$length;
				$deltat = $deltat*$1/$length;
				print OUT "Deltat: $deltat\n";
				print OUT "Number of integration steps:$length\n";
			} elsif( $read =~ /Output Data - Number of traces:\s*(.*)$/ ) {
				$ntrace = $1;
				print OUT "$read";
			} elsif(( $read =~ /^\d/ )||( $read =~ /^$/ )) {
				if( $line%($space*($ntrace+1)) <$ntrace+1 ) {
					print OUT $read;
				}
				$line++;
			} else {
				print OUT "$read";
			}
		}
	} else {
		while( $read = <IN> ) {
			print OUT $read if( !$line );
			$line = ($line+1) % $space;
		}
	}
	close IN;
	close OUT;
}
