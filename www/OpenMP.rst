.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

========================
OpenMP in GraphicsMagick
========================

GraphicsMagick has been transformed to use `OpenMP
<http://openmp.org/>`_ for the 1.3 release. OpenMP is a portable
framework for accelerating CPU-bound and memory-bound operations using
multiple threads. OpenMP originates in the super-computing world and
has been available in one form or another since the late '90s.

Since GCC 4.2 has introduced excellent OpenMP support, OpenMP has become
available to the masses. Microsoft Visual Studio Professional 2005 and
2008 support OpenMP so Windows users should benefit as well. Any
multi-CPU and/or multi-core system is potentially a good candidate for
use with OpenMP. Unfortunately, some older multi-CPU hardware is more
suitable for multi-processing than multi-threading. Recent multi-core
chipsets from Intel and AMD perform very well with OpenMP. The operating
system makes a difference when it comes to OpenMP acceleration, with
IBM's AIX, Linux, and Sun's Solaris working exceptionally well, and
FreeBSD and Apple's OS-X working less well.

Most image processing routines are comprised of loops which iterate
through the image pixels, image rows, or image regions. These loops are
accelerated using OpenMP by executing portions of the total loops in
different threads, and therefore on a different processor core. CPU-bound
algorithms benefit most from OpenMP, but memory-bound algorithms may also
benefit as well since the memory is accessed by different CPU cores, and
sometimes the CPUs have their own path to memory. For example, the AMD
Opteron is a NUMA (Non-Uniform Memory Architecture) design such that
multi-CPU systems split the system memory across CPUs so each CPU adds
more memory bandwidth as well.

For severely CPU-bound algorithms, it is not uncommon to see a linear
speed-up due to the number of cores. For example, a two core system
executes the algorithm twice as fast, and a four core system executes the
algorithm four times as fast. Memory-bound algorithms scale based on the
memory bandwith available to the cores. For example, memory-bound
algorithms scale up to almost 1.5X on my four core Opteron system due to
its NUMA architecture. Some systems/CPUs are able to immediately context
switch to another thread if the core would be blocked waiting for memory,
allowing multiple memory accesses to be pending at once, and thereby
improving throughput.

An approach used in GraphicsMagick is to recognize the various access
patterns in the existing code, and re-write the algorithms (sometimes
from scratch) to be based on a framework that we call "pixel iterators".
With this approach, the computation is restricted to a small unit (a
callback function) with very well defined properties, and no knowledge as
to how it is executed or where the data comes from. This approach removes
the loops from the code and puts the loops in the framework, which may be
adjusted based on experience. The continuing strategy will be to
recognize design patterns and build frameworks which support those
patterns. Sometimes algorithms are special/exotic enough that it is much
easier to instrument the code for OpenMP rather than to attempt to fit
the algorithm into a framework.

Since OpenMP is based on multi-threading, multiple threads access the
underlying pixel storage at once. The interface to this underlying
storage is called the "pixel cache". The original pixel cache code
(derived from ImageMagick) was thread safe only to the extent that it
allowed one thread per image. This code has now been re-written so that
multiple threads may safely and efficiently work on the pixels in one
image. The re-write also makes the pixel cache thread safe if a
multi-threaded application uses an OpenMP-fortified library.

The following is an example of per-core speed-up due to OpenMP on a
four-core system.  All the pixel quantum values have 30% gaussian
noise added::

  % for threads in 1 2 3 4
  do
    env OMP_NUM_THREADS=$threads gm benchmark -duration 10 convert \
      -size 2048x1080 pattern:granite -operator all Noise-Gaussian 30% null:
  done
  Results: 4 iter 10.93s user 10.93s total 0.366 iter/s (0.366 iter/s cpu)
  Results: 7 iter 21.10s user 10.59s total 0.661 iter/s (0.332 iter/s cpu)
  Results: 10 iter 30.12s user 10.09s total 0.991 iter/s (0.332 iter/s cpu)
  Results: 14 iter 42.29s user 10.72s total 1.306 iter/s (0.331 iter/s cpu)


Note that the "iter/s cpu" value is a measure of the number of iterations
given the amount of reported CPU time consumed. It is an effective
measure of relative efficacy. In the above example, the total speedup was
about 3.6X with only a slight loss of CPU efficiency as threads are added.

According to the OpenMP specification, the OMP_NUM_THREADS evironment
variable (used above) may be used to specify the number of threads
available to the application. Typically this is set to the number of
processor cores on the system but may be set lower to limit resource
consumption or (in some cases) to improve execution efficiency.

The following table shows the performance boost in GraphicsMagick 1.3.1
as threads are added on a four-core AMD Opteron system running Sun
Solaris 10:

.. table:: Performance Boost On Four Core AMD Operon System

   ===================================================================== ===== ===== ===== ===== ======= ====
   Operation                                                               1     2     3     4   iter/s  thds
   ===================================================================== ===== ===== ===== ===== ======= ====
   -noop                                                                  1.00  1.14  1.27  1.35  177.20   4
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.89  2.75  3.52   15.48   4
   -black-threshold 20%                                                   1.00  1.46  1.86  1.98   99.00   4
   -blur 0x0.5                                                            1.00  1.69  2.36  2.60   11.55   4
   -blur 0x1.0                                                            1.00  1.79  2.54  3.01    8.91   4
   -border 6x6                                                            1.00  1.24  1.46  1.49   81.00   4
   -channel red                                                           1.00  1.21  1.47  1.55   99.00   4
   -charcoal 0x1                                                          1.00  1.79  2.51  3.06    3.85   4
   -chop 800x600+200+300                                                  1.00  1.43  1.73  1.85  127.20   4
   -colors 16                                                             1.00  0.86  0.86  0.86    0.75   1
   -colorspace CMYK                                                       1.00  1.54  1.87  2.04   91.00   4
   -colorspace GRAY                                                       1.00  1.72  2.38  2.84   61.68   4
   -colorspace HSL                                                        1.00  1.93  2.82  3.66   18.53   4
   -colorspace HWB                                                        1.00  1.90  2.76  3.54   26.64   4
   -colorspace OHTA                                                       1.00  1.72  2.39  2.85   61.80   4
   -colorspace YCbCr                                                      1.00  1.72  2.36  2.86   62.40   4
   -colorspace YIQ                                                        1.00  1.69  2.32  2.79   61.88   4
   -colorspace YUV                                                        1.00  1.69  2.31  2.81   61.88   4
   -contrast -contrast -contrast                                          1.00  1.99  2.97  3.91    2.94   4
   +contrast +contrast +contrast                                          1.00  1.98  2.96  3.89    3.21   4
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.89  2.76  3.55   20.40   4
   -colorize 30%/20%/50%                                                  1.00  1.79  2.51  3.14   48.40   4
   -crop 1700x900+100+100                                                 1.00  1.24  1.44  1.49   93.20   4
   -cycle 200                                                             1.00  0.88  0.89  0.89    0.66   1
   -despeckle                                                             1.00  1.48  2.81  2.81    1.63   4
   -fill none -stroke gold -draw 'circle 800,500 1100,800'                1.00  1.02  1.02  1.03    8.73   4
   -fill green -stroke gold -draw 'circle 800,500 1100,800'               1.00  1.01  1.02  1.02    7.30   4
   -fill none -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.03  1.05  1.06   17.13   4
   -fill blue -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.02  1.03  1.05   13.57   4
   -fill none -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.04  1.08  1.09   22.86   4
   -fill blue -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.02  1.04  1.04   16.50   4
   -fill none -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.03  1.03  1.04   13.83   4
   -fill blue -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.02  1.03  1.04   12.97   4
   -fill none -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.03  1.06  1.06   27.94   4
   -fill blue -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.04  1.06  1.07   25.60   4
   -edge 0x1                                                              1.00  1.93  2.85  3.72   10.18   4
   -emboss 0x1                                                            1.00  1.79  2.48  3.06    7.58   4
   -enhance                                                               1.00  1.99  2.97  3.93    2.64   4
   -equalize                                                              1.00  1.23  1.36  1.44   30.48   4
   -flip                                                                  1.00  1.15  1.31  1.40   81.40   4
   -flop                                                                  1.00  1.35  1.64  1.77   82.60   4
   -frame 15x15+3+3                                                       1.00  1.14  1.34  1.38   80.00   4
   -gamma 1.6                                                             1.00  1.23  1.37  1.43   27.80   4
   -gaussian 0x0.5                                                        1.00  1.93  2.85  3.72   10.54   4
   -gaussian 0x1.0                                                        1.00  1.96  2.91  3.80    4.07   4
   -implode 0.5                                                           1.00  1.64  2.55  2.95    4.07   4
   -implode -1                                                            1.00  1.55  2.44  2.70    4.63   4
   -lat 10x10-5%                                                          1.00  1.95  2.90  3.79    3.16   4
   -level 10%,1.2,90%                                                     1.00  1.39  1.67  1.84   55.80   4
   -magnify                                                               1.00  1.01  1.02  1.01    4.59   3
   -map netscape:                                                         1.00  0.85  0.85  0.85    0.82   1
   -median 1                                                              1.00  0.89  1.32  1.57    0.59   4
   -median 2                                                              1.00  0.96  1.44  1.74    0.25   4
   -minify                                                                1.00  1.79  2.51  3.11   48.70   4
   -modulate 110/100/95                                                   1.00  1.94  2.85  3.72   12.92   4
   -monochrome                                                            1.00  0.90  0.90  0.90    0.59   1
   -motion-blur 0x3+30                                                    1.00  0.96  1.84  1.38    0.38   3
   -negate                                                                1.00  1.25  1.52  1.60  100.60   4
   +noise Uniform                                                         1.00  1.54  2.25  2.90    6.64   4
   +noise Gaussian                                                        1.00  1.80  2.70  3.56    1.36   4
   +noise Multiplicative                                                  1.00  1.70  2.53  3.31    2.10   4
   +noise Impulse                                                         1.00  1.52  2.20  2.86    6.63   4
   +noise Laplacian                                                       1.00  1.64  2.41  3.16    4.26   4
   +noise Poisson                                                         1.00  1.46  2.17  2.86    1.92   4
   -noise 1                                                               1.00  0.89  1.31  1.56    0.60   4
   -noise 2                                                               1.00  0.97  1.42  1.73    0.25   4
   -normalize                                                             1.00  1.26  1.41  1.49   31.47   4
   -fill blue -fuzz 35% -opaque red                                       1.00  1.66  2.24  2.70   85.00   4
   -operator all Add 2%                                                   1.00  1.72  2.35  2.84   72.25   4
   -operator all And 233                                                  1.00  1.28  1.56  1.64  100.20   4
   -operator all Assign 50%                                               1.00  1.24  1.48  1.53   99.40   4
   -operator all Depth 6                                                  1.00  1.56  2.03  2.33   83.00   4
   -operator all Divide 2                                                 1.00  1.72  2.35  2.88   67.20   4
   -operator all Gamma 0.7                                                1.00  1.41  1.69  1.85   51.60   4
   -operator all Negate 1.0                                               1.00  1.23  1.45  1.60  100.80   4
   -operator all LShift 2                                                 1.00  1.32  1.63  1.72   97.80   4
   -operator all Multiply 0.5                                             1.00  1.69  2.29  2.79   72.60   4
   -operator all Or 233                                                   1.00  1.31  1.58  1.66  100.00   4
   -operator all RShift 2                                                 1.00  1.38  1.66  1.80   98.20   4
   -operator all Subtract 10%                                             1.00  1.72  2.36  2.85   70.26   4
   -operator red Threshold 50%                                            1.00  1.30  1.57  1.65   99.20   4
   -operator gray Threshold 50%                                           1.00  1.34  1.64  1.81   98.80   4
   -operator all Threshold-White 80%                                      1.00  1.39  1.74  1.97  111.00   4
   -operator all Threshold-Black 10%                                      1.00  1.44  1.81  2.02  109.60   4
   -operator all Xor 233                                                  1.00  1.32  1.63  1.60   97.20   3
   -operator all Noise-Gaussian 30%                                       1.00  1.80  2.70  3.53    1.38   4
   -operator all Noise-Impulse 30%                                        1.00  1.53  2.28  2.96    7.03   4
   -operator all Noise-Laplacian 30%                                      1.00  1.65  2.46  3.24    4.43   4
   -operator all Noise-Multiplicative 30%                                 1.00  1.71  2.55  3.36    2.15   4
   -operator all Noise-Poisson 30%                                        1.00  1.46  2.18  2.87    1.96   4
   -operator all Noise-Uniform 30%                                        1.00  1.55  2.28  2.99    7.02   4
   -ordered-dither all 2x2                                                1.00  1.55  2.00  2.25   82.80   4
   -ordered-dither all 3x3                                                1.00  1.61  2.10  2.45   80.84   4
   -ordered-dither intensity 3x3                                          1.00  1.60  2.12  2.43   80.60   4
   -ordered-dither all 4x4                                                1.00  1.52  1.92  2.18   83.40   4
   -paint 0x1                                                             1.00  1.98  2.95  3.88    5.71   4
   -raise 10x10                                                           1.00  1.20  1.44  1.48  144.80   4
   -random-threshold all 20x80                                            1.00  1.23  1.78  2.29   22.27   4
   -density 75x75 -resample 50x50                                         1.00  1.86  2.67  3.47   11.27   4
   -resize 10%                                                            1.00  1.88  2.68  3.41   24.10   4
   -resize 50%                                                            1.00  1.91  2.79  3.56   17.26   4
   -resize 150%                                                           1.00  1.97  2.47  3.17    2.62   4
   -roll +20+10                                                           1.00  1.12  1.30  1.36   75.80   4
   -rotate 0                                                              1.00  1.27  1.33  1.59   88.20   4
   -rotate 45                                                             1.00  1.68  2.38  2.98    1.09   4
   -rotate 90                                                             1.00  1.28  1.55  1.58   41.72   4
   -rotate 180                                                            1.00  1.32  1.44  1.73   88.80   4
   -rotate 270                                                            1.00  1.25  1.53  1.55   41.40   4
   -sample 5%                                                             1.00  1.21  1.36  1.44  167.20   4
   -sample 50%                                                            1.00  1.04  1.12  1.12   73.60   4
   -sample 150%                                                           1.00  1.03  1.04  1.04   17.66   4
   -segment 0.5x0.25                                                      1.00  1.00  1.00  1.00    0.02   1
   -scale 5%                                                              1.00  1.04  1.06  1.06   24.06   4
   -scale 50%                                                             1.00  1.02  1.03  1.03   11.53   4
   -scale 150%                                                            1.00  1.01  1.01  1.01    5.12   4
   -shade 30x30                                                           1.00  1.91  2.76  3.57   14.74   4
   -sharpen 0x0.5                                                         1.00  1.94  2.86  3.71   10.43   4
   -sharpen 0x1.0                                                         1.00  1.96  2.91  3.82    4.07   4
   -shave 10x10                                                           1.00  1.37  1.60  1.65   82.00   4
   -shear 45x45                                                           1.00  1.66  2.28  2.94    1.38   4
   -solarize 50%                                                          1.00  1.37  1.71  1.81   96.20   4
   -spread 1                                                              1.00  1.62  2.16  2.53   68.86   4
   -spread 3                                                              1.00  1.57  2.14  2.50   59.68   4
   -swirl 90                                                              1.00  1.55  2.46  2.72    4.70   4
   -threshold 35%                                                         1.00  1.61  2.12  2.48   78.80   4
   -fuzz 35% -transparent red                                             1.00  1.70  2.32  2.79   73.40   4
   -trim                                                                  1.00  1.87  2.68  3.38   40.32   4
   -fuzz 5% -trim                                                         1.00  1.90  2.75  3.53   28.49   4
   -unsharp 0x0.5+20+1                                                    1.00  1.69  2.36  2.73   10.63   4
   -unsharp 0x1.0+20+1                                                    1.00  1.78  2.54  3.06    8.25   4
   -wave 25x150                                                           1.00  1.27  2.00  1.95    3.93   3
   -white-threshold 80%                                                   1.00  1.38  1.73  1.95  110.60   4
   ===================================================================== ===== ===== ===== ===== ======= ====

The following table shows the performance boostas threads are added on
2 CPU Sun SPARC workstation running Sun Solaris 10.  This system
obtains quite substantial benefit for most key algorithms:

.. table:: Performance Boost On Two CPU SPARC System
   
   ===================================================================== ===== ===== ======= ====
   Operation                                                               1     2   iter/s  thds
   ===================================================================== ===== ===== ======= ====
   -noop                                                                  1.00  1.29   68.00   2
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.91    1.88   2
   -black-threshold 20%                                                   1.00  1.66   16.04   2
   -blur 0x0.5                                                            1.00  1.76    2.08   2
   -blur 0x1.0                                                            1.00  1.78    1.54   2
   -border 6x6                                                            1.00  1.04   28.80   2
   -channel red                                                           1.00  1.62   21.76   2
   -charcoal 0x1                                                          1.00  1.85    0.60   2
   -chop 800x600+200+300                                                  1.00  1.39   31.47   2
   -colors 16                                                             1.00  0.80    0.23   1
   -colorspace CMYK                                                       1.00  1.51   16.37   2
   -colorspace GRAY                                                       1.00  1.92    7.00   2
   -colorspace HSL                                                        1.00  1.93    2.81   2
   -colorspace HWB                                                        1.00  1.91    3.20   2
   -colorspace OHTA                                                       1.00  1.83    7.00   2
   -colorspace YCbCr                                                      1.00  1.83    6.99   2
   -colorspace YIQ                                                        1.00  1.78    7.00   2
   -colorspace YUV                                                        1.00  1.85    7.00   2
   -contrast -contrast -contrast                                          1.00  1.96    0.46   2
   +contrast +contrast +contrast                                          1.00  1.97    0.48   2
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.91    2.49   2
   -colorize 30%/20%/50%                                                  1.00  1.88    6.85   2
   -crop 1700x900+100+100                                                 1.00  1.02   34.40   2
   -cycle 200                                                             1.00  0.84    0.20   1
   -despeckle                                                             1.00  1.56    0.17   2
   -fill none -stroke gold -draw 'circle 800,500 1100,800'                1.00  1.00    1.60   2
   -fill green -stroke gold -draw 'circle 800,500 1100,800'               1.00  1.01    1.30   2
   -fill none -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.02    3.32   2
   -fill blue -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.01    2.41   2
   -fill none -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.03    4.33   2
   -fill blue -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.02    2.92   2
   -fill none -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.01    2.64   2
   -fill blue -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.01    2.42   2
   -fill none -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.03    5.74   2
   -fill blue -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.02    5.10   2
   -edge 0x1                                                              1.00  1.99    1.44   2
   -emboss 0x1                                                            1.00  1.81    1.08   2
   -enhance                                                               1.00  1.96    0.46   2
   -equalize                                                              1.00  1.25    5.91   2
   -flip                                                                  1.00  1.00   30.08   1
   -flop                                                                  1.00  1.52   15.00   2
   -frame 15x15+3+3                                                       1.00  1.02   27.94   2
   -gamma 1.6                                                             1.00  1.28    7.14   2
   -gaussian 0x0.5                                                        1.00  1.99    1.34   2
   -gaussian 0x1.0                                                        1.00  1.98    0.52   2
   -implode 0.5                                                           1.00  1.84    0.90   2
   -implode -1                                                            1.00  1.83    1.13   2
   -lat 10x10-5%                                                          1.00  2.15    0.50   2
   -level 10%,1.2,90%                                                     1.00  1.43   12.20   2
   -magnify                                                               1.00  0.97    1.21   1
   -map netscape:                                                         1.00  0.80    0.25   1
   -median 1                                                              1.00  1.50    0.26   2
   -median 2                                                              1.00  1.59    0.11   2
   -minify                                                                1.00  1.86    9.45   2
   -modulate 110/100/95                                                   1.00  1.95    1.52   2
   -monochrome                                                            1.00  0.86    0.18   1
   -motion-blur 0x3+30                                                    1.00  1.88    0.14   2
   -negate                                                                1.00  1.58   21.03   2
   +noise Uniform                                                         1.00  1.58    1.46   2
   +noise Gaussian                                                        1.00  1.84    0.31   2
   +noise Multiplicative                                                  1.00  1.76    0.47   2
   +noise Impulse                                                         1.00  1.60    1.41   2
   +noise Laplacian                                                       1.00  1.77    0.81   2
   +noise Poisson                                                         1.00  1.53    0.42   2
   -noise 1                                                               1.00  1.52    0.25   2
   -noise 2                                                               1.00  1.60    0.11   2
   -normalize                                                             1.00  1.21    6.63   2
   -fill blue -fuzz 35% -opaque red                                       1.00  1.93    8.86   2
   -operator all Add 2%                                                   1.00  1.89    6.89   2
   -operator all And 233                                                  1.00  1.53   21.27   2
   -operator all Assign 50%                                               1.00  1.36   23.46   2
   -operator all Depth 6                                                  1.00  1.71   18.69   2
   -operator all Divide 2                                                 1.00  1.94    5.43   2
   -operator all Gamma 0.7                                                1.00  1.41   12.82   2
   -operator all Negate 1.0                                               1.00  1.52   21.07   2
   -operator all LShift 2                                                 1.00  1.49   21.27   2
   -operator all Multiply 0.5                                             1.00  1.89    6.86   2
   -operator all Or 233                                                   1.00  1.49   21.23   2
   -operator all RShift 2                                                 1.00  1.50   21.23   2
   -operator all Subtract 10%                                             1.00  1.91    7.17   2
   -operator red Threshold 50%                                            1.00  1.55   22.27   2
   -operator gray Threshold 50%                                           1.00  1.74   16.30   2
   -operator all Threshold-White 80%                                      1.00  1.66   16.57   2
   -operator all Threshold-Black 10%                                      1.00  1.70   16.44   2
   -operator all Xor 233                                                  1.00  1.51   21.27   2
   -operator all Noise-Gaussian 30%                                       1.00  1.81    0.31   2
   -operator all Noise-Impulse 30%                                        1.00  1.62    1.46   2
   -operator all Noise-Laplacian 30%                                      1.00  1.78    0.82   2
   -operator all Noise-Multiplicative 30%                                 1.00  1.76    0.47   2
   -operator all Noise-Poisson 30%                                        1.00  1.53    0.43   2
   -operator all Noise-Uniform 30%                                        1.00  1.61    1.49   2
   -ordered-dither all 2x2                                                1.00  1.61   13.46   2
   -ordered-dither all 3x3                                                1.00  1.66   12.25   2
   -ordered-dither intensity 3x3                                          1.00  1.65   12.35   2
   -ordered-dither all 4x4                                                1.00  1.67   13.49   2
   -paint 0x1                                                             1.00  1.97    0.83   2
   -raise 10x10                                                           1.00  1.19   51.40   2
   -random-threshold all 20x80                                            1.00  1.49    5.23   2
   -density 75x75 -resample 50x50                                         1.00  1.93    1.72   2
   -resize 10%                                                            1.00  1.83    3.52   2
   -resize 50%                                                            1.00  1.87    2.31   2
   -resize 150%                                                           1.00  1.75    0.64   2
   -roll +20+10                                                           1.00  1.00   27.80   1
   -rotate 0                                                              1.00  1.03   29.28   2
   -rotate 45                                                             1.00  1.51    0.34   2
   -rotate 90                                                             1.00  1.38   11.66   2
   -rotate 180                                                            1.00  1.37   13.49   2
   -rotate 270                                                            1.00  1.40   11.86   2
   -sample 5%                                                             1.00  1.22   63.47   2
   -sample 50%                                                            1.00  0.99   24.15   1
   -sample 150%                                                           1.00  1.09    4.71   2
   -segment 0.5x0.25                                                      1.00  1.00    0.00   1
   -scale 5%                                                              1.00  1.01    4.95   2
   -scale 50%                                                             1.00  1.02    3.47   2
   -scale 150%                                                            1.00  0.98    1.19   1
   -shade 30x30                                                           1.00  1.97    2.27   2
   -sharpen 0x0.5                                                         1.00  1.96    1.35   2
   -sharpen 0x1.0                                                         1.00  1.97    0.52   2
   -shave 10x10                                                           1.00  1.03   29.88   2
   -shear 45x45                                                           1.00  1.34    0.64   2
   -solarize 50%                                                          1.00  1.52   20.52   2
   -spread 1                                                              1.00  1.86   12.13   2
   -spread 3                                                              1.00  1.52   10.67   2
   -swirl 90                                                              1.00  1.83    1.12   2
   -threshold 35%                                                         1.00  1.77   12.62   2
   -fuzz 35% -transparent red                                             1.00  1.95    8.60   2
   -trim                                                                  1.00  1.98    4.18   2
   -fuzz 5% -trim                                                         1.00  1.94    3.43   2
   -unsharp 0x0.5+20+1                                                    1.00  1.81    1.59   2
   -unsharp 0x1.0+20+1                                                    1.00  1.89    1.28   2
   -wave 25x150                                                           1.00  1.67    1.08   2
   -white-threshold 80%                                                   1.00  1.69   16.53   2
   ===================================================================== ===== ===== ======= ====
   
The following table shows the boost on a four core IBM P5+ server
system running AIX.  The maximum per-core boost has dropped since we
previously benchmarked on this system, but the overall performance is
usually better (perhaps a sign of improved CPU efficiency). Note that
I don't have exclusive use to this system, and there were other users
logged in, so it is possible that performance was reduced by other
users:

.. table:: Performance Boost On Four Core IBM P5+ System

   ================================== ===== ===== ===== ===== ======= ====
   Operation                            1     2     3     4   iter/s  thds
   ================================== ===== ===== ===== ===== ======= ====
   -noop                               1.00  1.76  1.57  1.89  340.80   4
   -affine 1,0,0.785,1,0,0 -transform  1.00  1.99  2.17  2.93    4.21   4
   -black-threshold 20%                1.00  1.90  1.73  2.17  117.20   4
   -blur 0x0.5                         1.00  1.83  2.05  2.54    3.65   4
   -blur 0x1.0                         1.00  1.76  2.12  2.66    3.23   4
   -border 6x6                         1.00  1.68  1.50  1.77  154.80   4
   -charcoal 0x1                       1.00  1.78  2.13  2.71    1.39   4
   -chop 800x600+200+300               1.00  1.54  1.64  2.03  226.00   4
   -colors 16                          1.00  1.02  1.03  1.03    0.24   3
   -colorspace CMYK                    1.00  1.66  1.47  1.64   66.60   2
   -colorspace GRAY                    1.00  1.88  2.27  2.83   16.44   4
   -colorspace HSL                     1.00  1.98  2.13  2.86    8.40   4
   -colorspace HWB                     1.00  1.98  2.40  3.08    8.63   4
   -colorspace OHTA                    1.00  1.94  2.16  2.83   16.40   4
   -colorspace YCbCr                   1.00  1.89  2.27  2.83   16.44   4
   -colorspace YIQ                     1.00  1.94  2.27  2.84   16.47   4
   -colorspace YUV                     1.00  1.88  2.22  2.85   16.53   4
   -contrast -contrast -contrast       1.00  1.98  2.18  2.94    1.91   4
   +contrast +contrast +contrast       1.00  2.00  2.23  2.95    1.94   4
   -convolve 1,1,1,1,4,1,1,1,1         1.00  1.85  2.16  2.79    5.21   4
   -colorize 30%/20%/50%               1.00  1.97  2.73  3.60   15.08   4
   -crop 1700x900+100+100              1.00  1.74  1.56  1.89  189.60   4
   -despeckle                          1.00  1.48  2.64  2.67    0.29   4
   -edge 0x1                           1.00  2.00  2.16  2.82    5.86   4
   -emboss 0x1                         1.00  1.93  2.08  2.70    2.59   4
   -enhance                            1.00  1.99  2.35  3.12    1.14   4
   -equalize                           1.00  1.20  1.09  1.15   34.20   2
   -flip                               1.00  1.70  1.52  1.80  157.80   4
   -flop                               1.00  1.84  1.63  2.05  141.60   4
   -frame 15x15+3+3                    1.00  1.69  1.50  1.79  152.80   4
   -gamma 1.6                          1.00  1.44  1.20  1.35   71.40   2
   -gaussian 0x0.5                     1.00  1.89  2.20  2.91    2.87   4
   -gaussian 0x1.0                     1.00  1.99  2.17  2.71    1.22   4
   -implode 0.5                        1.00  1.85  2.05  2.71    2.80   4
   -implode -1                         1.00  1.87  2.10  2.72    2.81   4
   -lat 10x10-5%                       1.00  2.00  2.38  3.15    1.07   4
   -level 10%,1.2,90%                  1.00  1.43  1.21  1.33   71.46   2
   -median 1                           1.00  1.85  2.09  2.30    0.68   4
   -median 2                           1.00  1.87  2.17  2.45    0.27   4
   -modulate 110/100/95                1.00  1.99  2.18  2.87    6.20   4
   -motion-blur 0x3+30                 1.00  1.95  1.94  2.33    0.27   4
   -negate                             1.00  1.86  1.64  2.06  174.40   4
   +noise Uniform                      1.00  1.94  2.41  3.11    4.09   4
   +noise Gaussian                     1.00  1.94  2.32  3.01    1.35   4
   +noise Multiplicative               1.00  1.97  2.47  3.19    1.66   4
   +noise Impulse                      1.00  1.90  2.24  2.89    3.93   4
   +noise Laplacian                    1.00  1.98  2.35  3.07    2.84   4
   +noise Poisson                      1.00  1.87  2.40  3.10    1.13   4
   -noise 1                            1.00  1.86  2.06  2.42    0.70   4
   -noise 2                            1.00  1.87  2.20  2.51    0.28   4
   -normalize                          1.00  1.21  1.08  1.16   34.33   2
   -fill blue -fuzz 35% -opaque red    1.00  1.60  2.21  2.92   25.00   4
   -ordered-dither all 3x3             1.00  1.87  1.36  1.71   25.40   2
   -paint 0x1                          1.00  2.00  1.79  2.40    2.80   4
   -raise 10x10                        1.00  1.88  1.81  2.08  175.40   4
   -density 75x75 -resample 50x50      1.00  1.98  2.15  2.82    4.52   4
   -resize 10%                         1.00  1.92  1.94  2.48   12.28   4
   -resize 50%                         1.00  1.96  2.18  2.73    6.00   4
   -resize 150%                        1.00  1.98  1.93  2.44    1.46   4
   -roll +20+10                        1.00  1.73  1.55  1.82  138.40   4
   -rotate 0                           1.00  1.88  1.90  2.36  140.60   4
   -rotate 45                          1.00  0.56  1.43  1.56    0.41   4
   -rotate 90                          1.00  1.08  1.06  1.05   31.01   2
   -rotate 180                         1.00  1.33  1.66  2.05  146.20   4
   -rotate 270                         1.00  1.08  1.05  1.03   30.62   2
   -shade 30x30                        1.00  1.63  2.33  3.02    7.47   4
   -sharpen 0x0.5                      1.00  2.00  2.22  2.89    2.84   4
   -sharpen 0x1.0                      1.00  1.70  2.10  2.76    1.24   4
   -shave 10x10                        1.00  1.71  1.53  1.83  163.40   4
   -shear 45x45                        1.00  1.29  1.62  1.91    0.98   4
   -solarize 50%                       1.00  1.91  1.81  2.32  127.80   4
   -spread 1                           1.00  1.84  1.79  2.21   57.80   4
   -spread 3                           1.00  1.91  1.74  2.20   54.29   4
   -swirl 90                           1.00  1.97  2.02  2.66    3.02   4
   -threshold 35%                      1.00  1.80  1.67  1.80   49.00   4
   -fuzz 35% -transparent red          1.00  1.98  2.26  2.92   24.50   4
   -trim                               1.00  1.98  2.04  2.66   29.62   4
   -unsharp 0x0.5                      1.00  1.88  2.17  2.72    2.80   4
   -unsharp 0x1.0                      1.00  1.90  2.21  2.79    2.46   4
   -wave 25x150                        1.00  1.87  1.94  2.37    2.67   4
   -white-threshold 80%                1.00  1.90  1.59  2.02  113.00   4
   ================================== ===== ===== ===== ===== ======= ====

The following table shows the boost on a two core AMD64 Linux system.
Clearly OpenMP works well for this system.  Note that I don't have
exclusive use to this system, and there were other users logged in, so
it is possible that performance was reduced by other users:

.. table:: Performance Boost On Two Core AMD64 System

   ================================== ===== ===== ======= ====
   Operation                            1     2   iter/s  thds
   ================================== ===== ===== ======= ====
   -noop                               1.00  1.52  146.00   2
   -affine 1,0,0.785,1,0,0 -transform  1.00  1.41    7.75   2
   -black-threshold 20%                1.00  1.52   71.71   2
   -blur 0x0.5                         1.00  1.43    4.72   2
   -blur 0x1.0                         1.00  1.54    3.63   2
   -border 6x6                         1.00  1.20   56.89   2
   -charcoal 0x1                       1.00  1.61    1.74   2
   -chop 800x600+200+300               1.00  1.26  101.20   2
   -colors 16                          1.00  1.01    0.86   2
   -colorspace CMYK                    1.00  1.38   63.60   2
   -colorspace GRAY                    1.00  1.67   30.60   2
   -colorspace HSL                     1.00  1.93   12.38   2
   -colorspace HWB                     1.00  1.57   13.07   2
   -colorspace OHTA                    1.00  1.75   31.47   2
   -colorspace YCbCr                   1.00  1.78   32.60   2
   -colorspace YIQ                     1.00  1.77   32.47   2
   -colorspace YUV                     1.00  1.80   32.01   2
   -contrast -contrast -contrast       1.00  1.95    2.38   2
   +contrast +contrast +contrast       1.00  1.93    2.40   2
   -convolve 1,1,1,1,4,1,1,1,1         1.00  2.04    9.13   2
   -colorize 30%/20%/50%               1.00  1.68   15.28   2
   -crop 1700x900+100+100              1.00  1.33   71.00   2
   -despeckle                          1.00  1.50    0.92   2
   -edge 0x1                           1.00  2.06    8.71   2
   -emboss 0x1                         1.00  1.92    4.16   2
   -enhance                            1.00  1.95    1.35   2
   -equalize                           1.00  1.27   23.55   2
   -flip                               1.00  1.47   22.40   2
   -flop                               1.00  1.49   22.11   2
   -frame 15x15+3+3                    1.00  1.27   57.20   2
   -gamma 1.6                          1.00  1.05   19.48   2
   -gaussian 0x0.5                     1.00  1.74    4.40   2
   -gaussian 0x1.0                     1.00  1.99    1.98   2
   -implode 0.5                        1.00  1.75    2.88   2
   -implode -1                         1.00  1.75    3.58   2
   -lat 10x10-5%                       1.00  1.85    1.32   2
   -level 10%,1.2,90%                  1.00  1.42   40.80   2
   -median 1                           1.00  1.84    0.72   2
   -median 2                           1.00  1.84    0.26   2
   -modulate 110/100/95                1.00  1.84    8.10   2
   -motion-blur 0x3+30                 1.00  1.19    0.36   2
   -negate                             1.00  1.12   53.20   2
   +noise Uniform                      1.00  1.59    5.64   2
   +noise Gaussian                     1.00  1.93    0.91   2
   +noise Multiplicative               1.00  1.82    1.29   2
   +noise Impulse                      1.00  1.63    5.83   2
   +noise Laplacian                    1.00  1.70    2.14   2
   +noise Poisson                      1.00  1.89    1.66   2
   -noise 1                            1.00  1.70    0.66   2
   -noise 2                            1.00  1.79    0.25   2
   -normalize                          1.00  1.20   24.25   2
   -fill blue -fuzz 35% -opaque red    1.00  1.26   35.73   2
   -ordered-dither all 3x3             1.00  1.02   32.20   2
   -paint 0x1                          1.00  1.49    4.26   2
   -raise 10x10                        1.00  1.21   98.20   2
   -density 75x75 -resample 50x50      1.00  1.82    3.78   2
   -resize 10%                         1.00  1.61    9.05   2
   -resize 50%                         1.00  1.72    5.30   2
   -resize 150%                        1.00  1.91    1.43   2
   -roll +20+10                        1.00  1.42   21.07   2
   -rotate 0                           1.00  1.08   17.00   2
   -rotate 45                          1.00  1.71    0.80   2
   -rotate 90                          1.00  1.00   11.53   1
   -rotate 180                         1.00  1.06   15.94   2
   -rotate 270                         1.00  1.01   11.90   2
   -shade 30x30                        1.00  1.62    9.09   2
   -sharpen 0x0.5                      1.00  1.57    3.91   2
   -sharpen 0x1.0                      1.00  1.75    1.74   2
   -shave 10x10                        1.00  1.12   58.60   2
   -shear 45x45                        1.00  1.45    0.77   2
   -solarize 50%                       1.00  1.29   58.48   2
   -spread 1                           1.00  1.57   19.72   2
   -spread 3                           1.00  1.56   17.66   2
   -swirl 90                           1.00  1.55    3.34   2
   -threshold 35%                      1.00  1.61   53.49   2
   -fuzz 35% -transparent red          1.00  1.71   49.60   2
   -trim                               1.00  1.79   33.53   2
   -unsharp 0x0.5                      1.00  1.42    3.93   2
   -unsharp 0x1.0                      1.00  1.59    3.17   2
   -wave 25x150                        1.00  1.20    3.06   2
   -white-threshold 80%                1.00  1.37   70.20   2
   ================================== ===== ===== ======= ====

The following table shows the boost on a two core Apple PowerPC G5
system running OS-X Leopard. It can be seen that this system only
benefits from OpenMP for CPU-intensive operations and that normally
"fast" operations often become somewhat slower (but are still fast) as
threads are added.  Thread locking and context switching seems to be
unusually slow on PowerPC OS-X:

.. table:: Performance Boost On Two Core PowerPC G5 System

   ===================================================================== ===== ===== ======= ====
   Operation                                                               1     2   iter/s  thds
   ===================================================================== ===== ===== ======= ====
   -noop                                                                  1.00  0.99   25.00   1
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.54    3.37   2
   -black-threshold 20%                                                   1.00  1.07   19.36   2
   -blur 0x0.5                                                            1.00  1.45    2.50   2
   -blur 0x1.0                                                            1.00  1.57    1.97   2
   -border 6x6                                                            1.00  1.03   12.05   2
   -channel red                                                           1.00  1.01   19.01   2
   -charcoal 0x1                                                          1.00  1.64    0.88   2
   -chop 800x600+200+300                                                  1.00  1.00   18.73   2
   -colors 16                                                             1.00  0.77    0.56   1
   -colorspace CMYK                                                       1.00  1.11   16.14   2
   -colorspace GRAY                                                       1.00  1.47    9.72   2
   -colorspace HSL                                                        1.00  1.73    5.30   2
   -colorspace HWB                                                        1.00  1.75    5.81   2
   -colorspace OHTA                                                       1.00  1.49    9.84   2
   -colorspace YCbCr                                                      1.00  1.47    9.65   2
   -colorspace YIQ                                                        1.00  1.47    9.78   2
   -colorspace YUV                                                        1.00  1.47    9.74   2
   -contrast -contrast -contrast                                          1.00  1.92    1.31   2
   +contrast +contrast +contrast                                          1.00  1.94    1.34   2
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.71    3.71   2
   -colorize 30%/20%/50%                                                  1.00  1.44    8.35   2
   -crop 1700x900+100+100                                                 1.00  1.02   14.09   2
   -cycle 200                                                             1.00  0.80    0.49   1
   -despeckle                                                             1.00  1.48    0.24   2
   -fill none -stroke gold -draw 'circle 800,500 1100,800'                1.00  1.01    5.92   2
   -fill green -stroke gold -draw 'circle 800,500 1100,800'               1.00  1.00    4.58   1
   -fill none -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  0.99    9.25   1
   -fill blue -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.01    6.92   2
   -fill none -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.00   11.07   1
   -fill blue -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.00    7.91   2
   -fill none -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.01    8.22   2
   -fill blue -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.00    7.50   1
   -fill none -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.01   13.37   2
   -fill blue -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.00   12.20   1
   -edge 0x1                                                              1.00  1.73    3.76   2
   -emboss 0x1                                                            1.00  1.75    1.74   2
   -enhance                                                               1.00  1.96    0.62   2
   -equalize                                                              1.00  1.01   11.48   2
   -flip                                                                  1.00  0.99   11.90   1
   -flop                                                                  1.00  0.99   11.68   1
   -frame 15x15+3+3                                                       1.00  1.03   11.80   2
   -gamma 1.6                                                             1.00  1.02   11.66   2
   -gaussian 0x0.5                                                        1.00  1.83    2.05   2
   -gaussian 0x1.0                                                        1.00  1.96    0.80   2
   -implode 0.5                                                           1.00  1.75    1.89   2
   -implode -1                                                            1.00  1.65    2.33   2
   -lat 10x10-5%                                                          1.00  1.93    0.44   2
   -level 10%,1.2,90%                                                     1.00  1.01   15.68   2
   -magnify                                                               1.00  1.01    1.46   2
   -map netscape:                                                         1.00  0.75    0.61   1
   -median 1                                                              1.00  1.35    0.47   2
   -median 2                                                              1.00  1.22    0.16   2
   -minify                                                                1.00  1.65    6.97   2
   -modulate 110/100/95                                                   1.00  1.83    3.79   2
   -monochrome                                                            1.00  0.75    0.46   1
   -motion-blur 0x3+30                                                    1.00  1.01    0.20   2
   -negate                                                                1.00  0.98   19.16   1
   +noise Uniform                                                         1.00  1.72    2.62   2
   +noise Gaussian                                                        1.00  1.86    0.87   2
   +noise Multiplicative                                                  1.00  1.88    1.18   2
   +noise Impulse                                                         1.00  1.72    2.53   2
   +noise Laplacian                                                       1.00  1.82    1.71   2
   +noise Poisson                                                         1.00  1.91    0.95   2
   -noise 1                                                               1.00  1.02    0.35   2
   -noise 2                                                               1.00  1.43    0.19   2
   -normalize                                                             1.00  1.01   11.98   2
   -fill blue -fuzz 35% -opaque red                                       1.00  1.36   14.46   2
   -operator all Add 2%                                                   1.00  1.51   10.06   2
   -operator all And 233                                                  1.00  0.99   19.20   1
   -operator all Assign 50%                                               1.00  1.11   18.16   2
   -operator all Depth 6                                                  1.00  1.02   18.36   2
   -operator all Divide 2                                                 1.00  1.56    9.54   2
   -operator all Gamma 0.7                                                1.00  0.99   15.81   1
   -operator all Negate 1.0                                               1.00  0.99   19.12   1
   -operator all LShift 2                                                 1.00  0.98   18.89   1
   -operator all Multiply 0.5                                             1.00  1.52   10.10   2
   -operator all Or 233                                                   1.00  0.98   19.09   1
   -operator all RShift 2                                                 1.00  0.99   19.09   1
   -operator all Subtract 10%                                             1.00  1.56   10.24   2
   -operator red Threshold 50%                                            1.00  1.01   18.89   2
   -operator gray Threshold 50%                                           1.00  1.00   18.45   2
   -operator all Threshold-White 80%                                      1.00  1.09   20.92   2
   -operator all Threshold-Black 10%                                      1.00  1.05   20.04   2
   -operator all Xor 233                                                  1.00  0.98   19.25   1
   -operator all Noise-Gaussian 30%                                       1.00  1.92    0.94   2
   -operator all Noise-Impulse 30%                                        1.00  1.85    3.01   2
   -operator all Noise-Laplacian 30%                                      1.00  1.86    1.87   2
   -operator all Noise-Multiplicative 30%                                 1.00  1.94    1.27   2
   -operator all Noise-Poisson 30%                                        1.00  1.98    1.03   2
   -operator all Noise-Uniform 30%                                        1.00  1.82    3.08   2
   -ordered-dither all 2x2                                                1.00  1.09   15.48   2
   -ordered-dither all 3x3                                                1.00  1.15   15.28   2
   -ordered-dither intensity 3x3                                          1.00  1.13   15.08   2
   -ordered-dither all 4x4                                                1.00  1.08   15.31   2
   -paint 0x1                                                             1.00  1.82    4.05   2
   -raise 10x10                                                           1.00  1.01   22.86   2
   -random-threshold all 20x80                                            1.00  1.46    8.20   2
   -density 75x75 -resample 50x50                                         1.00  1.59    2.12   2
   -resize 10%                                                            1.00  1.71    5.43   2
   -resize 50%                                                            1.00  1.64    3.19   2
   -resize 150%                                                           1.00  1.66    0.55   2
   -roll +20+10                                                           1.00  0.91   11.33   1
   -rotate 0                                                              1.00  0.99   11.98   1
   -rotate 45                                                             1.00  1.69    0.24   2
   -rotate 90                                                             1.00  1.00    8.82   1
   -rotate 180                                                            1.00  1.00   11.83   1
   -rotate 270                                                            1.00  1.00    8.78   2
   -sample 5%                                                             1.00  1.01   24.30   2
   -sample 50%                                                            1.00  1.03   18.00   2
   -sample 150%                                                           1.00  1.01    6.65   2
   -segment 0.5x0.25                                                      1.00  1.00    0.01   1
   -scale 5%                                                              1.00  1.00    7.04   2
   -scale 50%                                                             1.00  1.00    5.25   1
   -scale 150%                                                            1.00  1.01    2.10   2
   -shade 30x30                                                           1.00  1.61    5.39   2
   -sharpen 0x0.5                                                         1.00  1.85    2.08   2
   -sharpen 0x1.0                                                         1.00  1.92    0.79   2
   -shave 10x10                                                           1.00  0.97   12.05   1
   -shear 45x45                                                           1.00  1.64    0.28   2
   -solarize 50%                                                          1.00  1.05   18.69   2
   -spread 1                                                              1.00  1.00   10.60   1
   -spread 3                                                              1.00  1.01    9.80   2
   -swirl 90                                                              1.00  1.66    2.02   2
   -threshold 35%                                                         1.00  1.01   14.48   2
   -fuzz 35% -transparent red                                             1.00  1.36   14.23   2
   -trim                                                                  1.00  1.72    6.67   2
   -fuzz 5% -trim                                                         1.00  1.80    4.09   2
   -unsharp 0x0.5+20+1                                                    1.00  1.56    1.98   2
   -unsharp 0x1.0+20+1                                                    1.00  1.62    1.64   2
   -wave 25x150                                                           1.00  1.36    1.79   2
   -white-threshold 80%                                                   1.00  1.07   20.56   2
   ===================================================================== ===== ===== ======= ====

The following shows the performance boost on a 2003 vintage 2-CPU
hyperthreaded Intel Xeon system running at 2.4GHz.  The operating
system used is FreeBSD 7.0.  Due to the hyperthreading support, this
system thinks it has four CPUs even though it really only has two
cores.  While the threading on this system behaves quite poorly for
"fast" algorithms, it is clear that OpenMP works well for "slow"
algorithms, and some algorithms show clear benefit from hyperthreading
since otherwise the maximum performance gain would be 2.0:

.. table:: Performance Boost On Two CPU Xeon System

   ===================================================================== ===== ===== ===== ===== ======= ====
   Operation                                                               1     2     3     4   iter/s  thds
   ===================================================================== ===== ===== ===== ===== ======= ====
   -noop                                                                  1.00  1.00  1.00  1.00   18.40   2
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.22  1.30  1.30    2.44   4
   -black-threshold 20%                                                   1.00  1.10  1.13  1.14   14.31   4
   -blur 0x0.5                                                            1.00  1.27  1.40  1.44    2.67   4
   -blur 0x1.0                                                            1.00  1.38  1.52  1.58    2.30   4
   -border 6x6                                                            1.00  1.00  1.00  1.00    8.57   4
   -channel red                                                           1.00  1.01  1.00  1.01   13.49   2
   -charcoal 0x1                                                          1.00  1.42  1.68  1.77    0.94   4
   -chop 800x600+200+300                                                  1.00  0.99  1.00  1.00   14.07   3
   -colors 16                                                             1.00  1.00  1.00  1.00    0.24   1
   -colorspace CMYK                                                       1.00  0.87  0.87  0.85   11.22   1
   -colorspace GRAY                                                       1.00  1.37  1.47  1.47    7.84   4
   -colorspace HSL                                                        1.00  1.45  1.89  2.03    4.84   4
   -colorspace HWB                                                        1.00  1.55  1.90  2.08    6.70   4
   -colorspace OHTA                                                       1.00  1.35  1.51  1.57    8.10   4
   -colorspace YCbCr                                                      1.00  1.40  1.52  1.57    8.07   4
   -colorspace YIQ                                                        1.00  1.38  1.52  1.58    8.11   4
   -colorspace YUV                                                        1.00  1.38  1.53  1.57    8.04   4
   -contrast -contrast -contrast                                          1.00  1.84  2.29  2.62    1.16   4
   +contrast +contrast +contrast                                          1.00  1.74  2.27  2.58    1.18   4
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.48  1.86  2.05    3.80   4
   -colorize 30%/20%/50%                                                  1.00  1.32  1.48  1.59    6.65   4
   -crop 1700x900+100+100                                                 1.00  0.99  0.99  1.00   10.00   1
   -cycle 200                                                             1.00  1.00  1.00  1.00    0.22   1
   -despeckle                                                             1.00  1.32  2.11  2.08    0.38   3
   -fill none -stroke gold -draw 'circle 800,500 1100,800'                1.00  1.00  1.00  1.00    3.33   1
   -fill green -stroke gold -draw 'circle 800,500 1100,800'               1.00  1.00  1.00  1.00    2.81   4
   -fill none -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.00  1.00  1.00    5.69   1
   -fill blue -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.00  1.00  1.00    4.47   4
   -fill none -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.00  0.99  1.00    6.87   1
   -fill blue -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.00  1.00  1.00    5.46   1
   -fill none -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.00  1.00  1.00    4.59   2
   -fill blue -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.01  1.01  1.00    4.29   2
   -fill none -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.00  1.00  1.00    8.49   2
   -fill blue -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.00  1.00  1.00    7.94   2
   -edge 0x1                                                              1.00  1.52  1.89  2.11    3.74   4
   -emboss 0x1                                                            1.00  1.52  1.87  1.99    1.87   4
   -enhance                                                               1.00  1.87  2.01  2.08    0.66   4
   -equalize                                                              1.00  1.01  1.02  1.02    6.92   4
   -flip                                                                  1.00  1.00  1.00  1.00    8.71   2
   -flop                                                                  1.00  1.00  0.99  1.00    8.80   4
   -frame 15x15+3+3                                                       1.00  1.00  1.00  1.00    8.46   1
   -gamma 1.6                                                             1.00  1.00  1.01  1.01    5.33   3
   -gaussian 0x0.5                                                        1.00  1.60  2.06  2.25    2.45   4
   -gaussian 0x1.0                                                        1.00  1.65  2.09  2.22    1.08   4
   -implode 0.5                                                           1.00  1.69  2.06  2.39    1.24   4
   -implode -1                                                            1.00  1.59  1.73  2.09    1.83   4
   -lat 10x10-5%                                                          1.00  1.49  1.94  2.06    0.93   4
   -level 10%,1.2,90%                                                     1.00  1.01  1.03  1.02    9.54   3
   -magnify                                                               1.00  1.00  1.00  1.00    2.06   4
   -map netscape:                                                         1.00  1.00  1.00  1.00    0.26   1
   -median 1                                                              1.00  0.67  1.73  1.07    0.29   3
   -median 2                                                              1.00  0.75  1.38  1.11    0.09   3
   -minify                                                                1.00  1.36  1.58  1.67    8.15   4
   -modulate 110/100/95                                                   1.00  1.65  2.08  2.30    3.52   4
   -monochrome                                                            1.00  1.01  1.01  1.01    0.21   4
   -motion-blur 0x3+30                                                    1.00  0.99  0.99  0.98    0.10   1
   -negate                                                                1.00  1.00  0.99  1.00   13.40   1
   +noise Uniform                                                         1.00  1.59  1.90  2.09    2.55   4
   +noise Gaussian                                                        1.00  1.82  2.27  2.63    0.84   4
   +noise Multiplicative                                                  1.00  1.81  2.23  2.57    1.17   4
   +noise Impulse                                                         1.00  1.52  1.74  1.55    2.45   3
   +noise Laplacian                                                       1.00  1.72  2.21  2.53    1.83   4
   +noise Poisson                                                         1.00  1.75  2.15  2.42    0.88   4
   -noise 1                                                               1.00  0.67  1.57  0.99    0.26   3
   -noise 2                                                               1.00  0.76  1.50  1.11    0.09   3
   -normalize                                                             1.00  1.01  1.02  1.02    7.33   4
   -fill blue -fuzz 35% -opaque red                                       1.00  1.20  1.27  1.31   13.60   4
   -operator all Add 2%                                                   1.00  1.20  1.35  1.38   12.36   4
   -operator all And 233                                                  1.00  1.01  1.01  1.01   13.40   2
   -operator all Assign 50%                                               1.00  1.01  1.01  1.01   13.54   4
   -operator all Depth 6                                                  1.00  1.01  1.01  1.00   13.23   3
   -operator all Divide 2                                                 1.00  1.32  1.51  1.52    9.17   4
   -operator all Gamma 0.7                                                1.00  1.00  1.00  1.00    8.77   1
   -operator all Negate 1.0                                               1.00  1.00  1.00  1.00   13.45   2
   -operator all LShift 2                                                 1.00  1.00  1.00  1.00   13.47   2
   -operator all Multiply 0.5                                             1.00  1.21  1.36  1.38   12.32   4
   -operator all Or 233                                                   1.00  1.01  1.00  0.99   13.49   2
   -operator all RShift 2                                                 1.00  1.00  1.00  1.00   13.47   4
   -operator all Subtract 10%                                             1.00  1.22  1.41  1.45   12.25   4
   -operator red Threshold 50%                                            1.00  1.01  1.01  1.00   13.49   3
   -operator gray Threshold 50%                                           1.00  1.04  1.05  1.04   13.54   3
   -operator all Threshold-White 80%                                      1.00  1.07  1.11  1.11   15.23   3
   -operator all Threshold-Black 10%                                      1.00  1.08  1.10  1.12   15.06   4
   -operator all Xor 233                                                  1.00  1.01  1.01  1.00   13.49   2
   -operator all Noise-Gaussian 30%                                       1.00  1.80  2.33  2.73    0.90   4
   -operator all Noise-Impulse 30%                                        1.00  1.59  1.92  2.01    3.21   4
   -operator all Noise-Laplacian 30%                                      1.00  1.79  2.40  2.85    2.20   4
   -operator all Noise-Multiplicative 30%                                 1.00  1.81  2.38  2.77    1.29   4
   -operator all Noise-Poisson 30%                                        1.00  1.72  2.23  2.53    0.95   4
   -operator all Noise-Uniform 30%                                        1.00  1.71  2.11  2.36    3.22   4
   -ordered-dither all 2x2                                                1.00  0.93  0.93  0.88   10.48   1
   -ordered-dither all 3x3                                                1.00  1.02  1.11  1.07    9.94   3
   -ordered-dither intensity 3x3                                          1.00  1.01  1.11  1.07    9.95   3
   -ordered-dither all 4x4                                                1.00  0.93  0.93  0.87   10.50   1
   -paint 0x1                                                             1.00  1.41  1.85  2.02    1.72   4
   -raise 10x10                                                           1.00  0.99  1.00  0.99   17.15   3
   -random-threshold all 20x80                                            1.00  1.37  1.68  1.76    7.48   4
   -density 75x75 -resample 50x50                                         1.00  1.47  1.73  1.82    1.86   4
   -resize 10%                                                            1.00  1.45  1.51  1.65    3.94   4
   -resize 50%                                                            1.00  1.48  1.63  1.71    2.44   4
   -resize 150%                                                           1.00  1.49  1.69  1.69    0.58   3
   -roll +20+10                                                           1.00  0.79  0.67  0.62    8.43   1
   -rotate 0                                                              1.00  1.00  1.01  1.00    8.64   3
   -rotate 45                                                             1.00  1.19  1.29  1.32    0.31   4
   -rotate 90                                                             1.00  1.00  1.00  0.99    6.74   1
   -rotate 180                                                            1.00  1.00  1.00  1.00    8.93   2
   -rotate 270                                                            1.00  1.00  1.00  0.99    6.99   3
   -sample 5%                                                             1.00  1.00  1.00  0.99   18.17   1
   -sample 50%                                                            1.00  1.00  1.00  1.00   13.65   3
   -sample 150%                                                           1.00  1.00  1.00  1.01    5.53   4
   -segment 0.5x0.25                                                      1.00  1.00  1.00  1.00    0.01   1
   -scale 5%                                                              1.00  1.00  1.00  1.00    9.74   1
   -scale 50%                                                             1.00  1.04  1.04  1.03    6.74   2
   -scale 150%                                                            1.00  1.06  0.98  1.04    2.36   2
   -shade 30x30                                                           1.00  1.71  2.21  2.59    3.67   4
   -sharpen 0x0.5                                                         1.00  1.54  2.06  2.24    2.45   4
   -sharpen 0x1.0                                                         1.00  1.66  2.08  2.21    1.07   4
   -shave 10x10                                                           1.00  1.01  1.01  1.01    8.76   2
   -shear 45x45                                                           1.00  1.18  1.23  1.27    0.37   4
   -solarize 50%                                                          1.00  1.01  1.01  1.00   13.49   2
   -spread 1                                                              1.00  1.00  0.99  1.00    7.45   2
   -spread 3                                                              1.00  0.99  1.00  1.00    7.51   3
   -swirl 90                                                              1.00  1.67  1.80  2.19    1.81   4
   -threshold 35%                                                         1.00  1.01  1.01  1.00    9.88   2
   -fuzz 35% -transparent red                                             1.00  1.19  1.29  1.31   13.18   4
   -trim                                                                  1.00  1.38  1.61  1.66    7.59   4
   -fuzz 5% -trim                                                         1.00  1.60  2.00  2.23    5.85   4
   -unsharp 0x0.5+20+1                                                    1.00  1.36  1.56  1.64    2.31   4
   -unsharp 0x1.0+20+1                                                    1.00  1.37  1.64  1.72    2.01   4
   -wave 25x150                                                           1.00  1.25  1.23  1.41    1.50   4
   -white-threshold 80%                                                   1.00  1.08  1.10  1.10   15.23   4
   ===================================================================== ===== ===== ===== ===== ======= ====


--------------------------------------------------------------------------

| Copyright (C) 2008 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

