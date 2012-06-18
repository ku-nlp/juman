package JumanDevel;

# 2012/5/28 kuro@i.kyoto-u.ac.jp

use Exporter;
use utf8;

@ISA = qw(Exporter);

@EXPORT = qw(read_juman_entry write_juman_entry);

######################################################################
sub read_juman_entry
{
    # JUMANのS式の1エントリを読む

    my($input) = @_;
    my($top_midashi, $midashi, $yomi, $pos, $pos2, $conj, $sem, $comment);
    my($open_p, $close_p);
    my(@m);

    chomp($input);
    
    $open_p = @{$input =~ [/\(/g]};
    $close_p = @{$input =~ [/\)/g]};
    return ("ERROR") if ($open_p != $close_p);

    if ($input =~ s/\s*\;\s*([^\)]+)$//) {
	$comment = $1;
    } else {
	$comment = "";
    }

    # "(あいず 1.6)" -> "あいず:1.6"
    $input =~ s/\(([^ \(\)]+) ([\d\.]+)\)/\1:\2 /g;
    $input =~ s/  / /g;
    $input =~ s/ \)/\)/;
    
    if ($input =~ /^\(([^ \(\)]+) \(([^ \(\)]+)/) {
	$pos = $1; $pos2 = $2;
    } elsif ($input =~ /^\(([^ \(\)]+)/) {
	$pos = $1; $pos2 = "";
    }
	
    $input =~ /見出し語 ([^\)]+)/;
    $midashi = $1;
    @m = split(/ /, $midashi);
    $top_midashi = shift(@m);
    $top_midashi =~ s/\:[\d\.]+$//;

    $input =~ /読み ([^\)]+)/;
    $yomi = $1;

    if ($input =~ /活用型 ([^ \)]+)/) {
	$conj = $1;
    } else {
	$conj = "";
    }

    if ($input =~ /意味情報 \"([^\"]+)\"/) {
	$sem = $1;
    } else {
	$sem = "";
    }

    return ($top_midashi, $midashi, $yomi, $pos, $pos2, $conj, $sem, $comment);
}

######################################################################
sub write_juman_entry
{
    # JUMANの1エントリをS式で出力

    my($midashi, $yomi, $pos, $pos2, $conj, $sem) = @_;
    my($string);

    $midashi =~ s/([^ \(\)]+):1\.0 /\1 /g;
    $midashi =~ s/([^ \(\)]+):1 /\1 /g;
    $midashi =~ s/([^ \(\)]+):1\.0$/\1/g;
    $midashi =~ s/([^ \(\)]+):1$/\1/g;
    $midashi =~ s/([^ \(\)]+):([0123456789\.]+)/\(\1 \2\)/g;
    $string = "(読み $yomi)(見出し語 $midashi)";
    $string .= "(活用型 $conj)" if ($conj);
    $string .= "(意味情報 \"$sem\")" if ($sem);
    $string = "($string)";
    $string = "($pos2 $string)" if ($pos2);
    $string = "($pos $string)";
    return $string;
}

######################################################################

1;

