#
# Common subroutines to support tests
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

#
# Retrieve quantum depth
#
sub quantumDepth ( ) {
  my($depth,$image);
  $image=Image::Magick->new;
  $depth=$image->Get('depth');
  undef $image;
  return $depth;
}

#
# Establish some global constants (MaxRGB, OpaqueOpacity, & QuantumDepth)
#
$QuantumDepth=quantumDepth();
$OpaqueOpacity=0;
if ($QuantumDepth == 8)
  {
    $MaxRGB=255;
  }
elsif ($QuantumDepth == 16)
  {
    $MaxRGB=65535;
  }
elsif ($QuantumDepth == 32)
  {
    $MaxRGB=4294967295;
  }

#
# Test reading a 16-bit file in which two signatures are possible,
# depending on whether 16-bit pixels data has been enabled
#
# Usage: testRead( read filename, expected md5 [, expected md5_16] );
#
sub testRead {
  my( $infile, $md5, $md5_16 ) =  @_;

  my($image);

  if ( !defined( $md5_16 ) )
    {
      $md5_16 = $md5;
    }
  
  $image=Image::Magick->new;
  $image->Set(size=>'512x512');
  $status=$image->ReadImage("$infile");
  if( "$status" ) {
    print "ReadImage $infile: $status";
    print "not ok $test\n";
  } else {
    $signature=$image->Get('signature');
    if ( ( $signature ne $md5 ) and ( $signature ne $md5_16 ) ) {
      print "Image: $infile, signatures do not match.\n";
      print "       Computed: $signature\n";
      print "       Expected: $md5\n";
      if ( $md5 ne $md5_16 ) {
         print "      if 16-bit: $md5_16\n";
      }
      #$image->Display();
      print "not ok $test\n";
    } else {
      print "ok $test\n";
    }
  }
}

#
# Test reading a file, and compare with a reference file
#
sub testReadCompare {
  my( $srcimage_name,$refimage_name, $read_options,
      $normalized_mean_error_max, $normalized_maximum_error_max) = @_;
  my($srcimage, $refimage, $normalized_mean_error, $normalized_maximum_error);

  if ( !defined( $md5_16 ) )
    {
      $md5_16 = $md5;
    }

  $errorinfo='';

  # Create images
  $srcimage=Image::Magick->new;
  $refimage=Image::Magick->new;  

  eval "\$status=\$srcimage->Set($read_options);";
  if ("$status")
    {
      $errorinfo = "Set($read_options): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  $status=$srcimage->ReadImage($srcimage_name);
  if ("$status")
    {
      $errorinfo = "Readimage ($srcimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

#  $status=$srcimage->write(filename=>"$refimage_name", compression=>'None');
#  warn "$status" if $status;

  $status=$refimage->ReadImage("$refimage_name");
  if ("$status")
    {
      $errorinfo = "Readimage ($refimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  $srcimage->set(depth=>8);

  # FIXME: The following statement should not be needed.
#  $status=$refimage->Set(type=>'TrueColor');
#  if ("$status")
#    {
#      $errorinfo = "Set(type=>'TrueColor'): $status";
#      goto COMPARE_RUNTIME_ERROR;
#    }

  $status=$srcimage->Compare($refimage);
  if ("$status")
    {
      $errorinfo = "Compare($refimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  $normalized_mean_error=0;
  $normalized_mean_error=$srcimage->GetAttribute('mean-error');
  if ( !defined($normalized_mean_error) )
    {
      $errorinfo = "GetAttribute('mean-error') returned undefined value!";
      goto COMPARE_RUNTIME_ERROR;
    }
  $normalized_maximum_error=0;
  $normalized_maximum_error=$srcimage->GetAttribute('maximum-error');
  if ( ! defined($normalized_maximum_error) )
    {
      $errorinfo = "GetAttribute('maximum-error') returned undefined value!";
      goto COMPARE_RUNTIME_ERROR;
    }
  if ( ($normalized_mean_error > $normalized_mean_error_max) ||
       ($normalized_maximum_error > $normalized_maximum_error_max) )
    {
      print("mean-error=$normalized_mean_error, maximum-error=$normalized_maximum_error\n");
      print "not ok $test\n";
      return 1
    }

  print "ok $test\n";
  return 0;

 COMPARE_RUNTIME_ERROR:
  warn("$errorinfo");
  print "not ok $test\n";
  return 1
}

#
# Test reading a file which requires a file size to read (GRAY, RGB, CMYK)
# or supports multiple resolutions (JBIG, JPEG, PCD)
#
# Usage: testRead( read filename, size, depth, expected md5 [, expected md5_16] );
#
sub testReadSized {
  my( $infile, $size, $depth, $md5, $md5_16 ) =  @_;
  
  my($image);

  $image=Image::Magick->new;

  # Set size attribute
  $status=$image->SetAttribute(size=>"$size");
  warn "$status" if "$status";

  # If depth is not zero, then set it
  if ( $depth != 0 ) {
    $status=$image->SetAttribute(depth=>"$depth");
    warn "$status" if "$status";
  }

  $status=$image->ReadImage("$infile");
  if( "$status" ) {
    print "ReadImage $infile: $status";
    print "not ok $test\n";
  } else {
    $signature=$image->Get('signature');
    if ( ( $signature ne $md5 ) and ( $signature ne $md5_16 ) ) {
      print "Image: $infile, signatures do not match.\n";
      print "       Computed: $signature\n";
      print "       Expected: $md5\n";
      if ( $md5 ne $md5_16 ) {
         print "      if 16-bit: $md5_16\n";
      }
      #$image->Display();
      print "not ok $test\n";
    } else {
      print "ok $test\n";
    }
  }
}

#
# Test writing a file by first reading a source image, writing to a new image,
# reading the written image, and comparing with expected MD5.
#
# Usage: testReadWrite( read filename, write filename, write options,
#    expected md5 [, expected md5_16] );
#
# .e.g
#
# testReadWrite( 'input.jpg', 'output.jpg', q/quality=>80, interlace=>'None'/,
#                'dc0a144a0b9480cd1e93757a30f01ae3' );
#
# If the MD5 of the written image is not what is expected, the written
# image is preserved.  Otherwise, the written image is removed.
#
sub testReadWrite {
  my( $infile, $outfile, $writeoptions, $md5, $md5_16 ) = @_;
  
  my($image);

  if ( !defined( $md5_16 ) )
    {
      $md5_16 = $md5;
    }

  $image=Image::Magick->new;
  $status=$image->ReadImage("$infile");
  $signature=$image->Get('signature');
  if( "$status" ) {
    print "ReadImage $infile: $status\n";
    print "not ok $test\n";
  } else {
    # Write image to file
    my $options = 'filename=>"$outfile", ' . "$writeoptions";
    #print "Using options: $options\n";
    eval "\$status=\$image->WriteImage( $options ) ;";
    if( $@ ) {
      print "$@\n";
      print "not ok $test\n";
      exit 1;
    }
    if( "$status" ) {
      print "WriteImage $outfile: $status\n";
      print "not ok $test\n";
    } else {
      my($image);

      # Read image just written
      $image=Image::Magick->new;
      $status=$image->ReadImage("$outfile");
      if( "$status" ) {
	print "ReadImage $outfile: $status\n";
	print "not ok $test\n";
      } else {
	# Check signature
	$signature=$image->Get('signature');
        if ( ( $signature ne $md5 ) and ( $signature ne $md5_16 ) ) {
          print "Image: $infile, signatures do not match.\n";
          print "       Computed: $signature\n";
          print "       Expected: $md5\n";
          if ( $md5 ne $md5_16 ) {
             print "      if 16-bit: $md5_16\n";
          }
	  print "not ok $test\n";
	} else {
	  print "ok $test\n";
	  ($file = $outfile) =~ s/.*://g;
	  unlink "$file";
	}
      }
    }
  }
}

#
# Test reading a file, and compare with a reference file
#
sub testReadWriteCompare {
  my( $srcimage_name, $outimage_name, $refimage_name,
      $read_options, $write_options,
      $normalized_mean_error_max, $normalized_maximum_error_max) = @_;
  my($srcimage, $refimage, $normalized_mean_error,
    $normalized_maximum_error);

  if ( !defined( $md5_16 ) )
    {
      $md5_16 = $md5;
    }

  $errorinfo='';

  $image=Image::Magick->new;
  $refimage=Image::Magick->new;  

  #
  # Read the initial image
  #
  $status=$image->ReadImage($srcimage_name);
  if ("$status")
    {
      $errorinfo = "Readimage ($srcimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  #
  # Write image to output file
  #
  eval "\$status=\$image->Set($write_options);";
  if ("$status")
    {
      $errorinfo = "Set($write_options): $status";
      goto COMPARE_RUNTIME_ERROR;
    }
  $image->Set(filename=>"$outimage_name");

  $status=$image->WriteImage( );
  if ("$status")
    {
      $errorinfo = "WriteImage ($outimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  undef $image;
  $image=Image::Magick->new;

  #
  # Read image from output file
  #
  eval "\$status=\$image->Set($read_options);";
  if ("$status")
    {
      $errorinfo = "Set($read_options): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  $image->ReadImage("$outimage_name");
  if ("$status")
    {
      $errorinfo = "WriteImage ($outimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

# eval "\$status=\$image->Set($write_options);";
# $status=$image->write(filename=>"$refimage_name", compression=>'None');
# warn "$status" if $status;

  #
  # Read reference image
  #
  $status=$refimage->ReadImage("$refimage_name");
  if ("$status")
    {
      $errorinfo = "Readimage ($refimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  #
  # Compare output file with reference image
  #

  $image->set(depth=>8);

  # FIXME: The following statement should not be needed.
#  $status=$refimage->Set(type=>'TrueColor');
#  if ("$status")
#    {
#      $errorinfo = "Set(type=>'TrueColor'): $status";
#      goto COMPARE_RUNTIME_ERROR;
#    }

  $status=$image->Compare($refimage);
  if ("$status")
    {
      $errorinfo = "Compare($refimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  $normalized_mean_error=0;
  $normalized_mean_error=$image->GetAttribute('mean-error');
  if ( !defined($normalized_mean_error) )
    {
      $errorinfo = "GetAttribute('mean-error') returned undefined value!";
      goto COMPARE_RUNTIME_ERROR;
    }
  $normalized_maximum_error=0;
  $normalized_maximum_error=$image->GetAttribute('maximum-error');
  if ( ! defined($normalized_maximum_error) )
    {
      $errorinfo = "GetAttribute('maximum-error') returned undefined value!";
      goto COMPARE_RUNTIME_ERROR;
    }

  if ( ($normalized_mean_error > $normalized_mean_error_max) ||
       ($normalized_maximum_error > $normalized_maximum_error_max) )
    {
      print("mean-error=$normalized_mean_error, maximum-error=$normalized_maximum_error\n");
      print "not ok $test\n";
      return 1
    }

  print "ok $test\n";
  undef $image;
  undef $refimage;
  return 0;

 COMPARE_RUNTIME_ERROR:
  warn("$errorinfo");
  print "not ok $test\n";
  undef $image;
  undef $refimage;
  return 1
}

#
# Test writing a file by first reading a source image, writing to a
# new image, and reading the written image.  Depends on detecting
# reported errors by ImageMagick
#
# Usage: testReadWrite( read filename, write filename, write options);
#
# .e.g
#
# testReadWrite( 'input.jpg', 'output.jpg', q/quality=>80, 'interlace'=>'None'/ );
#
# If the read of the written image is not what is expected, the
# written image is preserved.  Otherwise, the written image is
# removed.
#
sub testReadWriteNoVerify {
  my( $infile, $outfile, $writeoptions) = @_;
  
  my($image, $images);
  
  $image=Image::Magick->new;
  $status=$image->ReadImage("$infile");
  if( "$status" ) {
    print "$status\n";
    print "ReadImage $infile: not ok $test\n";
  } else {
    # Write image to file
    my $options = 'filename=>"$outfile", ' . $writeoptions;
    #print "Using options: $options\n";
    eval "\$status=\$image->WriteImage( $options ) ;";
    if( $@ ) {
      print "$@";
      print "not ok $test\n";
      exit 1;
    }
    if( "$status" ) {
      print "WriteImage $outfile: $status\n";
      print "not ok $test\n";
    } else {
      my($image);

      # Read image just written
      $image=Image::Magick->new;
      $status=$image->ReadImage("$outfile");
      if( "$status" ) {
	print "ReadImage $outfile: $status\n";
	print "not ok $test\n";
      } else {
	print "ok $test\n";
	unlink $outfile;
      }
    }
  }
}

#
# Test writing a file by first reading a source image, writing to a new image,
# reading the written image, and comparing with expected MD5.
#
# Usage: testReadWriteSized( read filename,
#                            write filename,
#                            read filename size,
#                            read filename depth,
#                            write options,
#                            expected md5 [,expected md5_16] );
#
# .e.g
#
# testReadWriteSized( 'input.jpg', 'output.jpg', '70x46', 8, q/quality=>80,
#                     'interlace'=>'None'/, 'dc0a144a0b9480cd1e93757a30f01ae3' );
#
# If the MD5 of the written image is not what is expected, the written
# image is preserved.  Otherwise, the written image is removed.  A depth of 0 is
# ignored.
#
sub testReadWriteSized {
  my( $infile, $outfile, $size, $readdepth, $writeoptions, $md5, $md5_16 ) = @_;
  
  my($image);
  
  $image=Image::Magick->new;

  # Set size attribute
  $status=$image->SetAttribute(size=>"$size");
  warn "$status" if "$status";

  # If read depth is not zero, then set it
  if ( $readdepth != 0 ) {
    $status=$image->SetAttribute(depth=>$readdepth);
    warn "$status" if "$status";
  }

  $status=$image->ReadImage("$infile");
  if( "$status" ) {
    print "ReadImage $infile: $status\n";
    print "not ok $test\n";
  } else {
    # Write image to file
    my $options = 'filename=>"$outfile", ' . "$writeoptions";
    #print "Using options: $options\n";
    eval "\$status=\$image->WriteImage( $options ) ;";
    if( $@ ) {
      print "$@\n";
      print "not ok $test\n";
      exit 1;
    }
    if( "$status" ) {
      print "WriteImage $outfile: $status\n";
      print "not ok $test\n";
    } else {
      my($image);

      $image=Image::Magick->new;

      if ( $readdepth != 0 ) {
        $status=$image->SetAttribute(depth=>$readdepth);
        warn "$status" if "$status";
      }
      # Set image size attribute
      $status=$image->SetAttribute(size=>"$size");
      warn "$status" if "$status";

      # Read image just written
      $status=$image->ReadImage("$outfile");
      if( "$status" ) {
	print "ReadImage $outfile: $status\n";
	print "not ok $test\n";
      } else {
	# Check signature
	$signature=$image->Get('signature');
        if ( ( $signature ne $md5 ) and ( $signature ne $md5_16 ) ) {
          print "Image: $infile, signatures do not match.\n";
          print "       Computed: $signature\n";
          print "       Expected: $md5\n";
          if ( $md5 ne $md5_16 ) {
             print "   if 16-bit: $md5_16\n";
          }
	  print "not ok $test\n";
          #$image->Display();
	} else {
	  print "ok $test\n";
	  #$image->Display();
	  ($file = $outfile) =~ s/.*://g;
	  unlink "$file";
	}
      }
    }
  }
}

#
# Test SetAttribute method
#
# Usage: testSetAttribute( name, attribute);
#
sub testSetAttribute {
  my( $srcimage, $name, $attribute ) = @_;

  my($image);
  
  # Create temporary image
  $image=Image::Magick->new;

  $status=$image->ReadImage("$srcimage");
  warn "Readimage: $status" if "$status";

  # Set image option
  print "Image Option  : $name=>$attribute\n";
  eval "\$status = \$image->Set('$name'=>'$attribute') ;";
  warn "SetImage: $status" if "$status";

  # Convert input values to expected output values
  $expected=$attribute;
  if ($attribute eq 'True' || $attribute eq 'true') {
    $expected = 1;
  } elsif ($attribute eq 'False' || $attribute eq 'false') {
    $expected = 0;
  }


  $value=$image->GetAttribute($name);

  if( defined( $value ) ) {
    if ("$expected" eq "$value") {
      print "ok $test\n";
    } else {
      print "Expected ($expected), Got ($value)\n";
      print "not ok $test\n";
    }
  } else {
    print "GetAttribute returned undefined value!\n";
    print "not ok $test\n";
  }
}

#
# Test GetAttribute method
#
# Usage: testGetAttribute( name, expected);
#
sub testGetAttribute {
  my( $srcimage, $name, $expected ) = @_;

  my($image);

  # Create temporary image
  $image=Image::Magick->new;

  $status=$image->ReadImage("$srcimage");
  warn "Readimage: $status" if "$status";

  $value=$image->GetAttribute($name);

  if( !defined( $expected ) && !defined( $value ) ) {
    # Undefined value is expected
    print "ok $test\n";
  } elsif ( !defined( $value ) ) {
    print "Expected ($expected), Got (undefined)\n";
    print "not ok $test\n";
  } else {
    if ("$expected" eq "$value") {
      print "ok $test\n";
    } else {
      print "Expected ($expected), Got ($value)\n";
      print "not ok $test\n";
    }
  }
}

#
# Test MontageImage method
#
# Usage: testMontage( input image attributes, montage options, expected MD5
#       [, expected MD5_16] );
#
sub testMontage {
  my( $imageOptions, $montageOptions, $md5, $md5_16 ) = @_;

  my($image);

  # Create image for image list
  $images=Image::Magick->new;

  # Create temporary image
  $image=Image::Magick->new;

  my @colors = ( '#000000', '#008000', '#C0C0C0', '#00FF00',
		 '#808080', '#808000', '#FFFFFF', '#FFFF00',
		 '#800000', '#000080', '#FF0000', '#0000FF',
		 '#800080', '#008080', '#FF00FF', '#00FFFF' );
  
  my $color;
  foreach $color ( @colors ) {

    # Generate image
    $image->Set(size=>'50x50');
    $status=$image->ReadImage("xc:$color");
    warn "Readimage: $status" if "$status";

    # Add image to list
    push( @$images, @$image);
    
    undef @$image;
  }

  # Set image options
  #print "Image Options  : $imageOptions\n";
  eval "\$status = \$images->Set($imageOptions) ;";
  warn "SetImage: $status" if "$status";

  #print "Border color : ", $images->Get('bordercolor'), "\n";
  #print "Matte color  : ", $images->Get('mattecolor'), "\n";
  #print "Pen color    : ", $images->Get('pen'), "\n";

  # Do montage
  #print "Montage Options: $montageOptions\n";
  eval "\$montage=\$images->Montage( $montageOptions ) ;";
  if( $@ ) {
    print "$@";
    print "not ok $test\n";
    return 1;
  }
  
  if( ! ref($montage) ) {
    print "not ok $test\n";
  } else {
    #$montage->Display();
    # Check MD5 signature
    $signature=$montage->GetAttribute('signature');
    if ( defined( $signature ) ) {
      if ( ( $signature ne $md5 ) and ( $signature ne $md5_16 ) ) {
          print "Test $test, signatures do not match.\n";
          print "       Computed: $signature\n";
          print "       Expected: $md5\n";
          if ( $md5 ne $md5_16 ) {
             print "   if 16-bit: $md5_16\n";
          }
	
        $status = $montage->Write("test_${test}_out.miff");
        warn "Write: $status" if "$status";
	
	print "not ok $test\n";
      } else {
	# Check montage directory
	my $directory = $montage->Get('directory');
	my $expected = join( "\n", @colors ) . "\n";
	if ( !defined($directory) ) {
	  print "ok $test\n";
	} elsif ( $directory  ne $expected) {
	  print("Invalid montage directory:\n\"$directory\"\n");
	  print("Expected:\n\"$expected\"\n");
	  print "not ok $test\n";
	} else {
	  # Check montage geometry
	  $montage_geom=$montage->Get('montage');
	  if( !defined($montage_geom) ) {
	    print("Montage geometry not defined!\n");
	    print "not ok $test\n";
	  } elsif ( $montage_geom !~ /^\d+x\d+\+\d+\+\d+$/ ) {
	    print("Montage geometry not in correct format: \"$montage_geom\"\n");
	    print "not ok $test\n";
	  } else {
	    print "ok $test\n";
	  }
	}
      }
    } else {
      warn "GetAttribute returned undefined value!";
      print "not ok $test\n";
    }
  }
}

#
# Test filter method using signature compare
#
# Usage: testFilterSignature( input image attributes, filter, options, expected MD5
#      [, expected MD5_16] );
#
sub testFilterSignature {
  my( $srcimage, $filter, $filter_options, $md5, $md5_16 ) = @_;

  my($image);

  print( $filter, " ...\n" );

  # Create temporary image
  $image=Image::Magick->new;

  $status=$image->ReadImage("$srcimage");
  warn "Readimage: $status" if "$status";

  $image->$filter($filter_options);
#$image->write(filename=>"reference/filter/$filter.miff", compression=>'None');

  $signature=$image->GetAttribute('signature');
  if ( defined( $signature ) ) {
    if ( ( $signature ne $md5 ) and ( $signature ne $md5_16 ) ) {
      print "Test $test, signatures do not match.\n";
      print "       Computed: $signature\n";
      print "       Expected: $md5\n";
      if ( $md5 ne $md5_16 ) {
         print "   if 16-bit: $md5_16\n";
      }
      #$image->Display();
      print "not ok $test\n";
    } else {
      print "ok $test\n";
    }
  } else {
    warn "GetAttribute returned undefined value!";
    print "not ok $test\n";
  }
}

#
# Test filter method using comparison with reference image
#
# Usage: testFilterCompare( input image, input image options, reference image, filter, filter options,
#                           normalized_mean_error,
#                           normalized_maximum_error );
sub testFilterCompare {
  my ($srcimage_name, $src_read_options, $refimage_name, $filter,
      $filter_options, $normalized_mean_error_max,
      $normalized_maximum_error_max) = @_;
  my($srcimage, $refimage, $normalized_mean_error,
    $normalized_maximum_error);
  my($status,$errorinfo);

  $errorinfo='';
  $status='';

  print( $filter, " ...\n" );

  # Create images
  $srcimage=Image::Magick->new;
  $refimage=Image::Magick->new;

  eval "\$status=\$srcimage->Set($src_read_options);";
  if ("$status")
    {
      $errorinfo = "Set($src_read_options): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  $status=$srcimage->ReadImage($srcimage_name);
  #eval "\$status=\$srcimage->ReadImage($srcimage_name);";
  if ("$status")
    {
      $errorinfo = "Readimage ($srcimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  eval "\$status=\$srcimage->$filter($filter_options);";
  if ("$status")
    {
      $errorinfo = "$filter ($filter_options): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  $srcimage->set(depth=>8);
#  $srcimage->write(filename=>"$refimage_name", compression=>'None');

  $status=$refimage->ReadImage("$refimage_name");
  if ("$status")
    {
      $errorinfo = "Readimage ($refimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  # FIXME: The following statement should not be needed.
#  $status=$refimage->Set(type=>'TrueColor');
#  if ("$status")
#    {
#      $errorinfo = "Set(type=>'TrueColor'): $status";
#      goto COMPARE_RUNTIME_ERROR;
#    }

  $status=$srcimage->Compare($refimage);
  if ("$status")
    {
      $errorinfo = "Compare($refimage_name): $status";
      goto COMPARE_RUNTIME_ERROR;
    }

  $normalized_mean_error=0;
  $normalized_mean_error=$srcimage->GetAttribute('mean-error');
  if ( !defined($normalized_mean_error) )
    {
      $errorinfo = "GetAttribute('mean-error') returned undefined value!";
      goto COMPARE_RUNTIME_ERROR;
    }
  $normalized_maximum_error=0;
  $normalized_maximum_error=$srcimage->GetAttribute('maximum-error');
  if ( ! defined($normalized_maximum_error) )
    {
      $errorinfo = "GetAttribute('maximum-error') returned undefined value!";
      goto COMPARE_RUNTIME_ERROR;
    }
  if ( ($normalized_mean_error > $normalized_mean_error_max) ||
       ($normalized_maximum_error > $normalized_maximum_error_max) )
    {
      print("mean-error=$normalized_mean_error, maximum-error=$normalized_maximum_error\n");
      print "not ok $test\n";
      return 1
    }

  print "ok $test\n";
  return 0;

 COMPARE_RUNTIME_ERROR:
  warn("$errorinfo");
  print "not ok $test\n";
  return 1
}
1;
