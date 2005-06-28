# $Id$
package Juman::Hinsi;
require 5.004_04; # For base pragma.
use Carp;
use English qw/ $LIST_SEPARATOR $WARNING /;
use Juman::Grammar qw/ $HINSI $BUNRUI $TYPE $FORM /;
use strict;
use base qw/ Exporter /;
use vars qw/ @EXPORT_OK %EXPORT_TAGS /;
@EXPORT_OK = qw/ get_hinsi get_hinsi_id get_bunrui get_bunrui_id get_type get_type_id get_form get_form_id /;
%EXPORT_TAGS = ( all => [ @EXPORT_OK ] );

=head1 NAME

Juman::Hinsi - Juman �ʻ��ηϤ򰷤��饤�֥��

=head1 SYNOPSIS

 use Juman::Hinsi qw/ get_hinsi_id /;
 $id = &get_hinsi_id( '̾��' );

=head1 DESCRIPTION

Juman �ʻ��ηϤξ�������뤿��δؿ����󶡤���饤�֥��Ǥ��롥

=head1 FUNCTIONS

=over 4

=item get_hinsi ( ID )

�ʻ��ֹ椫���ʻ������

=cut
sub _zerop {
    ( $_[0] =~ /\D/ )? $_[0] eq '*' : $_[0] == 0;
}

sub _indexp {
    ( $_[0] !~ /\D/ and $_[0] >= 1 );
}

sub get_hinsi {
    if( @_ == 2 ){
	shift;
    } elsif( @_ != 1 ){
        local $LIST_SEPARATOR = ', ';
        croak "get_hinsi(@_): requires an argument";
    }
    my( $x ) = @_;
    if( exists $HINSI->[0]->{$x} ){
	$x;
    } elsif( &_indexp($x) and defined $HINSI->[$x] ){
	$HINSI->[$x];
    } else {
	carp "Unknown hinsi ($x)" if $WARNING;
	undef;
    }
}

=item get_hinsi_id ( STR )

�ʻ줫���ʻ��ֹ������

=cut
sub get_hinsi_id {
    if( @_ == 2 ){
	shift;
    } elsif( @_ != 1 ){
        local $LIST_SEPARATOR = ', ';
        croak "get_hinsi_id(@_): requires an argument";
    }
    my( $x ) = @_;
    if( exists $HINSI->[0]->{$x} ){
	$HINSI->[0]->{$x};
    } elsif( &_indexp($x) and defined $HINSI->[$x] ){
	$x;
    } else {
	carp "Unknown hinsi id ($x)" if $WARNING;
	undef;
    }
}

=item get_bunrui ( HINSI, ID )

��ʬ���ֹ椫���ʬ�������

=cut
sub get_bunrui {
    if( @_ == 3 ){
	shift;
    } elsif( @_ != 2 ){
        local $LIST_SEPARATOR = ', ';
        croak "get_bunrui(@_): requires 2 arguments";
    }
    my( $hinsi, $x ) = @_;
    if( defined( $hinsi = &get_hinsi($hinsi) ) ){
	if( exists $BUNRUI->{$hinsi} ){
	    if( exists $BUNRUI->{$hinsi}->[0]->{$x} ){
		return $x;
	    } elsif( &_indexp($x) and defined $BUNRUI->{$hinsi}->[$x] ){
		return $BUNRUI->{$hinsi}->[$x];
	    }
	} elsif( &_zerop($x) ){
	    return '*';
	}
	carp "Unknown bunrui ($x)" if $WARNING;
    }
    undef;
}

=item get_bunrui_id ( HINSI, STR )

��ʬ�फ���ʬ���ֹ������

=cut
sub get_bunrui_id {
    if( @_ == 3 ){
	shift;
    } elsif( @_ != 2 ){
        local $LIST_SEPARATOR = ', ';
        croak "get_bunrui_id(@_): requires 2 arguments";
    }
    my( $hinsi, $x ) = @_;
    if( defined( $hinsi = &get_hinsi($hinsi) ) ){
	if( exists $BUNRUI->{$hinsi} ){
	    if( exists $BUNRUI->{$hinsi}->[0]->{$x} ){
		return $BUNRUI->{$hinsi}->[0]->{$x};
	    } elsif( &_indexp($x) and defined $BUNRUI->{$hinsi}->[$x] ){
		return $x;
	    }
	} elsif( &_zerop($x) ){
	    return 0;
	}
	carp "Unknown bunrui id ($x)" if $WARNING;
    }
    undef;
}

=item get_type ( ID )

���ѷ��ֹ椫����ѷ�������

=cut
sub get_type {
    if( @_ == 2 ){
	shift;
    } elsif( @_ != 1 ){
        local $LIST_SEPARATOR = ', ';
        croak "get_type_id(@_): requires an argument";
    }
    my( $x ) = @_;
    if( &_zerop($x) ){
	'*';
    } elsif( exists $TYPE->[0]->{$x} ){
	$x;
    } elsif( &_indexp($x) and defined $TYPE->[$x] ){
	$TYPE->[$x]->[0];
    } else {
	carp "Unknown katuyou type ($x)" if $WARNING;
	undef;
    }
}

=item get_type_id ( STR )

���ѷ�������ѷ��ֹ������

=cut
sub get_type_id {
    if( @_ == 2 ){
	shift;
    } elsif( @_ != 1 ){
        local $LIST_SEPARATOR = ', ';
        croak "get_type_id(@_): requires an argument";
    }
    my( $x ) = @_;
    if( &_zerop($x) ){
	0;
    } elsif( exists $TYPE->[0]->{$x} ){
	$TYPE->[0]->{$x};
    } elsif( &_indexp($x) and defined $TYPE->[$x] ){
	$x;
    } else {
	carp "Unknown katuyou id ($x)" if $WARNING;
	undef;
    }
}

=item get_form ( TYPE, ID )

���ѷ��ȳ��ѷ��ֹ椫����ѷ�������

=cut
sub get_form {
    if( @_ == 3 ){
	shift;
    } elsif( @_ != 2 ){
        local $LIST_SEPARATOR = ', ';
        croak "get_form(@_): requires 2 arguments";
    }
    my( $type, $x ) = @_;
    if( defined( $type = &get_type($type) ) ){
	if( $type eq '*' ){
	    if( &_zerop($x) ){
		return '*';
	    }
	} elsif( exists $FORM->{$type} ){
	    if( exists $FORM->{$type}->[0]->{$x} ){
		return $x;
	    } elsif( &_indexp($x) and defined $FORM->{$type}->[$x] ){
		return $FORM->{$type}->[$x]->[0];
	    }
	}
	carp "Unknown katuyou form ($x)" if $WARNING;
    }
    undef;
}

=item get_form_id ( TYPE, STR )

���ѷ��ȳ��ѷ�������ѷ��ֹ������

=cut
sub get_form_id {
    if( @_ == 3 ){
	shift;
    } elsif( @_ != 2 ){
        local $LIST_SEPARATOR = ', ';
        croak "get_form_id(@_): requires 2 arguments";
    }
    my( $type, $x ) = @_;
    if( defined( $type = &get_type($type) ) ){
	if( $type eq '*' ){
	    if( &_zerop($x) ){
		return 0;
	    }
	} elsif( exists $FORM->{$type} ){
	    if( exists $FORM->{$type}->[0]->{$x} ){
		return $FORM->{$type}->[0]->{$x};
	    } elsif( &_indexp($x) and defined $FORM->{$type}->[$x] ){
		return $x;
	    }
	}
	carp "Unknown katuyou form id ($x)" if $WARNING;
    }
    undef;
}

1;

=back

=head1 NOTES

C<Juman> ���֥������ȤΥ᥽�åɤȤ������Ѥ��뤳�Ȥ�Ǥ��롥

  Example:

     use Juman;
     $juman = new Juman();
     $id = $juman->get_hinsi_id( '̾��' );

=head1 SEE ALSO

=over 4

=item *

L<Juman>

=item *

L<Juman::Grammar>

=back

=head1 AUTHOR

=over 4

=item
�ڲ� ��̭ <tsuchiya@pine.kuee.kyoto-u.ac.jp>

=back

=cut

__END__
# Local Variables:
# mode: perl
# coding: euc-japan
# use-kuten-for-period: nil
# use-touten-for-comma: nil
# End:
