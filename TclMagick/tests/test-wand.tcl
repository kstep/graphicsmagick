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
if { $tcl_platform(platform) == "unix" } {
    set auto_path [linsert $auto_path 0 [file join .. unix]]
    package require TclMagick
} else {
    set dll [file join .. win debug tclMagick.dll]
    if {[file exists $dll]} {
        load $dll
    }
    package require TclMagick
}
puts [info script]


##########################################
# Global options
#
set IMG  "logo:"
set SEQ  "../images/sequence.miff"
set MAP  "../images/map6colors.gif"
set CLIP "../images/clippath.tif"
set TMP  "../tmp"

##########################################
# Check which tests should be performed
#
# ImageMagick only: FxImage, FxImageChannel, PreviewImages, TintImage
set TestFunctions {
    AdaptiveThresholdImage      img     1
    AddImage                    seq     1
    AddNoiseImage               img     1
    AffineTransformImage        img     1
    ???-how-to-AnnotateImage    img     0
    AppendImages                seq     1
    AverageImages               seq     1
    BlackThresholdImage         img     1
    BlurImage                   img     1
    BorderImage                 img     1
    CharcoalImage               img     1
    ChopImage                   img     1
    ClipImage                   img     1
    ClipPathImage               img     1
    CoalesceImages              seq     1
    ColorFloodfillImage         img     1
    ColorizeImage               img     1
    CommentImage                img     1
    CompareImages               img     1
    CompareImageChannels        img     1
    CompositeImage              img     1
    ContrastImage               img     1
    ConvolveImage               img     1
    CropImage                   img     1
    CycleColormapImage          img     1
    DeconstructImages           seq     1
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
    FxImage                     img     0
    FxImageChannel              img     0
    GammaImage                  img     1
    GammaImageChannel           img     1

    GetSetImage                 seq     1
    GetSetFilename              img     1
    GetSetBackgroundColor       img     1
    GetSetBluePrimary           img     1
    GetSetBorderColor           img     1
    GetSetChannelDepth          img     1
    GetChannelExtrema           img     1
    GetChannelMean              img     1
    GetSetColormapColor         img     1
    GetColors                   img     1
    GetSetColorspace            img     1
    GetSetCompose               img     1
    GetSetCompression           img     1
    GetSetDelay                 seq     1
    GetSetDepth                 img     1
    GetSetDispose               img     1
    GetExtrema                  img     1
    GetFormat                   img     1
    GetSetGamma                 img     1
    GetSetGreenPrimary          img     1
    GetHeight                   img     1
    GetSetIndex                 seq     1
    GetSetInterlaceScheme       img     1
    GetSetIterations            img     1
    GetSetMatteColor            img     1
    GetSetImageFilename         img     1
    GetSetPixels                img     1
    GetSetRemoveProfile         img     1
    GetSetRedPrimary            img     1
    GetSetRendering             img     1
    GetSetResolution            img     1
    GetSetScene                 seq     1
    GetImageSize                img     1
    GetSignature                img     1
    GetSetImageType             img     1
    GetSetImageUnits            img     1
    GetSetVirtualPixelMethod    img     1
    GetSetWhitePoint            img     1
    GetWidth                    img     1
    GetNumberImages             seq     1
    GetSetSamplingFactors       img     1
    GetSetSize                  img     1
    ImplodeImage                img     0
    LabelImage                  img     1
    LevelImage                  img     1
    LevelImageChannel           img     1
    MagnifyImage                img     1
    MapImage                    img     1
    MatteFloodfillImage         img     1
    MedianFilterImage           img     1
    MinifyImage                 img     1
    ModulateImage               img     1
    MontageImage                seq     1
    MorphImages                 seq     1
    MosaicImages                seq     1
    MotionBlurImage             img     1
    NegateImage                 img     1
    NormalizeImage              img     1

    NextPrevious                seq     1

    OilPaintImage               img     1
    OpaqueImage                 img     1
    PingImage                   SEQ     1
    PreviewImages               img     0
    QuantizeImage               img     1
    QueryFontMetrics            img     1
    RaiseImage                  img     1
    ReadImageBlob               SEQ     1
    ReduceNoiseImage            img     1
    RemoveImage                 seq     1
    ResampleImage               img     1
    ResizeImage                 img     1
    RollImage                   img     1
    RotateImage                 img     1
    SampleImage                 img     1
    ScaleImage                  img     1
    SeparateImageChannel        img     1

    SetOption                   img     1
    SetPassphrase               img     1

    SharpenImage                img     1
    ShaveImage                  img     1
    ShearImage                  img     1
    SolarizeImage               img     1
    SpreadImage                 img     1
    SteganoImage                img     1
    StereoImage                 img     1
    StripImage                  img     1
    SwirlImage                  img     1
    TextureImage                img     1    
    ThresholdImage              img     1
    ThresholdImageChannel       img     1
    TintImage                   img     0
    TransformImage              img     1
    TransparentImage            img     1
    TrimImage                   img     1
    UnsharpMaskImage            img     1
    WaveImage                   img     1
    WhiteThresholdImage         img     1
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
proc AddImage {seq} {
    set wand [$seq clone seqX]
    debug $wand

    $wand SetIndex 3
    set x [$wand GetImage imgX]
    $wand SetIndex 0
    $wand AddImage $x
    $wand WriteImages "$::TMP/x-Add.gif" 1

    magick delete $wand $x
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
   
    [magick create pixel pix] SetColor "blue"
    $draw push graph
        $draw SetStrokeWidth 1
        $draw SetStrokeColor pix
        $draw SetFillColor pix
        $draw SetFontSize 18
    $draw pop graph
    
    $wand AnnotateImage $draw 20 50 0 "Hello world"
    $wand WriteImage "$::TMP/x-Annotate.jpg"

    magick delete $draw $wand pix
}
proc AppendImages {seq} {
    set wand [$seq AppendImages 0 seqX]
    debug $wand
    $wand WriteImages "$::TMP/x-Append.gif" 1
    magick delete $wand
}
proc AverageImages {seq} {
    set wand [$seq AverageImages seqX]
    debug $wand
    $wand WriteImage "$::TMP/x-Average.jpg"
    magick delete $wand
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
    set wand [magick create wand]
    debug $wand
    
    $wand ReadImage $::CLIP
    $wand ClipImage
    $wand GammaImage 5.0

    $wand WriteImage "$::TMP/x-Clip-Gamma.jpg"
    magick delete $wand
}
proc ClipPathImage {img} {
    set wand [magick create wand]
    debug $wand
    
    $wand ReadImage $::CLIP
    $wand ClipPathImage "#1" 0
    $wand SolarizeImage 3

    $wand WriteImage "$::TMP/x-ClipPath-Solarize.jpg"
    magick delete $wand
}
proc CoalesceImages {img} {
    set new [$img CoalesceImages]
    $new WriteImages "$::TMP/x-Coalesce.gif" 1
    magick delete $new
}
proc ColorFloodfillImage {img} {
   set wand [$img clone imgX]
   set fill [magick create pixel]
   set border [magick create pixel]
   debug $wand $fill $border

   $fill SetColor "lightgreen"
   $border SetColor "black"

   set max [magick library -maxrgb]
   $wand ColorFloodfillImage $fill [expr {0.01 * $max}] {} 0 0
   $wand WriteImage "$::TMP/x-ColorFloodfill-1.jpg"

   $fill SetColor "lightblue"
   $wand ColorFloodfillImage $fill [expr {0.01 * $max}] $border 0 0
   $wand WriteImage "$::TMP/x-ColorFloodfill-2.jpg"

   magick delete $fill $border $wand
}
proc ColorizeImage {img} {
    set wand [$img clone imgX]
    set fill [magick create pixel]
    set opacity [magick create pixel]
    debug $wand $fill $opacity
    
    $fill color rgb(70%,0,0)
    $opacity SetColor rgb(0,0,0)
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
proc CompareImages {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand CompareImages $img peakabsoluteerror
    $wand BlurImage 1.0 1.0
    $wand CompareImages $img meansquarederror
    magick delete $wand
}
proc CompareImageChannels {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand BlurImage 1.0 1.0
    $wand CompareImageChannels $img red peakabsoluteerror
    $wand CompareImageChannels $img blue peakabsoluteerror
    magick delete $wand
}
proc CompositeImage {img} {
    set wand [$img clone imgX]
    set bg [magick create pixel]
    debug $wand $bg 
    
    $bg SetColor rgb(50%,50%,50%)
    $wand rotate $bg 90
    $wand CompositeImage $img add 0 0
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
proc DeconstructImages {seq} {
    set wand [$seq DeconstructImages seqX]
    debug $wand
    $wand WriteImage "$::TMP/x-Deconstruct-%d.jpg"
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
    set draw [magick create draw drawX]
    debug $wand $draw
   
    [magick create pixel whitePix] SetColor "white"
    [magick create pixel bluePix]  SetColor "blue"
    [magick create pixel redPix]   SetColor "red"
    
    $draw push graph
        $draw Rotate -45    
        $draw push graph
            $draw SetStrokeWidth 2
            $draw SetFillColor whitePix
            $draw SetStrokeColor bluePix
            $draw Rectangle -200 280 +90 310
        $draw pop graph

        $draw push graph
            $draw SetStrokeWidth 0.5
            $draw SetStrokeColor bluePix
            $draw SetFillColor bluePix
            $draw SetFontSize 18
            $draw Annotation -197 300 "Tcl/Tk + ImageMagick = TclMagick"
        $draw pop graph
    $draw pop graph

    $draw push graph
        $draw SetStrokeWidth 0.5
        $draw SetStrokeColor redPix
        $draw SetFillColor redPix
        $draw SetFontSize 14
        $draw Annotation 10 400 "Image %wx%h (%xx%y dpi) format=%m size=%b"
    $draw pop graph
    
    $wand DrawImage $draw
    $wand WriteImage "$::TMP/x-Draw.jpg"

    magick delete whitePix bluePix redPix
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
proc FxImage {img} {
    set wand [$img FxImage "r/2"]
    $wand WriteImage "$::TMP/x-Fx.jpg"
    magick delete $wand
}
proc FxImageChannel {img} {
    set wand [$img FxImageChannel blue "b/4"]
    $wand WriteImage "$::TMP/x-FxChannel.jpg"
    magick delete $wand
}
proc GammaImage {img} {
    set wand [$img clone imgX]
    debug $wand
    
    $wand GammaImage 2.0
    
    $wand WriteImage "$::TMP/x-Gamma.jpg"
    magick delete $wand
}
proc GammaImageChannel {img} {
    set wand [$img clone imgX]
    debug $wand
    
    $wand GammaImageChannel blue 2.0
    
    $wand WriteImage "$::TMP/x-GammaChannel.jpg"
    magick delete $wand
}
proc GetSetImage {seq} {
    set wand [$seq clone seqX]
    debug $wand

    $wand SetIndex 2
    set new [$wand GetImage imgX]
    $wand SetIndex 1
    while {[$wand HasNext]} {
        $wand SetImage $new
        $wand NextImage
    }    
    $wand WriteImages "$::TMP/x-SetImage.gif" 1
    
    magick delete $wand $new
}
proc GetSetFilename {img} {
    set wand [$img clone imgX]
    debug $wand
    
    $img GetFilename
    $wand filename "foo.jpg"
    $wand filename
    $wand SetFilename "bar.jpg"
    $wand GetFilename

    magick delete $wand
}
proc GetSetBackgroundColor {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel pixX]
    debug $wand $pix

    $pix set red 0.0 green 0.5 blue 0.5
    $wand SetBackgroundColor $pix
    $pix set red 0 green 0 blue 0
    $wand GetBackgroundColor $pix
    $pix GetColor 

    magick delete $pix $wand
}
proc GetSetBluePrimary {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetBluePrimary
    $wand SetBluePrimary 100 50
    $wand GetBluePrimary

    magick delete $wand
}
proc GetSetBorderColor {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel pixX]
    debug $wand $pix

    $pix set red 0.0 green 0.5 blue 0.5
    $wand SetBorderColor $pix
    $pix set red 0 green 0 blue 0
    $wand GetBorderColor $pix
    $pix GetColor 

    magick delete $pix $wand
}
proc GetSetChannelDepth {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetChannelDepth red
    $wand SetChannelDepth red 16
    $wand GetChannelDepth red

    magick delete $wand
}
proc GetChannelExtrema {img} {
    $img GetChannelExtrema blue
}
proc GetChannelMean {img} {
    $img GetChannelMean blue
}
proc GetSetColormapColor {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel pixX]
    debug $wand $pix

    $pix set red 0 green 0.5 blue 0.7
    $wand SetColormapColor 0 $pix
    $pix GetColor 
    $pix set red 0 green 0 blue 0
    $wand GetColormapColor 0 $pix
    $pix GetColor 

    magick delete $pix $wand
}
proc GetColors {img} {
    $img GetColors
}
proc GetSetColorspace {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetColorspace
    $wand SetColorspace "RGB"
    $wand GetColorspace

    magick delete $wand
}
proc GetSetCompose {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetCompose
    $wand SetCompose "xor"
    $wand GetCompose

    magick delete $wand
}
proc GetSetCompression {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetCompression
    $wand SetCompression "zip"
    $wand GetCompression

    magick delete $wand
}
proc GetSetDelay {seq} {
    set wand [$seq clone seqX]
    debug $wand

    $wand GetDelay
    $wand SetDelay 123
    $wand GetDelay

    magick delete $wand
}
proc GetSetDepth {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetDepth
    $wand SetDepth 16
    $wand GetDepth

    magick delete $wand
}
proc GetSetDispose {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetDispose
    $wand SetDispose "background"
    $wand GetDispose

    magick delete $wand
}
proc GetExtrema {img} {
    $img GetExtrema
}
proc GetFormat {img} {
    $img GetFormat
}
proc GetSetGamma {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetGamma
    $wand SetGamma 2.5
    $wand GetGamma

    magick delete $wand
}
proc GetSetGreenPrimary {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetGreenPrimary
    $wand SetGreenPrimary 100 50
    $wand GetGreenPrimary

    magick delete $wand
}
proc GetHeight {img} {
    $img GetHeight
}
proc GetSetIndex {seq} {
    set wand [$seq clone seqX]
    debug $wand

    $wand GetIndex
    $wand SetIndex 3
    $wand GetIndex

    magick delete $wand
}
proc GetSetInterlaceScheme {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetInterlaceScheme
    $wand SetInterlaceScheme "plane"
    $wand GetInterlaceScheme

    magick delete $wand
}
proc GetSetIterations {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetIterations
    $wand SetIterations 3
    $wand GetIterations

    magick delete $wand
}
proc GetSetMatteColor {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel pixX]
    debug $wand $pix

    $pix set red 0 green 0.5 blue 0.7
    $wand SetMatteColor $pix
    $pix GetColor 
    $pix set red 0 green 0 blue 0
    $wand GetMatteColor $pix
    $pix GetColor 

    magick delete $pix $wand
}
proc GetSetImageFilename {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetImageFilename
    $wand SetImageFilename "foo"
    $wand GetImageFilename

    magick delete $wand
}
proc GetSetPixels {img} {
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

    $wand SetPixels 0 0 3 3 "RGB" char $pix1
    set pix2 [$wand GetPixels 0 0 3 3 "RGB" char]
    if {! [string equal $pix1 $pix2]} {
        error "Get pixels do not match set pixels"
    }
    set data [$wand GetPixels 10 5 10 5 "RGB" char]
    puts [format "image pixels: %d bytes, expected: %d bytes" [string length $data] 150]
    $wand SetPixels 10 5 10 5 "RGB" char [binary format x150]

    $wand WriteImage "$::TMP/x-Pixels.jpg"
    magick delete $wand
}
proc GetSetRemoveProfile {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand SetProfile "icc" "foo"
    $wand SetProfile "8bim" "bar"
    $wand GetProfile "icc"
    $wand GetProfile "8bim"
    $wand RemoveProfile "icc"
    $wand GetProfile "icc"

    magick delete $wand
}
proc GetSetRemoveSetProfile {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand SetProfile ICC HALLO
    $wand WriteImage "$::TMP/x-Profile.jpg"
    magick delete $wand 
}
proc GetSetRedPrimary {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetRedPrimary
    $wand SetRedPrimary 100 50
    $wand GetRedPrimary

    magick delete $wand
}
proc GetSetRendering {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetRendering
    $wand SetRendering "perceptual"
    $wand GetRendering

    magick delete $wand
}
proc GetSetResolution {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetResolution
    $wand SetResolution 360 720
    $wand GetResolution

    magick delete $wand
}
proc GetSetScene {seq} {
    set wand [$seq clone seqX]
    debug $wand

    $wand GetScene
    $wand SetScene 3
    $wand GetScene

    magick delete $wand
}
proc GetImageSize {img} {
    $img GetImageSize
}
proc GetSignature {img} {
    $img GetSignature
}
proc GetSetImageType {seq} {
    set wand [$seq clone seqX]
    debug $wand

    $wand GetImageType
    $wand SetImageType grayscale
    $wand GetImageType

    magick delete $wand
}
proc GetSetImageUnits {seq} {
    set wand [$seq clone seqX]
    debug $wand

    $wand GetImageUnits
    $wand SetImageUnits "ppcm"
    $wand GetImageUnits

    magick delete $wand
}
proc GetSetVirtualPixelMethod {seq} {
    set wand [$seq clone seqX]
    debug $wand

    $wand GetVirtualPixelMethod
    $wand SetVirtualPixelMethod "mirror"
    $wand GetVirtualPixelMethod
    $wand SetVirtualPixelMethod "undefined"

    magick delete $wand
}
proc GetWidth {img} {
    $img GetWidth
}
proc GetNumberImages {seq} {
    $seq GetNumberImages
}
proc GetSetSamplingFactors {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetSamplingFactors
    $wand SetSamplingFactors {1 2 3 4 5}
    $wand GetSamplingFactors

    magick delete $wand
}
proc GetSetSize {img} {
    set wand [$img clone imgX]
    debug $wand
    
    $img GetSize
    $wand size 100 200
    $wand size
    $wand SetSize 300 400
    $wand GetSize

    magick delete $wand
}
proc GetSetWhitePoint {img} {
    set wand [$img clone imgX]
    debug $wand

    $wand GetWhitePoint
    $wand SetWhitePoint 100 50
    $wand GetWhitePoint

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
proc LevelImageChannel {img} {
    set wand [$img clone imgX]
    debug $wand  
    $wand LevelImageChannel blue 10000 2.0 50000
    $wand WriteImage "$::TMP/x-LevelChannel.jpg"
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
    
    $map ReadImage $::MAP
    $wand MapImage $map
    $wand WriteImage "$::TMP/x-Map.jpg"
    magick delete $wand $map
}
proc MatteFloodfillImage {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel]
    debug $wand $pix
    
    $pix color rgb(50%,50%,50%)
#    $pix set red 0.5 green 0.5 blue 0.5
    $pix GetColor 
    $wand MatteFloodfillImage 30000 10.0 $pix
    $wand WriteImage "$::TMP/x-MatteFlood.gif"
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
    $wand ModulateImage +90 +50 -30
    $wand WriteImage "$::TMP/x-Modulate.jpg"
    magick delete $wand 
}
proc MontageImage {seq} {
    set wand [$seq clone seqX]
    set draw [magick create drawing drawX]
    debug $wand $draw
   
    set x [$wand MontageImage $draw "40x40" "400x400" frame "4x4" imgX]
    $x WriteImage "$::TMP/x-Montage.jpg"

    magick delete $wand $draw $x
}
proc MorphImages {seq} {
    $seq GetNumberImages
    set wand [$seq MorphImages 5 seqY]
    debug $wand
    $wand GetNumberImages
    $wand WriteImages "$::TMP/x-Morph.gif" 1
    magick delete $wand
}
proc MosaicImages {seq} {
    set wand [$seq MosaicImages imgX]
    debug $wand
    $wand WriteImage "$::TMP/x-Mosaic.gif"
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
proc NextPrevious {seq} {
    set wand [$seq clone seqX]
    debug $wand

    set max [$wand GetNumberImages]
    for {set i 0} {$i < $max} {incr i} {
        $wand SetIndex $i
        $wand HasPrevious
        $wand HasNext
    }    
    $wand ResetIterator
    $wand GetIndex 
    
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
    set pix1 [magick create pixel pix1]
    set pix2 [magick create pixel pix2]
    debug $wand $pix1 $pix2

    $pix1 color rgb(100%,100%,100%)
    $pix2 color rgb(0,100%,100%)
    $wand OpaqueImage $pix1 $pix2 10
    $wand WriteImage "$::TMP/x-Opaque.jpg"
    
    magick delete $wand $pix1 $pix2
}
proc PingImage {filename} {
    set wand [magick create wand imgX]
    debug $wand
    
    $wand PingImage $filename
    $wand GetFormat
    $wand GetHeight
    $wand GetWidth
    $wand GetImageSize
    
    magick delete $wand
}
proc PreviewImages {img} {
    set wand [$img PreviewImages  grayscale imgX]
    debug $wand
    $wand GetNumberImages
    $wand WriteImage "$::TMP/x-Preview.jpg"
    magick delete $wand
}
proc QueryFontMetrics {img} {
    set draw [magick create draw drawX]
    debug $draw
    $img QueryFontMetrics $draw "Hello world"
    magick delete $draw
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
proc ReadImageBlob {filename} {
    set wand [magick create wand imgX]
    debug $wand
    
    set f [open $filename r]
    fconfigure $f -translation binary
    set data [read $f]
    close $f

    $wand ReadImageBlob $data
    $wand GetNumberImages
    
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
proc RemoveImage {seq} {
    set wand [$seq clone seqX]
    debug $wand

    $wand GetNumberImages
    $wand SetIndex 1
    while {[$wand HasNext]} {
        $wand RemoveImage
    }    
    $wand GetNumberImages
    $wand WriteImages "$::TMP/x-Remove.gif" 1
    
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
    
    $bg color rgb(50%,50%,50%)
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
proc SeparateImageChannel {img} {
    # Test RGB space
    foreach chan {red green blue} {
        set wand [$img clone imgX]
        debug $wand    
        $wand SeparateImageChannel $chan
        $wand WriteImage "$::TMP/x-Separate-$chan.jpg"
        magick delete $wand
    }
    # Test CMYK space
    foreach chan {cyan magenta yellow black} {
        set wand [$img clone imgX]
	$wand SetColorspace "CMYK"
        debug $wand    
        $wand SeparateImageChannel $chan
        $wand WriteImage "$::TMP/x-Separate-$chan.jpg"
        magick delete $wand
    }
}
proc SetOption {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand SetOption "jpeg" "perserve" "yes" 
    $wand WriteImage "$::TMP/x-SetOption.jpg"
    magick delete $wand 
}
proc SetPassphrase {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand SetPassphrase "foo" 
    $wand WriteImage "$::TMP/x-Passphrase.jpg"
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
    set pix [magick create pixel pix1]
    debug $wand $pix
    
    $pix color rgb(50%,50%,50%)
    $wand ShearImage $pix 30 20
    $wand WriteImage "$::TMP/x-Shear.jpg"
    magick delete $wand $pix
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
    set water [$img clone imgY]
    debug $wand $water

    $water resize 50 50
    set new [$wand SteganoImage $water]
    $new WriteImage "$::TMP/x-Stegano.jpg"
    
    magick delete $wand $water $new
}
proc StereoImage {img} {
    set wand [$img clone imgX]
    set another [$img clone imgY]
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
    $text ResizeImage 500 500 
    $wand ResizeImage 110 110 triangle
    
    set new [$text TextureImage $wand]
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
proc ThresholdImageChannel {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand ThresholdImageChannel blue 3000
    $wand WriteImage "$::TMP/x-ThresholdChannel.jpg"
    magick delete $wand
}
proc TintImage {img} {
    set wand [$img clone imgX]
    set pix1 [magick create pixel pix1]
    set pix2 [magick create pixel pix2]
    debug $wand $pix1 $pix2
    
    $pix1 color "blue"
    $pix2 color "black"
    $wand TintImage $pix1 $pix2
    $wand WriteImage "$::TMP/x-Tint.jpg"

    magick delete $wand $pix1 $pix2
}
proc TransformImage {img} {
    set wand [$img TransformImage "200x300+250+100" "600x600"]
    debug $wand 
    $wand WriteImage "$::TMP/x-Transform.gif"
    magick delete $wand
}
proc TransparentImage {img} {
    set wand [$img clone imgX]
    set pix [magick create pixel]
    debug $wand $pix
    
    $pix color "blue"
    $wand TransparentImage $pix 40000 100
    $wand WriteImage "$::TMP/x-Transparent.gif"

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
    
    $pix color "white"
    $wand WhiteThresholdImage $pix
    $wand WriteImage "$::TMP/x-WhiteThreshold.jpg"

    magick delete $wand $pix
}

##########################################
# Prepare tests
#

if {! [file isdirectory $::TMP] } {
    file mkdir $::TMP
}
debug magick

magick fonts 
magick formats 

set img [magick create wand img0]
set seq [magick create wand img1]
debug $img $seq

$img ReadImage $IMG
$img WriteImage "$::TMP/x-0.jpg"

$seq ReadImage $SEQ
magick names


##########################################
# Main test loop
#

set ERRORS  0
set TESTED  0
set SKIPPED 0

foreach {func var flag} $TestFunctions {
    if {$flag} {
        incr TESTED
        puts [format "%s:" $func $var]
        set num1 [llength [magick names]]

        set err [catch {$func [set $var]} result]
        if {$err} {
            incr ERRORS
            puts stderr $result
        }
        # Check for unfree'd resources 
        #
        set num2 [llength [magick names]]
        if {! $err && ($num2 > $num1)} {
            puts stderr "Check resources (magick names) !!!"
            set err 1
        }
        if {$err} {
            update ; after 5000
        }
    } else {
        incr SKIPPED
    }
}

puts "*********** READY **************"
if {$SKIPPED} {
    puts [format "tested=%d errors=%d skipped=%d" $TESTED $ERRORS $SKIPPED]
}
if {!$ERRORS} {
#    after 3000 exit
}
