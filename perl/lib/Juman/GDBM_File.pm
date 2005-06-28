# $Id$
package Juman::GDBM_File;
require 5.000;
use Carp qw/ croak /;
use English qw/ $PERL_VERSION /;
use GDBM_File qw/ GDBM_WRCREAT GDBM_READER GDBM_WRITER /;
use POSIX qw/ O_CREAT O_RDONLY O_RDWR O_WRONLY /;
use strict;
use vars qw/ @EXPORT @ISA /;
@ISA = qw/ GDBM_File /;
@EXPORT = qw/ O_CREAT O_RDONLY O_RDWR O_WRONLY GDBM_WRCREAT GDBM_READER GDBM_WRITER /;

=head1 NAME

Juman::GDBM_File - Wrapper class of GDBM_File

=head1 SYNOPSIS

 use Juman::GDBM_File;
 use encoding "euc-jp";
 tie( %hash, 'Juman::GDBM_File', $dbfile, &GDBM_WRCREAT, 0640 ) or die;
 $hash{"ź��"} = "��";
 while( my( $key, $value ) = each %hash ){
     print "$key:$value\n";
 }

=head1 DESCRIPTION

Perl-5.8.x ������ʸ�������ɤȤ��� Unicode ����Ѥ��Ƥ��롥���Τ��ᡤ��
�ܸ� EUC �ǵ��Ҥ��줿�ǡ����١����ե�����򻲾Ȥ�����ˤϡ�ź������
��񤭹�����ꡤ�ɤ߽Ф����ꤹ�����ˡ��������Ū�� encode/decode ���
��ɬ�פ����롥

���� C<Juman::GDBM_File> ���饹�ϡ������ʸ�������ɤ���¸����Ƥ���ǡ�
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
	*{Juman::GDBM_File::encode} = sub { $_[0]; };
	*{Juman::GDBM_File::decode} = sub { $_[0]; };
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

=head1 CONSTRUCTOR

C<GDBM_File> ��Ʊ��ν񼰤ǡ�Ϣ�����������Ǥ��롥

    tie( %hash, 'Juman::GDBM_File', $dbfile, &GDBM_WRCREAT, 0640 );

��5�����ˤϡ��ǡ����١����ե�����򿷵�����������Υե�����°������
�ꤵ��Ƥ��롥

��5�������ά�������ϡ�C<DB_File> �ߴ��ν񼰤��Ȥ��Ƥ���ȸ��ʤ���
�롥

    tie( %hash, 'Juman::GDBM_File', $dbfile, &O_CREAT );

���ξ�硤�ǡ����١����ե�����򳫤��⡼�ɤ���ꤷ�Ƥ�����4�����ˤϡ�
C<O_CREAT>, C<O_RDWR> �ʤ� C<DB_File> �����Υǡ����١����򳫤�����Ʊ��
�����Ȥ����ǡ����١����ե�����򿷵�����������Υե�����°���ϡ�
C<umask> ���֤��ͤ��鼫ưŪ�˻��Ф���롥

=cut
sub TIEHASH {
    my $class = shift;
    my $name  = shift;
    my $mode  = shift;
    my $permission;
    if( @_ ){ # GDBM_File style
	$permission = shift;
    } else {  # DB_File style
	if ( $mode == &O_CREAT ) {
	    $mode = &GDBM_WRCREAT;
	} elsif ( $mode == &O_RDONLY ) {
	    $mode = &GDBM_READER;
	} elsif ( $mode == &O_RDWR ) {
	    $mode = &GDBM_READER | &GDBM_WRITER;
	} elsif ( $mode == &O_WRONLY ) {
	    $mode = &GDBM_WRITER;
	} else {
	    croak "$class (TIEHASH): Unknown mode $mode is specified";
	}
	$permission = 0666 & (~umask);
    }
    my $new = GDBM_File->TIEHASH( $name, $mode, $permission );
    bless $new, $class;
}

1;

=head1 SEE ALSO

=over 4

=item *

L<GDBM_File>

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
