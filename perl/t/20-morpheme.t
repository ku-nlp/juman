# -*- perl -*-

use strict;
use Test;

BEGIN { plan tests => 18 }

use Juman::Morpheme;

my $spec = "�Ǥ��� �Ǥ��� �� Ƚ��� 4 * 0 Ƚ��� 25 �ǥ��������Ϣ�ѷ� 18\n";
my $mrph = Juman::Morpheme->new( $spec );

ok(defined $mrph);
ok($mrph->midasi eq '�Ǥ���');
ok($mrph->yomi eq '�Ǥ���');
ok($mrph->genkei eq '��');
ok($mrph->hinsi eq 'Ƚ���');
ok($mrph->hinsi_id == 4);
ok($mrph->bunrui eq '*');
ok($mrph->bunrui_id == 0);
ok($mrph->katuyou1 eq 'Ƚ���');
ok($mrph->katuyou1_id == 25);
ok($mrph->katuyou2 eq '�ǥ��������Ϣ�ѷ�');
ok($mrph->katuyou2_id == 18);
ok($mrph->spec eq $spec );

$spec = "�Ǥ��� �Ǥ��� �� Ƚ��� 4 * 0 Ƚ��� 25 �ǥ��������Ϣ�ѷ� 18 NIL\n";
$mrph = Juman::Morpheme->new( $spec );
ok(defined $mrph);
ok($mrph->imis eq "NIL");
ok($mrph->spec eq $spec);

$spec = "@ @ @ ̤����� 15 ����¾ 1 * 0 * 0";
$mrph = Juman::Morpheme->new( $spec );
ok(defined $mrph);
ok($mrph->midasi eq '@');
