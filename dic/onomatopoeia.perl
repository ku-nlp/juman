#!/usr/bin/env perl

# 非反復型オノマトペの候補を生成するプログラム

# by Ryohei Sasano <sasano@pi.titech.ac.jp>
# Modified by Daisuke Kawahara <dk@i.kyoto-u.ac.jp>

# 入力例: ＨっＨり 30
# 出力例: (副詞 ((読み ぽっこり)(見出し語 (ぽっこり 3.0))(意味情報 "自動認識")))

use strict;
use utf8;
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";

my $OUTPUT_POS = '副詞';
my $OUTPUT_IMI = '"自動認識 スル接続可能"';

my %CHARS;
@{$CHARS{'Ｈ'}} = qw/あ い う え お か き く け こ さ し す せ そ た ち つ て と
		     な に ぬ ね の は ひ ふ へ ほ ま み む め も や ゆ よ ら り る れ ろ わ
		     が ぎ ぐ げ ご ざ じ ず ぜ ぞ だ ぢ づ で ど ば び ぶ べ ぼ ぱ ぴ ぷ ぺ ぽ/;
@{$CHARS{'Ｋ'}} = qw/ア イ ウ エ オ カ キ ク ケ コ サ シ ス セ ソ タ チ ツ テ ト
		     ナ ニ ヌ ネ ノ ハ ヒ フ ヘ ホ マ ミ ム メ モ ヤ ユ ヨ ラ リ ル レ ロ ワ
		     ガ ギ グ ゲ ゴ ザ ジ ズ ゼ ゾ ダ ヂ ヅ デ ド バ ビ ブ ベ ボ パ ピ プ ペ ポ/;
@{$CHARS{'ＨＹ'}} = qw/きゃ きゅ きょ しゃ しゅ しょ ちゃ ちゅ ちょ にゃ にゅ にょ ひゃ ひゅ ひょ みゃ みゅ みょ
		       りゃ りゅ りょ ぎゃ ぎゅ ぎょ じゃ じゅ じょ ぢゃ ぢゅ ぢょ びゃ びゅ びょ ぴゃ ぴゅ ぴょ/;
@{$CHARS{'ＫＹ'}} = qw/キャ キュ キョ シャ シュ ショ チャ チュ チョ ニャ ニュ ニョ ヒャ ヒュ ヒョ ミャ ミュ ミョ
		       リャ リュ リョ ギャ ギュ ギョ ジャ ジュ ジョ ヂャ ヂュ ヂョ ビャ ビュ ビョ ピャ ピュ ピョ/;

# 入力のフォーマット: "パターン コスト"
# 例: ＨっＨり 30
while (<STDIN>) {
    if (/^\#/) { # comment line
	;
    }
    elsif (/^(\S+)\s+([.\d]+)/) {
	my ($pattern, $cost) = ($1, $2);
	&produce_entries_recursively($cost / 10, '', split(//, $pattern));
    }
    else {
	warn "Invalid pattern: $_";
    }
}


sub produce_entries_recursively {
    my ($cost, $entry, @pattern) = @_;

    if (@pattern) {
	my $current_pattern = shift(@pattern);
	if ($current_pattern eq 'Ｈ' or $current_pattern eq 'Ｋ') {
	    if (@pattern and $pattern[0] eq 'Ｙ') { # 次の文字が'Ｙ'のときはそれも取る
		$current_pattern .= shift(@pattern);
	    }
	    for my $char (@{$CHARS{$current_pattern}}) {
		&produce_entries_recursively($cost, $entry . $char, @pattern);
	    }
	}
	else { # 普通の文字のとき
	    &produce_entries_recursively($cost, $entry . $current_pattern, @pattern);
	}
    }
    else {
	printf "(%s ((読み %s)(見出し語 (%s %.1f))(意味情報 %s)))\n", $OUTPUT_POS, $entry, $entry, $cost, $OUTPUT_IMI;
    }
}
