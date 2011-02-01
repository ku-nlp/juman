# $Id$
package Juman::Fork;
require 5.004_04; # For base pragma.
use English qw/ $PERL_VERSION /;
use IO::Handle;
use IO::Pipe;
use POSIX;
use Time::HiRes;
use strict;
use base qw/ Exporter /;
use vars qw/ @EXPORT_OK $TIMEOUT /;
@EXPORT_OK = qw/ $TIMEOUT /;

=head1 NAME

Juman::Fork - ��Ʊ���˼¹Ԥ����ҥץ�������������

=head1 SYNOPSIS

 use Juman::Fork;
 $p = new Juman::Fork( "sort" );
 $p->print( "abc\n", "def\n", "ace\n" );
 $p->close;
 while( $_ = $p->getline ){
     print;
 }

=head1 DESCRIPTION

C<Juman::Fork> �ϡ����ꤵ�줿���ޥ�ɤ� fork ���ƻҥץ����Ȥ��Ƽ¹�
��������ɸ�����Ϥؤν񤭹��ߤȡ�ɸ����ϵڤ�ɸ�२�顼���Ϥ�����ɤ߽�
����Ԥ�����Υ⥸�塼��Ǥ���

=head1 CONSTRUCTOR

=over 4

=item new ( COMMAND [,ARGV] )

C<Juman::Fork> ���֥������Ȥ��������ޤ����ҥץ����Ȥ��Ƽ¹Ԥ��륳��
��ɤ���1�����˻��ꤷ����2�����ʹߤˤ��Υ��ޥ�ɤ��Ф��륳�ޥ�ɥ饤��
���ץ�������ꤷ�ޤ���

Example:

   $p = new Juman::Fork( "cat" "-n" );

=back

=head1 METHODS

=over 4

=item print( [STR,] )

�����ˤ�äƻ��ꤵ�줿ʸ�����ҥץ�����ɸ�����Ϥ��Ϥ��᥽�åɤǤ���

=item printf( FORMAT [,ARG] )

��1�����ˤ�äƻ��ꤵ�줿�񼰤˽��äơ����ꤵ�줿ʸ�����ҥץ�����
ɸ�����Ϥ��Ϥ��᥽�åɤǤ���

=item getline()

�ҥץ�����ɸ����ϵڤ�ɸ�२�顼���Ϥ���1��ʬ�Υǡ�������Ф��᥽��
�ɤǤ���C<timeout> �ˤ�ä����ꤵ�줿���ְ�����ɤ߽Ф���ʤ���С�
C<undef> ���֤��ޤ���

=item timeout( VAL )

�ҥץ����ν��Ϥ� C<getline> �᥽�åɤˤ�äƼ��Ф����Υ����ॢ��
�Ȼ��֤����ꤹ��᥽�åɤǤ��������ॢ���Ȼ��֤ν���ͤˤ��ѿ� 
C<$Juman::Fork::TIMEOUT> ���ͤ��Ȥ��ޤ���

=item alive()

�ҥץ������ĤäƤ��뤫Ĵ�٤�᥽�åɤǤ���

=item pid()

�ҥץ����� PID ���֤��᥽�åɤǤ���

=item close()

�ҥץ�����ɸ�����Ϥ�Ϣ�뤵��Ƥ���ѥ��פ��Ĥ���᥽�åɤǤ���

=item kill()

�ҥץ���������λ����᥽�åɤǤ���

=back

=head1 MEMO

Perl-5.8 �ʹߤξ�硤�ҥץ����Ȥ��̿��ˤϡ� C<encoding> �ץ饰�ޤǻ�
�ꤵ�줿ʸ�������ɤ��Ȥ��ޤ���

=cut
BEGIN {
    if( $PERL_VERSION > 5.008 ){
	require Juman::Encode;
	Juman::Encode->import( qw/ set_encoding / );
    } else {
	*{Juman::Fork::set_encoding} = sub { undef; };
    }
}

=head1 AUTHOR

TSUCHIYA Masatoshi <tsuchiya@pine.kuee.kyoto-u.ac.jp>

=cut

# �ǥե���ȤΥ����ॢ���Ȼ���
$TIMEOUT = 60;

# ���ꤵ�줿���ޥ�ɤ�ҥץ����Ȥ��� fork ����
sub new {
    my( $this, @argv ) = @_;
    ( @argv >= 1 ) || die 'Usage: $p = new Juman::Fork( command, [arguments] )';

    my $read  = new IO::Pipe;
    my $write = new IO::Pipe;

  FORK: {
	if( my $pid = fork ){
	    # �ƥץ���¦�ν���
	    $read->reader;
	    $write->writer;
#	    &set_encoding( $read );
#	    &set_encoding( $write );
	    $this = {
		     PID     => $pid,
		     READ    => $read,
		     WRITE   => $write,
		     TIMEOUT => $TIMEOUT,
		    };
	    bless $this;
	    return $this;
	} elsif( defined $pid ){
	    # �ҥץ���¦�ν���
	    $write->reader;
	    $read->writer;
	    STDOUT->fdopen( $read, "w" );
	    STDERR->fdopen( $read, "w" );
	    STDIN->fdopen( $write, "r" );
	    exec join( " ", @argv );
	    exit 0;
	} elsif( $! =~ /No more process/ ){
	    sleep 5;
	    redo FORK;
	} else {
	    die "Can't fork: $!\n";
	}
    }
}


# �ҥץ�����ɸ�����Ϥ�ʸ�����񤭹���
sub print {
    my $this = shift;
    $this->{WRITE}->print( @_ );
    $this->{WRITE}->flush;		# ����Ū�˥ե�å��夹��
    1;
}


# �ҥץ�����ɸ�����Ϥ��Ф�����դ�����
sub printf {
    my $this = shift;
    my $fmt  = shift;
    $this->{WRITE}->print( sprintf( $fmt, @_ ) );
    $this->{WRITE}->flush;		# ����Ū�˥ե�å��夹��
    1;
}


# �ҥץ�����ɸ�����Ϥ��Ĥ���ؿ�
sub close {
    my( $this ) = @_;
    if( $this and $this->{WRITE} ){
	$this->{WRITE}->print( "\004" ); # ��� Ctrl-D �����äƤ���
	$this->{WRITE}->close;
    }
}


# �����ॢ���Ȥλ��֤����ꤹ��ؿ�
sub timeout {
    my( $this, $timeout ) = @_;
    $this->{TIMEOUT} = eval $timeout;
}


# �ҥץ�����ɸ����Ϥ�ɸ�२�顼���Ϥ��饿���ॢ���ȤĤ����ɤ߽Ф�
sub getline {
    my( $this ) = @_;
    my $buf = "";
    local $SIG{ALRM} = sub { die "SIGALRM is received\n"; };
    eval {
	alarm $this->{TIMEOUT};
	$buf = $this->{READ}->getline;
	alarm 0;
    };
    if( $@ =~ /SIGALRM is received/ ){
	return undef;
    }
    $buf;
}


# �ҥץ����� PID ���֤��ؿ�
sub pid {
    my( $this ) = @_;
    $this->{PID};
}


# �ҥץ������ޤ������Ƥ��뤫Ĵ�٤�ؿ�
sub alive {
    my( $this ) = @_;
    ( waitpid( $this->{PID},&POSIX::WNOHANG ) == 0 ) && ( $? == -1 );
}


# �ҥץ���������λ����ؿ�
sub kill {
    my( $this ) = @_;
    $this->close;
#    sleep 1;
    kill 15, $this->{PID};
    Time::HiRes::sleep 0.01;
    kill 9, $this->{PID};
    $this->alive();			# To avoid zombie.
    $this->{PID} = 0;
    1;
}

1;
__END__
# Local Variables:
# mode: perl
# coding: euc-japan
# use-kuten-for-period: nil
# use-touten-for-comma: nil
# End:
