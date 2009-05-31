.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

========================
OpenMP in GraphicsMagick
========================

.. contents::
  :local:

Overview
========

GraphicsMagick has been transformed to use `OpenMP
<http://openmp.org/>`_ for the 1.3 release. OpenMP is a portable
framework for accelerating CPU-bound and memory-bound operations using
multiple threads. OpenMP originates in the super-computing world and
has been available in one form or another since the late '90s.

Since GCC 4.2 has introduced excellent OpenMP support, OpenMP has become
available to the masses. Microsoft Visual Studio Professional 2005 and
2008 support OpenMP so Windows users can benefit as well. Any
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
  Results: 5 iter 11.41s user 11.46s total 0.436 iter/s (0.438 iter/s cpu)
  Results: 9 iter 20.55s user 10.30s total 0.874 iter/s (0.438 iter/s cpu)
  Results: 13 iter 29.74s user 10.01s total 1.299 iter/s (0.437 iter/s cpu)
  Results: 17 iter 38.90s user 10.08s total 1.687 iter/s (0.437 iter/s cpu)

Note that the "iter/s cpu" value is a measure of the number of iterations
given the amount of reported CPU time consumed. It is an effective
measure of relative efficacy. In the above example, the total speedup was
about 3.9X with only a slight loss of CPU efficiency as threads are added.

According to the OpenMP specification, the OMP_NUM_THREADS evironment
variable (used above) may be used to specify the number of threads
available to the application. Typically this is set to the number of
processor cores on the system but may be set lower to limit resource
consumption or (in some cases) to improve execution efficiency.

Results
=======

A simple scheme was developed in order to evaluate the performance
boost with varying numbers of threads.  GraphicsMagick's built-in
benchmark facility is used.  The selected algorithm is executed
repeatedly until a specified amount of time has elapsed.  The input
image is generated on the fly by tiling a small image over a large
area using a specification like `-size 1920x1080 tile:model.pnm`.  It
is important to note that the time to generate the input image is
included in the benchmark timings so that even if an algorithm
achieves perfect linear scaling, the measured difference is likely to
be less than the number of cores used and the impact could be
substantial if image generation is slow.

The first test executed is `-noop` since it does no work other than
to generate the input image.  This represents how fast it is possible
to go based on how fast the input image may be generated.

Sun Solaris / AMD Opteron
-------------------------

.. Last update: Sat May 30 14:49:38 CDT 2009

The following table shows the performance boost in GraphicsMagick
1.4 as threads are added on a four-core AMD Opteron 3.0GHz system
running Sun Solaris 10:

.. table:: Performance Boost On Four Core AMD Operon System

   ============================================== ===== ===== ===== ===== ======= ====
   Operation                                        1     2     3     4   iter/s  thds
   ============================================== ===== ===== ===== ===== ======= ====
   -noop                                           1.00  1.32  1.55  1.60  186.80   4
   -affine 1,0,0.785,1,0,0 -transform              1.00  1.91  2.75  3.43   17.89   4
   -blur 0x0.5                                     1.00  1.69  2.19  2.32   11.29   4
   -blur 0x1.0                                     1.00  1.77  2.39  2.68    8.58   4
   -charcoal 0x1                                   1.00  1.80  2.45  2.90    4.05   4
   -colorspace CMYK                                1.00  1.57  1.84  2.04   92.00   4
   -colorspace GRAY                                1.00  1.79  2.50  3.05   55.20   4
   -colorspace HSL                                 1.00  1.95  2.85  3.62   17.60   4
   -colorspace HWB                                 1.00  1.92  2.77  3.51   27.24   4
   -colorspace OHTA                                1.00  1.80  2.53  3.07   53.60   4
   -colorspace YCbCr                               1.00  1.79  2.51  3.07   54.09   4
   -colorspace YIQ                                 1.00  1.80  2.52  3.05   52.49   4
   -colorspace YUV                                 1.00  1.80  2.53  3.11   53.60   4
   -contrast -contrast -contrast                   1.00  1.99  2.95  3.82    3.02   4
   +contrast +contrast +contrast                   1.00  1.97  2.94  3.80    3.29   4
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  1.90  2.74  3.43   21.91   4
   -colorize 30%/20%/50%                           1.00  1.84  2.57  3.18   48.30   4
   -despeckle                                      1.00  1.50  2.88  2.87    1.73   3
   -edge 0x1                                       1.00  1.93  2.83  3.61   12.03   4
   -emboss 0x1                                     1.00  1.78  2.44  2.96    8.63   4
   -enhance                                        1.00  1.99  2.97  3.86    2.58   4
   -gaussian 0x0.5                                 1.00  1.92  2.82  3.60   12.53   4
   -gaussian 0x1.0                                 1.00  1.97  2.90  3.76    5.14   4
   -implode 0.5                                    1.00  1.97  2.94  3.80    5.24   4
   -implode -1                                     1.00  1.96  2.92  3.76    6.45   4
   -lat 10x10-5%                                   1.00  1.99  2.92  3.75    3.82   4
   -median 1                                       1.00  1.98  2.92  3.79    1.59   4
   -median 2                                       1.00  1.99  2.95  3.84    0.59   4
   -minify                                         1.00  1.80  2.53  3.11   48.80   4
   -modulate 110/100/95                            1.00  1.96  2.86  3.65   14.29   4
   +noise Uniform                                  1.00  1.93  2.72  3.41   13.49   4
   +noise Gaussian                                 1.00  1.99  2.94  3.82    1.76   4
   +noise Multiplicative                           1.00  1.97  2.92  3.79    2.73   4
   +noise Impulse                                  1.00  1.88  2.68  3.39   13.92   4
   +noise Laplacian                                1.00  1.95  2.85  3.65    5.98   4
   +noise Poisson                                  1.00  1.95  2.87  3.69    3.78   4
   -noise 1                                        1.00  1.97  2.92  3.79    1.63   4
   -noise 2                                        1.00  2.00  2.97  3.87    0.61   4
   -fill blue -fuzz 35% -opaque red                1.00  1.64  2.19  2.55   96.40   4
   -operator all Add 2%                            1.00  1.76  2.39  2.90   72.40   4
   -operator all And 233                           1.00  1.39  1.70  1.78  103.40   4
   -operator all Assign 50%                        1.00  1.27  1.53  1.68  103.80   4
   -operator all Depth 6                           1.00  1.56  2.03  2.34   82.04   4
   -operator all Divide 2                          1.00  1.74  2.39  2.88   67.60   4
   -operator all Gamma 0.7                         1.00  1.42  1.66  1.79   52.40   4
   -operator all Negate 1.0                        1.00  1.34  1.62  1.71  102.20   4
   -operator all LShift 2                          1.00  1.52  1.83  1.99   99.80   4
   -operator all Multiply 0.5                      1.00  1.73  2.36  2.84   73.20   4
   -operator all Or 233                            1.00  1.39  1.64  1.73   99.80   4
   -operator all RShift 2                          1.00  1.43  1.77  1.85   99.40   4
   -operator all Subtract 10%                      1.00  1.75  2.41  2.86   69.80   4
   -operator red Threshold 50%                     1.00  1.42  1.71  1.80  101.40   4
   -operator gray Threshold 50%                    1.00  1.42  1.79  1.96   99.00   4
   -operator all Threshold-White 80%               1.00  1.45  1.80  1.97  111.80   4
   -operator all Threshold-Black 10%               1.00  1.48  1.85  2.08  111.40   4
   -operator all Xor 233                           1.00  1.42  1.72  1.81  101.40   4
   -operator all Noise-Gaussian 30%                1.00  1.99  2.96  3.84    1.80   4
   -operator all Noise-Impulse 30%                 1.00  1.96  2.87  3.67   15.54   4
   -operator all Noise-Laplacian 30%               1.00  1.98  2.92  3.75    6.25   4
   -operator all Noise-Multiplicative 30%          1.00  1.99  2.97  3.85    2.80   4
   -operator all Noise-Poisson 30%                 1.00  1.96  2.88  3.78    3.94   4
   -operator all Noise-Uniform 30%                 1.00  1.96  2.87  3.66   15.20   4
   -ordered-dither all 2x2                         1.00  1.55  1.97  2.05   82.00   4
   -ordered-dither all 3x3                         1.00  1.61  2.14  2.33   78.40   4
   -ordered-dither intensity 3x3                   1.00  1.65  2.15  2.47   81.00   4
   -ordered-dither all 4x4                         1.00  1.55  1.97  2.14   85.20   4
   -paint 0x1                                      1.00  1.97  2.93  3.79    5.59   4
   -random-threshold all 20x80                     1.00  1.79  2.48  3.02   49.70   4
   -recolor '1,0,0,0,1,0,0,0,1'                    1.00  1.30  1.55  1.61  187.80   4
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  1.84  2.56  3.16   51.80   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  1.86  2.65  3.26   44.60   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  1.85  2.63  3.29   44.71   4
   -density 75x75 -resample 50x50                  1.00  1.92  2.71  3.37   12.03   4
   -resize 10%                                     1.00  1.89  2.67  3.33   23.70   4
   -resize 50%                                     1.00  1.92  2.80  3.46   17.60   4
   -resize 150%                                    1.00  2.15  3.09  3.99    3.19   4
   -rotate 45                                      1.00  1.73  2.41  2.93    1.04   4
   -segment 0.5x0.25                               1.00  1.48  1.66  1.77    0.17   4
   -shade 30x30                                    1.00  1.92  2.76  3.52   14.29   4
   -sharpen 0x0.5                                  1.00  1.91  2.80  3.61   12.53   4
   -sharpen 0x1.0                                  1.00  1.97  2.90  3.76    5.12   4
   -shear 45x45                                    1.00  1.63  2.26  2.66    1.29   4
   -solarize 50%                                   1.00  1.49  1.89  2.04  100.00   4
   -swirl 90                                       1.00  1.97  2.93  3.77    6.35   4
   -fuzz 35% -transparent red                      1.00  1.65  2.21  2.58   86.80   4
   -trim                                           1.00  1.87  2.66  3.32   45.51   4
   -fuzz 5% -trim                                  1.00  1.90  2.73  3.47   32.53   4
   -unsharp 0x0.5+20+1                             1.00  1.74  2.30  2.54    9.72   4
   -unsharp 0x1.0+20+1                             1.00  1.83  2.50  2.89    7.72   4
   -wave 25x150                                    1.00  1.54  1.64  1.70    3.37   4
   ============================================== ===== ===== ===== ===== ======= ====

Sun Solaris / UltraSPARC III
----------------------------

.. Last update: Sun May 31 10:59:59 CDT 2009

The following table shows the performance boost as threads are added
on 2 CPU Sun SPARC 1.2GHz workstation running Sun Solaris 10.  This
system obtains quite substantial benefit for most key algorithms:

.. table:: Performance Boost On Two CPU SPARC System

   ============================================== ===== ===== ======= ====
   Operation                                        1     2   iter/s  thds
   ============================================== ===== ===== ======= ====
   -noop                                           1.00  1.26   67.80   2
   -affine 1,0,0.785,1,0,0 -transform              1.00  1.91    1.96   2
   -blur 0x0.5                                     1.00  1.77    1.83   2
   -blur 0x1.0                                     1.00  1.90    1.44   2
   -charcoal 0x1                                   1.00  1.82    0.56   2
   -colorspace CMYK                                1.00  1.68   16.60   2
   -colorspace GRAY                                1.00  1.92    6.96   2
   -colorspace HSL                                 1.00  1.83    2.64   2
   -colorspace HWB                                 1.00  1.92    3.23   2
   -colorspace OHTA                                1.00  1.81    6.71   2
   -colorspace YCbCr                               1.00  1.85    6.96   2
   -colorspace YIQ                                 1.00  1.78    6.75   2
   -colorspace YUV                                 1.00  1.94    6.97   2
   -contrast -contrast -contrast                   1.00  1.92    0.46   2
   +contrast +contrast +contrast                   1.00  1.97    0.49   2
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  1.89    2.44   2
   -colorize 30%/20%/50%                           1.00  1.91    6.81   2
   -despeckle                                      1.00  1.53    0.17   2
   -edge 0x1                                       1.00  1.96    1.45   2
   -emboss 0x1                                     1.00  1.81    1.08   2
   -enhance                                        1.00  1.91    0.45   2
   -gaussian 0x0.5                                 1.00  1.91    1.33   2
   -gaussian 0x1.0                                 1.00  1.98    0.52   2
   -implode 0.5                                    1.00  1.90    0.88   2
   -implode -1                                     1.00  1.88    1.10   2
   -lat 10x10-5%                                   1.00  1.87    0.52   2
   -median 1                                       1.00  1.91    0.32   2
   -median 2                                       1.00  1.90    0.13   2
   -minify                                         1.00  1.86    9.29   2
   -modulate 110/100/95                            1.00  1.95    1.74   2
   +noise Uniform                                  1.00  1.92    2.00   2
   +noise Gaussian                                 1.00  1.98    0.33   2
   +noise Multiplicative                           1.00  1.96    0.52   2
   +noise Impulse                                  1.00  1.91    1.88   2
   +noise Laplacian                                1.00  1.89    0.94   2
   +noise Poisson                                  1.00  1.98    0.56   2
   -noise 1                                        1.00  1.91    0.31   2
   -noise 2                                        1.00  1.98    0.13   2
   -fill blue -fuzz 35% -opaque red                1.00  1.95    8.78   2
   -operator all Add 2%                            1.00  1.90    6.85   2
   -operator all And 233                           1.00  1.62   21.32   2
   -operator all Assign 50%                        1.00  1.52   23.46   2
   -operator all Depth 6                           1.00  1.56   18.60   2
   -operator all Divide 2                          1.00  1.83    5.23   2
   -operator all Gamma 0.7                         1.00  1.42   12.80   2
   -operator all Negate 1.0                        1.00  1.49   19.52   2
   -operator all LShift 2                          1.00  1.64   21.27   2
   -operator all Multiply 0.5                      1.00  1.84    6.59   2
   -operator all Or 233                            1.00  1.60   21.20   2
   -operator all RShift 2                          1.00  1.54   20.04   2
   -operator all Subtract 10%                      1.00  1.89    7.16   2
   -operator red Threshold 50%                     1.00  1.53   22.36   2
   -operator gray Threshold 50%                    1.00  1.69   16.17   2
   -operator all Threshold-White 80%               1.00  1.69   15.68   2
   -operator all Threshold-Black 10%               1.00  1.71   15.94   2
   -operator all Xor 233                           1.00  1.56   19.88   2
   -operator all Noise-Gaussian 30%                1.00  1.99    0.33   2
   -operator all Noise-Impulse 30%                 1.00  1.83    1.80   2
   -operator all Noise-Laplacian 30%               1.00  1.96    0.98   2
   -operator all Noise-Multiplicative 30%          1.00  1.98    0.52   2
   -operator all Noise-Poisson 30%                 1.00  1.89    0.54   2
   -operator all Noise-Uniform 30%                 1.00  1.78    1.87   2
   -ordered-dither all 2x2                         1.00  1.73   12.85   2
   -ordered-dither all 3x3                         1.00  1.73   11.48   2
   -ordered-dither intensity 3x3                   1.00  1.62   10.98   2
   -ordered-dither all 4x4                         1.00  1.75   13.49   2
   -paint 0x1                                      1.00  1.93    0.76   2
   -random-threshold all 20x80                     1.00  1.76    6.65   2
   -recolor '1,0,0,0,1,0,0,0,1'                    1.00  1.27   68.00   2
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  1.88    5.52   2
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  1.95    4.60   2
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  1.81    4.17   2
   -density 75x75 -resample 50x50                  1.00  1.83    1.68   2
   -resize 10%                                     1.00  1.83    3.47   2
   -resize 50%                                     1.00  1.75    2.08   2
   -resize 150%                                    1.00  1.70    0.58   2
   -rotate 45                                      1.00  1.54    0.33   2
   -segment 0.5x0.25                               1.00  1.44    0.02   2
   -shade 30x30                                    1.00  1.96    2.28   2
   -sharpen 0x0.5                                  1.00  1.95    1.35   2
   -sharpen 0x1.0                                  1.00  1.91    0.50   2
   -shear 45x45                                    1.00  1.29    0.61   2
   -solarize 50%                                   1.00  1.63   19.76   2
   -swirl 90                                       1.00  1.85    1.06   2
   -fuzz 35% -transparent red                      1.00  1.87    8.91   2
   -trim                                           1.00  1.96    4.21   2
   -fuzz 5% -trim                                  1.00  1.95    2.94   2
   -unsharp 0x0.5+20+1                             1.00  1.86    1.48   2
   -unsharp 0x1.0+20+1                             1.00  1.74    1.10   2
   -wave 25x150                                    1.00  1.69    0.98   2
   ============================================== ===== ===== ======= ====

IBM AIX / IBM Power5+
---------------------

.. Last update: Sun May 31 14:20:46 UTC 2009

The following table shows the boost on a four core IBM P5+ server
system (IBM System p5 505 Express with (2) 2.1Ghz CPUs) running AIX:

.. table:: Performance Boost On Four Core IBM P5+ System

   ============================================== ===== ===== ===== ===== ======= ====
   Operation                                        1     2     3     4   iter/s  thds
   ============================================== ===== ===== ===== ===== ======= ====
   -noop                                           1.00  1.58  1.64  1.69  291.40   4
   -affine 1,0,0.785,1,0,0 -transform              1.00  1.99  2.63  3.22    8.71   4
   -blur 0x0.5                                     1.00  1.97  2.50  3.00    2.69   4
   -blur 0x1.0                                     1.00  1.98  2.64  3.29    2.13   4
   -charcoal 0x1                                   1.00  1.98  2.68  3.37    0.96   4
   -colorspace CMYK                                1.00  1.47  1.62  1.60   69.06   3
   -colorspace GRAY                                1.00  1.93  2.47  2.94   17.20   4
   -colorspace HSL                                 1.00  1.95  2.45  2.97    8.17   4
   -colorspace HWB                                 1.00  1.97  2.58  3.15    8.58   4
   -colorspace OHTA                                1.00  1.87  2.46  2.93   17.20   4
   -colorspace YCbCr                               1.00  1.89  2.42  2.94   17.20   4
   -colorspace YIQ                                 1.00  1.93  2.45  2.92   17.06   4
   -colorspace YUV                                 1.00  1.89  2.43  2.92   17.10   4
   -contrast -contrast -contrast                   1.00  2.01  2.48  2.95    2.08   4
   +contrast +contrast +contrast                   1.00  2.00  2.50  3.00    2.15   4
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  2.00  2.77  3.52    3.84   4
   -colorize 30%/20%/50%                           1.00  1.98  2.75  3.53   11.71   4
   -despeckle                                      1.00  1.49  2.78  2.78    0.27   3
   -edge 0x1                                       1.00  2.00  2.77  3.51    4.05   4
   -emboss 0x1                                     1.00  1.95  2.66  3.34    1.81   4
   -enhance                                        1.00  2.00  2.75  3.50    0.90   4
   -gaussian 0x0.5                                 1.00  1.99  2.78  3.57    1.98   4
   -gaussian 0x1.0                                 1.00  1.99  2.86  3.69    0.79   4
   -implode 0.5                                    1.00  2.00  2.50  3.03    2.99   4
   -implode -1                                     1.00  1.97  2.52  3.04    2.99   4
   -lat 10x10-5%                                   1.00  1.95  2.43  2.86    1.34   4
   -median 1                                       1.00  1.94  2.41  2.80    0.79   4
   -median 2                                       1.00  1.97  2.39  2.77    0.30   4
   -minify                                         1.00  1.98  2.82  3.67    9.72   4
   -modulate 110/100/95                            1.00  1.53  2.45  2.88    6.84   4
   +noise Uniform                                  1.00  1.77  2.68  3.38    5.19   4
   +noise Gaussian                                 1.00  1.98  2.54  3.10    2.06   4
   +noise Multiplicative                           1.00  1.96  2.73  3.46    2.32   4
   +noise Impulse                                  1.00  1.85  2.61  3.24    5.26   4
   +noise Laplacian                                1.00  1.95  2.75  3.53    3.58   4
   +noise Poisson                                  1.00  1.97  2.74  3.47    2.20   4
   -noise 1                                        1.00  1.95  2.40  2.77    0.78   4
   -noise 2                                        1.00  1.97  2.39  2.77    0.30   4
   -fill blue -fuzz 35% -opaque red                1.00  1.95  2.38  2.83   38.80   4
   -operator all Add 2%                            1.00  1.83  2.77  3.55   15.60   4
   -operator all And 233                           1.00  1.72  1.74  1.77  161.80   4
   -operator all Assign 50%                        1.00  1.72  1.72  1.77  171.80   4
   -operator all Depth 6                           1.00  1.73  1.80  1.83  134.20   4
   -operator all Divide 2                          1.00  1.86  2.78  3.60   15.68   4
   -operator all Gamma 0.7                         1.00  1.57  1.59  1.62  104.00   4
   -operator all Negate 1.0                        1.00  1.72  1.74  1.80  163.40   4
   -operator all LShift 2                          1.00  1.70  1.77  1.82  165.20   4
   -operator all Multiply 0.5                      1.00  1.94  2.78  3.59   15.64   4
   -operator all Or 233                            1.00  1.71  1.75  1.81  164.20   4
   -operator all RShift 2                          1.00  1.72  1.77  1.81  163.80   4
   -operator all Subtract 10%                      1.00  1.98  2.75  3.49   16.73   4
   -operator red Threshold 50%                     1.00  1.71  1.89  2.05  168.00   4
   -operator gray Threshold 50%                    1.00  1.80  1.89  1.95  111.60   4
   -operator all Threshold-White 80%               1.00  1.81  1.93  2.05  120.00   4
   -operator all Threshold-Black 10%               1.00  1.81  2.01  2.17  118.20   4
   -operator all Xor 233                           1.00  1.58  1.72  1.77  160.20   4
   -operator all Noise-Gaussian 30%                1.00  1.99  2.56  3.13    2.09   4
   -operator all Noise-Impulse 30%                 1.00  1.94  2.68  3.38    5.56   4
   -operator all Noise-Laplacian 30%               1.00  1.94  2.80  3.60    3.70   4
   -operator all Noise-Multiplicative 30%          1.00  2.00  2.75  3.50    2.37   4
   -operator all Noise-Poisson 30%                 1.00  1.99  2.77  3.54    2.26   4
   -operator all Noise-Uniform 30%                 1.00  2.00  2.73  3.49    5.43   4
   -ordered-dither all 2x2                         1.00  1.65  1.69  1.62   56.49   3
   -ordered-dither all 3x3                         1.00  1.70  1.72  1.67   48.90   3
   -ordered-dither intensity 3x3                   1.00  1.70  1.63  1.69   48.20   2
   -ordered-dither all 4x4                         1.00  1.65  1.74  1.65   57.09   3
   -paint 0x1                                      1.00  2.03  2.31  2.47    6.34   4
   -random-threshold all 20x80                     1.00  1.63  2.40  2.79   22.60   4
   -recolor '1,0,0,0,1,0,0,0,1'                    1.00  1.55  1.60  1.60  277.00   4
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  1.98  2.65  3.35   11.31   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  1.99  2.61  3.21    7.37   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  1.99  2.58  3.22    7.38   4
   -density 75x75 -resample 50x50                  1.00  1.95  2.42  3.19    3.94   4
   -resize 10%                                     1.00  1.99  2.57  3.33    9.34   4
   -resize 50%                                     1.00  1.97  2.56  3.40    5.54   4
   -resize 150%                                    1.00  1.93  2.23  2.83    1.39   4
   -rotate 45                                      1.00  1.70  1.51  1.67    0.47   2
   -segment 0.5x0.25                               1.00  1.15  1.15  1.15    0.03   2
   -shade 30x30                                    1.00  1.88  2.56  3.13   10.38   4
   -sharpen 0x0.5                                  1.00  1.99  2.83  3.63    1.99   4
   -sharpen 0x1.0                                  1.00  2.00  2.86  3.69    0.79   4
   -shear 45x45                                    1.00  1.83  2.34  2.26    1.24   3
   -solarize 50%                                   1.00  1.58  2.04  2.23  111.00   4
   -swirl 90                                       1.00  1.99  2.47  2.96    3.24   4
   -fuzz 35% -transparent red                      1.00  1.96  2.24  2.52   33.47   4
   -trim                                           1.00  1.96  2.31  2.69   28.54   4
   -fuzz 5% -trim                                  1.00  1.80  2.65  3.28    9.49   4
   -unsharp 0x0.5+20+1                             1.00  1.77  2.58  3.18    2.27   4
   -unsharp 0x1.0+20+1                             1.00  1.87  2.70  3.38    1.84   4
   -wave 25x150                                    1.00  1.61  2.42  2.82    2.66   4
   ============================================== ===== ===== ===== ===== ======= ====

Linux / Intel Xeon
------------------

These tests were run on an eight core Intel Xeon (L5335) system
running at 2.00GHz.  This system is built using two CPUs, each of
which are multi-chip modules (MCM) containing a pair of dual-core
CPUs.  While the total performance of the Intel Xeon CPU can be quite
impressive, we are not impressed with the performance of Intel's
MCM-based CPUs with OpenMP.  These MCM-based CPUs are very good for
SMP but not very good for multi-threading.  While this build did not
use the DisableSlowOpenMP option to disable OpenMP for fast
algorithms, we now recommend it for systems using Intel multi-chip
modules.

.. table:: Performance Boost On An Eight Core Xeon System

   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ===== ======= ====
   Operation                                                               1     2     3     4     5     6     7     8   iter/s  thds
   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ===== ======= ====
   -noop                                                                  1.00  1.39  1.62  1.58  1.56  1.52  1.40  1.25   68.80   3
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.84  2.54  3.05  3.38  3.54  3.69  3.62   15.60   7
   -blur 0x0.5                                                            1.00  1.52  1.79  1.94  1.97  2.07  2.08  1.96    7.63   7
   -blur 0x1.0                                                            1.00  1.62  1.99  2.17  2.27  2.43  2.48  2.48    7.06   7
   -charcoal 0x1                                                          1.00  1.65  2.11  2.40  2.61  2.75  2.91  2.91    3.56   8
   -colorspace CMYK                                                       1.00  1.39  1.54  1.56  1.55  1.50  1.45  1.38   37.73   4
   -colorspace GRAY                                                       1.00  1.52  1.82  1.86  1.86  1.78  1.79  1.68   31.08   5
   -colorspace HSL                                                        1.00  1.93  2.81  3.56  4.29  4.89  5.34  5.70   17.06   8
   -colorspace HWB                                                        1.00  1.89  2.70  3.38  4.00  4.40  4.82  5.07   21.40   8
   -colorspace OHTA                                                       1.00  1.54  1.81  1.86  1.86  1.77  1.71  1.66   31.01   5
   -colorspace YCbCr                                                      1.00  1.51  1.79  1.88  1.88  1.78  1.77  1.64   31.47   5
   -colorspace YIQ                                                        1.00  1.53  1.80  1.85  1.85  1.77  1.77  1.65   30.88   4
   -colorspace YUV                                                        1.00  1.54  1.81  1.89  1.86  1.77  1.78  1.66   31.40   4
   -contrast -contrast -contrast                                          1.00  1.98  2.94  3.86  4.79  5.66  6.46  7.23    5.12   8
   +contrast +contrast +contrast                                          1.00  1.97  2.93  3.85  4.78  5.66  6.44  7.20    5.19   8
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.83  2.59  3.18  3.69  3.98  4.21  4.33   21.56   8
   -colorize 30%/20%/50%                                                  1.00  1.84  2.60  3.20  3.70  4.09  4.24  4.34   25.00   8
   -despeckle                                                             1.00  1.48  2.81  2.82  2.82  2.81  2.81  2.81    1.22   4
   -edge 0x1                                                              1.00  1.85  2.62  3.28  3.81  4.05  4.46  4.59   20.40   8
   -emboss 0x1                                                            1.00  1.76  2.41  2.94  3.31  3.68  3.89  4.07    9.72   8
   -enhance                                                               1.00  2.00  2.98  3.96  4.92  5.83  6.74  7.61    2.85   8
   -gaussian 0x0.5                                                        1.00  1.89  2.74  3.49  4.08  4.73  5.15  5.47   14.88   8
   -gaussian 0x1.0                                                        1.00  1.95  2.88  3.77  4.62  5.40  6.05  6.72    7.04   8
   -implode 0.5                                                           1.00  1.65  2.02  2.55  2.96  3.64  3.74  4.31    4.56   8
   -implode -1                                                            1.00  1.32  1.85  2.26  3.08  3.05  3.44  2.91    4.62   7
   -lat 10x10-5%                                                          1.00  1.95  2.87  3.72  4.50  5.25  5.93  6.49    4.09   8
   -median 1                                                              1.00  1.06  1.48  0.96  1.00  1.34  1.39  1.44    0.48   3
   -median 2                                                              1.00  0.80  1.72  1.16  1.12  1.53  1.56  1.95    0.23   8
   -minify                                                                1.00  1.83  2.46  2.88  3.16  3.29  3.24  3.21   32.53   6
   -modulate 110/100/95                                                   1.00  1.94  2.82  3.61  4.37  5.00  5.51  5.91   15.68   8
   -motion-blur 0x3+30                                                    1.00  1.78  2.50  3.30  4.09  4.96  5.82  6.32    3.20   8
   +noise Uniform                                                         1.00  1.83  2.45  2.93  3.32  3.60  3.83  3.95   11.90   8
   +noise Gaussian                                                        1.00  1.97  2.85  3.77  4.66  5.48  6.25  6.99    2.67   8
   +noise Multiplicative                                                  1.00  1.92  2.84  3.67  4.49  5.25  5.93  6.56    3.93   8
   +noise Impulse                                                         1.00  1.76  2.36  2.83  3.22  3.49  3.68  3.83   11.80   8
   +noise Laplacian                                                       1.00  1.91  2.73  3.45  4.13  4.70  5.20  5.63    6.60   8
   +noise Poisson                                                         1.00  1.95  2.85  3.70  4.51  5.25  5.92  6.59    4.00   8
   -noise 1                                                               1.00  0.60  1.48  0.91  1.08  1.28  1.61  1.49    0.50   7
   -noise 2                                                               1.00  0.80  1.68  1.13  1.14  1.70  1.63  1.93    0.22   8
   -fill blue -fuzz 35% -opaque red                                       1.00  1.59  1.97  2.08  2.03  1.92  1.83  1.67   47.70   4
   -operator all Add 2%                                                   1.00  1.62  1.93  2.06  2.15  2.03  1.97  1.85   41.92   5
   -operator all And 233                                                  1.00  1.25  1.37  1.38  1.39  1.36  1.30  1.21   41.92   5
   -operator all Assign 50%                                               1.00  1.25  1.35  1.38  1.38  1.31  1.27  1.21   41.83   5
   -operator all Depth 6                                                  1.00  1.28  1.39  1.41  1.43  1.38  1.30  1.24   40.60   5
   -operator all Divide 2                                                 1.00  1.61  1.98  2.07  2.21  2.07  2.03  1.92   41.83   5
   -operator all Gamma 0.7                                                1.00  1.17  1.24  1.26  1.25  1.22  1.19  1.14   27.40   4
   -operator all Negate 1.0                                               1.00  1.25  1.35  1.38  1.37  1.34  1.27  1.21   41.80   4
   -operator all LShift 2                                                 1.00  1.25  1.35  1.36  1.36  1.32  1.29  1.21   41.32   5
   -operator all Multiply 0.5                                             1.00  1.57  1.92  2.01  2.10  2.06  1.93  1.84   41.40   5
   -operator all Or 233                                                   1.00  1.25  1.37  1.38  1.36  1.35  1.29  1.20   41.80   4
   -operator all RShift 2                                                 1.00  1.24  1.35  1.38  1.37  1.32  1.26  1.21   41.72   4
   -operator all Subtract 10%                                             1.00  1.63  2.00  2.16  2.33  2.26  2.18  2.03   41.32   5
   -operator red Threshold 50%                                            1.00  1.24  1.38  1.39  1.39  1.34  1.29  1.21   42.03   5
   -operator gray Threshold 50%                                           1.00  1.36  1.49  1.50  1.51  1.45  1.41  1.33   41.40   5
   -operator all Threshold-White 80%                                      1.00  1.45  1.65  1.62  1.59  1.54  1.43  1.31   48.20   3
   -operator all Threshold-Black 10%                                      1.00  1.46  1.68  1.62  1.62  1.52  1.46  1.34   47.51   3
   -operator all Xor 233                                                  1.00  1.26  1.37  1.38  1.37  1.31  1.27  1.21   41.63   4
   -operator all Noise-Gaussian 30%                                       1.00  1.98  2.96  3.91  4.86  5.78  6.62  7.52    2.94   8
   -operator all Noise-Impulse 30%                                        1.00  1.97  2.81  3.54  4.26  4.77  5.23  5.58   18.56   8
   -operator all Noise-Laplacian 30%                                      1.00  1.98  2.92  3.79  4.66  5.49  6.18  6.84    8.35   8
   -operator all Noise-Multiplicative 30%                                 1.00  1.99  2.94  3.86  4.81  5.70  6.53  7.33    4.49   8
   -operator all Noise-Poisson 30%                                        1.00  1.98  2.95  3.87  4.80  5.67  6.52  7.33    4.58   8
   -operator all Noise-Uniform 30%                                        1.00  1.88  2.71  3.39  4.08  4.63  5.04  5.34   18.80   8
   -ordered-dither all 2x2                                                1.00  1.41  1.61  1.64  1.62  1.60  1.50  1.45   35.86   4
   -ordered-dither all 3x3                                                1.00  1.50  1.81  1.88  1.88  1.82  1.75  1.65   35.73   4
   -ordered-dither intensity 3x3                                          1.00  1.51  1.80  1.90  1.88  1.78  1.73  1.66   36.13   4
   -ordered-dither all 4x4                                                1.00  1.41  1.59  1.66  1.62  1.60  1.53  1.44   36.20   4
   -paint 0x1                                                             1.00  1.95  2.84  3.72  4.53  5.28  5.88  6.44    9.38   8
   -random-threshold all 20x80                                            1.00  1.72  2.30  2.68  3.00  3.10  3.13  3.04   32.27   7
   -density 75x75 -resample 50x50                                         1.00  1.89  2.67  3.34  3.76  4.21  4.23  4.25   12.05   8
   -resize 10%                                                            1.00  1.88  2.63  3.25  3.67  3.68  3.77  4.13   22.60   8
   -resize 50%                                                            1.00  1.86  2.60  3.22  2.50  3.96  3.99  4.09   14.77   8
   -resize 150%                                                           1.00  1.77  2.25  2.71  2.50  3.60  3.80  3.74    4.42   7
   -rotate 45                                                             1.00  1.25  1.49  1.67  1.84  1.93  2.03  2.05    1.25   8
   -segment 0.5x0.25                                                      1.00  1.54  1.90  2.15  2.33  2.48  2.58  2.67    0.13   8
   -shade 30x30                                                           1.00  1.91  2.76  3.53  4.27  4.87  5.34  5.69   13.74   8
   -sharpen 0x0.5                                                         1.00  1.89  2.74  3.49  4.06  4.75  5.14  5.47   14.88   8
   -sharpen 0x1.0                                                         1.00  1.95  2.88  3.77  4.63  5.39  6.08  6.70    7.02   8
   -shear 45x45                                                           1.00  0.97  1.15  1.24  1.36  1.42  1.46  1.46    1.52   8
   -solarize 50%                                                          1.00  1.25  1.36  1.40  1.40  1.33  1.29  1.22   42.00   5
   -swirl 90                                                              1.00  1.30  1.82  2.26  3.14  2.96  3.44  3.64    5.03   8
   -fuzz 35% -transparent red                                             1.00  1.57  1.86  1.96  1.99  1.93  1.80  1.72   41.83   5
   -trim                                                                  1.00  1.77  2.41  2.86  3.08  3.22  2.54  2.53   38.00   6
   -fuzz 5% -trim                                                         1.00  1.84  2.50  3.11  3.62  3.79  3.94  4.00   29.28   8
   -unsharp 0x0.5+20+1                                                    1.00  1.57  1.92  2.12  2.20  2.29  2.31  2.21    6.86   7
   -unsharp 0x1.0+20+1                                                    1.00  1.66  2.08  2.30  2.44  2.56  2.65  2.61    6.39   7
   -wave 25x150                                                           1.00  1.19  1.44  1.81  1.71  1.97  2.04  2.39    4.08   8
   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ===== ======= ====

Apple OS-X/IBM G5
-----------------

.. Last update: Sun May 31 11:03:45 CDT 2009

The following table shows the boost on a two core Apple PowerPC G5
system (2.5GHz) running OS-X Leopard:

.. table:: Performance Boost On Two Core PowerPC G5 System

   ============================================== ===== ===== ======= ====
   Operation                                        1     2   iter/s  thds
   ============================================== ===== ===== ======= ====
   -noop                                           1.00  1.01   23.70   2
   -affine 1,0,0.785,1,0,0 -transform              1.00  1.57    3.17   2
   -blur 0x0.5                                     1.00  1.63    1.72   2
   -blur 0x1.0                                     1.00  1.70    1.15   2
   -charcoal 0x1                                   1.00  1.76    0.56   2
   -colorspace CMYK                                1.00  1.12   15.84   2
   -colorspace GRAY                                1.00  1.41    9.04   2
   -colorspace HSL                                 1.00  1.72    5.59   2
   -colorspace HWB                                 1.00  1.70    5.45   2
   -colorspace OHTA                                1.00  1.46    9.43   2
   -colorspace YCbCr                               1.00  1.46    9.40   2
   -colorspace YIQ                                 1.00  1.43    9.27   2
   -colorspace YUV                                 1.00  1.46    9.38   2
   -contrast -contrast -contrast                   1.00  1.89    1.40   2
   +contrast +contrast +contrast                   1.00  1.91    1.44   2
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  1.81    2.56   2
   -colorize 30%/20%/50%                           1.00  1.38    8.58   2
   -despeckle                                      1.00  1.49    0.21   2
   -edge 0x1                                       1.00  1.77    2.55   2
   -emboss 0x1                                     1.00  1.81    1.13   2
   -enhance                                        1.00  1.91    0.64   2
   -gaussian 0x0.5                                 1.00  1.83    1.23   2
   -gaussian 0x1.0                                 1.00  1.90    0.45   2
   -implode 0.5                                    1.00  1.75    1.84   2
   -implode -1                                     1.00  1.80    2.46   2
   -lat 10x10-5%                                   1.00  1.91    0.44   2
   -median 1                                       1.00  1.89    0.64   2
   -median 2                                       1.00  1.93    0.25   2
   -minify                                         1.00  1.59    7.71   2
   -modulate 110/100/95                            1.00  1.78    4.00   2
   +noise Uniform                                  1.00  1.58    3.35   2
   +noise Gaussian                                 1.00  1.87    0.97   2
   +noise Multiplicative                           1.00  1.82    1.43   2
   +noise Impulse                                  1.00  1.63    3.29   2
   +noise Laplacian                                1.00  1.76    1.99   2
   +noise Poisson                                  1.00  1.71    1.40   2
   -noise 1                                        1.00  1.42    0.48   2
   -noise 2                                        1.00  1.92    0.26   2
   -fill blue -fuzz 35% -opaque red                1.00  1.26   15.48   2
   -operator all Add 2%                            1.00  1.57    7.80   2
   -operator all And 233                           1.00  0.99   18.65   1
   -operator all Assign 50%                        1.00  1.12   18.00   2
   -operator all Depth 6                           1.00  1.02   18.20   2
   -operator all Divide 2                          1.00  1.61    8.38   2
   -operator all Gamma 0.7                         1.00  1.00   15.08   1
   -operator all Negate 1.0                        1.00  1.01   19.05   2
   -operator all LShift 2                          1.00  1.00   18.69   1
   -operator all Multiply 0.5                      1.00  1.51    7.68   2
   -operator all Or 233                            1.00  1.03   18.80   2
   -operator all RShift 2                          1.00  1.03   18.89   2
   -operator all Subtract 10%                      1.00  1.59    7.86   2
   -operator red Threshold 50%                     1.00  0.98   18.56   1
   -operator gray Threshold 50%                    1.00  1.04   18.45   2
   -operator all Threshold-White 80%               1.00  1.09   19.64   2
   -operator all Threshold-Black 10%               1.00  1.12   19.92   2
   -operator all Xor 233                           1.00  1.01   18.49   2
   -operator all Noise-Gaussian 30%                1.00  1.88    1.03   2
   -operator all Noise-Impulse 30%                 1.00  1.67    4.00   2
   -operator all Noise-Laplacian 30%               1.00  1.88    2.26   2
   -operator all Noise-Multiplicative 30%          1.00  1.89    1.58   2
   -operator all Noise-Poisson 30%                 1.00  1.86    1.63   2
   -operator all Noise-Uniform 30%                 1.00  1.62    4.02   2
   -ordered-dither all 2x2                         1.00  1.07   14.94   2
   -ordered-dither all 3x3                         1.00  1.15   15.05   2
   -ordered-dither intensity 3x3                   1.00  1.10   14.37   2
   -ordered-dither all 4x4                         1.00  1.08   14.91   2
   -paint 0x1                                      1.00  1.52    3.86   2
   -random-threshold all 20x80                     1.00  1.27   11.71   2
   -recolor '1,0,0,0,1,0,0,0,1'                    1.00  0.96   23.90   1
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  1.66    6.93   2
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  1.69    6.17   2
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  1.71    6.21   2
   -density 75x75 -resample 50x50                  1.00  1.70    1.65   2
   -resize 10%                                     1.00  1.73    3.95   2
   -resize 50%                                     1.00  1.68    2.31   2
   -resize 150%                                    1.00  1.69    0.48   2
   -rotate 45                                      1.00  1.69    0.24   2
   -segment 0.5x0.25                               1.00  1.26    0.05   2
   -shade 30x30                                    1.00  1.52    5.42   2
   -sharpen 0x0.5                                  1.00  1.88    1.26   2
   -sharpen 0x1.0                                  1.00  1.89    0.45   2
   -shear 45x45                                    1.00  1.66    0.28   2
   -solarize 50%                                   1.00  1.11   18.81   2
   -swirl 90                                       1.00  1.84    2.12   2
   -fuzz 35% -transparent red                      1.00  1.30   15.64   2
   -trim                                           1.00  1.73    6.14   2
   -fuzz 5% -trim                                  1.00  1.76    4.52   2
   -unsharp 0x0.5+20+1                             1.00  1.64    1.46   2
   -unsharp 0x1.0+20+1                             1.00  1.73    1.04   2
   -wave 25x150                                    1.00  1.61    2.17   2
   ============================================== ===== ===== ======= ====

FreeBSD / Intel Xeon
--------------------

.. Last update: Sun May 31 11:02:13 CDT 2009

The following shows the performance boost on a 2003 vintage 2-CPU
hyperthreaded Intel Xeon system running at 2.4GHz.  The operating
system used is FreeBSD 7.0.  Due to the hyperthreading support, this
system thinks it has four CPUs even though it really only has two
cores.  This can lead to very strange results since sometimes it seems
that the first two threads allocated may be from the same CPU,
resulting in much less boost than expected, but obtaining full boost
with four threads.  While the threading on this system behaves quite
poorly for "fast" algorithms, it is clear that OpenMP works well for
"slow" algorithms, and some algorithms show clear benefit from
hyperthreading since otherwise the maximum performance gain would be
2.0 rather than up to 2.84:

.. table:: Performance Boost On Two CPU Xeon System

   ============================================== ===== ===== ===== ===== ======= ====
   Operation                                        1     2     3     4   iter/s  thds
   ============================================== ===== ===== ===== ===== ======= ====
   -noop                                           1.00  1.02  1.02  1.02   18.80   2
   -affine 1,0,0.785,1,0,0 -transform              1.00  1.28  1.31  1.31    2.51   4
   -blur 0x0.5                                     1.00  1.23  1.38  1.41    2.82   4
   -blur 0x1.0                                     1.00  1.31  1.48  1.52    2.44   4
   -charcoal 0x1                                   1.00  1.42  1.65  1.73    0.99   4
   -colorspace CMYK                                1.00  0.87  0.88  0.84   11.22   1
   -colorspace GRAY                                1.00  1.37  1.46  1.49    7.93   4
   -colorspace HSL                                 1.00  1.50  1.89  2.02    4.83   4
   -colorspace HWB                                 1.00  1.55  1.87  2.08    6.68   4
   -colorspace OHTA                                1.00  1.38  1.53  1.58    8.11   4
   -colorspace YCbCr                               1.00  1.43  1.55  1.59    8.05   4
   -colorspace YIQ                                 1.00  1.34  1.52  1.58    8.12   4
   -colorspace YUV                                 1.00  1.39  1.53  1.58    8.11   4
   -contrast -contrast -contrast                   1.00  1.69  2.27  2.58    1.21   4
   +contrast +contrast +contrast                   1.00  1.75  2.24  2.53    1.24   4
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  1.54  1.85  2.02    3.99   4
   -colorize 30%/20%/50%                           1.00  1.38  1.49  1.60    6.68   4
   -despeckle                                      1.00  1.32  2.11  2.12    0.38   4
   -edge 0x1                                       1.00  1.55  1.89  2.07    3.95   4
   -emboss 0x1                                     1.00  1.51  1.77  1.60    1.82   3
   -enhance                                        1.00  1.64  2.00  2.06    0.65   4
   -gaussian 0x0.5                                 1.00  1.63  2.00  2.17    2.62   4
   -gaussian 0x1.0                                 1.00  1.57  2.02  2.09    1.20   4
   -implode 0.5                                    1.00  1.69  2.15  2.37    1.24   4
   -implode -1                                     1.00  1.55  2.07  2.28    2.02   4
   -lat 10x10-5%                                   1.00  1.44  1.96  2.05    1.00   4
   -median 1                                       1.00  1.69  2.11  2.30    0.38   4
   -median 2                                       1.00  1.74  2.13  2.33    0.14   4
   -minify                                         1.00  1.37  1.61  1.68    8.09   4
   -modulate 110/100/95                            1.00  1.57  2.01  2.21    4.00   4
   +noise Uniform                                  1.00  1.42  1.71  1.79    2.69   4
   +noise Gaussian                                 1.00  1.80  2.31  2.74    0.94   4
   +noise Multiplicative                           1.00  1.79  2.44  2.93    1.23   4
   +noise Impulse                                  1.00  1.44  1.67  1.70    2.72   4
   +noise Laplacian                                1.00  1.62  2.06  2.33    1.94   4
   +noise Poisson                                  1.00  1.78  2.28  2.61    1.10   4
   -noise 1                                        1.00  1.71  2.09  2.31    0.37   4
   -noise 2                                        1.00  1.79  2.13  2.33    0.14   4
   -fill blue -fuzz 35% -opaque red                1.00  1.20  1.31  1.33   13.67   4
   -operator all Add 2%                            1.00  1.22  1.36  1.39   12.44   4
   -operator all And 233                           1.00  1.01  1.01  0.99   13.49   2
   -operator all Assign 50%                        1.00  1.01  1.01  1.01   13.54   2
   -operator all Depth 6                           1.00  1.00  1.02  1.02   13.23   3
   -operator all Divide 2                          1.00  1.31  1.50  1.48    9.09   3
   -operator all Gamma 0.7                         1.00  0.96  0.96  0.98    8.27   1
   -operator all Negate 1.0                        1.00  1.02  1.02  0.99   12.36   2
   -operator all LShift 2                          1.00  1.02  1.05  1.02   12.44   3
   -operator all Multiply 0.5                      1.00  1.19  1.31  1.36   11.31   4
   -operator all Or 233                            1.00  1.04  1.05  1.04   12.60   3
   -operator all RShift 2                          1.00  1.01  0.99  1.02   12.60   4
   -operator all Subtract 10%                      1.00  1.18  1.35  1.35   11.16   4
   -operator red Threshold 50%                     1.00  1.01  0.98  0.99   12.40   2
   -operator gray Threshold 50%                    1.00  1.03  1.02  1.03   12.60   4
   -operator all Threshold-White 80%               1.00  1.03  1.10  1.10   14.33   3
   -operator all Threshold-Black 10%               1.00  1.13  1.25  1.26   15.18   4
   -operator all Xor 233                           1.00  1.01  0.99  1.00   13.61   2
   -operator all Noise-Gaussian 30%                1.00  1.88  2.45  2.91    1.01   4
   -operator all Noise-Impulse 30%                 1.00  1.61  1.85  1.88    3.49   4
   -operator all Noise-Laplacian 30%               1.00  1.75  2.36  2.76    2.33   4
   -operator all Noise-Multiplicative 30%          1.00  1.85  2.55  3.11    1.36   4
   -operator all Noise-Poisson 30%                 1.00  1.82  2.44  2.88    1.23   4
   -operator all Noise-Uniform 30%                 1.00  1.49  1.90  1.96    3.38   4
   -ordered-dither all 2x2                         1.00  0.91  0.89  0.84   10.97   1
   -ordered-dither all 3x3                         1.00  0.99  1.04  1.01    9.79   3
   -ordered-dither intensity 3x3                   1.00  0.98  1.05  1.00    9.92   3
   -ordered-dither all 4x4                         1.00  0.91  0.89  0.84   10.97   1
   -paint 0x1                                      1.00  1.58  2.05  2.24    1.87   4
   -random-threshold all 20x80                     1.00  1.43  1.74  1.81    7.97   4
   -recolor '1,0,0,0,1,0,0,0,1'                    1.00  1.00  1.00  1.00   18.54   3
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  1.24  1.52  1.57   10.52   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  1.27  1.55  1.58    9.24   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  1.25  1.55  1.60    9.38   4
   -density 75x75 -resample 50x50                  1.00  1.46  1.70  1.81    1.79   4
   -resize 10%                                     1.00  1.41  1.45  1.64    3.91   4
   -resize 50%                                     1.00  1.47  1.59  1.66    2.37   4
   -resize 150%                                    1.00  1.48  1.72  1.74    0.57   4
   -rotate 45                                      1.00  1.23  1.39  1.43    0.32   4
   -segment 0.5x0.25                               1.00  1.44  1.50  1.50    0.07   3
   -shade 30x30                                    1.00  1.69  2.23  2.61    3.69   4
   -sharpen 0x0.5                                  1.00  1.62  2.00  2.16    2.60   4
   -sharpen 0x1.0                                  1.00  1.69  2.01  2.10    1.20   4
   -shear 45x45                                    1.00  1.18  1.32  1.34    0.37   4
   -solarize 50%                                   1.00  1.01  0.99  1.01   13.49   4
   -swirl 90                                       1.00  1.65  2.16  2.43    1.98   4
   -fuzz 35% -transparent red                      1.00  1.18  1.29  1.30   13.20   4
   -trim                                           1.00  0.94  1.12  1.31    5.98   4
   -fuzz 5% -trim                                  1.00  1.33  1.70  1.81    4.75   4
   -unsharp 0x0.5+20+1                             1.00  1.33  1.54  1.61    2.40   4
   -unsharp 0x1.0+20+1                             1.00  1.34  1.60  1.67    2.10   4
   -wave 25x150                                    1.00  1.39  1.83  1.92    1.80   4
   ============================================== ===== ===== ===== ===== ======= ====

Windows XP / MSVC / Intel Core 2 Quad
-------------------------------------

.. Last update: Sun May 31 13:01:10 CDT 2009

This system is Windows XP Professional (SP3) using the Visual Studio
2008 compiler and a Q8 build.  The system CPU is a 2.83 GHz Core 2
Quad Processor (Q9550).  This processor is a multi-chip module (MCM)
based on two Core 2 CPUs bonded to a L3 cache in the same chip
package.

The following shows the performance boost for a Q8 build:

.. table:: Performance Boost on an Intel Core 2 Quad (Q9550) system

   ============================================== ===== ===== ===== ===== ======= ====
   Operation                                        1     2     3     4   iter/s  thds
   ============================================== ===== ===== ===== ===== ======= ====
   -noop                                           1.00  1.00  0.99  0.96   73.60   1
   -affine 1,0,0.785,1,0,0 -transform              1.00  1.72  1.54  1.78    8.40   4
   -blur 0x0.5                                     1.00  1.67  2.10  2.40    8.61   4
   -blur 0x1.0                                     1.00  1.72  2.27  2.58    7.53   4
   -charcoal 0x1                                   1.00  1.73  2.31  2.68    3.21   4
   -colorspace CMYK                                1.00  1.06  1.09  1.15   43.20   4
   -colorspace GRAY                                1.00  1.69  1.79  1.82   18.54   4
   -colorspace HSL                                 1.00  1.91  2.71  3.44   17.09   4
   -colorspace HWB                                 1.00  1.81  2.49  3.04   20.74   4
   -colorspace OHTA                                1.00  1.67  1.81  1.84   18.74   4
   -colorspace YCbCr                               1.00  1.68  1.88  1.83   19.08   3
   -colorspace YIQ                                 1.00  1.69  1.87  1.85   19.00   3
   -colorspace YUV                                 1.00  1.69  1.89  1.83   19.20   3
   -contrast -contrast -contrast                   1.00  1.97  2.88  3.75    4.04   4
   +contrast +contrast +contrast                   1.00  1.97  2.89  3.80    4.08   4
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  1.71  2.48  3.07   14.95   4
   -colorize 30%/20%/50%                           1.00  1.70  2.26  2.61   22.73   4
   -despeckle                                      1.00  1.46  2.78  2.79    0.89   4
   -edge 0x1                                       1.00  1.84  2.71  3.43    8.97   4
   -emboss 0x1                                     1.00  1.75  2.41  2.95    7.48   4
   -enhance                                        1.00  1.97  2.92  3.83    2.83   4
   -gaussian 0x0.5                                 1.00  1.88  2.66  3.40    9.45   4
   -gaussian 0x1.0                                 1.00  1.99  2.89  3.75    4.53   4
   -implode 0.5                                    1.00  1.91  2.80  3.61    6.85   4
   -implode -1                                     1.00  1.91  2.82  3.53    6.70   4
   -lat 10x10-5%                                   1.00  1.88  2.66  3.36    4.42   4
   -median 1                                       1.00  1.92  2.75  3.63    1.51   4
   -median 2                                       1.00  1.97  2.88  3.84    0.56   4
   -minify                                         1.00  1.65  2.18  2.61   31.60   4
   -modulate 110/100/95                            1.00  1.88  2.65  3.33   14.15   4
   +noise Uniform                                  1.00  1.68  2.18  2.57   13.40   4
   +noise Gaussian                                 1.00  1.93  2.83  3.63    3.27   4
   +noise Multiplicative                           1.00  1.90  2.74  3.49    4.56   4
   +noise Impulse                                  1.00  1.70  2.22  2.61   12.64   4
   +noise Laplacian                                1.00  1.74  2.60  3.24    7.61   4
   +noise Poisson                                  1.00  1.90  2.71  3.44    4.97   4
   -noise 1                                        1.00  1.91  2.74  3.53    1.49   4
   -noise 2                                        1.00  2.03  2.98  3.92    0.56   4
   -fill blue -fuzz 35% -opaque red                1.00  1.34  1.51  1.62   56.00   4
   -operator all Add 2%                            1.00  1.71  2.20  2.51   29.40   4
   -operator all And 233                           1.00  1.04  1.08  1.08   48.84   3
   -operator all Assign 50%                        1.00  1.08  1.06  1.03   49.44   2
   -operator all Depth 6                           1.00  1.03  1.10  1.14   49.40   4
   -operator all Divide 2                          1.00  1.76  2.26  2.64   30.10   4
   -operator all Gamma 0.7                         1.00  1.01  1.07  1.10   40.20   4
   -operator all Negate 1.0                        1.00  1.08  1.07  1.04   49.40   2
   -operator all LShift 2                          1.00  1.05  1.11  1.12   49.40   4
   -operator all Multiply 0.5                      1.00  1.70  2.20  2.63   30.60   4
   -operator all Or 233                            1.00  1.02  1.08  1.07   49.24   3
   -operator all RShift 2                          1.00  1.08  1.09  1.06   48.80   3
   -operator all Subtract 10%                      1.00  1.71  2.22  2.71   30.60   4
   -operator red Threshold 50%                     1.00  1.05  1.06  1.07   48.60   4
   -operator gray Threshold 50%                    1.00  0.97  1.06  1.13   51.00   4
   -operator all Threshold-White 80%               1.00  1.13  1.10  1.05   56.60   2
   -operator all Threshold-Black 10%               1.00  1.12  1.15  1.16   55.20   4
   -operator all Xor 233                           1.00  1.06  1.05  1.04   48.60   2
   -operator all Noise-Gaussian 30%                1.00  1.97  2.91  3.80    3.61   4
   -operator all Noise-Impulse 30%                 1.00  1.86  2.59  3.21   17.29   4
   -operator all Noise-Laplacian 30%               1.00  1.92  2.78  3.58    9.11   4
   -operator all Noise-Multiplicative 30%          1.00  1.96  2.88  3.75    5.13   4
   -operator all Noise-Poisson 30%                 1.00  1.96  2.87  3.74    5.66   4
   -operator all Noise-Uniform 30%                 1.00  1.85  2.53  3.13   18.20   4
   -ordered-dither all 2x2                         1.00  1.06  1.06  1.12   42.00   4
   -ordered-dither all 3x3                         1.00  1.20  1.18  1.18   41.00   2
   -ordered-dither intensity 3x3                   1.00  1.16  1.14  1.18   41.00   4
   -ordered-dither all 4x4                         1.00  1.11  1.08  1.09   41.28   2
   -paint 0x1                                      1.00  1.94  2.84  3.68    4.83   4
   -random-threshold all 20x80                     1.00  1.60  1.97  2.20   34.69   4
   -recolor '1,0,0,0,1,0,0,0,1'                    1.00  1.02  1.01  1.01   73.00   2
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  1.69  2.19  2.59   30.91   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  1.77  2.39  2.91   24.73   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  1.81  2.41  2.91   24.40   4
   -density 75x75 -resample 50x50                  1.00  1.80  2.15  2.45   10.11   4
   -resize 10%                                     1.00  1.83  2.39  2.76   23.45   4
   -resize 50%                                     1.00  1.85  2.18  2.52   13.12   4
   -resize 150%                                    1.00  1.89  2.23  2.69    4.43   4
   -rotate 45                                      1.00  1.61  1.91  2.11    1.93   4
   -segment 0.5x0.25                               1.00  1.42  1.64  1.77    0.17   4
   -shade 30x30                                    1.00  1.72  2.16  2.59   22.26   4
   -sharpen 0x0.5                                  1.00  1.88  2.66  3.35    9.31   4
   -sharpen 0x1.0                                  1.00  1.99  2.89  3.78    4.57   4
   -shear 45x45                                    1.00  1.37  1.58  1.66    2.66   4
   -solarize 50%                                   1.00  1.05  1.14  1.12   48.45   3
   -swirl 90                                       1.00  1.92  2.77  3.57    7.48   4
   -fuzz 35% -transparent red                      1.00  1.33  1.41  1.52   49.00   4
   -trim                                           1.00  1.57  1.92  2.12   45.40   4
   -fuzz 5% -trim                                  1.00  1.74  2.30  2.85   31.10   4
   -unsharp 0x0.5+20+1                             1.00  1.71  2.25  2.63    7.48   4
   -unsharp 0x1.0+20+1                             1.00  1.73  2.33  2.71    6.63   4
   -wave 25x150                                    1.00  1.82  2.44  2.79    6.56   4
   ============================================== ===== ===== ===== ===== ======= ====

Windows XP / MinGW / Intel Core 2 Quad
--------------------------------------

.. Last update: Sun May 31 12:00:36 CDT 2009

TDM's GCC/mingw32 build 1.902.0 was downloaded from
http://www.tdragon.net/recentgcc/ and installed on the same Windows XP
system with the 2.83 GHz Core 2 Quad Processor (Q9550) as described
above.  This processor is a multi-chip module (MCM) based on two Core
2 CPUs bonded to a L3 cache in the same chip package.

After several false tries, GraphicsMagick was successfully built as a
static executable using OpenMP.  Given that the compiler is an
experimental distribution and that GraphicsMagick had not been tested
with OpenMP on MinGW before, problems were expected.  However, there
were no execution problems and performance ended up being quite nice.

The following shows the performance boost for a Q8 build:

.. table:: Performance Boost on an Intel Core 2 Quad (Q9550) system

   ============================================== ===== ===== ===== ===== ======= ====
   Operation                                        1     2     3     4   iter/s  thds
   ============================================== ===== ===== ===== ===== ======= ====
   -noop                                           1.00  1.01  0.97  1.00   75.20   2
   -affine 1,0,0.785,1,0,0 -transform              1.00  1.74  1.48  1.68    9.06   2
   -blur 0x0.5                                     1.00  1.45  1.67  1.82   11.10   4
   -blur 0x1.0                                     1.00  1.56  1.86  2.01    9.25   4
   -charcoal 0x1                                   1.00  1.62  1.97  2.21    4.25   4
   -colorspace CMYK                                1.00  1.11  1.08  1.10   42.20   2
   -colorspace GRAY                                1.00  1.39  1.13  1.04   32.90   2
   -colorspace HSL                                 1.00  1.91  2.62  3.25   19.08   4
   -colorspace HWB                                 1.00  1.81  2.44  2.97   26.00   4
   -colorspace OHTA                                1.00  1.41  1.08  1.03   33.09   2
   -colorspace YCbCr                               1.00  1.43  1.14  1.05   32.90   2
   -colorspace YIQ                                 1.00  1.40  1.08  1.03   33.10   2
   -colorspace YUV                                 1.00  1.39  1.13  1.03   32.90   2
   -contrast -contrast -contrast                   1.00  2.31  3.55  4.72    4.25   4
   +contrast +contrast +contrast                   1.00  2.27  3.49  4.63    4.31   4
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  1.72  2.22  2.52   20.54   4
   -colorize 30%/20%/50%                           1.00  1.67  2.00  2.39   30.40   4
   -despeckle                                      1.00  1.47  2.59  2.60    1.58   4
   -edge 0x1                                       1.00  1.84  2.54  3.14   13.04   4
   -emboss 0x1                                     1.00  1.71  2.28  2.62    9.91   4
   -enhance                                        1.00  2.06  3.08  4.09    3.15   4
   -gaussian 0x0.5                                 1.00  1.82  2.51  3.07   13.52   4
   -gaussian 0x1.0                                 1.00  1.89  2.77  3.61    5.96   4
   -implode 0.5                                    1.00  2.07  3.03  4.00    5.25   4
   -implode -1                                     1.00  2.10  3.02  3.97    5.20   4
   -lat 10x10-5%                                   1.00  1.86  2.73  3.48    4.36   4
   -median 1                                       1.00  1.97  2.87  3.74    1.65   4
   -median 2                                       1.00  1.99  2.97  3.91    0.63   4
   -minify                                         1.00  1.65  2.01  2.33   36.09   4
   -modulate 110/100/95                            1.00  1.90  2.68  3.33   16.05   4
   +noise Uniform                                  1.00  1.70  2.21  2.58   12.80   4
   +noise Gaussian                                 1.00  2.88  4.65  6.31    2.63   4
   +noise Multiplicative                           1.00  2.57  4.01  5.28    3.82   4
   +noise Impulse                                  1.00  1.67  2.19  2.50   13.47   4
   +noise Laplacian                                1.00  2.62  4.01  5.17    6.38   4
   +noise Poisson                                  1.00  1.97  2.89  3.74    3.08   4
   -noise 1                                        1.00  1.96  2.82  3.70    1.67   4
   -noise 2                                        1.00  2.01  3.01  3.98    0.64   4
   -fill blue -fuzz 35% -opaque red                1.00  1.31  1.48  1.59   55.03   4
   -operator all Add 2%                            1.00  1.37  1.59  1.61   47.20   4
   -operator all And 233                           1.00  1.10  1.10  1.12   50.00   4
   -operator all Assign 50%                        1.00  1.03  1.13  1.08   51.60   3
   -operator all Depth 6                           1.00  1.09  1.11  1.12   49.00   4
   -operator all Divide 2                          1.00  1.45  1.68  1.74   47.20   4
   -operator all Gamma 0.7                         1.00  1.09  1.10  1.11   36.80   4
   -operator all Negate 1.0                        1.00  1.04  1.13  1.09   51.60   3
   -operator all LShift 2                          1.00  1.06  1.10  1.12   50.00   4
   -operator all Multiply 0.5                      1.00  1.39  1.62  1.68   47.25   4
   -operator all Or 233                            1.00  1.07  1.08  1.10   50.40   4
   -operator all RShift 2                          1.00  1.01  1.13  1.09   51.20   3
   -operator all Subtract 10%                      1.00  1.36  1.58  1.71   47.20   4
   -operator red Threshold 50%                     1.00  1.07  1.14  1.09   51.63   3
   -operator gray Threshold 50%                    1.00  1.08  1.06  1.10   50.00   4
   -operator all Threshold-White 80%               1.00  1.13  1.17  1.12   59.40   3
   -operator all Threshold-Black 10%               1.00  1.14  1.14  1.16   55.80   4
   -operator all Xor 233                           1.00  1.05  1.11  1.08   50.20   3
   -operator all Noise-Gaussian 30%                1.00  2.91  4.76  6.57    2.80   4
   -operator all Noise-Impulse 30%                 1.00  1.84  2.56  3.12   19.00   4
   -operator all Noise-Laplacian 30%               1.00  2.72  4.28  5.71    7.35   4
   -operator all Noise-Multiplicative 30%          1.00  2.63  4.19  5.67    4.19   4
   -operator all Noise-Poisson 30%                 1.00  1.97  2.92  3.83    3.32   4
   -operator all Noise-Uniform 30%                 1.00  1.85  2.56  3.13   17.55   4
   -ordered-dither all 2x2                         1.00  1.03  1.03  1.06   40.60   4
   -ordered-dither all 3x3                         1.00  1.09  1.14  1.12   41.00   3
   -ordered-dither intensity 3x3                   1.00  1.08  1.09  1.11   39.80   4
   -ordered-dither all 4x4                         1.00  1.03  1.06  1.06   40.48   3
   -paint 0x1                                      1.00  1.93  2.79  3.60    5.87   4
   -random-threshold all 20x80                     1.00  1.55  1.95  2.10   35.40   4
   -recolor '1,0,0,0,1,0,0,0,1'                    1.00  1.03  1.03  1.03   74.20   2
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  1.74  2.35  2.75   27.40   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  1.63  2.10  2.43   33.70   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  1.70  2.18  2.46   33.49   4
   -density 75x75 -resample 50x50                  1.00  1.58  2.04  2.24   10.50   4
   -resize 10%                                     1.00  1.78  2.43  2.91   22.26   4
   -resize 50%                                     1.00  1.63  2.10  2.40   13.36   4
   -resize 150%                                    1.00  1.60  2.06  2.36    5.18   4
   -rotate 45                                      1.00  1.53  1.91  2.11    1.88   4
   -segment 0.5x0.25                               1.00  1.48  1.74  1.89    0.18   4
   -shade 30x30                                    1.00  1.81  2.51  3.12   14.46   4
   -sharpen 0x0.5                                  1.00  1.83  2.55  3.04   13.36   4
   -sharpen 0x1.0                                  1.00  1.91  2.75  3.57    5.92   4
   -shear 45x45                                    1.00  1.38  1.65  1.68    2.54   4
   -solarize 50%                                   1.00  1.11  1.10  1.10   49.80   2
   -swirl 90                                       1.00  2.18  3.34  4.30    5.92   4
   -fuzz 35% -transparent red                      1.00  1.29  1.44  1.51   48.60   4
   -trim                                           1.00  1.68  2.18  2.44   37.89   4
   -fuzz 5% -trim                                  1.00  1.78  2.36  2.87   28.91   4
   -unsharp 0x0.5+20+1                             1.00  1.52  1.86  2.00    9.80   4
   -unsharp 0x1.0+20+1                             1.00  1.60  2.01  2.25    8.64   4
   -wave 25x150                                    1.00  1.85  2.57  2.92    5.43   4
   ============================================== ===== ===== ===== ===== ======= ====

Windows Vista 64-bit / Phenom X4 9550
-------------------------------------

The following shows the performance of 32-bit GraphicsMagick running
on a Window Vista 64-bit system with the AMD Phenom X4 9550 (2200 MHz)
quad-core CPU chip.  For reasons which are not clear, the performance
boost is often over 5X on this four core system.  Testing with
somewhat different input showed performance boost of over 6X.  Since
the performance boost is based on timings with just one thread,
perhaps there are latency issues when running with one thread which
are nullified with many threads.

.. table:: Performance Boost on an AMD Phenom system

   ===================================================================== ===== ===== ===== ===== ======= ====
   Operation                                                               1     2     3     4   iter/s  thds
   ===================================================================== ===== ===== ===== ===== ======= ====
   -noop                                                                  1.00  1.23  1.11  1.08  134.38   2
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  2.32  3.46  4.14   15.68   4
   -black-threshold 20%                                                   1.00  1.62  1.88  1.91   83.47   4
   -blur 0x0.5                                                            1.00  1.86  2.74  3.32   10.75   4
   -blur 0x1.0                                                            1.00  2.02  3.06  3.88    8.23   4
   -border 6x6                                                            1.00  1.18  1.10  1.05   63.91   2
   -channel red                                                           1.00  1.53  1.53  1.46   90.06   3
   -charcoal 0x1                                                          1.00  2.26  3.55  4.35    3.91   4
   -chop 800x600+200+300                                                  1.00  1.34  1.35  1.32   92.67   3
   -colors 16                                                             1.00  1.12  1.09  1.09    0.66   2
   -colorspace CMYK                                                       1.00  1.79  1.99  2.06   75.68   4
   -colorspace GRAY                                                       1.00  2.06  2.80  3.06   58.71   4
   -colorspace HSL                                                        1.00  2.19  3.39  4.22   24.68   4
   -colorspace HWB                                                        1.00  2.01  3.04  3.73   27.56   4
   -colorspace OHTA                                                       1.00  2.03  2.80  3.13   61.10   4
   -colorspace YCbCr                                                      1.00  1.90  2.60  2.91   60.72   4
   -colorspace YIQ                                                        1.00  2.10  2.71  3.00   59.92   4
   -colorspace YUV                                                        1.00  1.86  2.47  2.76   60.50   4
   -contrast -contrast -contrast                                          1.00  2.28  3.79  5.11    4.33   4
   +contrast +contrast +contrast                                          1.00  2.52  4.15  5.60    4.38   4
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  2.22  3.46  4.59   11.38   4
   -colorize 30%/20%/50%                                                  1.00  2.38  3.40  4.04   37.95   4
   -crop 1700x900+100+100                                                 1.00  1.12  1.14  1.07   69.10   3
   -cycle 200                                                             1.00  1.01  1.05  1.01    0.53   3
   -despeckle                                                             1.00  1.91  3.79  3.92    1.36   4
   -fill none -stroke gold -draw 'circle 800,500 1100,800'                1.00  1.03  0.97  1.00    5.36   2
   -fill green -stroke gold -draw 'circle 800,500 1100,800'               1.00  1.16  1.13  1.08    4.36   2
   -fill none -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.02  0.87  0.96   10.78   2
   -fill blue -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.01  0.80  0.98    9.10   2
   -fill none -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  0.85  0.96  0.88   14.94   1
   -fill blue -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.02  0.93  0.95   10.78   2
   -fill none -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  0.95  1.00  0.90    9.92   1
   -fill blue -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.06  1.04  0.98    9.44   2
   -fill none -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.09  1.03  1.06   18.37   2
   -fill blue -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.14  1.13  1.11   18.60   2
   -edge 0x1                                                              1.00  2.22  3.52  4.68   10.65   4
   -emboss 0x1                                                            1.00  1.93  3.07  3.88    8.51   4
   -enhance                                                               1.00  2.34  3.92  5.39    1.28   4
   -equalize                                                              1.00  1.31  1.50  1.65   25.28   4
   -flip                                                                  1.00  1.26  1.15  1.11   65.90   2
   -flop                                                                  1.00  1.36  1.36  1.27   61.10   2
   -frame 15x15+3+3                                                       1.00  1.23  1.16  1.08   62.51   2
   -gamma 1.6                                                             1.00  1.80  2.36  2.61   68.70   4
   -gaussian 0x0.5                                                        1.00  2.25  3.84  5.00   11.44   4
   -gaussian 0x1.0                                                        1.00  2.21  3.63  4.85    3.58   4
   -implode 0.5                                                           1.00  2.20  3.08  4.31    5.50   4
   -implode -1                                                            1.00  2.14  2.95  4.39    6.06   4
   -lat 10x10-5%                                                          1.00  2.22  3.54  4.90    3.04   4
   -level 10%,1.2,90%                                                     1.00  1.86  2.45  2.63   67.89   4
   -magnify                                                               1.00  0.95  0.96  0.98    3.48   1
   -map netscape:                                                         1.00  1.02  1.04  0.99    0.76   3
   -median 1                                                              1.00  1.46  2.80  3.59    0.69   4
   -median 2                                                              1.00  2.15  2.28  3.10    0.24   4
   -minify                                                                1.00  2.08  3.03  3.71   36.63   4
   -modulate 110/100/95                                                   1.00  2.35  3.67  4.87   16.18   4
   -monochrome                                                            1.00  1.05  1.05  1.05    0.55   2
   -motion-blur 0x3+30                                                    1.00  2.05  3.55  4.84    2.68   4
   -negate                                                                1.00  1.41  1.52  1.49   87.86   3
   +noise Uniform                                                         1.00  2.06  3.16  4.13    7.99   4
   +noise Gaussian                                                        1.00  2.18  3.75  4.97    2.73   4
   +noise Multiplicative                                                  1.00  2.32  3.77  5.11    3.24   4
   +noise Impulse                                                         1.00  2.32  3.78  4.76    8.53   4
   +noise Laplacian                                                       1.00  2.05  3.40  4.39    5.13   4
   +noise Poisson                                                         1.00  2.27  3.69  5.04    2.75   4
   -noise 1                                                               1.00  2.01  2.66  3.29    0.62   4
   -noise 2                                                               1.00  1.99  2.42  3.34    0.26   4
   -normalize                                                             1.00  1.39  1.46  1.65   23.69   4
   -fill blue -fuzz 35% -opaque red                                       1.00  1.82  2.39  2.70   67.29   4
   -operator all Add 2%                                                   1.00  2.05  2.79  3.27   54.71   4
   -operator all And 233                                                  1.00  1.53  1.67  1.65   87.26   3
   -operator all Assign 50%                                               1.00  1.55  1.55  1.51   88.66   3
   -operator all Depth 6                                                  1.00  1.98  2.71  3.00   68.29   4
   -operator all Divide 2                                                 1.00  2.17  3.18  4.03   34.64   4
   -operator all Gamma 0.7                                                1.00  1.85  2.35  2.60   68.11   4
   -operator all Negate 1.0                                               1.00  1.58  1.67  1.63   88.88   3
   -operator all LShift 2                                                 1.00  1.52  1.63  1.63   86.46   3
   -operator all Multiply 0.5                                             1.00  2.01  2.79  3.29   54.91   4
   -operator all Or 233                                                   1.00  1.38  1.51  1.50   88.48   3
   -operator all RShift 2                                                 1.00  1.62  1.70  1.67   87.08   3
   -operator all Subtract 10%                                             1.00  1.93  2.64  3.08   52.72   4
   -operator red Threshold 50%                                            1.00  1.29  1.36  1.33   90.26   3
   -operator gray Threshold 50%                                           1.00  1.52  1.66  1.68   86.06   4
   -operator all Threshold-White 80%                                      1.00  1.64  1.87  1.87   87.46   3
   -operator all Threshold-Black 10%                                      1.00  1.63  1.81  1.87   88.26   4
   -operator all Xor 233                                                  1.00  1.54  1.63  1.60   88.66   3
   -operator all Noise-Gaussian 30%                                       1.00  2.35  3.97  5.30    2.85   4
   -operator all Noise-Impulse 30%                                        1.00  2.17  3.53  4.62    9.56   4
   -operator all Noise-Laplacian 30%                                      1.00  2.10  3.48  4.62    5.51   4
   -operator all Noise-Multiplicative 30%                                 1.00  2.23  3.66  5.01    3.37   4
   -operator all Noise-Poisson 30%                                        1.00  2.37  3.99  5.37    2.84   4
   -operator all Noise-Uniform 30%                                        1.00  2.38  3.89  5.07    8.71   4
   -ordered-dither all 2x2                                                1.00  1.85  2.20  2.33   68.09   4
   -ordered-dither all 3x3                                                1.00  1.86  2.34  2.49   67.09   4
   -ordered-dither intensity 3x3                                          1.00  1.78  2.29  2.45   66.49   4
   -ordered-dither all 4x4                                                1.00  1.77  2.15  2.26   67.50   4
   -paint 0x1                                                             1.00  1.95  3.18  4.23    7.82   4
   -raise 10x10                                                           1.00  1.29  1.18  1.14  115.22   2
   -random-threshold all 20x80                                            1.00  1.87  3.23  3.94   27.98   4
   -density 75x75 -resample 50x50                                         1.00  1.88  3.02  4.15   11.98   4
   -resize 10%                                                            1.00  2.10  3.98  5.26   22.77   4
   -resize 50%                                                            1.00  1.94  3.15  3.97   15.83   4
   -resize 150%                                                           1.00  1.99  3.25  4.23    5.42   4
   -roll +20+10                                                           1.00  1.11  1.09  1.05   58.32   2
   -rotate 0                                                              1.00  1.27  1.23  1.20   62.11   2
   -rotate 45                                                             1.00  1.90  2.97  3.52    2.18   4
   -rotate 90                                                             1.00  1.24  1.13  1.15   41.81   2
   -rotate 180                                                            1.00  1.21  1.17  1.10   64.10   2
   -rotate 270                                                            1.00  1.20  1.11  1.12   41.33   2
   -sample 5%                                                             1.00  1.16  1.07  1.01  128.00   2
   -sample 50%                                                            1.00  1.02  1.08  0.99   77.49   3
   -sample 150%                                                           1.00  0.97  0.95  0.97   23.77   1
   -segment 0.5x0.25                                                      1.00  1.35  1.59  1.71    0.11   4
   -scale 5%                                                              1.00  0.97  0.98  1.03   18.85   4
   -scale 50%                                                             1.00  0.89  0.87  0.85   12.43   1
   -scale 150%                                                            1.00  1.00  1.00  1.00    4.43   4
   -shade 30x30                                                           1.00  2.08  3.23  4.07   19.70   4
   -sharpen 0x0.5                                                         1.00  2.09  3.58  4.68   11.44   4
   -sharpen 0x1.0                                                         1.00  1.97  3.37  4.54    3.59   4
   -shave 10x10                                                           1.00  1.19  1.13  1.08   64.70   2
   -shear 45x45                                                           1.00  1.92  2.81  3.15    3.19   4
   -solarize 50%                                                          1.00  1.70  1.87  1.86   82.07   3
   -spread 1                                                              1.00  1.61  2.01  2.15   54.91   4
   -spread 3                                                              1.00  1.73  2.20  2.34   52.56   4
   -swirl 90                                                              1.00  2.10  2.87  3.35    5.60   4
   -threshold 35%                                                         1.00  1.62  2.03  2.21   69.29   4
   -fuzz 35% -transparent red                                             1.00  1.76  2.36  2.64   65.10   4
   -trim                                                                  1.00  2.19  3.15  3.91   46.59   4
   -fuzz 5% -trim                                                         1.00  2.12  3.41  4.25   22.63   4
   -unsharp 0x0.5+20+1                                                    1.00  1.96  2.86  3.58    9.05   4
   -unsharp 0x1.0+20+1                                                    1.00  1.96  2.90  3.63    7.21   4
   -wave 25x150                                                           1.00  1.79  2.52  3.07    4.69   4
   -white-threshold 80%                                                   1.00  1.59  1.81  1.82   87.08   4
   ===================================================================== ===== ===== ===== ===== ======= ====

--------------------------------------------------------------------------

| Copyright (C) 2008 - 2009 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

