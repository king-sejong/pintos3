# -*- perl -*-
use strict;
use warnings;
use tests::tests;
use tests::random;
check_archive ({"testme" => [random_bytes (5678)]});
pass;
