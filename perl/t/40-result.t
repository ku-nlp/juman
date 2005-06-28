# -*- perl -*-

use strict;
use Test;

BEGIN { plan tests => 7 }

my $result = <<'__sample__';
������ ���������� ������ ̾�� 6 ����̾�� 1 * 0 * 0
���� �������� ���� ̾�� 6 ����̾�� 2 * 0 * 0
�� �� �� ���� 9 ��³���� 3 * 0 * 0
�¹� ���ä��� �¹� ̾�� 6 ����̾�� 2 * 0 * 0
�� �줤 �� ̾�� 6 ����̾�� 1 * 0 * 0
@ �� ���Ȥ� �� ̾�� 6 ����̾�� 1 * 0 * 0
@ �� ���ᤷ �� ̾�� 6 ����̾�� 1 * 0 * 0
EOS
__sample__

use Juman::Result;

my $x = Juman::Result->new( $result );
ok( defined $x );
ok( $x->mrph == 5 );
ok( "�����ǲ��Ϥμ¹���" eq join( "", map( $_->midasi, $x->mrph_list ) ) );

$x = undef;
$x = Juman::Result->new( [ map("$_\n",split(/\n/,$result)) ] );
ok( defined $x );
ok( $x->mrph == 5 );

$x = undef;
$x = Juman::Result->new( result => $result, pattern => q/^EOS$/ );
ok( defined $x );
ok( $x->mrph == 5 );
