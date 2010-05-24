# 	$Id$	

package Juman;
require 5.004_04; # For base pragma.
use Carp;
use Juman::Result;
use strict;
use vars qw/ $VERSION %DEFAULT /;
use base qw/ Juman::Process Juman::Hinsi /;

=head1 NAME

Juman -	�����ǲ��Ϥ�Ԥ��⥸�塼��

=head1 SYNOPSIS

 use Juman;
 $juman = new Juman;
 $result = $juman->analysis( "����ʸ������ǲ��Ϥ��Ƥ���������" );
 print $result->all();

=head1 DESCRIPTION

C<Juman> �ϡ������ǲ��ϴ� JUMAN �� Perl �������Ѥ��뤿��Υ⥸�塼���
���롥

ñ��˷����ǲ��Ϥ�Ԥ������ʤ�С�C<Juman::Simple> �����ѤǤ��롥
C<Juman::Simple> �ϡ�C<Juman> �⥸�塼��Υ�åѡ��Ǥ��ꡤ����ñ�˷�
���ǲ��ϴ�����ѤǤ���褦���߷פ���Ƥ��롥

=head1 CONSTRUCTOR

C<Juman> ���֥������Ȥ��������륳�󥹥ȥ饯���ϡ��ʲ��ΰ���������դ�
�롥

=head2 Synopsis

    $juman = new Juman
               [ -Server        => string,]
               [ -Port          => integer,]
               [ -Command       => string,]
               [ -Timeout       => integer,]
               [ -Option        => string,]
               [ -Rcfile        => filename,]
               [ -IgnorePattern => string,]

=head2 Options

=over 4

�ư����ΰ�̣�ϼ����̤ꡥ

=item -Server

JUMAN �����С��Υۥ���̾����ά���줿���ϡ��Ķ��ѿ� C<JUMANSERVER> ��
���ꤵ�줿�����С������Ѥ���롥�Ķ��ѿ�����ꤵ��Ƥ��ʤ����ϡ�
Juman ��ҥץ����Ȥ��ƸƤӽФ���

=item -Port

�����С��Υݡ����ֹ桥

=item -Command

Juman �μ¹ԥե�����̾��Juman �����С������Ѥ��ʤ����˻��Ȥ���롥

=item -Timeout

�����С��ޤ��ϻҥץ������̿���������Ԥ����֡�

=item -Option

JUMAN ��¹Ԥ���ݤΥ��ޥ�ɥ饤���������ά�������ϡ�
C<$Juman::DEFAULT{option}> ���ͤ��Ѥ����롥

������������ե��������ꤹ�� C<-r> ���ץ����ȡ�KNP �ˤ�ä�̵�뤵
����Ƭ�ѥ��������ꤹ�� C<-i> ���ץ����ˤĤ��Ƥϡ����줾����̤� 
C<-Rcfile>, C<-IgnorePattern> �ˤ�äƻ��ꤹ��٤��Ǥ��롥

=item -Rcfile

JUMAN ������ե��������ꤹ�륪�ץ����

���Υ��ץ����ȡ�Juman �����С������Ѥ�ξΩ���ʤ����Ȥ�¿�����äˡ�����
�С������Ѥ��Ƥ��뼭��Ȱ㤦�������ꤷ�Ƥ�������ե�����ϡ��տޤ���
�̤�ˤ�ư��ʤ���

=item -IgnorePattern

JUMAN �ˤ�ä�̵�뤵����Ƭ�ѥ�����

=back

=head1 METHODS

=over 4

=item analysis( STR )

���ꤵ�줿ʸ���� STR ������ǲ��Ϥ������η�̤� C<Juman::Result> ����
�������ȤȤ����֤���

=item juman ( STR )

C<analysis> ����̾��

=back

=head1 ENVIRONMENT

=over 4

=item JUMANSERVER

�Ķ��ѿ� C<JUMANSERVER> �����ꤵ��Ƥ�����ϡ����ꤵ��Ƥ���ۥ��Ȥ� 
Juman �����С��Ȥ������Ѥ��롥

=back

=head1 SEE ALSO

=over 4

=item *

L<Juman::Result>

=item *

L<Juman::Simple>

=back

=head1 HISTORY

This module is the completely rewritten version of the original module
written by Taku Kudoh <taku-ku@is.aist-nara.ac.jp>.

=head1 AUTHOR

=over 4

=item
TSUCHIYA Masatoshi <tsuchiya@pine.kuee.kyoto-u.ac.jp>

=back

=head1 COPYRIGHT

���ѵڤӺ����ۤˤĤ��Ƥ� GPL2 �ޤ��� Artistic License �˽��äƤ���������

=cut

# �С������ɽ��
$VERSION = '0.5.7';

# �������ޥ������ѿ�
%DEFAULT =
    ( command => &Juman::Process::which_command('juman'),
      server  => $ENV{JUMANSERVER} || '',	# Juman �����С��Υۥ���̾
      port    => 32000,				# Juman �����С��Υݡ����ֹ�
      timeout => 30,				# Juman �����С��α������Ԥ�����
      option  => '-e2 -B',
      rcfile  => (exists($ENV{HOME}) ? $ENV{HOME} : '') . '/.jumanrc',
      mclass  => $Juman::Result::DEFAULT{mclass},
      ignorepattern => '',
    );

# Juman ��ҥץ����Ȥ��Ƽ¹Ԥ����硤ɸ����ϤΥХåե���󥰤ˤ��
# �ƽ��Ϥ���Ťˤʤ�ʤ��褦�ˤ��뤿��Τ��ޤ��ʤ�
sub BEGIN {
    unless( $DEFAULT{server} ){
	require FileHandle or die "Juman.pm (BEGIN): Can't load module: FileHandle\n";
	STDOUT->autoflush(1);
    }
}

sub new {
    my $class = shift @_;
    my $this = {};
    bless $this, $class;

    if( @_ == 1 ){
	# ��С������η����ǸƤӽФ��줿���ν���
	my( $argv ) = @_;
	$this->setup( $argv, \%DEFAULT );
#	$this->setup( { 'option' => $argv }, \%DEFAULT );
    } else {
	# �����������ǸƤӽФ��줿���ν���
	my( %option ) = @_;
	$this->setup( \%option, \%DEFAULT );
    }

    if( $this->{OPTION}->{rcfile} and $this->{OPTION}->{server} ){
	carp "Rcfile option may not work with Juman server";
    }

    $this;
}

# EUC-JP��3�Х��ȥ����ɤ򢮤��Ѵ�
sub conv_3bytecode_to_geta {
    my ($buf) = @_;
    my ($ret_buf);

    while ($buf =~ /([^\x80-\xfe]|[\x80-\x8e\x90-\xfe][\x80-\xfe]|\x8f[\x80-\xfe][\x80-\xfe])/g) {
	my $chr = $1;
	if ($chr =~ /^\x8f/) { # 3byte code (JISX0212)
	    $ret_buf .= '��';
	}
	else {
	    $ret_buf .= $chr;
	}
    }

    return $ret_buf;
}

sub juman_lines {
    my( $this, $str ) = @_;
    my $socket  = $this->open();
    my $pattern = $this->pattern();
    my @buf;

    # UTF�ե饰������å�����
    if (utf8::is_utf8($str)) {
	require Encode;
	# euc-jp�ˤʤ�ʸ����JISX0212�������(3�Х���)�Ϣ����Ѵ�
	$str = &conv_3bytecode_to_geta(Encode::encode('euc-jp', $str, sub {'��'}));
	$this->{input_is_utf8} = 1;
    }
    else {
	$this->{input_is_utf8} = 0;
    }

    # �ץ�����ʸ����������
    $str =~ s/[\r\n\f\t]*$/\n/s;
    $socket->print( $str );
    # ���Ϸ�̤��ɤ߽Ф�
    while( defined( $str = $socket->getline ) ){
	if ($this->{input_is_utf8}) {
	    $str = Encode::decode('euc-jp', $str);
	}
	push( @buf, $str );
	last if $str =~ /$pattern/;
    }
    \@buf;
}
    
# �����ǲ��Ϥ�Ԥ��᥽�å�
sub analysis { &juman(@_); }
sub juman {
    my( $this, $str ) = @_;
    new Juman::Result( result  => &juman_lines( $this, $str ),
		       pattern => $this->pattern(),
		       mclass  => $this->{OPTION}->{mclass} );
}

1;
__END__
# Local Variables:
# mode: perl
# coding: euc-japan
# use-kuten-for-period: nil
# use-touten-for-comma: nil
# End:
