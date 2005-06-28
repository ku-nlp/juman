# $Id$
package Juman::KULM::Morpheme;
require 5.000;
use Carp;
use strict;

=head1 NAME

Juman::KULM::Morpheme - KULM �ߴ� API

=head1 SYNOPSIS

���Υ��饹��ߥ����󥰤��ƻ��Ѥ��롥

=head1 DESCRIPTION

C<KULM::Juman::M> �ߴ��Υ᥽�åɤ� C<Juman::Morpheme> ���饹���ɲä��롥

=head1 METHODS

=over 4

=item get ($attr)

���ꤵ�줿°�����֤���

=cut
my %KULM = ( M      => 'midasi',
	     Y      => 'yomi',
	     G      => 'genkei',
	     H1     => 'hinsi',
	     H1_ID  => 'hinsi_id',
	     H2     => 'bunrui',
	     H2_ID  => 'bunrui_id',
	     K1     => 'katuyou1',
	     K1_ID  => 'katuyou1_id',
	     K2     => 'katuyou2',
	     K2_ID  => 'katuyou2_id',
	     I      => 'imis',
	     Doukei => 'doukei' );

sub get {
    my( $this, $attr ) = @_;
    if( defined $KULM{$attr} ){
	$attr = $KULM{$attr};
	$this->$attr();
    } else {
	croak "Unknown attribute: $attr";
    }
}

=item gets (@attr)

���ꤵ�줿°���Υꥹ�Ȥ��֤���C<all> �Ȥ������꤬��ǽ�Ǥ��롥

=cut
sub gets {
    my( $this, @attr ) = @_;
    if( $attr[0] eq "all" ){
	map( $this->$_(), @Juman::Morpheme::ATTRS );
    } else {
	map( $this->get($_), @attr );
    }
}

=item string ($delimiter, @attr)

���ꤵ�줿°���� C<$delimiter> �Ƿ�礷��ʸ������֤���

=cut
sub string {
    my $this = shift;
    my $delimiter = shift;
    join( $delimiter || " ", grep( defined($_), $this->gets( @_ ? @_ : "all" ) ) );
}

1;

=back

=head1 SEE ALSO

=over 4

=item *

L<Juman::Morpheme>

=item *

L<KULM::Juman::M>

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
