#!/usr/bin/perl
#
# $Id$
#
# Format ImageMagick comments into POD-format documentation
# Produces *.pod files corresponding to *.c files
#

#
# Source files to use
#
@srcs = ('animate.c',
	 'annotate.c',
	 'attribute.c',
	 'blob.c',
	 'cache.c',
	 'cache_view.c',
	 'color.c',
	 'constitute.c',
	 'decorate.c',
	 'display.c',
	 'effect.c',
	 'enhance.c',
	 'error.c',
	 'image.c',
	 'list.c',
	 'magick.c',
	 'memory.c',
	 'monitor.c',
	 'montage.c',
	 'quantize.c',
	 'render.c',
	 'registry.c',
	 'segment.c',
	 'shear.c',
	 'signature.c',
	 'stream.c',
	 'transform.c',
	 'widget.c',
	 'zoom.c' );

$tmpname = "/tmp/fmtdocs.$$";
#$tmpname = '/tmp/fmtdocs.tmp';

#@srcs = ('render.c');

#
# What is for source files
#
%whatis =
(
 'animate',	'Methods to Interactively Animate an Image Sequence',
 'annotate',	'Methods to Annotate an Image',
 'attribute',	'Image Text Attributes Methods',
 'blob',	'Methods to Read or Write Binary Large OBjects',
 'color',	'Methods to Count the Colors in an Image',
 'constitute',	'Methods to Constitute an Image',
 'decorate',	'ImageMagick Image Decoration Methods',
 'display',	'Methods to Interactively Display and Edit an Image',
 'render',	'Methods to Draw on an Image',
 'effect',	'ImageMagick Image Effects Methods',
 'enhance',	'Methods to Enhance an Image',
 'error',	'ImageMagick Error Methods',
 'image',	'ImageMagick Image Methods',
 'list',	'Methods to Work with Image Lists',
 'cache',	'Methods to Get or Set Image Pixels',
 'cache_view',	'ImageMagick Cache Views Methods',
 'magick',	'Methods to Read or List ImageMagick Image formats',
 'memory',	'ImageMagick Memory Allocation Methods',
 'monitor',	'ImageMagick Progress Monitor Methods',
 'montage',	'Methods to Create a Montage',
 'quantize',	'Methods to Reduce the Number of Unique Colors in an Image',
 'Registry',	'ImageMagick Registry Methods',
 'segment',	'Methods to Segment an Image with Thresholding Fuzzy c-Means',
 'shear',	'Methods to Shear or Rotate an Image by an Arbitrary Angle',
 'signature',	'Methods to Compute a Digital Signature for an Image',
 'stream',	'Image Pixel FIFO',
 'transform',	'Methods to Transform an Image',
 'zoom',	'Methods to Resize an Image',
);

#
# Key words to replace with HTML links
#
my %keywords =
  (
   AnnotateInfo		=> 'types/AnnotateInfo.html',
   BlobInfo		=> 'types/BlobInfo.html',
   ChromaticityInfo	=> 'types/ChromaticityInfo.html',
   ClassType		=> 'types/Enumerations.html#ClassType',
   ColorPacket		=> 'types/ColorPacket.html',
   ColorspaceType	=> 'types/Enumerations.html#ColorspaceType',
   CompositeOperator	=> 'types/Enumerations.html#CompositeOperator',
   CompressionType	=> 'types/Enumerations.html#CompressionType',
   DrawInfo		=> 'types/DrawInfo.html',
   ExceptionInfo	=> 'types/ExceptionInfo.html',
   ExceptionType	=> 'types/Enumerations.html#ExceptionType',
   FilterTypes		=> 'types/Enumerations.html#FilterTypes',
   FrameInfo		=> 'types/FrameInfo.html',
   GravityType		=> 'types/Enumerations.html#GravityType',
   Image		=> 'types/Image.html',
   ImageInfo		=> 'types/ImageInfo.html',
   ImageType		=> 'types/Enumerations.html#ImageType',
   InterlaceType	=> 'types/Enumerations.html#InterlaceType',
   LayerType		=> 'types/Enumerations.html#LayerType',
   MagickInfo		=> 'types/MagickInfo.html',
   MontageInfo		=> 'types/MontageInfo.html',
   NoiseType		=> 'types/Enumerations.html#NoiseType',
   PaintMethod		=> 'types/Enumerations.html#PaintMethod',
   PixelPacket		=> 'types/PixelPacket.html',
   PointInfo		=> 'types/PointInfo.html',
   ProfileInfo		=> 'types/ProfileInfo.html',
   QuantizeInfo		=> 'types/QuantizeInfo.html',
   QuantumTypes		=> 'types/Enumerations.html#QuantumTypes',
   RectangleInfo	=> 'types/RectangleInfo.html',
   RenderingIntent	=> 'types/Enumerations.html#RenderingIntent',
   ResolutionType	=> 'types/Enumerations.html#ResolutionType',
   XResourceInfo	=> 'types/XResourceInfo.html',
   ViewInfo		=> 'types/ViewInfo.html',
);


foreach $src (@srcs) {

    # Compute POD name
    ($base = $src) =~ s/\.[^\.]*$//g;
    $doc = "${base}.pod";

    print( "Processing $src -> $doc\n" );

    pass1($src, $tmpname);	# Make easily parsed
    pass2($tmpname, $doc);	# Format
}
#unlink($tmpname);
exit(0);

#
# Pre-process file into intermediate form
#
# Initializes globals:
#
#  @functions	- Function names
#  %synopsis	- Function synopsis
#
sub pass1 {
  my($infile, $tmpfile) = @_;
  
  my $inpara = 0;	# Set to 1 if in paragraph
  my $inlist = 0;	# Set to 1 if in list-item paragraph
  
  # Open C source file
  open( IN, "<$infile" ) || die("Failed to open \"$infile\" for read\n" );
  
  # Open TMP file
  open( TMP, ">$tmpfile" ) || die("Failed to open \"$tmpfile\" for write\n" );
  
  undef @functions;
  undef %synopsis;
  
  # Skip past first form feed
  while(<IN>) {
    last if m/\014/;
  }

LINE:  
  while(<IN>) {
    if (m/^\+/) {
      while(<IN>) {
	last unless m/^%/;
      }
      next;
    }
    next unless m/^%/ ;
    chop;
    
    # Extract and save function title
    if (m/^%\s+((\w )+)\s+%/) {
      ($ftitle = $1) =~ s/ //g;
      push(@functions, $ftitle);
      print( TMP "===$ftitle\n" );
      next;
    }
    
    # Zap text we don't want
    next if ( m/^%.+%/ );	# "%*%
    s/^%\s{0,2}//;
    
    # Extract and save synopsis info
    if (m /\(\)/ ) {
      # nothing
      ;
    }
    elsif ( m/${ftitle}\(.*\)$/ ) {
      s/,/ , /g;
      s/\(/ ( /g;
      s/\)/ ) /g;
      s/\*/ * /g;
      s/\s+/ /g;

      s/\(\s+\*/(*/g;
      s/ ,/,/g;
      s/ \(/(/g;
      s/\) /)/g;
      s/ \* / */g;

      s/^\s*//;
      $synopsis{$ftitle} = $_ . ';'; # Append semi-colon, prototype style
      print ( TMP " " . $synopsis{$ftitle} . "\n" );
      next LINE;
     }
     elsif ( m/${ftitle}\(.*/ ) {
      $synopsis{$ftitle} = $_;
      do {
	$_ = <IN>;
	chop;
	# Zap text we don't want
	next if m/^%.+%/;	# "%*%
	s/^%\s{0,2}//;
	$synopsis{$ftitle} .= $_;
      } until m/^\s*$/;
      $_ = $synopsis{$ftitle};

      s/,/ , /g;
      s/\(/ ( /g;
      s/\)/ ) /g;
      s/\*/ * /g;
      s/\s+/ /g;

      s/\(\s+\*/(*/g;
      s/ ,/,/g;
      s/ \(/(/g;
      s/\) /)/g;
      s/ \* / */g;

      s/^\s*//;
      $synopsis{$ftitle} = $_ . ';'; # Append semi-colon, prototype style
      print ( TMP " " . $synopsis{$ftitle} . "\n" );
      next LINE;
    }
  
  # Keep track of paragraphing
  if( ! m/^$/ ) {
    if ( $inpara == 0 ) {
      $inpara = 1;	# Start of paragraph
      $para = "$_";	# Start paragraph string
    } else {
      # Inside paragraph
      $para .= " $_";	# Add line to paragraph
    }
  }
  # Keep track of list items so they can
  # be wrapped as a paragraph
  if( m/^\s+(o[^:]+:|o|[0-9]\.)\s(.*)/ ) {
    $inlist = 1;
  }
  
  
  if ( $inpara == 1 ) {
    if( $para =~ m/^\s+\S+/ && ! $inlist ) {
      # Lines that start with a space shouldn't be munged
      $inpara = 0;	# End of paragraph
      $inlist = 0;
      $para .= "";	# Terminate paragraph
      print( TMP "$para\n" );
    }
    elsif( m/^$/ ) {
      # End of paragraph
      $inpara = 0;	# End of paragraph
      $inlist = 0;
      $para .= "";	# Terminate paragraph
      
      $para =~ s/^\s+//g;		# Eliminate any leading space
      $para =~ s/\s+/ /g;		# Canonicalize whitespace
      $para =~ s/ $//;		# Trim final space
      $para =~ s/([a-zA-Z0-9][.!?][)'"]*) /$1  /g; #' Fix sentance ends
		  print( TMP "\n$para\n\n" );
		}
    }
  }
  
  close( TMP );
  close( IN );
}

#
# Second pass
# Process into formatted form
#
sub pass2 {
    my($infile, $outfile) = @_;

    my $func;

    my $inlist = 0;		# Set to one if in indented list

    # Open input file
    open( IN, "<$infile" ) || die("Failed to open \"$infile\" for read\n" );

    # Open output file
    open( OUT, ">$outfile" ) || die("Failed to open \"$tmpfile\" for write\n" );

    # Name field
    print( OUT head1("NAME") );
    print( OUT "${base} - $whatis{$base}\n\n" );

    # Synopsis field (function signatures)
    print( OUT head1("SYNOPSIS") );
    foreach $func (sort( @functions )) {
      if (defined $synopsis{$func} ) {
	$_ = $synopsis{$func};
	s/$func/ B<$func>/;
	s/^\s*//;
	my $synopsis = $_;
	print( OUT $synopsis, "\n\n" );
      }
    }
    
    # Description field
    print( OUT head1("FUNCTION DESCRIPTIONS") );

    while(<IN>){
	chop;
	next if m/^$/;

	# Match list element
	if( m/^(o[^:]+:|o|[0-9]\.?)\s(.*)/ ) {
	    my $bullet = $1;
	    my $bullet_text = $2;

	    print( OUT startlist() ) unless $inlist;
	    $inlist = 1;
	    print( OUT item($bullet), "$bullet_text\n\n" );
	    next;
	} else {
	    print( OUT endlist() ) if $inlist;
	    $inlist = 0;
	}

	# Match synopsis item
	if( defined $func && m/$func\s*\(.*\)/ ) {
	  # Split all words with spaces to aid with tokenization
	  s/,/ , /g;
	  s/\(/ ( /g;
	  s/\)/ ) /g;
	  s/\*/ * /g;

	  my $html = '';

	  # Replace tokens matching keywords with HTML links.
TOKEN:	  foreach $token ( split(' ', $_ ) ) {
	    foreach $keyword ( %keywords ) {
	      if ( $token eq $keyword ) {
		$html .= linked( $keyword, $keywords{$keyword} );
		$html .= " ";
		next TOKEN;
	      }
	    }
	    $html .= "$token ";
	  }
	  $_ = $html;
	  # Remove excess spaces
	  s/\s+/ /g;
	  s/ ,/,/g;
	  s/\* /*/g;
	  s/\)\s*\;/);/;
	  s/^\s*//;
          s/ \( *\)/\(\)/;
		  
	  print( OUT html("<blockquote>$_</blockquote>") );
	    next;
	}

	# Match function title
	if( m/===([a-zA-Z0-9]+)/ ) {
	    $func = $1;
	    print( OUT head2($func) );
	    next;
	}

        print( OUT "\n") if /^[^ ]/;
	print( OUT "$_\n") ;
        print( OUT "\n") if /^[^ ]/;
    }

    close( OUT );
    close( IN );
}

#
# Return level 1 heading
# Similar to: <H1>heading</H1>
#
sub head1 {
    my($heading) = @_;
    return( "=head1 $heading\n\n" );
}

#
# Return level 2 heading
# Similar to: <H2>heading</H2>
#
sub head2 {
    my($heading) = @_;
    return( "=head2 $heading\n\n" );
}


#
# Return item
# Simlar to: <I>
#
sub item {
    my($item) = @_;
    return( "=item $item\n\n" );
}


#
# Start list
# Similar to: <UL>
#
sub startlist {
    return( "=over 4\n\n" )
}

#
# End list
# Similar to: </UL>
#
sub endlist {
    return( "=back\n\n" );
}

#
# Preformatted text
# Similar to <PRE></PRE>
#
sub formated {
    my($text) = @_;
    return( " $text\n\n" );
}

#
# Raw HTML paragraph
#
sub html {
  my($html) = @_;
  return return( "=for html $html\n\n" );
}

#
# HTML Linl
# Similar to: <A HREF="url">description</A>
#
sub linked {
  local($description, $url) = @_;
  return( "<A HREF=\"" . $url . "\">" . $description . "</A>" );
}
