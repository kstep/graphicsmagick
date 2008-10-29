.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

========================
OpenMP in GraphicsMagick
========================

GraphicsMagick is being transformed to use `OpenMP <http://openmp.org/>`_
for the forthcoming 1.3 release. OpenMP is a portable framework for
accelerating CPU-bound and memory-bound operations using multiple
threads. OpenMP originates in the super-computing world and has been
available in one form or another since the late '90s.

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
algorithms scale up to almost 3X on my four core Opteron system due to
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
four-core system.  All the pixel quantum values are divided by 2::

  % for threads in 1 2 3 4
  do
    env OMP_NUM_THREADS=$threads gm benchmark -duration 10 convert \
      -size 2048x1080 pattern:granite -operator all divide 2.0 null:
  done
  Results: 248 iter 10.00s user 10.00s total 24.800 iter/s (24.800 iter/s cpu)
  Results: 451 iter 19.43s user 10.01s total 45.055 iter/s (23.212 iter/s cpu)
  Results: 595 iter 27.64s user 10.01s total 59.441 iter/s (21.527 iter/s cpu)
  Results: 767 iter 35.92s user 10.01s total 76.623 iter/s (21.353 iter/s cpu)

Note that the "iter/s cpu" value is a measure of the number of iterations
given the amount of reported CPU time consumed. It is an effective
measure of relative efficacy. In the above example, the total speedup was
over 3X with only a small loss of CPU efficiency as threads are added.

According to the OpenMP specification, the OMP_NUM_THREADS evironment
variable (used above) may be used to specify the number of threads
available to the application. Typically this is set to the number of
processor cores on the system but may be set lower to limit resource
consumption or (in some cases) to improve execution efficiency.

The   following   table   shows   the   performance  boost  in  development
GraphicsMagick  1.3  as threads are added on a four-core AMD Opteron system
running  Sun  Solaris  10.  Similar results are obtained on Linux, IBM AIX,
and Apple OS-X systems:

.. table:: Performance Boost On Four Core AMD Operon System

   ================================== ===== ===== ===== ===== =============
   Operation                            1     2     3     4   iter/s : thds
   ================================== ===== ===== ===== ===== =============
   -noop                               1.00  1.40  1.57  1.81  245.00 :  4
   -affine 1,0,0.785,1,0,0 -transform  1.00  1.87  2.66  3.24   20.79 :  4
   -black-threshold 20%                1.00  1.57  1.90  2.27  129.20 :  4
   -blur 0x0.5                         1.00  1.51  1.86  2.12    9.84 :  4
   -blur 0x1.0                         1.00  1.65  2.10  2.53    7.71 :  4
   -border 6x6                         1.00  1.48  1.57  1.76   92.00 :  4
   -charcoal 0x1                       1.00  1.70  2.26  2.74    3.63 :  4
   -chop 800x600+200+300               1.00  1.45  1.59  1.80  156.40 :  4
   -colorspace CMYK                    1.00  1.51  1.75  2.02  103.00 :  4
   -colorspace GRAY                    1.00  1.77  2.36  2.94   67.20 :  4
   -colorspace HSL                     1.00  1.94  2.77  3.59   24.30 :  4
   -colorspace HWB                     1.00  1.94  2.74  3.62   32.21 :  4
   -colorspace OHTA                    1.00  1.76  2.40  2.97   67.40 :  4
   -colorspace YCbCr                   1.00  1.74  2.38  2.96   67.86 :  4
   -colorspace YIQ                     1.00  1.81  2.43  2.90   64.60 :  4
   -colorspace YUV                     1.00  1.82  2.39  2.99   67.40 :  4
   -contrast -contrast -contrast       1.00  1.99  2.87  3.75    4.34 :  4
   +contrast +contrast +contrast       1.00  1.98  2.87  3.73    4.54 :  4
   -convolve 1,1,1,1,4,1,1,1,1         1.00  1.92  2.75  3.47   20.92 :  4
   -colorize 30%/20%/50%               1.00  1.87  2.56  3.32   57.48 :  4
   -crop 1700x900+100+100              1.00  1.38  1.52  1.79  116.00 :  4
   -despeckle                          1.00  1.48  2.83  2.82    1.91 :  3
   -edge 0x1                           1.00  1.96  2.81  3.75   10.98 :  4
   -emboss 0x1                         1.00  1.85  2.50  3.17    8.46 :  4
   -enhance                            1.00  1.99  2.90  3.89    3.10 :  4
   -equalize                           1.00  1.29  1.41  1.41   30.88 :  4
   -flip                               1.00  1.56  1.61  1.93  101.40 :  4
   -flop                               1.00  1.40  1.63  1.87   96.80 :  4
   -frame 15x15+3+3                    1.00  1.47  1.56  1.77   96.00 :  4
   -gamma 1.6                          1.00  1.27  1.38  1.47   30.88 :  4
   -gaussian 0x0.5                     1.00  1.96  2.82  3.67   11.42 :  4
   -gaussian 0x1.0                     1.00  1.98  2.87  3.84    4.32 :  4
   -implode 0.5                        1.00  1.61  2.38  3.09    5.43 :  4
   -implode -1                         1.00  1.51  2.21  2.86    6.90 :  4
   -lat 10x10-5%                       1.00  1.97  2.83  3.80    3.02 :  4
   -level 10%,1.2,90%                  1.00  1.51  1.77  1.97   63.40 :  4
   -median 1                           1.00  1.09  1.67  1.99    0.86 :  4
   -median 2                           1.00  1.22  1.83  2.23    0.35 :  4
   -modulate 110/100/95                1.00  1.96  2.82  3.72   16.77 :  4
   -motion-blur 0x3+30                 1.00  0.80  1.19  1.52    0.43 :  4
   -negate                             1.00  1.48  1.62  1.87  123.60 :  4
   +noise Uniform                      1.00  1.46  2.00  2.58   11.73 :  4
   +noise Gaussian                     1.00  1.58  2.32  3.04    1.91 :  4
   +noise Multiplicative               1.00  1.69  2.33  3.16    3.25 :  4
   +noise Impulse                      1.00  1.32  1.80  2.28   11.53 :  4
   +noise Laplacian                    1.00  1.30  1.88  2.43    4.93 :  4
   +noise Poisson                      1.00  1.37  1.87  2.51    2.47 :  4
   -noise 1                            1.00  1.12  1.69  2.03    0.86 :  4
   -noise 2                            1.00  1.24  1.85  2.29    0.35 :  4
   -normalize                          1.00  1.31  1.42  1.45   32.34 :  4
   -fill blue -fuzz 35% -opaque red    1.00  1.76  2.34  2.95  100.00 :  4
   -ordered-dither all 3x3             1.00  1.71  2.22  2.68   95.80 :  4
   -paint 0x1                          1.00  1.63  2.42  3.12    5.07 :  4
   -raise 10x10                        1.00  1.42  1.46  1.64  181.20 :  4
   -density 75x75 -resample 50x50      1.00  1.84  2.61  3.33    9.54 :  4
   -resize 10%                         1.00  1.76  2.38  2.99   20.92 :  4
   -resize 50%                         1.00  1.87  2.64  3.39   11.07 :  4
   -resize 150%                        1.00  1.93  2.47  3.39    3.19 :  4
   -roll +20+10                        1.00  0.83  0.83  0.83    2.26 :  1
   -rotate 0                           1.00  1.45  1.58  1.85   97.60 :  4
   -rotate 45                          1.00  1.51  2.15  2.70    1.03 :  4
   -rotate 90                          1.00  0.98  1.03  1.00   31.00 :  3
   -rotate 180                         1.00  1.47  1.66  1.93   99.60 :  4
   -rotate 270                         1.00  1.00  1.02  0.99   30.54 :  3
   -shade 30x30                        1.00  1.91  2.64  3.33   29.23 :  4
   -sharpen 0x0.5                      1.00  1.97  2.83  3.77   11.46 :  4
   -sharpen 0x1.0                      1.00  1.97  2.86  3.82    4.30 :  4
   -shave 10x10                        1.00  1.40  1.61  1.79   99.00 :  4
   -shear 45x45                        1.00  1.46  2.05  2.33    1.07 :  4
   -solarize 50%                       1.00  1.60  1.87  2.29  121.00 :  4
   -spread 1                           1.00  1.67  2.02  2.40   88.40 :  4
   -spread 3                           1.00  1.61  1.96  2.18   70.00 :  4
   -swirl 90                           1.00  1.50  2.18  2.76    6.79 :  4
   -threshold 35%                      1.00  1.70  2.14  2.66   98.60 :  4
   -fuzz 35% -transparent red          1.00  1.80  2.28  2.98   98.80 :  4
   -trim                               1.00  1.89  2.62  3.44   57.48 :  4
   -unsharp 0x0.5                      1.00  1.57  1.96  2.32    8.46 :  4
   -unsharp 0x1.0                      1.00  1.71  2.20  2.65    6.67 :  4
   -wave 25x150                        1.00  1.28  1.73  2.21    5.80 :  4
   -white-threshold 80%                1.00  1.53  1.76  2.20  137.20 :  4
   ================================== ===== ===== ===== ===== =============

The following table shows the boost on a four core IBM P5+ server system
running AIX:

.. table:: Performance Boost On Four Core IBM P5+ System

   ================================== ===== ===== ===== ===== =============
   Operation                            1     2     3     4   iter/s : thds
   ================================== ===== ===== ===== ===== =============
   -noop                               1.00  1.73  1.67  2.03  334.80 :  4
   -affine 1,0,0.785,1,0,0 -transform  1.00  1.92  2.20  2.64    6.80 :  4
   -black-threshold 20%                1.00  1.40  1.50  1.00   79.80 :  3
   -blur 0x0.5                         1.00  1.85  2.53  3.26    2.57 :  4
   -blur 0x1.0                         1.00  1.86  2.52  3.44    1.72 :  4
   -border 6x6                         1.00  1.67  1.41  1.04  139.60 :  2
   -charcoal 0x1                       1.00  1.72  2.41  3.30    0.78 :  4
   -chop 800x600+200+300               1.00  0.77  1.60  0.99  158.00 :  3
   -colors 16                          1.00  0.59  1.17  1.18    0.29 :  4
   -colorspace CMYK                    1.00  1.45  1.18  1.50   64.00 :  4
   -colorspace GRAY                    1.00  1.33  2.50  1.92   13.17 :  3
   -colorspace HSL                     1.00  2.43  2.89  3.90    8.22 :  4
   -colorspace HWB                     1.00  1.96  2.49  3.19    8.55 :  4
   -colorspace OHTA                    1.00  1.87  2.09  2.92   17.33 :  4
   -colorspace YCbCr                   1.00  1.91  2.22  2.92   17.36 :  4
   -colorspace YIQ                     1.00  1.83  2.26  2.83   16.80 :  4
   -colorspace YUV                     1.00  1.90  2.24  2.85   16.93 :  4
   -contrast -contrast -contrast       1.00  1.88  2.18  2.90    1.99 :  4
   +contrast +contrast +contrast       1.00  1.97  1.34  2.89    2.01 :  4
   -convolve 1,1,1,1,4,1,1,1,1         1.00  1.93  2.68  3.51    3.52 :  4
   -colorize 30%/20%/50%               1.00  5.43  9.37 12.69   11.78 :  4
   -crop 1700x900+100+100              1.00  0.99  1.62  1.89  181.80 :  4
   -despeckle                          1.00  1.49  2.78  2.75    0.27 :  3
   -edge 0x1                           1.00  2.00  2.61  3.30    3.44 :  4
   -emboss 0x1                         1.00  1.65  2.66  3.47    1.57 :  4
   -enhance                            1.00  1.82  2.61  3.41    0.89 :  4
   -equalize                           1.00  1.21  0.96  1.09   33.47 :  2
   -flip                               1.00  1.65  1.55  1.80  153.00 :  4
   -flop                               1.00  1.20  1.65  2.03  145.20 :  4
   -frame 15x15+3+3                    1.00  1.51  1.44  1.38  124.80 :  2
   -gamma 1.6                          1.00  1.44  1.21  1.37   72.20 :  2
   -gaussian 0x0.5                     1.00  1.98  2.80  3.64    1.67 :  4
   -gaussian 0x1.0                     1.00  1.99  2.84  3.77    0.63 :  4
   -implode 0.5                        1.00  1.96  2.25  2.97    2.77 :  4
   -implode -1                         1.00  1.90  2.25  2.95    2.75 :  4
   -lat 10x10-5%                       1.00  1.99  2.22  2.72    1.00 :  4
   -level 10%,1.2,90%                  1.00  1.17  1.21  1.36   67.80 :  4
   -median 1                           1.00  1.88  1.94  2.70    0.76 :  4
   -median 2                           1.00  1.93  2.10  2.65    0.28 :  4
   -modulate 110/100/95                1.00  2.00  2.21  2.76    6.05 :  4
   -motion-blur 0x3+30                 1.00  1.83  2.02  2.62    1.08 :  4
   -negate                             1.00  2.06  1.86  2.31  176.40 :  4
   +noise Uniform                      1.00  1.94  2.54  3.34    4.62 :  4
   +noise Gaussian                     1.00  1.87  2.36  3.09    1.86 :  4
   +noise Multiplicative               1.00  1.98  2.57  3.35    2.06 :  4
   +noise Impulse                      1.00  1.95  2.46  3.21    4.55 :  4
   +noise Laplacian                    1.00  1.89  2.59  3.47    3.18 :  4
   +noise Poisson                      1.00  1.95  2.53  3.28    1.44 :  4
   -noise 1                            1.00  1.69  2.09  2.63    0.74 :  4
   -noise 2                            1.00  1.92  2.08  2.63    0.28 :  4
   -normalize                          1.00  1.17  1.19  1.23   34.26 :  4
   -fill blue -fuzz 35% -opaque red    1.00  1.97  2.03  3.00   26.55 :  4
   -ordered-dither all 3x3             1.00  1.73  1.59  1.80   49.90 :  4
   -paint 0x1                          1.00  1.33  1.98  2.68    5.86 :  4
   -raise 10x10                        1.00  1.88  1.95  1.82  136.00 :  3
   -density 75x75 -resample 50x50      1.00  1.99  2.53  3.38    3.71 :  4
   -resize 10%                         1.00  1.91  2.55  3.36    8.15 :  4
   -resize 50%                         1.00  2.00  2.62  3.40    4.79 :  4
   -resize 150%                        1.00  1.99  2.25  2.88    1.33 :  4
   -roll +20+10                        1.00  1.01  1.02  1.01    0.60 :  3
   -rotate 0                           1.00  1.69  1.96  2.19  127.80 :  4
   -rotate 45                          1.00  1.17  1.37  1.51    0.42 :  4
   -rotate 90                          1.00  1.08  1.05  1.08   31.54 :  2
   -rotate 180                         1.00  1.78  1.33  2.04  145.40 :  4
   -rotate 270                         1.00  1.07  1.10  1.03   32.00 :  3
   -shade 30x30                        1.00  2.00  2.38  3.21   10.78 :  4
   -sharpen 0x0.5                      1.00  1.99  2.72  3.64    1.68 :  4
   -sharpen 0x1.0                      1.00  1.99  2.77  3.77    0.63 :  4
   -shave 10x10                        1.00  1.69  1.56  1.49  143.80 :  2
   -shear 45x45                        1.00  1.23  1.73  1.55    0.88 :  3
   -solarize 50%                       1.00  1.52  1.48  2.14   98.20 :  4
   -spread 1                           1.00  1.54  1.93  2.29   65.40 :  4
   -spread 3                           1.00  1.62  1.96  2.30   56.20 :  4
   -swirl 90                           1.00  1.82  2.40  2.99    2.76 :  4
   -threshold 35%                      1.00  1.48  1.60  1.71   44.20 :  4
   -fuzz 35% -transparent red          1.00  1.70  2.27  2.74   22.40 :  4
   -trim                               1.00  1.86  2.58  3.15   10.34 :  4
   -unsharp 0x0.5                      1.00  1.85  2.56  3.05    2.23 :  4
   -unsharp 0x1.0                      1.00  1.88  2.60  3.37    2.48 :  4
   -wave 25x150                        1.00  1.75  2.26  2.75    2.58 :  4
   -white-threshold 80%                1.00  1.61  1.71  2.23  124.80 :  4
   ================================== ===== ===== ===== ===== =============


The following table shows the boost on a two core AMD64 Linux system:

.. table:: Performance Boost On Two Core AMD64 System

   ================================== ===== ===== =============
   Operation                            1     2   iter/s : thds
   ================================== ===== ===== =============
   -noop                               1.00  1.26  144.80 :  2
   -affine 1,0,0.785,1,0,0 -transform  1.00  1.26    8.04 :  2
   -black-threshold 20%                1.00  1.12   53.29 :  2
   -blur 0x0.5                         1.00  1.38    4.58 :  2
   -blur 0x1.0                         1.00  1.55    3.72 :  2
   -border 6x6                         1.00  1.27   62.87 :  2
   -charcoal 0x1                       1.00  1.64    1.81 :  2
   -chop 800x600+200+300               1.00  1.48  102.00 :  2
   -colors 16                          1.00  1.01    0.91 :  2
   -colorspace CMYK                    1.00  1.50   66.00 :  2
   -colorspace GRAY                    1.00  1.76   33.07 :  2
   -colorspace HSL                     1.00  1.93   12.45 :  2
   -colorspace HWB                     1.00  1.86   15.94 :  2
   -colorspace OHTA                    1.00  1.79   33.00 :  2
   -colorspace YCbCr                   1.00  1.82   32.87 :  2
   -colorspace YIQ                     1.00  1.75   31.34 :  2
   -colorspace YUV                     1.00  1.85   32.60 :  2
   -contrast -contrast -contrast       1.00  1.98    2.41 :  2
   +contrast +contrast +contrast       1.00  1.97    2.46 :  2
   -convolve 1,1,1,1,4,1,1,1,1         1.00  1.99    8.84 :  2
   -colorize 30%/20%/50%               1.00  1.65   15.11 :  2
   -crop 1700x900+100+100              1.00  1.33   71.26 :  2
   -despeckle                          1.00  1.47    0.92 :  2
   -edge 0x1                           1.00  1.98    8.28 :  2
   -emboss 0x1                         1.00  1.86    4.05 :  2
   -enhance                            1.00  1.80    1.25 :  2
   -equalize                           1.00  1.20   23.35 :  2
   -flip                               1.00  1.46   23.06 :  2
   -flop                               1.00  1.50   22.22 :  2
   -frame 15x15+3+3                    1.00  1.25   62.27 :  2
   -gamma 1.6                          1.00  1.02   19.36 :  2
   -gaussian 0x0.5                     1.00  2.01    5.06 :  2
   -gaussian 0x1.0                     1.00  1.99    1.98 :  2
   -implode 0.5                        1.00  1.59    2.62 :  2
   -implode -1                         1.00  1.23    2.80 :  2
   -lat 10x10-5%                       1.00  1.82    1.31 :  2
   -level 10%,1.2,90%                  1.00  1.46   41.20 :  2
   -median 1                           1.00  0.99    0.39 :  1
   -median 2                           1.00  1.11    0.16 :  2
   -modulate 110/100/95                1.00  1.96    8.64 :  2
   -motion-blur 0x3+30                 1.00  0.86    1.15 :  1
   -negate                             1.00  1.52   75.00 :  2
   +noise Uniform                      1.00  1.65    5.83 :  2
   +noise Gaussian                     1.00  1.92    0.82 :  2
   +noise Multiplicative               1.00  1.89    1.33 :  2
   +noise Impulse                      1.00  1.58    5.72 :  2
   +noise Laplacian                    1.00  1.89    2.04 :  2
   +noise Poisson                      1.00  1.92    1.68 :  2
   -noise 1                            1.00  1.03    0.40 :  2
   -noise 2                            1.00  1.13    0.16 :  2
   -normalize                          1.00  1.30   25.75 :  2
   -fill blue -fuzz 35% -opaque red    1.00  1.78   50.90 :  2
   -ordered-dither all 3x3             1.00  1.44   43.31 :  2
   -paint 0x1                          1.00  1.55    4.48 :  2
   -raise 10x10                        1.00  1.54  128.20 :  2
   -density 75x75 -resample 50x50      1.00  1.83    3.82 :  2
   -resize 10%                         1.00  1.80   10.02 :  2
   -resize 50%                         1.00  1.77    5.34 :  2
   -resize 150%                        1.00  1.70    1.44 :  2
   -roll +20+10                        1.00  0.98    2.05 :  1
   -rotate 0                           1.00  1.42   22.36 :  2
   -rotate 45                          1.00  1.58    0.81 :  2
   -rotate 90                          1.00  1.11   12.72 :  2
   -rotate 180                         1.00  1.31   19.56 :  2
   -rotate 270                         1.00  0.99   11.64 :  1
   -shade 30x30                        1.00  1.39    7.74 :  2
   -sharpen 0x0.5                      1.00  1.73    4.36 :  2
   -sharpen 0x1.0                      1.00  1.93    1.93 :  2
   -shave 10x10                        1.00  1.34   61.20 :  2
   -shear 45x45                        1.00  1.64    0.72 :  2
   -solarize 50%                       1.00  1.54   69.20 :  2
   -spread 1                           1.00  1.00   12.75 :  1
   -spread 3                           1.00  1.62   18.56 :  2
   -swirl 90                           1.00  1.30    2.95 :  2
   -threshold 35%                      1.00  1.13   36.00 :  2
   -fuzz 35% -transparent red          1.00  1.68   49.70 :  2
   -trim                               1.00  1.65   22.07 :  2
   -unsharp 0x0.5                      1.00  1.48    4.39 :  2
   -unsharp 0x1.0                      1.00  1.43    4.25 :  2
   -wave 25x150                        1.00  1.52    4.30 :  2
   -white-threshold 80%                1.00  1.67   76.45 :  2
   ================================== ===== ===== =============


--------------------------------------------------------------------------

| Copyright (C) 2008 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

