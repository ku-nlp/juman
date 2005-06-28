# -*- perl -*-

use strict;
use Test;

BEGIN { plan tests => 6 }

use Juman::Morpheme;
use Juman::MList;

my $mlist = Juman::MList->new();
ok( defined $mlist );

$mlist->push_mrph( Juman::Morpheme->new( "��ʸ �����֤� ��ʸ ̾�� 6 ����̾�� 1 * 0 * 0\n") );
$mlist->push_mrph( Juman::Morpheme->new( "���� �������� ���� ̾�� 6 ����̾�� 2 * 0 * 0\n" ) );

ok( $mlist->mrph == 2 );
ok( $mlist->mrph(0)->midasi eq '��ʸ' );
ok( $mlist->mrph(-1)->midasi eq '����' );
ok( join('',map($_->midasi,$mlist->mrph(0,1))) eq '��ʸ����' );
ok( join('',map($_->midasi,$mlist->mrph_list)) eq '��ʸ����' );
