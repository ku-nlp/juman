# $Id$
package Juman::Katuyou;
require 5.000;
use Juman::Grammar qw/ $FORM /;
use Juman::Hinsi qw/ get_form_id /;
use strict;

=head1 NAME

Juman::Katuyou - �����ǥ��֥������Ȥγ��ѷ�������

=head1 DESCRIPTION

�����ǥ��֥������� L<Juman::Morpheme> �γ��ѷ�������᥽�åɤ��ɲ�
���롥

=head1 FILE

���ѷ����� F<JUMAN.katuyou.db> �򻲾Ȥ��롥���μ���ϡ�Juman ���Τ���
°���Ƥ��뼭�� F<JUMAN.katuyou> ���鵡��Ū���������졤F<Katuyou.pm> ��
Ʊ���ǥ��쥯�ȥ�˥��󥹥ȡ��뤵��Ƥ���Ϥ��Ǥ��롥

�ʤ������ѷ������ BerkeleyDB �����Ǻ������졤L<DB_File> �⥸�塼���
�̤��ƥ�����������롥juman-perl �Υ��󥹥ȡ������ L<DB_File> �⥸�塼
�뤬¸�ߤ��ʤ��ȡ�����κ����ϹԤ��ʤ��Τǡ��ܥ⥸�塼����󶡤����
�᥽�åɤ����ѤǤ��ʤ���

=head1 METHODS

=over 4

=item kihonkei

�����Ǥδ��ܷ����֤���

=cut
sub kihonkei {
    my( $this ) = @_;
    $this->change_katuyou2( '���ܷ�' );
}

=item change_katuyou2 ( FORM )

���ꤵ�줿���ѷ� I<FORM> (���ܷ���̿����ʤ�)����Ŀ����ʷ����ǥ��֥���
���Ȥ��֤������ꤵ�줿���ѷ���¸�ߤ��ʤ�����̤����ͤ��֤���

=cut
sub change_katuyou2 {
    my( $this, $form ) = @_;

    my $type = $this->katuyou1;
    my $id = &get_form_id( $type, $form );
    if( defined $id and $id > 0 ){
	# �ѹ�����ѷ���¸�ߤ�����
	my $new = &_dup( $this );
	my @oldgobi = @{ $FORM->{$type}->[$this->katuyou2_id] };
	my @newgobi = @{ $FORM->{$type}->[$id] };
	$new->{midasi} = &_change_gobi( $this->midasi, $oldgobi[1], $newgobi[1] );
	$new->{yomi}   = &_change_gobi( $this->yomi,
					( $oldgobi[2] || $oldgobi[1] ),
					( $newgobi[2] || $newgobi[1] ) );
	$new->{katuyou2} = $form;
	$new->{katuyou2_id} = $id;
	$new;
    } else {
	# �ѹ�����ѷ���¸�ߤ��ʤ����
	undef;
    }
}

# �������Ѳ������������ؿ�
sub _change_gobi {
    my( $str, $cut, $add ) = @_;

    unless( $cut eq '*' ){
	$str =~ s/$cut\Z//;
    }
    unless( $add eq '*' ){
	$str .= $add;
    }
    $str;
}

# �����ǥ��֥������Ȥ�ʣ�����������ؿ�
sub _dup {
    my( $this ) = @_;
    my $new = {};
    while( my( $key, $value ) = each %$this ){
	$new->{$key} = $value;
    }
    bless $new, ref $this;
}

1;

=back

=head1 SEE ALSO

=over 4

=item *

L<Juman::Morpheme>

=back

=head1 HISTORY

���Υ⥸�塼��ϡ�L<KULM::Juman::Katuyou> �⥸�塼��򸶷��Ȥ��ƺ�����
�줿��

=head1 AUTHORS

=over 4

=item
��ƣ ���� <sato@i.kyoto-u.ac.jp>

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
