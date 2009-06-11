# Common code fragment for utilities tests
#
SRCDIR=`dirname $0`
SRCDIR=`cd $SRCDIR && pwd`
TOPSRCDIR=`cd $srcdir && pwd`
mkdir -p utilities/tests
cd utilities/tests || exit 1
GENERIC_TTF="${TOPSRCDIR}/PerlMagick/demo/Generic.ttf"
MODEL_MIFF="${TOPSRCDIR}/Magick++/demo/model.miff"
SMILE_MIFF="${TOPSRCDIR}/Magick++/demo/smile.miff"
SUNRISE_JPEG="${TOPSRCDIR}/utilities/tests/sunrise.jpg"
#CONVERT_FLAGS='-monitor'
#COMPOSITE_FLAGS='-monitor'
#MONTAGE_FLAGS='-monitor'
