#!/usr/bin/perl -w

print "Scans and creates a map of included files relations\n";
#open(WORLDS, "C:\\Impressive Title 0.5\\media\\terrains\\Worlds.txt") || die $!;
opendir(DIR, ".\\src\\client\\include") or die $!;
open(DOT, ">.\\include-relations.gv") || die $!;
print DOT "digraph G {\n\tnode [color=\"red\", overlap=false, splines=true];\n";
print "\n";
while (my $wline = readdir(DIR)) 
	{
	$wline =~ s/\r|\n//g;
	my $nexth = ".\\src\\client\\include\\$wline";
	print "I will now open $nexth\n";
	open(WORLD, "$nexth") or next;
	while (my $line = <WORLD>) 
		{ 
		$line =~ s/\r|\n//g;
		if ($line =~ m/#include/)
			{
			if ($line =~ /.*\"(.*)".*/)
				{
				my $filename = $1;
				if (-e ".\\src\\client\\include\\$filename")
					{
					print "Including $filename in $wline\n";
					print DOT "\t\"$wline\" -> \"$filename\" [color=\"blue\"];\n";
					}
				}
			}
		}
	close WORLD;
	}
close DIR;
print DOT "}\n";
close DOT;
exit 0;
print "Running DOT";
system("dot -Lg -Tpng:cairo \"C:\\Users\\Jesper Staun Hansen\\Desktop\\IMP\\IT-portals.gv\" > \"C:\\Users\\Jesper Staun Hansen\\Desktop\\IMP\\IT-portals.png\"");
print "\n";