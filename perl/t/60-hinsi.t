# -*- perl -*-

use strict;
use Test;

BEGIN { plan tests => 38 }

use Juman;
use Juman::Hinsi ":all";
use Juman::Morpheme;

my $juman = new Juman;
ok( defined $juman );
ok( $juman->get_hinsi_id( '̾��' ) == 6 );
ok( &get_hinsi( 6 ) eq '̾��' );

for( <DATA> ){
    if( $_ !~ m/\A\#/ and $_ =~ /\S/ ){
	my $m = Juman::Morpheme->new( $_, 0 );
	ok( defined $m );
	ok( $m->hinsi eq &get_hinsi($m->hinsi_id) );
	ok( $m->hinsi_id == &get_hinsi_id($m->hinsi) );
	ok( $m->bunrui eq &get_bunrui($m->hinsi_id,$m->bunrui_id) );
	ok( $m->bunrui_id == &get_bunrui_id($m->hinsi,$m->bunrui) );
	ok( $m->katuyou1 eq &get_type($m->katuyou1_id) );
	ok( $m->katuyou1_id == &get_type_id($m->katuyou1) );
#	ok( $m->katuyou2 eq &get_form($m->katuyou1_id,$m->katuyou2_id) );
#	ok( $m->katuyou2_id == &get_form_id($m->katuyou1,$m->katuyou2) );
    }
}

__DATA__
�֤� ������ �֤� ���ƻ� 3 * 0 �����ƻ쥢������ 18 ���ܷ� 2 NIL
�� �Ϥ� �� ̾�� 6 ����̾�� 1 * 0 * 0 NIL
�� �� �� ���� 9 �ʽ��� 1 * 0 * 0 NIL
�餤�� ������ �餯 ư�� 2 * 0 �Ҳ�ư�쥫�� 2 ���� 8 NIL
�Τ� �Τ� �Τ� ��ư�� 5 * 0 �ʷ��ƻ� 21 ���ܷ� 2 NIL
