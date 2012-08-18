# Common code fragment for utilities tests
#
SRCDIR=`dirname $0`
SRCDIR=`cd $SRCDIR && pwd`
TOPSRCDIR=`cd $srcdir && pwd`
. $TOPSRCDIR/scripts/tap-functions.shi
mkdir -p utilities/tests
cd utilities/tests || exit 1
BETARGB_PROFILE="${TOPSRCDIR}/utilities/tests/BetaRGB.icc"
GENERIC_TTF="${TOPSRCDIR}/PerlMagick/demo/Generic.ttf"
MODEL_MIFF="${TOPSRCDIR}/Magick++/demo/model.miff"
SMILE_MIFF="${TOPSRCDIR}/Magick++/demo/smile.miff"
SUNRISE_JPEG="${TOPSRCDIR}/utilities/tests/sunrise.jpg"
SUNRISE_MIFF="${TOPSRCDIR}/utilities/tests/sunrise.miff"
#CONVERT_FLAGS='-monitor'
#COMPOSITE_FLAGS='-monitor'
#MONTAGE_FLAGS='-monitor'
