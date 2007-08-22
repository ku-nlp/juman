# $Id$
package Juman::MList;
require 5.003_07; # For UNIVERSAL->isa().
use strict;
use base qw/ Juman::KULM::MList /;
use Encode;

=head1 NAME

Juman::MList - �������󥪥֥�������

=head1 SYNOPSIS

  $result = new Juman::MList();

=head1 DESCRIPTION

����������ݻ����륪�֥������ȡ�

=head1 CONSTRUCTOR

=over 4

=item new ( [MRPHS] )

���ꤵ�줿����������ݻ����륪�֥������Ȥ��������롥��ά���줿���ϡ�
��������������ͤȤ����Ѥ��롥

=cut
sub new {
    my $new = bless( {}, shift );
    if( @_ ){
	$new->push_mrph( @_ );
    }
    $new;
}

=back

=head1 METHODS

=over 4

=item mrph ( NUM )

�� I<NUM> ���ܤη����Ǥ��֤���

=item mrph

���Ƥη����ǤΥꥹ�Ȥ��֤���

=begin comment

C<mrph> �᥽�åɤμ��Τ� C<Juman::KULM::MList> ���������Ƥ��롥

=end comment

=item mrph_list

���Ƥη����ǤΥꥹ�Ȥ��֤���

=cut
sub mrph_list {
    my( $this ) = @_;
    if( defined $this->{mrph} ){
	@{$this->{mrph}};
    } else {
	wantarray ? () : 0;
    }
}

=item push_mrph ( @MRPH )

���ꤵ�줿���������ʸ�����ɲä��롥

=cut
sub push_mrph {
    my( $this, @mrph ) = @_;
    $this->{MLIST_READONLY} and die;
    grep( ! $_->isa('Juman::Morpheme'), @mrph ) and die;
    push( @{$this->{mrph}}, @mrph );
}

=item set_readonly

����������Ф���񤭹��ߤ��Ե��Ĥ����ꤹ�롥

=cut
sub set_readonly {
    my( $this ) = @_;
    $this->{MLIST_READONLY} = 1;
}

# �����ߴ�����ݻ����뤿�����̾��
sub set_mlist_readonly {
    shift->set_readonly();
}

=item spec

�����������ʸ������֤���Juman �ˤ����Ϥ�Ʊ�������η�̤������롥

=cut
sub spec {
    my( $this ) = @_;
    my $str;
    for my $mrph ( $this->mrph_list() ){
	$str .= $mrph->spec();
	for my $doukei ( $mrph->doukei() ){
	    $str .= '@ ' . $doukei->spec();
	}
    }
    $str;
}

=item repname

�����������ɽɽ�����֤���

=cut
sub repname {
    my( $this ) = @_;
    my $imi_pat = '��̣ͭ';
    my $pred_pat = '�Ѹ�';
    my $tokusyu_pat = '�ü�';

    if( utf8::is_utf8( $this->fstring ) ){
	$imi_pat = decode('euc-jp', $imi_pat);
	$pred_pat = decode('euc-jp', $pred_pat);
	$tokusyu_pat = decode('euc-jp', $tokusyu_pat);
    }

    my ( @ret );
    for my $mrph ( $this->mrph_list() ){
	if ( $mrph->fstring =~ /<$imi_pat>/ or # ���ʤϰ�̣ͭ�Τ�
	     ( $this->fstring =~ /<$pred_pat/ and # �Ѹ��ΤȤ����ü�ʳ����٤�
	       $mrph->hinsi ne $tokusyu_pat ) ){
	    if ( @ret ) {
		my ( @new_ret );
		my $org_rep = $mrph->repname();
		my $rep = $org_rep ? $org_rep : $mrph->make_repname(); # �ʤ���к��
		for my $old_ret ( @ret ) {
		    push( @new_ret, $old_ret . '+' . $rep ) if $rep;
		}
		if ( $org_rep ) { # ��ä�����Ʊ���۵���ˤĤ��Ƥ�Ʊ���ˤʤ�Τǥ����å�
		    for my $rep ($mrph->get_doukei_reps) {
			for my $old_ret ( @ret ) {
			    push( @new_ret, $old_ret . '+' . $rep );
			}
		    }
		}
		@ret = @new_ret if @new_ret;
	    }
	    else { # �����
		my $org_rep = $mrph->repname();
		my $rep = $org_rep ? $org_rep : $mrph->make_repname(); # �ʤ���к��
		push( @ret, $rep ) if $rep;
		if ( $org_rep ) { # ��ä�����Ʊ���۵���ˤĤ��Ƥ�Ʊ���ˤʤ�Τǥ����å�
		    for my $rep ($mrph->get_doukei_reps) {
			push( @ret, $rep );
		    }
		}
	    }
	}
    }

    my ( %scan ); # ��ʣ����
    join( '?', grep(!$scan{$_}++, sort @ret) );
}

=back

=head1 SEE ALSO

=over 4

=item *

L<Juman::Result>

=item *

L<Juman::Morpheme>

=back

=head1 AUTHOR

=over 4

=item
�ڲ� ��̭ <tsuchiya@pine.kuee.kyoto-u.ac.jp>

=cut

1;
__END__
# Local Variables:
# mode: perl
# coding: euc-japan
# use-kuten-for-period: nil
# use-touten-for-comma: nil
# End:
