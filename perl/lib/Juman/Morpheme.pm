# $Id$
package Juman::Morpheme;
require 5.004_04; # For base pragma.
use Carp;
use strict;
use base qw/ Juman::Katuyou Juman::KULM::Morpheme /;
use vars qw/ @ATTRS /;
use Encode;
=head1 NAME

Juman::Morpheme - 形態素オブジェクト in Juman

=head1 SYNOPSIS

  $m = new Juman::Morpheme( "解析 かいせき 解析 名詞 6 サ変名詞 2 * 0 * 0" );

=head1 DESCRIPTION

形態素の各種情報を保持するオブジェクト．

=head1 CONSTRUCTOR

=over 4

=item new ( SPEC, ID )

第1引数 I<SPEC> に Juman の出力を代入して呼び出すと，その行の内容を解
析し，相当する形態素オブジェクトを生成する．

=back

=cut
@ATTRS = ( 'midasi',
	   'yomi',
	   'genkei',
	   'hinsi',
	   'hinsi_id',
	   'bunrui',
	   'bunrui_id',
	   'katuyou1',
	   'katuyou1_id',
	   'katuyou2',
	   'katuyou2_id',
	   'imis' );

sub new {
    my( $class, $spec, $id ) = @_;
    my $this = $id ? { id => $id } : {};

    my @value;
    my( @keys ) = @ATTRS;
    $spec =~ s/\s*$//;
    if( $spec =~ s/^\\  \\  \\  特殊 1 空白 6// ){
	@value = ( '\ ', '\ ', '\ ', '特殊', '1', '空白', '6' );
	push( @value, split( / /, $spec, scalar(@keys) - 7 ) );
    } else {
	@value = split( / /, $spec, scalar(@keys) );
    }
    while( @keys and @value ){
	my $key = shift @keys;
	$this->{$key} = shift @value;
    }

    bless $this, $class;
}

=head1 METHODS

=over 4

=item midasi

形態素の見出しを返す．

=cut
sub midasi {
    shift->{midasi};
}

=item yomi

形態素の読みを返す．

=cut
sub yomi {
    shift->{yomi};
}

=item genkei

形態素の原形を返す．

=cut
sub genkei {
    shift->{genkei};
}

=item hinsi

形態素の品詞を返す．

=cut
sub hinsi {
    shift->{hinsi};
}

=item hinsi_id

形態素の品詞番号を返す．

=cut
sub hinsi_id {
    shift->{hinsi_id} || undef;
}

=item bunrui

形態素の細分類を返す．

=cut
sub bunrui {
    shift->{bunrui};
}

=item bunrui_id

形態素の細分類番号を返す．

=cut
sub bunrui_id {
    shift->{bunrui_id};
}

=item katuyou1

形態素の活用型を返す．

=cut
sub katuyou1 {
    shift->{katuyou1};
}

=item katuyou1_id

形態素の活用型番号を返す．

=cut
sub katuyou1_id {
    shift->{katuyou1_id};
}

=item katuyou2

形態素の活用形を返す．

=cut
sub katuyou2 {
    shift->{katuyou2};
}

=item katuyou2_id

形態素の活用形番号を返す．

=cut
sub katuyou2_id {
    shift->{katuyou2_id};
}

=item imis

形態素の意味情報を返す．意味情報は，Juman に C<-e2> オプションを指定し
て実行すると得られる．

=cut
sub imis {
    shift->{imis};
}

=item repname

形態素の代表表記を返す．

=cut
sub repname {
    my ( $this ) = @_;
    my $pat = '代表表記';
    if( utf8::is_utf8( $this->midasi ) ){
	$pat = decode('euc-jp', $pat);
    }

    if ( defined $this->{imis} ){
	if ($this->{imis} =~ /$pat:([^\"\s]+)/){
	    return $1;
	}
    }
    return undef;
}

=item make_repname

形態素の代表表記を作る．

=cut
sub make_repname {
    my ( $this ) = @_;
    return $this->genkei . '/' . $this->yomi;
}

=item kanou_dousi

形態素の可能動詞を返す．

=cut
sub kanou_dousi {
    my ( $this ) = @_;
    my $pat = '可能動詞';
    if( utf8::is_utf8( $this->midasi ) ) {
	$pat = decode('euc-jp', $pat);
    }

    if ( defined $this->{imis} ) {
	if ($this->{imis} =~ /$pat:([^\"\s]+)/) {
	    return $1;
 	}
    }
    return undef;
}

=item push_doukei( DOUKEI )

同形異義語 I<DOUKEI> を登録する．

=cut
sub push_doukei {
    my( $this, $doukei ) = @_;
    $this->{doukei} ||= [];
    push( @{$this->{doukei}}, $doukei );
}

=item doukei

形態素の同形異義語のリストを返す．

=cut
sub doukei {
    my( $this ) = @_;
    if( defined $this->{doukei} ){
	@{$this->{doukei}};
    } else {
	wantarray ? () : 0;
    }
}

=item id

コンストラクタを呼び出したときに指定した ID を返す．

=cut
sub id {
    my( $this ) = @_;
    $this->{id};
}

=item spec

形態素オブジェクトを文字列に変換する．

=cut
sub spec {
    my( $this ) = @_;
    sprintf( "%s\n", join( ' ', map( $this->{$_}, grep( defined $this->{$_}, @ATTRS ) ) ) );
}

1;

=back

=head1 SEE ALSO

=over 4

=item *

L<Juman::Katuyou>

=item *

L<Juman::KULM::Morpheme>

=back

=head1 AUTHOR

=over 4

=item
土屋 雅稔 <tsuchiya@pine.kuee.kyoto-u.ac.jp>

=cut

__END__
# Local Variables:
# mode: perl
# coding: euc-japan
# use-kuten-for-period: nil
# use-touten-for-comma: nil
# End:
