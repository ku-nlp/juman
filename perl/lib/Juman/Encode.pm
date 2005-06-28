# $Id$
package Juman::Encode;
require 5.008;
use strict;
use base qw/ Exporter /;
our @EXPORT_OK = qw/ encode decode set_encoding /;

=head1 NAME

Juman::Encode - character encoding functions

=head1 DESCRIPTION

Perl-5.8.x ������ʸ�������ɤȤ��� Unicode ����Ѥ��Ƥ��롥���Τ��ᡤ��
�ܸ� EUC ��ȤäƤ���ץ����Ȥ������Ϥ�Ԥ����䡤���ܸ� EUC �ǵ���
���줿�ǡ����򻲾Ȥ�����ˤϡ��������Ū�� encode/decode ��Ԥ�ɬ��
�����롥

���Υ饤�֥��Ǥϡ����Τ���δؿ���������Ƥ��롥

=head1 FUNCTIONS

=over 4

=item $octets = encode ( $string )

ʸ�����C<encoding> �ץ饰�ޤǻ��ꤵ��Ƥ���ʸ�������ɤ� encode ���ơ�
�Х���������롥

=cut
sub encode {
    my( $string ) = @_;
    if( $string and ${^ENCODING} ){
	${^ENCODING}->encode( $string );
    } else {
	$string;
    }
}

=item $string = decode ( $octets )

�Х������C<encoding> �ץ饰�ޤǻ��ꤵ��Ƥ���ʸ�������ɤ� decode ��
�ơ�ʸ��������롥

=cut
sub decode {
    my( $string ) = @_;
    if( $string and ${^ENCODING} ){
	${^ENCODING}->decode( $string );
    } else {
	$string;
    }
}

=item set_encoding ( $handle )

���ꤵ�줿�ե�����ϥ�ɥ�Ȥ��̿��ˡ�C<encoding> �ץ饰�ޤǻ��ꤵ���
����ʸ�������ɤ�Ȥ��褦�����ꤹ�롥

=cut
sub set_encoding {
    my( $fh ) = @_;
    if( ${^ENCODING} ){
	my $name = ${^ENCODING}->name();
	binmode( $fh, ":encoding($name)" );
    }
}

1;

=back

������δؿ��⡤C<encoding> �ץ饰�ޤ����ꤵ��Ƥ��ʤ����ˤϡ����⤷
�ʤ���

=head1 MEMO

���Υ饤�֥��ϡ��ѿ� C<${^ENCODING}> �򻲾Ȥ���ɬ�פΤ���ؿ������
�򡤰����ˤޤȤ�뤿���Ƴ���������ʤ��ʤ�С��ѿ� C<${^ENCODING}> 
�򻲾Ȥ��褦�Ȥ��륳���ɤϡ�Jperl-5.005 �Ǥ� syntax error �Ȥʤ뤿���
���롥

  Sample Script:
    use English qw/ $PERL_VERSION /;
    if( $PERL_VERSION > 5.008 ){
        ${^ENCODING}->encode( ... );
    }

  Error Message:
    syntax error at sample.perl line 3, near "{^"

=head1 SEE ALSO

=over 4

=item *

L<encoding>

=item *

L<Encode>

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
