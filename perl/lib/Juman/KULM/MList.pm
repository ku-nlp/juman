# $Id$
package Juman::KULM::MList;
require 5.000;
use strict;

=head1 NAME

Juman::KULM::MList - KULM �ߴ� API

=head1 SYNOPSIS

���Υ��饹��ߥ����󥰤��ƻ��Ѥ��롥

=head1 DESCRIPTION

C<KULM::Juman::MLMixin> �ߴ��Υ᥽�åɤ� C<Juman::MList> ���饹���ɲ�
���롥

=head1 METHODS

=over 4

=item mrph ( NUM )

�� I<NUM> ���ܤη����Ǥ��֤���

=item mrph

���Ƥη����ǤΥꥹ�Ȥ��֤���

=cut
sub mrph {
    my $this = shift;
    if( @_ ){
	( $this->mrph_list )[ @_ ];
    } else {
	$this->mrph_list;
    }
}

=item mrph_num

���������Ĺ�����֤���

=cut
sub mrph_num {
    scalar( shift->mrph_list );
}

1;

=back

=head1 SEE ALSO

=over 4

=item *

L<Juman::MList>

=item *

L<KULM::Juman::MLMixin>

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
