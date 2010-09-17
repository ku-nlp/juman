# $Id$
package Juman::Result;
require 5.004_04; # For base pragma.
use Juman::Morpheme;
use strict;
use base qw/ Juman::MList /;
use vars qw/ %DEFAULT /;

=head1 NAME

Juman::Result - �����ǲ��Ϸ�̥��֥������� in Juman

=head1 SYNOPSIS

  $result = new Juman::Result( "����...\n...\nEOS\n" );

=head1 DESCRIPTION

Juman �ˤ������ǲ��Ϥη�̤��ݻ����륪�֥������ȡ�

=head1 CONSTRUCTOR

=over 4

=item new ( RESULT )

Juman �ν��Ϥ�Ԥ�ñ�̤Ȥ��Ƴ�Ǽ���줿�ꥹ�Ȥ��Ф����ե���� RESULT
������Ȥ��ƸƤӽФ��ȡ����η����ǲ��Ϸ�̤�ɽ�����֥������Ȥ��������롥

=item new ( OPTIONS )

�ʲ��γ�ĥ���ץ�������ꤷ�ƥ��󥹥ȥ饯����ƤӽФ���

=over 4

=item result => RESULT

Juman �ν���ʸ���󡤤ޤ��ϡ�����ʸ�����Ԥ�ñ�̤Ȥ��Ƴ�Ǽ���줿�ꥹ��
���Ф����ե���󥹤���ꤹ�롥

=item pattern => STRING

���Ϸ�̤�ü���뤿��Υѥ��������ꤹ�롥

=item mclass => NAME

�����ǥ��֥������Ȥ���ꤹ�롥̵����ξ��ϡ�C<Juman::Morpheme> ����
���롥

=cut
%DEFAULT = ( pattern => '^EOS$',
	     mclass  => 'Juman::Morpheme' );

sub new {
    my $class = shift;

    my( %opt ) = %DEFAULT;
    if( @_ == 1 ){
	$opt{result} = shift;
    } else {
	while( @_ ){
	    my $key = shift;
	    my $val = shift;
	    $key =~ s/\A-+//;
	    $opt{lc($key)} = $val;
	}
    }
    my $result  = $opt{result};
    my $pattern = $opt{pattern};
    my $mclass  = $opt{mclass};
    return undef unless( $result and $pattern and $mclass );

    # ʸ����ľ�ܻ��ꤵ�줿���
    $result = [ map( "$_\n", split( /\n/, $result ) ) ] unless ref $result;

    my $this = {};
    bless $this, $class;

    my( $str );
    while ( defined( $str = shift @$result ) ) {
	if ( $str =~ m!$pattern! and @$result == 0 ) {
	    $this->{_eos} = $str;
	    last;
	} elsif ( $str =~ m!\A\@ \@ \@ [^\@]! ){
	    # ��@�פΤߤ���ʤ�̤�������������
	    $this->push_mrph( $mclass->new( $str, scalar($this->mrph) ) );
	} elsif ( $str =~ s!\A\@ !! ) {
	    # ��@�פ���Ƭ�ˤ����Ʊ����
	    ( $this->mrph )[-1]->push_doukei( $mclass->new( $str, scalar($this->mrph) ) );
	} else {
	    $this->push_mrph( $mclass->new( $str, scalar($this->mrph) ) );
	}
    }
    $this->set_readonly();

    $this;
}

=back

=head1 METHODS

�ܥ��֥������Ȥϡ��������󥪥֥������� C<Juman::MList> ��Ѿ�����褦
��������Ƥ��롥�������äơ��������󥪥֥������Ȥ��Ф���ͭ���ʰʲ��Υ�
���åɤ����Ѳ�ǽ�Ǥ��롥

=over 4

=item mrph

=cut

=item spec

�����������ʸ������֤���Juman �ˤ����Ϥ�Ʊ�������η�̤������롥

=cut
sub spec {
    my( $this ) = @_;
    sprintf( "%s%s",
	     $this->Juman::MList::spec(),
	     $this->{_eos} );
}


=back

=head1 SEE ALSO

=over 4

=item *

L<Juman::Morpheme>

=item *

L<Juman::MList>

=back

=head1 AUTHOR

=over 4

=item
�ڲ� ��̭ <tsuchiya@pine.kuee.kyoto-u.ac.jp>

=cut

# �����ߴ������ݤĤ���Υ᥽�å�
sub all {
    my( $this ) = @_;
    $this->spec();
}

1;
__END__
# Local Variables:
# mode: perl
# coding: euc-japan
# use-kuten-for-period: nil
# use-touten-for-comma: nil
# End:
