###########################################################
# MagickWand test
# - The purpose is to test the correctness of TclMagick,
#   not to produce meaningful output
#
###########################################################


############################################
# Load debugging version 
# or require TclMagick package from library
#
if {[file exists ../debug/tclMagick.dll]} {
    load ../debug/tclMagick.dll
}
package require TclMagick
puts [info script]


##########################################
# Global options
#
set IMG "logo:"
set SEQ "../images/sequence.miff"
set TMP "../tmp"

##########################################
# Check which tests should be performed
#
set TestFunctions {
    AdaptiveThresholdImage      img     1
    AddNoiseImage               img     1
    AffineTransformImage        img     1
    ???-how-to-AnnotateImage    img     0
    BlackThresholdImage         img     1
    BlurImage                   img     1
    BorderImage                 img     1
    ChannelImage                img     1
    CharcoalImage               img     1
    ChopImage                   img     1
    ???-how-to-ClipImage        img     0
    ColorFloodfillImage         img     1
    ColorizeImage               img     1
    CommentImage                img     1
    CompareImage                img     1
    CompositeImage              img     1
    ContrastImage               img     1
    ConvolveImage               img     1
    CropImage                   img     1
    CycleColormapImage          img     1
    DescribeImage               img     1
    DespeckleImage              img     1
    DrawImage                   img     1
    EdgeImage                   img     1
    EmbossImage                 img     1
    EnhanceImage                img     1
    EqualizeImage               img     1
    FlattenImage                seq     1
    FlipImage                   img     1
    FlopImage                   img     1
    FrameImage                  img     1
    Filename                    img     1
    GammaImage			img	1
    GetImageSignature		img	1
    ImplodeImage		img	1
    LabelImage 			img	1
    LevelImage 			img	1
    MagnifyImage		img	1
    MapImage 			img	1
    MatteFloodfillImage 	img	1
    MedianFilterImage 		img	1
    MinifyImage			img	1
    ModulateImage 		img	1
    MotionBlurImage 		img	1
    NegateImage 		img	1
    NormalizeImage		img	1
    OilPaintImage 		img	1
    OpaqueImage 		img	1
    ProfileImage		img	1
    QuantizeImage 		img	1
    RaiseImage 			img	1
    ReduceNoiseImage 		img	1
    ResampleImage		img	1
    ResizeImage                 img     1
    RollImage 			img	1
    RotateImage                 img     1
    SampleImage 		img	1
    SetFuzzyColorDistance       img     1
    ScaleImage 			img	1
    SharpenImage 		img	1
    ShaveImage 			img	1
    ShearImage 			img	1
    Size                        img     1
    SolarizeImage		img	1
    SpreadImage 		img	1
    SteganoImage		img	1
    StereoImage 		img	1
    StripImage			img	1
    SwirlImage 			img	1
    TextureImage		img	1
    ThresholdImage		img	1
    TransparentImage		img	1
    TrimImage 			img	1
    UnsharpMaskImage		img	1
    WaveImage 			img	1
    WhiteThresholdImage 	img	1

    Pixels                      img     1
    SequenceTest                seq     1
}

############################################
# Command debugging
#
catch { 
    wm withdraw . 
    console show 
    console eval {wm protocol . WM_DELETE_WINDOW exit}
}
proc debug {args} {
    foreach cmdMask $args {
        foreach cmd [info commands $cmdMask] {
            trace add execution $cmd leave debugLeave
        }
    }
}
proc debugLeave {cmdstr code result op} {
    puts [format "    %s ==>{%s}" [string range $cmdstr 0 50] [string range $result 0 50]]
    update
}

##########################################
# Single image tests
#
proc AdaptiveThresholdImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand AdaptiveThresholdImage 5 5 0
    $wand WriteImage "$::TMP/x-Adaptive.jpg"
    magick delete $wand
}
proc AddNoiseImage {img} {
    foreach noise {impulse poisson} {   ;# uniform gaussian multiplicativegaussian impulse laplacian poisson
        set wand [$img clone imgX]
        debug $wand
        $wand AddNoiseImage $noise
        $wand WriteImage "$::TMP/x-AddNoise-$noise.jpg"
        magick delete $wand
    }
}
proc AffineTransformImage {img} {
    set wand [$img clone imgX]
    set draw [magick create drawing]
    debug $wand $draw
   
    $draw Rotate 10
    $draw Affine 1.0 3.0 3.0 2.0 2.0 2.0
    $wand AffineTransformImage $draw
    $wand WriteImage "$::TMP/x-Affine.jpg"

    magick delete $draw $wand
}
proc AnnotateImage {img} {
    set wand [$img clone imgX]
    set draw [magick create drawing]
    debug $wand $draw
   
    $draw push graph
        $draw SetStrokeWidth 1
        $draw SetStrokeColorString "blue"
        $draw SetFillColorString "blue"
        $draw SetFontSize 18
        $draw Annotation 10 170 "TclMagick"
    $draw pop graph
    
    $wand AnnotateImage $draw
    $wand WriteImage "$::TMP/x-Annotate.jpg"

    magick delete $draw $wand
}
proc BlackThresholdImage {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel]
    debug $wand $pix

    $pix set red 0.5 green 0.5 blue 0.5
    $pix GetColor 
    $wand BlackThresholdImage $pix
    $wand WriteImage "$::TMP/x-BlackThreshold.jpg"

    magick delete $pix $wand
}
proc BlurImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand BlurImage 2 3
    $wand WriteImage "$::TMP/x-Blur.jpg"
    magick delete $wand
}
proc BorderImage {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel]
    debug $wand $pix
    
    $pix set red 1.0 green 1.0 blue 0.0
    $pix GetColor 
    $wand BorderImage $pix 4 6
    $wand WriteImage "$::TMP/x-Border.jpg"

    magick delete $pix $wand
}
proc ChannelImage {img} {
    foreach chan {red green yellow} {
        set wand [$img clone imgX]
        debug $wand    
        $wand ChannelImage $chan
        $wand WriteImage "$::TMP/x-Channel-$chan.jpg"
        magick delete $wand
    }
}
proc CharcoalImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand CharcoalImage 2.0 1.0
    $wand WriteImage "$::TMP/x-Charcoal.jpg"
    magick delete $wand
}
proc ChopImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ChopImage 100 60 350 200
    $wand WriteImage "$::TMP/x-Chop.jpg"
    magick delete $wand
}
proc ClipImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ClipImage
    $wand WriteImage "$::TMP/x-Clip.jpg"
    magick delete $wand
}
proc ColorFloodfillImage {img} {
    set wand [$img clone imgX]
    set fill [magick create pixel]
    set border [magick create pixel]
    debug $wand $fill $border
    
    $fill color 50%,0,0
    $border color 0,0,50%
    $wand ColorFloodfillImage $fill 1.0 $border 100 100
    
    $wand WriteImage "$::TMP/x-ColorFloodfill.jpg"
    magick delete $fill $border $wand
}
proc ColorizeImage {img} {
    set wand [$img clone imgX]
    set fill [magick create pixel]
    set opacity [magick create pixel]
    debug $wand $fill $opacity
    
    $fill color 70%,0,0
    $opacity SetColor 0,0,0,0
    $wand ColorizeImage $fill $opacity
    
    $wand WriteImage "$::TMP/x-Colorize.jpg"
    magick delete $fill $opacity $wand
}
proc CommentImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand CommentImage "This is a comment"    
    $wand WriteImage "$::TMP/x-Comment.jpg"
    magick delete $wand
}
proc CompareImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand CompareImage $img
    $wand BlurImage 1.0 1.0
    $wand CompareImage $img
    magick delete $wand
}
proc CompositeImage {img} {
    set wand [$img clone imgX]
    set bg [magick create pixel]
    debug $wand $bg 
    
    $bg SetColor 50%,50%,50%
    $wand rotate $bg 90
    $wand CompositeImage add $img 0 0
    $wand WriteImage "$::TMP/x-Composite.jpg"

    magick delete $wand $bg
}
proc ContrastImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ContrastImage 1
    $wand WriteImage "$::TMP/x-Contrast.jpg"
    magick delete $wand
}
proc ConvolveImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ConvolveImage 3 {0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1}
    $wand WriteImage "$::TMP/x-Convolve.jpg"
    magick delete $wand
}
proc CropImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand CropImage 100 60 350 200
    $wand WriteImage "$::TMP/x-Crop.jpg"
    magick delete $wand
}
proc CycleColormapImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand CycleColormapImage 10
    $wand WriteImage "$::TMP/x-CycleColormap.jpg"
    magick delete $wand
}
proc DescribeImage {img} {
    set txt [$img DescribeImage]
    set f [open "$::TMP/x-Describe.txt" w]
    puts $f $txt
    close $f
}
proc DespeckleImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand Despeckle
    $wand WriteImage "$::TMP/x-Despeckle.jpg"
    magick delete $wand
}
proc DrawImage {img} {
    set wand [$img clone imgX]
    set draw [magick create draw]
    debug $wand $draw
   
    $draw push graph
	$draw Rotate -45    
	$draw push graph
	    $draw SetStrokeWidth 2
	    $draw SetFillColorString "white"
	    $draw SetStrokeColorString "blue"
	    $draw Rectangle -200 280 +90 310
	$draw pop graph

	$draw push graph
	    $draw SetStrokeWidth 0.5
	    $draw SetStrokeColorString "blue"
	    $draw SetFillColorString "blue"
	    $draw SetFontSize 18
	    $draw Annotation -197 300 "Tcl/Tk + ImageMagick = TclMagick"
	$draw pop graph
    $draw pop graph

    $draw push graph
        $draw SetStrokeWidth 0.5
        $draw SetStrokeColorString "blue"
        $draw SetFillColorString "blue"
        $draw SetFontSize 14
        $draw Annotation 10 400 "Image %wx%h (%xx%y dpi) format=%m size=%b"
    $draw pop graph
    
    $wand DrawImage $draw
    $wand WriteImage "$::TMP/x-Draw.jpg"

    magick delete $draw $wand
}
proc EdgeImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand EdgeImage 4.0
    $wand WriteImage "$::TMP/x-Edge.jpg"
    magick delete $wand
}
proc EmbossImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand EmbossImage 2.0 5.0
    $wand WriteImage "$::TMP/x-Emboss.jpg"
    magick delete $wand
}
proc EnhanceImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand AddNoiseImage impulse
    $wand EnhanceImage
    $wand WriteImage "$::TMP/x-Enhance.jpg"
    magick delete $wand
}
proc EqualizeImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand EqualizeImage
    $wand WriteImage "$::TMP/x-Equalize.jpg"
    magick delete $wand
}
proc FlattenImage {seq} {
    set wand [$seq FlattenImage]
    debug $wand
    $wand WriteImage "$::TMP/x-Flatten.jpg"
    magick delete $wand
}
proc FlipImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand FlipImage
    $wand WriteImage "$::TMP/x-Flip.jpg"
    magick delete $wand
}
proc FlopImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand FlopImage
    $wand WriteImage "$::TMP/x-Flop.jpg"
    magick delete $wand
}
proc FrameImage {img} {
    set wand [$img clone imgX]
    set frm [magick create pixel]
    debug $wand $frm
    
    $frm set red 1.0 green 1.0 blue 0
    $frm GetColor 
    $wand FrameImage $frm 8 8 3 3
    
    $wand WriteImage "$::TMP/x-Frame.jpg"
    magick delete $frm $wand
}
proc Filename {img} {
    set wand [$img clone imgX]
    debug $wand
    
    $img GetFilename
    $wand filename "foo.jpg"
    $wand filename
    $wand SetFilename "bar.jpg"
    $wand GetFilename

    magick delete $wand
}
proc GammaImage {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel]
    debug $wand $pix
    
    $pix set red 0.5 green 0.5 blue 0.5
    $pix GetColor 
    $wand GammaImage $pix
    
    $wand WriteImage "$::TMP/x-Gamma.jpg"
    magick delete $pix $wand
}
proc GetImageSignature {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand GetImageSignature
    magick delete $wand
}
proc ImplodeImage {img} {
    set wand [$img clone imgX]
    debug $wand  
    $wand ImplodeImage 1.0
    $wand WriteImage "$::TMP/x-Implode.jpg"
    magick delete $wand
}
proc LabelImage {img} {
    set wand [$img clone imgX]
    debug $wand  
    $wand LabelImage "This is my label"
    $wand WriteImage "$::TMP/x-Label.jpg"
    magick delete $wand
}
proc LevelImage {img} {
    set wand [$img clone imgX]
    debug $wand  
    $wand LevelImage 10000 2.0 50000
    $wand WriteImage "$::TMP/x-Level.jpg"
    magick delete $wand
}
proc MagnifyImage {img} {
    set wand [$img clone imgX]
    debug $wand  
    $wand MagnifyImage
    $wand WriteImage "$::TMP/x-Magnify.jpg"
    magick delete $wand
}
proc MapImage {img} {
    set wand [$img clone imgX]
    set map [magick create wand]
    debug $wand $map
    
    $map ReadImage "xc:gray"
    $wand MapImage $map
    $wand WriteImage "$::TMP/x-Map.jpg"
    magick delete $wand $map
}
proc MatteFloodfillImage {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel]
    debug $wand $pix
    
    $pix set red 0.5 green 0.5 blue 0.5
    $pix GetColor 
    $wand MatteFloodfillImage 30000 10.0 $pix
    $wand WriteImage "$::TMP/x-MatteFlood.jpg"
    magick delete $wand $pix
}
proc MedianFilterImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand MedianFilterImage 3
    $wand WriteImage "$::TMP/x-Median.jpg"
    magick delete $wand 
}
proc MinifyImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand MinifyImage
    $wand WriteImage "$::TMP/x-Minify.jpg"
    magick delete $wand 
}
proc ModulateImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ModulateImage -30.0 +50.0
    $wand WriteImage "$::TMP/x-Modulate.jpg"
    magick delete $wand 
}
proc MotionBlurImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand MotionBlurImage 0 5 45
    $wand WriteImage "$::TMP/x-MotionBlur.jpg"
    magick delete $wand 
}
proc NegateImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand NegateImage no
    $wand WriteImage "$::TMP/x-Negate.jpg"
    magick delete $wand 
}
proc NormalizeImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand NormalizeImage
    $wand WriteImage "$::TMP/x-Normalize.jpg"
    magick delete $wand 
}
proc OilPaintImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand OilPaintImage 3
    $wand WriteImage "$::TMP/x-OilPaint.jpg"
    magick delete $wand 
}
proc OpaqueImage {img} {
    set wand [$img clone imgX]
    set pix1 [magick create pixel]
    set pix2 [magick create pixel]
    debug $wand $pix1 $pix2

    $pix1 color 1,1,1
    $pix2 color 0,1,1
    $wand OpaqueImage $pix1 $pix2 10
    $wand WriteImage "$::TMP/x-Opaque-%0d.gif"
    
    magick delete $wand $pix1 $pix2
}
proc ProfileImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ProfileImage ICC HALLO
    $wand WriteImage "$::TMP/x-Profile.jpg"
    magick delete $wand 
}
proc QuantizeImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand QuantizeImage 10 GRAY 1 no no
    $wand WriteImage "$::TMP/x-Quantize.jpg"
    magick delete $wand 
}
proc RaiseImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand RaiseImage 100 100 100 100 yes
    $wand WriteImage "$::TMP/x-Raise.jpg"
    magick delete $wand 
}
proc ReduceNoiseImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand AddNoise impulse
    $wand ReduceNoiseImage 1
    $wand WriteImage "$::TMP/x-ReduceNoise.jpg"
    magick delete $wand 
}
proc ResampleImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ResampleImage 150 150 triangle 2.0
    $wand WriteImage "$::TMP/x-Resample.jpg"
    magick delete $wand 
}
proc ResizeImage {img} {
    set wand [$img clone imgX]
    debug $wand
    
    $wand ResizeImage 100 200 triangle
    $wand WriteImage "$::TMP/x-Resize.jpg"
    magick delete $wand
}
proc RollImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand TrimImage
    $wand RollImage -60 -30
    $wand WriteImage "$::TMP/x-Roll.jpg"
    magick delete $wand 
}
proc RotateImage {img} {
    set wand [$img clone imgX]
    set bg [magick create pixel]
    debug $wand $bg 
    
    $bg color 0.5,0,5,0.5
    $wand RotateImage $bg -45
    $wand WriteImage "$::TMP/x-Rotate.jpg"

    magick delete $wand $bg
}
proc SampleImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand SampleImage 200 300
    $wand WriteImage "$::TMP/x-Sample.jpg"
    magick delete $wand 
}
proc ScaleImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ScaleImage 300 200
    $wand WriteImage "$::TMP/x-Scale.jpg"
    magick delete $wand 
}
proc SetFuzzyColorDistance {img} {
    set wand [$img clone imgX]
    debug $wand
    
    $wand SetFuzzyColorDistance 10
    $wand WriteImage "$::TMP/x-SetFuzzy.jpg"
    magick delete $wand
}
proc SharpenImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand SharpenImage 4.0 2.0
    $wand WriteImage "$::TMP/x-Sharpen.jpg"
    magick delete $wand 
}
proc ShaveImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ShaveImage 50 30
    $wand WriteImage "$::TMP/x-Shave.jpg"
    magick delete $wand 
}
proc ShearImage {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel]
    debug $wand $pix
    
    $pix color 0.5,0.5,0.5
    $wand ShearImage $pix 30 20
    $wand WriteImage "$::TMP/x-Shear.jpg"
    magick delete $wand $pix
}
proc Size {img} {
    set wand [$img clone imgX]
    debug $wand
    
    $img GetSize
    $wand size 100 200
    $wand size
    $wand SetSize 300 400
    $wand GetSize

    magick delete $wand
}
proc SolarizeImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand SolarizeImage 10
    $wand WriteImage "$::TMP/x-Solarize.jpg"
    magick delete $wand
}
proc SpreadImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand SpreadImage 3
    $wand WriteImage "$::TMP/x-Spread.jpg"
    magick delete $wand
}
proc SteganoImage {img} {
    set wand [$img clone imgX]
    set water [$img clone]
    debug $wand $water

    $water resize 50 50
    set new [$wand SteganoImage $water]
    $new WriteImage "$::TMP/x-Stegano.jpg"
    
    magick delete $wand $water $new
}
proc StereoImage {img} {
    set wand [$img clone imgX]
    set another [$img clone]
    debug $wand $another

    $another RollImage 5 5
    set new [$wand StereoImage $another]
    $new WriteImage "$::TMP/x-Stereo.jpg"

    magick delete $wand $another $new
}
proc StripImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand StripImage
    set txt [$wand DescribeImage]
    $wand WriteImage "$::TMP/x-Strip.jpg"
    magick delete $wand
}
proc SwirlImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand SwirlImage 45
    $wand WriteImage "$::TMP/x-Swirl.jpg"
    magick delete $wand
}
proc TextureImage {img} {
    set wand [$img clone imgX]
    set text [magick create wand]
    debug $wand $text
    
    $text ReadImage "xc:gray"
    set new [$wand TextureImage $text]
    $new WriteImage "$::TMP/x-Texture.jpg"

    magick delete $wand $text $new
}
proc ThresholdImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ThresholdImage 30000
    $wand WriteImage "$::TMP/x-Threshold.jpg"
    magick delete $wand
}
proc TransparentImage {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel]
    debug $wand $pix
    
    $pix color 0,0,1
    $wand TransparentImage $pix 40000 100
    $wand WriteImage "$::TMP/x-Transparent-%0d.gif"

    magick delete $wand $pix
}
proc TrimImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand TrimImage 10
    $wand WriteImage "$::TMP/x-Trim.jpg"
    magick delete $wand
}
proc UnsharpMaskImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand UnsharpMaskImage 0 3 50 10
    $wand WriteImage "$::TMP/x-Unsharp.jpg"
    magick delete $wand
}
proc WaveImage {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand WaveImage 4 10
    $wand WriteImage "$::TMP/x-Wave.jpg"
    magick delete $wand
}
proc WhiteThresholdImage {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel]
    debug $wand $pix
    
    $pix color 1,1,1
    $wand WhiteThresholdImage $pix
    $wand WriteImage "$::TMP/x-WhiteThreshold.jpg"

    magick delete $wand $pix
}

proc Pixels {img} {
    set wand [$img clone imgX]
    debug $wand
    set pixelList {
          0   0   0
          0   0 255
          0 255   0
          0 255 255
        255 255 255
        255   0   0
        255   0 255
        255 255   0
        128 128 128
    }
    set pix1 [binary format c* $pixelList]

    $wand SetImagePixels 0 0 3 3 "RGB" char $pix1
    set pix2 [$wand GetImagePixels 0 0 3 3 "RGB" char]
    if {! [string equal $pix1 $pix2]} {
        error "Get pixels do not match set pixels"
    }
    set data [$wand GetImagePixels 10 5 10 5 "RGB" char]
    puts [format "image pixels: %d bytes, expected: %d bytes" [string length $data] 150]
    $wand SetImagePixels 10 5 10 5 "RGB" char [binary format x150]

    $wand WriteImage "$::TMP/x-Pixels.jpg"
    magick delete $wand
}

##########################################
# Image sequence test
#
proc SequenceTest {seq} {
    set wand [$seq clone imgX]
    debug $wand
    
    set N [$wand GetNumberOfImages] 
    if {$N != 5} {
        error [format "Expected 5 images, got %d" $N]
    }
    $wand WriteImages "$::TMP/y-seq0.gif" 1

    $wand SetImage 3
    $wand WriteImage "$::TMP/y-seq-Extract-%0d.gif"

    $wand SetImage 0
    $wand RemoveImage

    set new [$wand DeconstructImages]
    $new WriteImages "$::TMP/y-seq-Deconstruct.gif"
    magick delete $new
    
    $wand SetImage 1
    $wand ResizeImage 40 20
    $wand SetImage 3
    $wand ResizeImage 40 40
    $wand WriteImages "$::TMP/y-seq1.gif"

    set new [$wand CoalesceImages]
    $new WriteImages "$::TMP/y-seq-Coalesce.gif"
    magick delete $new

    magick delete $wand
}

##########################################
# Main test loop
#

debug magick

set img [magick create wand img0]
set seq [magick create wand img1]
debug $img $seq

$img ReadImage $IMG
$img WriteImage "$::TMP/x-0.jpg"

$seq ReadImage $SEQ
magick names

set ERRORS 0
set SKIPPED 0
foreach {func var flag} $TestFunctions {
    if {$flag} {
        puts [format "%s:" $func $var]
        set err [catch {$func [set $var]} result]
        if {$err} {
            incr ERRORS
            puts stderr $result
        }
    } else {
        incr SKIPPED
#        puts [format "--- skip: %s ---" $func]
    }
}

puts "*********** READY **************"
if {$SKIPPED} {
    puts [format "%d tests skipped" $SKIPPED]
}
if {!$ERRORS} {
    after 3000 exit
}
