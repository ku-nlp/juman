# $Id$
package Juman::Simple;
require 5.000;
use Juman;
use strict;
use base qw/ Exporter /;
use vars qw/ @EXPORT /;
@EXPORT = qw/ juman /;

=head1 NAME

Juman::Simple - �����ǲ��Ϥ�Ԥ��⥸�塼��

=head1 DESCRIPTION

C<Juman::Simple> �ϡ�Juman ���Ѥ��Ʒ����ǲ��Ϥ�Ԥ��ؿ� C<juman> ����
������⥸�塼��Ǥ��롥

���Υ⥸�塼���Ȥ��ȡ�C<Juman> �⥸�塼����ñ�ˡ����������¤��줿
�������Ѥ��뤳�Ȥ��Ǥ��롥�㤨�С����Υ⥸�塼��ϡ��ǽ�˺������� 
C<Juman> ���֥������Ȥ�����Ѥ���Τǡ����ץ����������ѹ��ʤɤϤǤ�
�ʤ��������٤�����Ƿ����ǲ��Ϥ�Ԥ�ɬ�פ�������ϡ�C<Juman> �⥸�塼
���ľ�ܸƤӽФ����ȡ�

=head1 FUNCTION

=over 4

=item juman ($str)

���ꤵ�줿ʸ������оݤȤ��Ʒ����ǲ��Ϥ�Ԥ��ؿ���C<Juman::Result> ��
�֥������Ȥ��֤���

  Example:

    use Juman::Simple;
    $result = &juman( "����ʸ������ǲ��Ϥ��Ƥ���������" );
    print $result->all();

�����ǲ��ϤΥ��ץ������ѹ�������ϡ�C<use> �λ����ǻ��ꤷ�Ƥ�����

  Example:

    use Juman::Simple -Option => "-B -e2";
    $result = &juman( "����ʸ������ǲ��Ϥ��Ƥ���������" );
    print $result->all();

���ץ����ˤϡ�C<Juman::new> �μ����դ��륪�ץ�����Ʊ����Τ������
���롥

=cut
my @OPTION;
my $JUMAN;

sub import {
    my $class = shift;
    @OPTION = @_;
    $class->export_to_level( 1 );
}

sub juman {
    my( $str ) = @_;
    $JUMAN ||= Juman->new( @OPTION );
    $JUMAN->analysis( $str );
}

1;

=back

=head1 SEE ALSO

=over 4

=item *

L<Juman>

=item *

L<Juman::Result>

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
