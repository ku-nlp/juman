# $Id$
package Juman::DB_File;
require 5.004_04; # For base pragma.
use English qw/ $PERL_VERSION /;
use POSIX qw/ O_CREAT O_RDONLY O_RDWR O_WRONLY /;
use strict;
use base qw/ DB_File /;
use vars qw/ @EXPORT /;
@EXPORT = qw/ O_CREAT O_RDONLY O_RDWR O_WRONLY /;

=head1 NAME

Juman::DB_File - Wrapper class of DB_File

=head1 SYNOPSIS

 use Juman::DB_File;
 use encoding "euc-jp";
 tie( %hash, 'Juman::DB_File', $dbfile, &O_CREAT ) or die;
 $hash{"ź��"} = "��";
 while( my( $key, $value ) = each %hash ){
     print "$key:$value\n";
 }

=head1 DESCRIPTION

Perl-5.8.x ������ʸ�������ɤȤ��� Unicode ����Ѥ��Ƥ��롥���Τ��ᡤ��
�ܸ� EUC �ǵ��Ҥ��줿�ǡ����١����ե�����򻲾Ȥ�����ˤϡ�ź������
��񤭹�����ꡤ�ɤ߽Ф����ꤹ�����ˡ��������Ū�� encode/decode ���
��ɬ�פ����롥

���� C<Juman::DB_File> ���饹�ϡ������ʸ�������ɤ���¸����Ƥ���ǡ�
���١����ե�����򰷤�����ˡ�Ʃ��Ū�� encode/decode ��Ԥ���

=head1 ENCODING

���Υ��饹�����Ѥ�����ϡ��ǡ����١����Ȥ������ϻ��˻Ȥ�ʸ�������ɤ�
C<encoding> �ץ饰�ޤǻ��ꤹ�롥C<encoding> �ץ饰�ޤˤ����꤬¸�ߤ�
�ʤ����ϡ��ޤä����Ѵ���Ԥ�ʤ���

=cut
BEGIN {
    if( $PERL_VERSION > 5.008 ){
	require Juman::Encode;
	Juman::Encode->import( qw/ encode decode / );
    } else {
	*{Juman::DB_File::encode} = sub { $_[0]; };
	*{Juman::DB_File::decode} = sub { $_[0]; };
    }
}

# �ǡ����١����˥�����������᥽�åɤ��񤭤��Ƥ��롥ɬ�פʥ᥽�åɤ�
# �ܺ٤ˤĤ��Ƥϡ�perldoc perltie �򻲾ȡ�
sub FETCH {
    my( $this, $key ) = @_;
    &decode( $this->SUPER::FETCH( &encode( $key ) ) );
}

sub STORE {
    my( $this, $key, $value ) = @_;
    $this->SUPER::STORE( &encode( $key ), &encode( $value ) );
}

sub DELETE {
    my( $this, $key ) = @_;
    $this->SUPER::DELETE( &encode( $key ) );
}

sub EXISTS {
    my( $this, $key ) = @_;
    $this->SUPER::EXISTS( &encode( $key ) );
}

sub FIRSTKEY {
    my( $this ) = @_;
    &decode( $this->SUPER::FIRSTKEY() );
}

sub NEXTKEY {
    my( $this, $lastkey ) = @_;
    &decode( $this->SUPER::NEXTKEY( &encode( $lastkey ) ) );
}

1;

=head1 SEE ALSO

=over 4

=item *

L<DB_File>

=item *

L<perltie>

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
