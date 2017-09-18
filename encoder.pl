#!/usr/bin/env perl

use strict;
use warnings;
use IO::File;

my $file = "player/obj/local/armeabi/event-player";

open(my $fh, '<', $file) or die "Cannot open file: ${file}: ${!}";

binmode $fh;
my $data = do { local $/; <$fh> };
close $fh;

my $text = pack('u*', $data);
my $out = IO::File->new("encoded_player", "w");
$out->print($text);
