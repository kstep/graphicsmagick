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
if {[file exists ../win/debug/tclMagick.dll]} {
    load ../win/debug/tclMagick.dll
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
    AddImage                    img     0
    AddNoiseImage               img     1
    AffineTransformImage        img     1
    ???-how-to-AnnotateImage    img     0
    AppendImage                 img     0
    AverageImage                img     0
    BlackThresholdImage         img     1
    BlurImage                   img     1
    BorderImage                 img     1
    ChannelImage                img     1
    CharcoalImage               img     1
    ChopImage                   img     1
    ClipImage                   img     0
    ClipPathImage               img     0
    Clone-No-Test               img     0
    CoalesceImages              img     0
    ???-how-to-ClipImage        img     0
    ColorFloodfillImage         img     1
    ColorizeImage               img     1
    CommentImage                img     1
    CompareImages               img     1
    CompareImageChannels        img     0
    CompositeImage              img     1
    ContrastImage               img     1
    ConvolveImage               img     1
    CropImage                   img     1
    CycleColormapImage          img     1
    DeconstructImages           img     0
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
    FxImageChannel              img     0
    GammaImage                  img     1
    GammaImageChannel           img     0

    GetSetFilename              img     1
    GetSetBackgroundColor       img     0
    GetSetBluePrimary           img     0
    GetSetBorderColor           img     0
    GetSetChannelDepth          img     0
    GetChannelExtrema           img     0
    GetChannelMean              img     0
    GetSetColormapColor         img     0
    GetColors                   img     0
    GetSetColorspace            img     0
    GetSetCompose               img     0
    GetSetCompression           img     0
    GetSetDelay                 img     0
    GetSetDepth                 img     0
    GetSetDispose               img     0
    GetExtrema                  img     0
    Getformat                   img     0
    GetSetGamma                 img     0
    GetsetGreenPrimary          img     0
    GetHeight                   img     0
    GetSetIndex                 img     0
    GetSetInterlaceScheme       img     0
    GetSetIterations            img     0
    GetSetMatteColor            img     0
    GetSetImageFilename         img     0
    GetSetRemoveProfile         img     0
    GetSetRedPrimary            img     0
    GetSetRendering             img     0
    GetSetResolution            img     0
    GetSetScene                 img     0
    GetImageSize                img     0
    GetSignature                img     1
    GetSetImageType             img     0
    GetSetImageUnits            img     0
    GetSetVirtualPixelMethod    img     0
    GetSetWhitePoint            img     0
    GetWidth                    img     0
    GetSetSamplingFactors       img     0
    GetSetSize                  img     1
    
    ImplodeImage                img     1
    LabelImage                  img     1
    LevelImage                  img     1
    LevelImageChannel           img     0
    MagnifyImage                img     1
    MapImage                    img     1
    MatteFloodfillImage         img     1
    MedianFilterImage           img     1
    MinifyImage                 img     1
    ModulateImage               img     1
    MontageImage                img     0
    MorphImages                 img     0
    MosaicImages                img     0
    MotionBlurImage             img     1
    NegateImage                 img     1
    NormalizeImage              img     1
    OilPaintImage               img     1
    OpaqueImage                 img     1
    PingImage                   img     0
    PreviewImages               img     0
    QuantizeImage               img     1
    QueryFontMetrics            img     0
    RaiseImage                  img     1
    ReadBlobImage               img     0
    ReduceNoiseImage            img     1
    ResampleImage               img     1
    ResizeImage                 img     1
    RollImage                   img     1
    RotateImage                 img     1
    SampleImage                 img     1
    ScaleImage                  img     1

    SetImage                    img     0
    SetOption                   img     0
    SetPassphrase               img     0

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
    ThresholdImageChannel       img     0
    TintImage                   img     0
    TransformImage              img     0
    
    TransparentImage            img     1
    TrimImage                   img     1
    UnsharpMaskImage            img     1
    WaveImage                   img     1
    WhiteThresholdImage         img     1

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
    
    $fill color rgb(50%,0,0)
    $border color rgb(0,0,50%)
    $wand ColorFloodfillImage $fill 1.0 $border 100 100
    
    $wand WriteImage "$::TMP/x-ColorFloodfill.jpg"
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
proc GammaImage {img} {
    set wand [$img clone imgX]
    debug $wand
    
    $wand GammaImage 2.0
    
    $wand WriteImage "$::TMP/x-Gamma.jpg"
    magick delete $wand
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
proc GetSetRemoveSetProfile {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand SetProfile ICC HALLO
    $wand WriteImage "$::TMP/x-Profile.jpg"
    magick delete $wand 
}
proc GetSignature {img} {
    set wand [$img clone imgX]
    debug $wand
    $wand GetSignature
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

    $pix1 color rgb(100%,100%,100%)
    $pix2 color rgb(0,100%,100%)
    $wand OpaqueImage $pix1 $pix2 10
    $wand WriteImage "$::TMP/x-Opaque-%0d.gif"
    
    magick delete $wand $pix1 $pix2
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
    
    $pix color "blue"
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
    
    $pix color "white"
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

##########################################
# Image sequence test
#
proc SequenceTest {seq} {
    set wand [$seq clone imgX]
    debug $wand
    
    set N [$wand GetNumberImages] 
    if {$N != 5} {
        error [format "Expected 5 images, got %d" $N]
    }
    $wand WriteImages "$::TMP/y-seq0.gif" 1

    $wand SetIndex 3
    $wand WriteImage "$::TMP/y-seq-Extract-%0d.gif"

    $wand SetIndex 0
    $wand RemoveImage

    set new [$wand DeconstructImages]
    $new WriteImages "$::TMP/y-seq-Deconstruct.gif"
    magick delete $new
    
    $wand SetIndex 1
    $wand ResizeImage 40 20
    $wand SetIndex 3
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
