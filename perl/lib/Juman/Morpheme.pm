# $Id$
package Juman::Morpheme;
require 5.004_04; # For base pragma.
use Carp;
use strict;
use base qw/ Juman::Katuyou Juman::KULM::Morpheme /;
use vars qw/ @ATTRS /;
use Encode;
=head1 NAME

Juman::Morpheme - �����ǥ��֥������� in Juman

=head1 SYNOPSIS

  $m = new Juman::Morpheme( "���� �������� ���� ̾�� 6 ����̾�� 2 * 0 * 0" );

=head1 DESCRIPTION

�����ǤγƼ������ݻ����륪�֥������ȡ�

=head1 CONSTRUCTOR

=over 4

=item new ( SPEC, ID )

��1���� I<SPEC> �� Juman �ν��Ϥ��������ƸƤӽФ��ȡ����ιԤ����Ƥ��
�Ϥ���������������ǥ��֥������Ȥ��������롥

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
    if( $spec =~ s/^\\  \\  \\  �ü� 1 ���� 6// ){
	@value = ( '\ ', '\ ', '\ ', '�ü�', '1', '����', '6' );
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

�����Ǥθ��Ф����֤���

=cut
sub midasi {
    shift->{midasi};
}

=item yomi

�����Ǥ��ɤߤ��֤���

=cut
sub yomi {
    shift->{yomi};
}

=item genkei

�����Ǥθ������֤���

=cut
sub genkei {
    shift->{genkei};
}

=item hinsi

�����Ǥ��ʻ���֤���

=cut
sub hinsi {
    shift->{hinsi};
}

=item hinsi_id

�����Ǥ��ʻ��ֹ���֤���

=cut
sub hinsi_id {
    shift->{hinsi_id} || undef;
}

=item bunrui

�����Ǥκ�ʬ����֤���

=cut
sub bunrui {
    shift->{bunrui};
}

=item bunrui_id

�����Ǥκ�ʬ���ֹ���֤���

=cut
sub bunrui_id {
    shift->{bunrui_id};
}

=item katuyou1

�����Ǥγ��ѷ����֤���

=cut
sub katuyou1 {
    shift->{katuyou1};
}

=item katuyou1_id

�����Ǥγ��ѷ��ֹ���֤���

=cut
sub katuyou1_id {
    shift->{katuyou1_id};
}

=item katuyou2

�����Ǥγ��ѷ����֤���

=cut
sub katuyou2 {
    shift->{katuyou2};
}

=item katuyou2_id

�����Ǥγ��ѷ��ֹ���֤���

=cut
sub katuyou2_id {
    shift->{katuyou2_id};
}

=item imis

�����Ǥΰ�̣������֤�����̣����ϡ�Juman �� C<-e2> ���ץ�������ꤷ
�Ƽ¹Ԥ���������롥

=cut
sub imis {
    shift->{imis};
}

=item repname

�����Ǥ���ɽɽ�����֤���

=cut
sub repname {
    my ( $this ) = @_;
    my $pat = '��ɽɽ��';
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

=item repnames

�����Ǥ���ɽɽ����ۣ������������ϡ�?�פ�Ϣ��ˤ��֤���
����(��0)��Ϳ����ȡ�������ä�������ۣ������̵�뤹��.

=cut
sub repnames {
    my ( $this, $flag ) = @_;

    my $pat = '�������';
    if( utf8::is_utf8( $this->midasi ) ){
	$pat = decode('euc-jp', $pat);
    }

    my ( @ret );
    my $org_rep = $this->repname();
    my $rep = $org_rep ? $org_rep : $this->make_repname(); # �ʤ���к��
    push( @ret, $rep ) if $rep;
    if ( $org_rep && # ��ä�����Ʊ���۵���ˤĤ��Ƥ�Ʊ���ˤʤ�Τǥ����å�
	 # $flag��Ω�äƤ�����ϲ�����ä�������ۣ������̵��
	 !( $flag && $this->spec =~ /<$pat>/ )) { 
	push @ret, $this->get_doukei_reps;
    }

    my ( %scan ); # ��ʣ����
    join( '?', grep(!$scan{$_}++, sort @ret) );

}

=item get_doukei_reps

�����Ǥ�Ʊ���۵������ɽɽ�����֤���

=cut
sub get_doukei_reps {
    my ( $this ) = @_;

    my ( @reps );
    for my $doukei ( $this->doukei() ) { # Ʊ���۵��� (@)
	my $rep = $doukei->repname();
	$rep = $doukei->make_repname() unless $rep;
	push( @reps, $rep ) if $rep;
    }

    return @reps;
}

=item make_repname

�����Ǥ���ɽɽ�����롥

=cut
sub make_repname {
    my ( $this ) = @_;
    my $basic_form_str = '���ܷ�';
    if( utf8::is_utf8( $this->midasi ) ){
	$basic_form_str = decode( 'euc-jp', $basic_form_str );
    }

    # ���Ѹ�ʤ���ܷ����᤹

    my $new_m = $this->change_katuyou2( $basic_form_str );
    if ( $new_m ){ # ���Ѹ�
	return $new_m->genkei . '/' . $new_m->yomi;
    }
    else {
	return $this->genkei . '/' . $this->yomi;
    }
}

=item kanou_dousi

�����Ǥβ�ǽư����֤���

=cut
sub kanou_dousi {
    my ( $this ) = @_;
    my $pat = '��ǽư��';
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

Ʊ���۵��� I<DOUKEI> ����Ͽ���롥

=cut
sub push_doukei {
    my( $this, $doukei ) = @_;
    $this->{doukei} ||= [];
    push( @{$this->{doukei}}, $doukei );
}

=item doukei

�����Ǥ�Ʊ���۵���Υꥹ�Ȥ��֤���

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

���󥹥ȥ饯����ƤӽФ����Ȥ��˻��ꤷ�� ID ���֤���

=cut
sub id {
    my( $this ) = @_;
    $this->{id};
}

=item spec

�����ǥ��֥������Ȥ�ʸ������Ѵ����롥

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
�ڲ� ��̭ <tsuchiya@pine.kuee.kyoto-u.ac.jp>

=cut

__END__
# Local Variables:
# mode: perl
# coding: euc-japan
# use-kuten-for-period: nil
# use-touten-for-comma: nil
# End:
