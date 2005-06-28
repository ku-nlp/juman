# -*- perl -*-

use strict;
use English qw/ $WARNING /;
use Test;

BEGIN { plan tests => 8 }

use Juman::Morpheme;

my $mrph = Juman::Morpheme->new( "ư�� ������ ư�� ư�� 2 * 0 �Ҳ�ư�쥫�� 2 ̤���� 3\n" );
ok(defined $mrph);

my $new;
{
    local $WARNING = 0;
    $new = $mrph->change_katuyou2( '¸�ߤ��ʤ����ѷ�' );
}
ok(!defined $new);

$new = $mrph->change_katuyou2( '̿���' );
ok(defined $new);
ok($new->midasi() eq 'ư��' );
ok($new->katuyou2_id == 5);

$new = $mrph->kihonkei();
ok(defined $new);
ok($new->midasi() eq 'ư��' );
ok($new->katuyou2_id == 2);
