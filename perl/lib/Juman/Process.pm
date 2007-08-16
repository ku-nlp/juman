# $Id$
package Juman::Process;
require 5.000;
use English qw/ $PERL_VERSION /;
use IO::Socket::INET;
use Juman::Fork;
use strict;

=head1 NAME

Juman::Process - �ץ������֥�������

=head1 METHODS

=over 4

=item setup( OPTION, DEFAULT )

�桼�����λ��ꤹ�륪�ץ����Υϥå�����Ф����ե���󥹤ȡ��ǥե���
���ͤΥϥå�����Ф����ե���󥹤�����Ȥ��ƸƤӽФ��ȡ����󥹥���
�ѿ���Ŭ�ڤ����ꤹ�롥

=item open

�ҥץ����������������Υץ������̿����륽���åȤ��֤���

=item close

�ҥץ����Ȥ��̿������åȤ��Ĥ��롥

=item pattern

�����ǲ��Ϸ��/��ʸ���Ϸ�̤ν�ü�򸡽Ф��뤿�������ɽ���ѥ�������
��Ф���

=head1 STRUCTURE

�ʲ��������ѿ��������ФȤ��ƥϥå���˳�Ǽ����Ƥ��롣

    $this->{OPTION}     ���ץ����ؤΥϥå���
    $this->{SOCKET}     JUMAN���̿����륽���åȤؤΥϥå���
    $this->{PATTERN}    ���Ϸ�̤ν�λ�򸡽Ф��뤿�������ɽ��

=cut

# ���󥹥����ѿ������ꤹ��᥽�å�
sub setup {
    my( $this, $option, $default ) = @_;

    # �桼�����ˤ�äƻ��ꤵ�줿���ץ������оݤȤ��ơ��ʲ�����������Ԥ�
    #   (1) ʸ������Ƭ�� - �������
    #   (2) ���ƾ�ʸ�������줹��
    my %opt;
    while( my( $key, $value ) = each %$option ){
	$key =~ s/^-+//;
	$opt{lc($key)} = $value;
    }

    # �桼�����ˤ�äƻ��ꤵ�줿���ץ����ȡ��ǥե�����ͤ򺮹礷�ơ�
    # �ºݤΥ��ץ�����Ϣ�������������롥���λ����ǥե�����ͤ�¸��
    # ���ʤ����ץ����(= �����ʥ��ץ����)�ϡ�ñ��̵�뤵��롥
    while( my( $key, $value ) = each %$default ){
	if( defined $opt{$key} ){
	    $this->{OPTION}->{$key} = $opt{$key};
	} elsif( $value ){
	    $this->{OPTION}->{$key} = $value;
	}
    }

    # -Command ���ץ���󤬻��ꤵ�줿���� -Server ���ץ�����̵�뤹�롥
    if( $opt{command} ){
	delete $this->{OPTION}->{server};
    } elsif( $this->{OPTION}->{server} ){
	delete $this->{OPTION}->{command};
    }
    if( $opt{jumancommand} ){
	delete $this->{OPTION}->{jumanserver};
    } elsif( $this->{OPTION}->{jumanserver} ){
	delete $this->{OPTION}->{jumancommand};
    }

    if( my $argv = $this->{OPTION}->{option} ){
	# ����ե�����򥳥ޥ�ɥ饤�󥪥ץ����Ȥ��ƻ��ꤷ�����
	if( $argv =~ s/\-r\s+(\S+)\s*// ){
	    die "Conflicted option." if defined $this->{OPTION}->{rcfile};
	    $this->{OPTION}->{rcfile} = ( $opt{rcfile} = $1 );
	}
	# Juman �����ϻ���̵�뤹��ԤΥѥ�����򥳥ޥ�ɥ饤�󥪥ץ���
	# ��Ȥ��ƻ��ꤷ�����
	if( $argv =~ s/\-i\s+(\S+)\s*// ){
	    die "Conflicted option." if defined $this->{OPTION}->{ignorepattern};
	    $this->{OPTION}->{ignorepattern} = ( $opt{ignorepattern} = $1 );
	}
	$this->{OPTION}->{option} = $argv;
    }

    my $rcfile = $this->{OPTION}->{rcfile};
    unless( $rcfile and -r $rcfile ){
	die "Can't read initialize file($rcfile): $!\n" if $opt{rcfile};
	delete $this->{OPTION}->{rcfile};
    }

    $rcfile = $this->{OPTION}->{jumanrcfile};
    unless( $rcfile and -r $rcfile ){
	die "Can't read initialize file($rcfile): $!\n" if $opt{jumanrcfile};
	delete $this->{OPTION}->{jumanrcfile};
    }

    if( defined $this->{OPTION}->{ignorepattern} ){
	$this->{PATTERN}
	    = sprintf( '(?:^EOS$|^%s)', quotemeta $this->{OPTION}->{ignorepattern} );
    } else {
	$this->{PATTERN} = '^EOS$';
    }
}

# �������������������ؿ�
sub generate_option {
    my( $this, $remote ) = @_;
    my $option = $this->{OPTION}->{option};
    # Juman �����ϻ���̵�뤹��ԤΥѥ������������ɲä���
    if( defined $this->{OPTION}->{ignorepattern} ){
	$option .= sprintf( ' -i %s', $this->{OPTION}->{ignorepattern} );
    }
    # �ץ����������Υޥ���Ǽ¹Ԥ�����ϡ�����ե�����������
    # ���ꤹ��ɬ�פ�����
    unless( $remote ){
	if( my $rcfile = $this->{OPTION}->{rcfile} ){
	    $option .= sprintf( ' -r %s', $rcfile  );
	}
    }
    $option;
}

# �ͥåȥ����Υ����С��Ȥ��̿��򳫻Ϥ��������ؿ�
sub open_remote_socket {
    my( $this ) = @_;

    my $host = $this->{OPTION}->{server};
    return undef unless $host;

    my $port = $this->{OPTION}->{port};
    my $sock = new IO::Socket::INET( PeerAddr => $host,
				     PeerPort => $port,
				     Proto => 'tcp' )
	or die "Can't connect server: host=$host, port=$port\n";
    $sock->timeout( $this->{OPTION}->{timeout} );
#    &set_encoding( $sock );

    # �����С��� greeting message ���ǧ����
    my $res;
    ( $res = $sock->getline ) =~ /^200/
	or die "Illegal response: host=$host, port=$port, response=$res\n";

    # ����ե��������������
    if( my $rcfile = $this->{OPTION}->{rcfile} ){
	open( RC, "< $rcfile" )
	    or die "Can't open initialize file($rcfile): $!\n";
	$sock->print( "RC\n", <RC>, "\n", pack("c",0x0b), "\n" );
	close RC;
	( $res = $sock->getline ) =~ /^200/
	    or die "Configuration error: rcfile=$rcfile, response=$res\n";
    }

    # �����С��˥��ޥ�ɥ饤�󥪥ץ������Ϥ�
    my $option = $this->generate_option( 'remote' );
    $sock->print( "RUN $option\n" );
    ( $res = $sock->getline ) =~ /^200/
	or die "Configuration error: option=$option, response=$res\n";

    # �������줿�����åȤ�Ͽ���Ƥ���
    $this->{SOCKET}->{REMOTE} = $sock;
}

# ������ޥ����ǻҥץ�����¹Ԥ��������ؿ�
sub open_local_socket {
    my( $this ) = @_;

    # juman/knp �� server-client mode ��ư��ʤ��褦�ˤ��Ƥ��롥
    local %ENV;
    delete $ENV{JUMANSERVER};
    delete $ENV{KNPSERVER};

    my $command = $this->{OPTION}->{command};
    my $option = $this->generate_option();
    my $sock = new Juman::Fork( $command, $option )
	or die "Can't fork: command=$command, option=$option\n";
    $sock->timeout( $this->{OPTION}->{timeout} );
    $this->{SOCKET}->{LOCAL} = $sock;
}

# �����åȤ���������᥽�å�
sub open {
    my( $this ) = @_;
    $this->{SOCKET}->{REMOTE}
	or $this->{SOCKET}->{LOCAL}
	    or $this->open_remote_socket()
		or $this->open_local_socket();
}

# �����åȤ��Ĥ���᥽�å�
sub close {
    my( $this ) = @_;
    my $fh;
    if( $fh = $this->{SOCKET}->{REMOTE} ){
	$fh->print( pack("c",0x0b) . "\nQUIT\n" );
	$fh->close;
    } elsif( $fh = $this->{SOCKET}->{LOCAL} ){
	if( $fh->alive ){
	    $fh->close;
	    if ( $fh->alive ) {
		# Call waitpid() to avoid zombie.
		$fh->kill;
	    }
	}
    }
    delete $this->{SOCKET};
    1;
}

sub DESTROY {
    my( $this ) = @_;
    $this->close();
}

sub pattern {
    my( $this ) = @_;
    $this->{PATTERN} || undef;
}

sub which_command {
    my( $bin ) = @_;
    for my $p ( split( /:/, $ENV{PATH} ) ){
	return "$p/$bin" if -x "$p/$bin";
    }
}

=head1 MEMO

Perl-5.8 �ʹߤξ�硤�ͥåȥ����Υ����С��Ȥ��̿��ˤϡ� 
C<encoding> �ץ饰�ޤǻ��ꤵ�줿ʸ�������ɤ��Ȥ��ޤ���

=cut
BEGIN {
    if( $PERL_VERSION > 5.008 ){
	require Juman::Encode;
	Juman::Encode->import( qw/ set_encoding / );
    } else {
	*{Juman::Process::set_encoding} = sub { undef; };
    }
}

1;
__END__
# Local Variables:
# mode: perl
# coding: euc-japan
# use-kuten-for-period: nil
# use-touten-for-comma: nil
# End:
