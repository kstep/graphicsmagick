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

The following table shows the performance boost in GraphicsMagick
1.4 as threads are added on a four-core AMD Opteron 3.0GHz system
running Sun Solaris 10:

.. table:: Performance Boost On Four Core AMD Operon System

   ====================================== ===== ===== ===== ===== ======= ====
   Operation                                1     2     3     4   iter/s  thds
   ====================================== ===== ===== ===== ===== ======= ====
   -noop                                   1.00  1.20  1.39  1.45  176.20   4
   -affine 1,0,0.785,1,0,0 -transform      1.00  1.91  2.81  3.48   17.06   4
   -blur 0x0.5                             1.00  1.75  2.25  2.44   11.64   4
   -blur 0x1.0                             1.00  1.80  2.43  2.79    8.88   4
   -charcoal 0x1                           1.00  1.81  2.48  2.94    4.10   4
   -colorspace CMYK                        1.00  1.59  2.08  2.19   90.80   4
   -colorspace GRAY                        1.00  1.73  2.41  2.85   64.60   4
   -colorspace HSL                         1.00  1.93  2.81  3.59   20.28   4
   -colorspace HWB                         1.00  1.92  2.81  3.52   28.29   4
   -colorspace OHTA                        1.00  1.69  2.29  2.76   64.80   4
   -colorspace YCbCr                       1.00  1.75  2.42  2.85   64.67   4
   -colorspace YIQ                         1.00  1.74  2.40  2.83   64.40   4
   -colorspace YUV                         1.00  1.71  2.30  2.77   64.60   4
   -contrast -contrast -contrast           1.00  1.99  2.95  3.85    3.33   4
   +contrast +contrast +contrast           1.00  1.99  2.94  3.85    3.64   4
   -convolve 1,1,1,1,4,1,1,1,1             1.00  1.91  2.76  3.50   22.75   4
   -colorize 30%/20%/50%                   1.00  1.82  2.61  3.18   51.70   4
   -despeckle                              1.00  1.47  2.81  2.81    1.44   3
   -edge 0x1                               1.00  1.93  2.82  3.64   12.10   4
   -emboss 0x1                             1.00  1.80  2.46  3.03    8.68   4
   -enhance                                1.00  1.99  2.96  3.87    2.39   4
   -gaussian 0x0.5                         1.00  1.93  2.81  3.65   12.57   4
   -gaussian 0x1.0                         1.00  1.97  2.91  3.79    5.02   4
   -implode 0.5                            1.00  1.63  2.57  2.95    4.05   4
   -implode -1                             1.00  1.55  2.46  2.70    4.62   4
   -lat 10x10-5%                           1.00  1.95  2.90  3.35    3.30   4
   -median 1                               1.00  0.83  1.24  1.53    0.56   4
   -median 2                               1.00  0.88  1.34  1.65    0.23   4
   -minify                                 1.00  1.81  2.60  3.20   47.40   4
   -modulate 110/100/95                    1.00  1.94  2.84  3.66   15.71   4
   +noise Uniform                          1.00  1.58  2.32  3.00    6.20   4
   +noise Gaussian                         1.00  1.78  2.66  3.48    1.46   4
   +noise Multiplicative                   1.00  1.69  2.52  3.29    2.05   4
   +noise Impulse                          1.00  1.57  2.32  2.96    6.48   4
   +noise Laplacian                        1.00  1.66  2.46  3.18    4.01   4
   +noise Poisson                          1.00  1.45  2.15  2.81    1.88   4
   -noise 1                                1.00  0.81  1.28  1.55    0.56   4
   -noise 2                                1.00  0.91  1.35  1.67    0.23   4
   -fill blue -fuzz 35% -opaque red        1.00  1.68  2.36  2.79   73.60   4
   -operator all Add 2%                    1.00  1.72  2.30  2.79   71.46   4
   -operator all And 233                   1.00  1.55  1.94  1.93   97.80   3
   -operator all Assign 50%                1.00  1.18  1.48  1.49   98.80   4
   -operator all Depth 6                   1.00  1.62  2.19  2.47   81.40   4
   -operator all Divide 2                  1.00  1.71  2.39  2.86   66.40   4
   -operator all Gamma 0.7                 1.00  1.39  1.66  1.80   51.00   4
   -operator all Negate 1.0                1.00  1.23  1.47  1.57   97.40   4
   -operator all LShift 2                  1.00  1.31  1.56  1.66   94.40   4
   -operator all Multiply 0.5              1.00  1.77  2.45  2.90   71.40   4
   -operator all Or 233                    1.00  1.24  1.45  1.58   98.40   4
   -operator all RShift 2                  1.00  1.31  1.66  1.73   97.60   4
   -operator all Subtract 10%              1.00  1.71  2.33  2.81   69.20   4
   -operator red Threshold 50%             1.00  1.44  1.85  1.93   98.80   4
   -operator gray Threshold 50%            1.00  1.32  1.64  1.78   97.40   4
   -operator all Threshold-White 80%       1.00  1.37  1.71  1.86  110.00   4
   -operator all Threshold-Black 10%       1.00  1.43  1.80  2.01  107.00   4
   -operator all Xor 233                   1.00  1.24  1.55  1.59   97.00   4
   -operator all Noise-Gaussian 30%        1.00  1.80  2.69  3.53    1.50   4
   -operator all Noise-Impulse 30%         1.00  1.58  2.35  3.07    6.89   4
   -operator all Noise-Laplacian 30%       1.00  1.68  2.49  3.26    4.17   4
   -operator all Noise-Multiplicative 30%  1.00  1.71  2.56  3.35    2.10   4
   -operator all Noise-Poisson 30%         1.00  1.45  2.17  2.84    1.92   4
   -operator all Noise-Uniform 30%         1.00  1.60  2.39  3.11    6.56   4
   -ordered-dither all 2x2                 1.00  1.52  1.99  2.29   80.20   4
   -ordered-dither all 3x3                 1.00  1.62  2.25  2.53   78.60   4
   -ordered-dither intensity 3x3           1.00  1.70  2.29  2.59   78.60   4
   -ordered-dither all 4x4                 1.00  1.57  2.07  2.30   78.80   4
   -paint 0x1                              1.00  1.97  2.92  3.81    5.71   4
   -random-threshold all 20x80             1.00  1.27  1.85  2.35   21.91   4
   -density 75x75 -resample 50x50          1.00  1.98  2.80  3.53   11.64   4
   -resize 10%                             1.00  1.94  2.79  3.47   21.56   4
   -resize 50%                             1.00  1.94  2.84  3.53   17.10   4
   -resize 150%                            1.00  1.83  2.55  3.70    3.03   4
   -rotate 45                              1.00  1.58  2.11  2.63    0.96   4
   -segment 0.5x0.25                       1.00  1.42  1.66  1.80    0.15   4
   -shade 30x30                            1.00  1.85  2.60  3.25   25.75   4
   -sharpen 0x0.5                          1.00  1.91  2.79  3.56   12.33   4
   -sharpen 0x1.0                          1.00  1.96  2.92  3.79    5.02   4
   -shear 45x45                            1.00  1.42  2.24  2.68    1.36   4
   -solarize 50%                           1.00  1.43  1.74  1.89   96.20   4
   -swirl 90                               1.00  1.58  2.50  2.77    4.58   4
   -fuzz 35% -transparent red              1.00  1.72  2.37  2.86   69.60   4
   -trim                                   1.00  1.85  2.69  3.33   40.12   4
   -fuzz 5% -trim                          1.00  1.93  2.84  3.60   23.75   4
   -unsharp 0x0.5+20+1                     1.00  1.79  2.40  2.68   10.18   4
   -unsharp 0x1.0+20+1                     1.00  1.78  2.45  2.87    7.98   4
   -wave 25x150                            1.00  1.35  2.07  2.10    3.79   4
   ====================================== ===== ===== ===== ===== ======= ====

Sun Solaris / UltraSPARC III
----------------------------

The following table shows the performance boost as threads are added
on 2 CPU Sun SPARC 1.2GHz workstation running Sun Solaris 10.  This
system obtains quite substantial benefit for most key algorithms:

.. table:: Performance Boost On Two CPU SPARC System

   ====================================== ===== ===== ======= ====
   Operation                                1     2   iter/s  thds
   ====================================== ===== ===== ======= ====
   -noop                                   1.00  1.27   69.20   2
   -affine 1,0,0.785,1,0,0 -transform      1.00  1.94    1.95   2
   -blur 0x0.5                             1.00  1.79    1.85   2
   -blur 0x1.0                             1.00  1.88    1.43   2
   -charcoal 0x1                           1.00  1.86    0.59   2
   -colorspace CMYK                        1.00  1.75   16.63   2
   -colorspace GRAY                        1.00  1.89    6.97   2
   -colorspace HSL                         1.00  1.94    2.79   2
   -colorspace HWB                         1.00  1.90    3.21   2
   -colorspace OHTA                        1.00  1.91    6.99   2
   -colorspace YCbCr                       1.00  1.89    6.99   2
   -colorspace YIQ                         1.00  1.90    6.97   2
   -colorspace YUV                         1.00  1.88    6.97   2
   -contrast -contrast -contrast           1.00  1.97    0.47   2
   +contrast +contrast +contrast           1.00  1.96    0.49   2
   -convolve 1,1,1,1,4,1,1,1,1             1.00  1.93    2.52   2
   -colorize 30%/20%/50%                   1.00  1.91    6.77   2
   -despeckle                              1.00  1.53    0.17   2
   -edge 0x1                               1.00  1.96    1.45   2
   -emboss 0x1                             1.00  1.81    1.08   2
   -enhance                                1.00  1.95    0.46   2
   -gaussian 0x0.5                         1.00  1.95    1.36   2
   -gaussian 0x1.0                         1.00  1.97    0.51   2
   -implode 0.5                            1.00  1.96    0.92   2
   -implode -1                             1.00  1.96    1.16   2
   -lat 10x10-5%                           1.00  2.19    0.51   2
   -median 1                               1.00  1.06    0.18   2
   -median 2                               1.00  1.13    0.08   2
   -minify                                 1.00  1.79    8.93   2
   -modulate 110/100/95                    1.00  1.92    1.74   2
   +noise Uniform                          1.00  1.55    1.40   2
   +noise Gaussian                         1.00  1.81    0.30   2
   +noise Multiplicative                   1.00  1.75    0.46   2
   +noise Impulse                          1.00  1.55    1.35   2
   +noise Laplacian                        1.00  1.74    0.81   2
   +noise Poisson                          1.00  1.53    0.41   2
   -noise 1                                1.00  1.10    0.18   2
   -noise 2                                1.00  1.15    0.08   2
   -fill blue -fuzz 35% -opaque red        1.00  1.87    9.27   2
   -operator all Add 2%                    1.00  1.89    6.85   2
   -operator all And 233                   1.00  1.67   21.43   2
   -operator all Assign 50%                1.00  1.61   23.95   2
   -operator all Depth 6                   1.00  1.67   18.69   2
   -operator all Divide 2                  1.00  1.89    5.44   2
   -operator all Gamma 0.7                 1.00  1.45   12.82   2
   -operator all Negate 1.0                1.00  1.62   20.60   2
   -operator all LShift 2                  1.00  1.70   21.32   2
   -operator all Multiply 0.5              1.00  1.90    6.84   2
   -operator all Or 233                    1.00  1.69   21.47   2
   -operator all RShift 2                  1.00  1.67   21.32   2
   -operator all Subtract 10%              1.00  1.87    7.13   2
   -operator red Threshold 50%             1.00  1.67   22.51   2
   -operator gray Threshold 50%            1.00  1.72   16.10   2
   -operator all Threshold-White 80%       1.00  1.74   16.50   2
   -operator all Threshold-Black 10%       1.00  1.76   16.47   2
   -operator all Xor 233                   1.00  1.66   21.27   2
   -operator all Noise-Gaussian 30%        1.00  1.82    0.30   2
   -operator all Noise-Impulse 30%         1.00  1.55    1.38   2
   -operator all Noise-Laplacian 30%       1.00  1.76    0.81   2
   -operator all Noise-Multiplicative 30%  1.00  1.75    0.47   2
   -operator all Noise-Poisson 30%         1.00  1.53    0.41   2
   -operator all Noise-Uniform 30%         1.00  1.55    1.44   2
   -ordered-dither all 2x2                 1.00  1.76   13.41   2
   -ordered-dither all 3x3                 1.00  1.78   12.33   2
   -ordered-dither intensity 3x3           1.00  1.77   12.23   2
   -ordered-dither all 4x4                 1.00  1.75   13.44   2
   -paint 0x1                              1.00  1.96    0.82   2
   -random-threshold all 20x80             1.00  1.43    4.98   2
   -density 75x75 -resample 50x50          1.00  1.90    1.73   2
   -resize 10%                             1.00  1.90    3.58   2
   -resize 50%                             1.00  1.90    2.30   2
   -resize 150%                            1.00  1.83    0.64   2
   -rotate 45                              1.00  1.55    0.34   2
   -segment 0.5x0.25                       1.00  1.41    0.02   2
   -shade 30x30                            1.00  1.99    2.23   2
   -sharpen 0x0.5                          1.00  1.97    1.36   2
   -sharpen 0x1.0                          1.00  1.97    0.51   2
   -shear 45x45                            1.00  1.36    0.64   2
   -solarize 50%                           1.00  1.67   20.56   2
   -swirl 90                               1.00  1.97    1.13   2
   -fuzz 35% -transparent red              1.00  1.95    8.57   2
   -trim                                   1.00  1.94    4.21   2
   -fuzz 5% -trim                          1.00  1.93    3.39   2
   -unsharp 0x0.5+20+1                     1.00  1.86    1.50   2
   -unsharp 0x1.0+20+1                     1.00  1.89    1.21   2
   -wave 25x150                            1.00  1.88    1.12   2
   ====================================== ===== ===== ======= ====

IBM AIX / IBM Power5+
---------------------
   
The following table shows the boost on a four core IBM P5+ server
system (IBM System p5 505 Express with (2) 2.1Ghz CPUs) running AIX:

.. table:: Performance Boost On Four Core IBM P5+ System

   ====================================== ===== ===== ===== ===== ======= ====
   Operation                                1     2     3     4   iter/s  thds
   ====================================== ===== ===== ===== ===== ======= ====
   -noop                                   1.00  1.61  1.56  1.65  291.40   4
   -affine 1,0,0.785,1,0,0 -transform      1.00  1.99  2.61  3.09    8.35   4
   -blur 0x0.5                             1.00  1.98  2.52  3.03    2.72   4
   -blur 0x1.0                             1.00  1.96  2.63  3.28    2.12   4
   -charcoal 0x1                           1.00  1.89  2.69  3.37    0.96   4
   -colorspace CMYK                        1.00  1.49  1.63  1.61   69.80   3
   -colorspace GRAY                        1.00  1.94  2.45  2.91   17.00   4
   -colorspace HSL                         1.00  1.95  2.46  2.98    8.20   4
   -colorspace HWB                         1.00  1.97  2.56  3.14    8.58   4
   -colorspace OHTA                        1.00  1.93  2.47  2.93   17.17   4
   -colorspace YCbCr                       1.00  1.89  2.50  2.98   17.26   4
   -colorspace YIQ                         1.00  1.91  2.47  2.95   17.23   4
   -colorspace YUV                         1.00  1.95  2.43  2.97   17.23   4
   -contrast -contrast -contrast           1.00  2.00  2.48  2.96    2.10   4
   +contrast +contrast +contrast           1.00  1.99  2.50  3.00    2.15   4
   -convolve 1,1,1,1,4,1,1,1,1             1.00  2.00  2.76  3.53    3.85   4
   -colorize 30%/20%/50%                   1.00  1.93  2.76  3.55   11.78   4
   -despeckle                              1.00  1.49  2.77  2.78    0.27   4
   -edge 0x1                               1.00  1.99  2.78  3.55    4.10   4
   -emboss 0x1                             1.00  1.94  2.67  3.32    1.80   4
   -enhance                                1.00  1.96  2.75  3.49    0.90   4
   -gaussian 0x0.5                         1.00  1.99  2.80  3.57    1.98   4
   -gaussian 0x1.0                         1.00  2.00  2.85  3.59    0.76   4
   -implode 0.5                            1.00  1.75  2.50  3.08    2.98   4
   -implode -1                             1.00  2.00  2.52  3.03    2.99   4
   -lat 10x10-5%                           1.00  1.99  2.43  2.87    1.34   4
   -median 1                               1.00  1.66  2.29  2.47    0.70   4
   -median 2                               1.00  1.78  2.42  2.60    0.28   4
   -minify                                 1.00  2.00  2.86  3.70    9.72   4
   -modulate 110/100/95                    1.00  1.98  2.40  2.56    6.05   4
   +noise Uniform                          1.00  1.81  2.74  3.38    4.77   4
   +noise Gaussian                         1.00  2.00  2.55  3.08    1.93   4
   +noise Multiplicative                   1.00  1.98  2.69  3.40    2.10   4
   +noise Impulse                          1.00  1.97  2.60  3.20    4.86   4
   +noise Laplacian                        1.00  1.97  2.75  3.50    3.31   4
   +noise Poisson                          1.00  1.83  2.67  3.34    1.93   4
   -noise 1                                1.00  1.68  2.20  2.59    0.73   4
   -noise 2                                1.00  1.75  2.39  2.60    0.28   4
   -fill blue -fuzz 35% -opaque red        1.00  1.95  2.44  2.87   38.80   4
   -operator all Add 2%                    1.00  1.95  2.75  3.55   15.60   4
   -operator all And 233                   1.00  1.66  1.78  1.85  165.00   4
   -operator all Assign 50%                1.00  1.70  1.75  1.81  173.60   4
   -operator all Depth 6                   1.00  1.76  1.80  1.87  134.80   4
   -operator all Divide 2                  1.00  1.98  2.78  3.60   15.68   4
   -operator all Gamma 0.7                 1.00  1.60  1.62  1.67  105.60   4
   -operator all Negate 1.0                1.00  1.68  1.76  1.78  162.20   4
   -operator all LShift 2                  1.00  1.21  1.77  1.77  159.00   4
   -operator all Multiply 0.5              1.00  1.95  2.65  3.48   15.20   4
   -operator all Or 233                    1.00  1.35  1.62  1.62  140.00   4
   -operator all RShift 2                  1.00  1.71  1.76  1.81  163.60   4
   -operator all Subtract 10%              1.00  1.96  2.75  3.49   16.77   4
   -operator red Threshold 50%             1.00  1.78  1.92  2.08  166.60   4
   -operator gray Threshold 50%            1.00  1.79  1.90  1.99  112.40   4
   -operator all Threshold-White 80%       1.00  1.81  1.75  1.68  106.20   2
   -operator all Threshold-Black 10%       1.00  1.49  1.81  2.01  108.00   4
   -operator all Xor 233                   1.00  1.47  1.72  1.80  150.60   4
   -operator all Noise-Gaussian 30%        1.00  1.88  2.46  3.12    1.97   4
   -operator all Noise-Impulse 30%         1.00  1.84  2.55  2.97    4.47   4
   -operator all Noise-Laplacian 30%       1.00  1.98  2.84  3.28    2.97   4
   -operator all Noise-Multiplicative 30%  1.00  1.77  2.66  3.38    2.09   4
   -operator all Noise-Poisson 30%         1.00  1.82  2.74  3.60    1.96   4
   -operator all Noise-Uniform 30%         1.00  1.93  2.76  3.43    4.79   4
   -ordered-dither all 2x2                 1.00  1.46  1.56  1.65   52.20   4
   -ordered-dither all 3x3                 1.00  1.36  1.73  1.28   43.91   3
   -ordered-dither intensity 3x3           1.00  1.50  1.66  1.66   43.71   4
   -ordered-dither all 4x4                 1.00  1.22  1.53  1.69   53.40   4
   -paint 0x1                              1.00  1.30  2.00  2.54    6.34   4
   -random-threshold all 20x80             1.00  1.73  2.22  2.77   19.16   4
   -density 75x75 -resample 50x50          1.00  1.86  2.45  2.68    3.19   4
   -resize 10%                             1.00  1.89  2.53  2.58    7.13   4
   -resize 50%                             1.00  1.79  2.55  2.44    4.08   3
   -resize 150%                            1.00  1.83  2.17  2.56    1.19   4
   -rotate 45                              1.00  1.71  1.46  1.65    0.45   2
   -segment 0.5x0.25                       1.00  1.12  1.16  1.20    0.03   4
   -shade 30x30                            1.00  1.69  2.52  3.16   10.34   4
   -sharpen 0x0.5                          1.00  1.91  2.73  3.50    1.92   4
   -sharpen 0x1.0                          1.00  1.90  2.81  3.67    0.78   4
   -shear 45x45                            1.00  1.67  2.16  1.96    1.18   3
   -solarize 50%                           1.00  1.64  1.95  1.90   89.00   3
   -swirl 90                               1.00  1.83  2.34  2.27    2.52   3
   -fuzz 35% -transparent red              1.00  1.58  2.01  2.49   32.73   4
   -trim                                   1.00  1.82  2.29  2.86   27.34   4
   -fuzz 5% -trim                          1.00  1.74  2.54  2.88    8.27   4
   -unsharp 0x0.5+20+1                     1.00  1.94  2.59  3.06    2.17   4
   -unsharp 0x1.0+20+1                     1.00  1.75  2.67  3.26    1.76   4
   -wave 25x150                            1.00  1.79  2.36  2.64    2.38   4
   ====================================== ===== ===== ===== ===== ======= ====

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
   -black-threshold 20%                                                   1.00  1.57  1.83  1.84  1.88  1.81  1.69  1.56   44.71   5
   -blur 0x0.5                                                            1.00  1.52  1.79  1.94  1.97  2.07  2.08  1.96    7.63   7
   -blur 0x1.0                                                            1.00  1.62  1.99  2.17  2.27  2.43  2.48  2.48    7.06   7
   -border 6x6                                                            1.00  1.39  1.63  1.67  1.63  1.58  1.48  1.32   33.33   4
   -channel red                                                           1.00  1.25  1.38  1.37  1.39  1.36  1.26  1.21   42.12   5
   -charcoal 0x1                                                          1.00  1.65  2.11  2.40  2.61  2.75  2.91  2.91    3.56   8
   -chop 800x600+200+300                                                  1.00  1.41  1.62  1.64  1.65  1.51  1.43  1.33   51.90   5
   -colors 16                                                             1.00  1.03  1.01  1.05  1.05  1.05  1.05  1.04    0.82   7
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
   -crop 1700x900+100+100                                                 1.00  1.40  1.61  1.63  1.64  1.57  1.39  1.30   37.92   5
   -cycle 200                                                             1.00  1.03  1.00  1.07  1.06  1.06  1.06  1.06    0.68   4
   -despeckle                                                             1.00  1.48  2.81  2.82  2.82  2.81  2.81  2.81    1.22   4
   -fill none -stroke gold -draw 'circle 800,500 1100,800'                1.00  1.05  1.06  1.02  1.09  1.06  1.07  0.98    7.12   5
   -fill green -stroke gold -draw 'circle 800,500 1100,800'               1.00  1.05  1.05  1.01  1.07  1.04  1.06  1.01    5.95   5
   -fill none -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  0.93  1.08  1.12  1.12  1.08  1.09  1.05   11.68   4
   -fill blue -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.04  1.06  1.08  1.08  1.03  1.06  1.02    9.54   5
   -fill none -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.10  1.13  1.13  1.06  1.11  1.04  0.98   14.97   4
   -fill blue -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.07  1.08  1.06  1.01  1.05  1.03  0.96   11.71   3
   -fill none -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.09  1.11  1.14  1.13  1.12  1.09  1.10    9.34   4
   -fill blue -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.09  1.04  1.13  1.11  1.11  1.08  1.09    8.95   4
   -fill none -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.08  1.16  1.16  1.17  1.14  1.13  1.10   19.96   5
   -fill blue -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.07  1.15  1.15  1.17  1.13  1.11  1.09   18.69   5
   -edge 0x1                                                              1.00  1.85  2.62  3.28  3.81  4.05  4.46  4.59   20.40   8
   -emboss 0x1                                                            1.00  1.76  2.41  2.94  3.31  3.68  3.89  4.07    9.72   8
   -enhance                                                               1.00  2.00  2.98  3.96  4.92  5.83  6.74  7.61    2.85   8
   -equalize                                                              1.00  1.16  1.24  1.27  1.27  1.22  1.24  1.19   22.75   5
   -flip                                                                  1.00  1.38  1.60  1.62  1.61  1.56  1.46  1.30   32.47   4
   -flop                                                                  1.00  1.40  1.63  1.67  1.66  1.56  1.50  1.34   32.93   4
   -frame 15x15+3+3                                                       1.00  1.40  1.62  1.67  1.66  1.57  1.41  1.32   32.73   4
   -gamma 1.6                                                             1.00  1.55  1.91  2.10  2.21  2.20  2.24  2.17   31.34   7
   -gaussian 0x0.5                                                        1.00  1.89  2.74  3.49  4.08  4.73  5.15  5.47   14.88   8
   -gaussian 0x1.0                                                        1.00  1.95  2.88  3.77  4.62  5.40  6.05  6.72    7.04   8
   -implode 0.5                                                           1.00  1.65  2.02  2.55  2.96  3.64  3.74  4.31    4.56   8
   -implode -1                                                            1.00  1.32  1.85  2.26  3.08  3.05  3.44  2.91    4.62   7
   -lat 10x10-5%                                                          1.00  1.95  2.87  3.72  4.50  5.25  5.93  6.49    4.09   8
   -level 10%,1.2,90%                                                     1.00  1.17  1.25  1.26  1.25  1.23  1.20  1.14   28.40   4
   -magnify                                                               1.00  1.03  1.04  1.04  1.04  1.03  1.00  1.02    3.91   5
   -map netscape:                                                         1.00  1.00  1.00  0.99  1.00  1.00  0.99  1.00    0.91   3
   -median 1                                                              1.00  1.06  1.48  0.96  1.00  1.34  1.39  1.44    0.48   3
   -median 2                                                              1.00  0.80  1.72  1.16  1.12  1.53  1.56  1.95    0.23   8
   -minify                                                                1.00  1.83  2.46  2.88  3.16  3.29  3.24  3.21   32.53   6
   -modulate 110/100/95                                                   1.00  1.94  2.82  3.61  4.37  5.00  5.51  5.91   15.68   8
   -monochrome                                                            1.00  1.10  0.96  1.11  1.11  1.11  1.11  1.12    0.65   8
   -motion-blur 0x3+30                                                    1.00  1.78  2.50  3.30  4.09  4.96  5.82  6.32    3.20   8
   -negate                                                                1.00  1.26  1.37  1.36  1.37  1.34  1.29  1.21   41.40   3
   +noise Uniform                                                         1.00  1.83  2.45  2.93  3.32  3.60  3.83  3.95   11.90   8
   +noise Gaussian                                                        1.00  1.97  2.85  3.77  4.66  5.48  6.25  6.99    2.67   8
   +noise Multiplicative                                                  1.00  1.92  2.84  3.67  4.49  5.25  5.93  6.56    3.93   8
   +noise Impulse                                                         1.00  1.76  2.36  2.83  3.22  3.49  3.68  3.83   11.80   8
   +noise Laplacian                                                       1.00  1.91  2.73  3.45  4.13  4.70  5.20  5.63    6.60   8
   +noise Poisson                                                         1.00  1.95  2.85  3.70  4.51  5.25  5.92  6.59    4.00   8
   -noise 1                                                               1.00  0.60  1.48  0.91  1.08  1.28  1.61  1.49    0.50   7
   -noise 2                                                               1.00  0.80  1.68  1.13  1.14  1.70  1.63  1.93    0.22   8
   -normalize                                                             1.00  1.17  1.26  1.29  1.27  1.26  1.27  1.21   24.55   4
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
   -raise 10x10                                                           1.00  1.41  1.65  1.67  1.63  1.55  1.40  1.30   61.00   4
   -random-threshold all 20x80                                            1.00  1.72  2.30  2.68  3.00  3.10  3.13  3.04   32.27   7
   -density 75x75 -resample 50x50                                         1.00  1.89  2.67  3.34  3.76  4.21  4.23  4.25   12.05   8
   -resize 10%                                                            1.00  1.88  2.63  3.25  3.67  3.68  3.77  4.13   22.60   8
   -resize 50%                                                            1.00  1.86  2.60  3.22  2.50  3.96  3.99  4.09   14.77   8
   -resize 150%                                                           1.00  1.77  2.25  2.71  2.50  3.60  3.80  3.74    4.42   7
   -roll +20+10                                                           1.00  1.36  1.57  1.60  1.62  1.53  1.42  1.33   31.08   5
   -rotate 0                                                              1.00  1.40  1.63  1.66  1.64  1.60  1.45  1.34   33.27   4
   -rotate 45                                                             1.00  1.25  1.49  1.67  1.84  1.93  2.03  2.05    1.25   8
   -rotate 90                                                             1.00  1.26  1.29  1.24  1.20  1.16  1.15  1.09   17.89   3
   -rotate 180                                                            1.00  1.41  1.65  1.68  1.67  1.61  1.49  1.39   32.73   4
   -rotate 270                                                            1.00  1.23  1.27  1.27  1.20  1.16  1.06  1.09   17.69   3
   -sample 5%                                                             1.00  1.37  1.57  1.60  1.54  1.50  1.36  1.24   67.20   4
   -sample 50%                                                            1.00  1.23  1.34  1.35  1.32  1.27  1.19  1.13   38.52   4
   -sample 150%                                                           1.00  1.12  1.14  1.15  1.14  1.13  1.09  1.07   10.67   4
   -segment 0.5x0.25                                                      1.00  1.54  1.90  2.15  2.33  2.48  2.58  2.67    0.13   8
   -scale 5%                                                              1.00  1.08  1.10  1.11  1.10  1.09  1.07  1.04   12.18   4
   -scale 50%                                                             1.00  1.06  1.10  1.09  1.09  1.08  1.06  1.04   11.44   3
   -scale 150%                                                            1.00  1.02  1.04  1.04  1.03  1.03  1.03  1.02    3.52   4
   -shade 30x30                                                           1.00  1.91  2.76  3.53  4.27  4.87  5.34  5.69   13.74   8
   -sharpen 0x0.5                                                         1.00  1.89  2.74  3.49  4.06  4.75  5.14  5.47   14.88   8
   -sharpen 0x1.0                                                         1.00  1.95  2.88  3.77  4.63  5.39  6.08  6.70    7.02   8
   -shave 10x10                                                           1.00  1.39  1.62  1.63  1.62  1.57  1.43  1.31   33.27   4
   -shear 45x45                                                           1.00  0.97  1.15  1.24  1.36  1.42  1.46  1.46    1.52   8
   -solarize 50%                                                          1.00  1.25  1.36  1.40  1.40  1.33  1.29  1.22   42.00   5
   -spread 1                                                              1.00  1.58  1.98  2.16  2.15  2.01  1.96  1.84   32.07   4
   -spread 3                                                              1.00  1.54  1.91  2.09  2.09  2.06  1.94  1.83   29.94   5
   -swirl 90                                                              1.00  1.30  1.82  2.26  3.14  2.96  3.44  3.64    5.03   8
   -threshold 35%                                                         1.00  1.54  1.86  1.86  1.85  1.80  1.74  1.64   37.92   3
   -fuzz 35% -transparent red                                             1.00  1.57  1.86  1.96  1.99  1.93  1.80  1.72   41.83   5
   -trim                                                                  1.00  1.77  2.41  2.86  3.08  3.22  2.54  2.53   38.00   6
   -fuzz 5% -trim                                                         1.00  1.84  2.50  3.11  3.62  3.79  3.94  4.00   29.28   8
   -unsharp 0x0.5+20+1                                                    1.00  1.57  1.92  2.12  2.20  2.29  2.31  2.21    6.86   7
   -unsharp 0x1.0+20+1                                                    1.00  1.66  2.08  2.30  2.44  2.56  2.65  2.61    6.39   7
   -wave 25x150                                                           1.00  1.19  1.44  1.81  1.71  1.97  2.04  2.39    4.08   8
   -white-threshold 80%                                                   1.00  1.45  1.64  1.60  1.58  1.54  1.43  1.29   48.30   3
   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ===== ======= ====

Apple OS-X/IBM G5
-----------------

The following table shows the boost on a two core Apple PowerPC G5
system (2.5GHz) running OS-X Leopard:

.. table:: Performance Boost On Two Core PowerPC G5 System

   ====================================== ===== ===== ======= ====
   Operation                                1     2   iter/s  thds
   ====================================== ===== ===== ======= ====
   -noop                                   1.00  1.00   23.31   2
   -affine 1,0,0.785,1,0,0 -transform      1.00  1.55    3.41   2
   -blur 0x0.5                             1.00  1.55    1.85   2
   -blur 0x1.0                             1.00  1.65    1.36   2
   -charcoal 0x1                           1.00  1.68    0.69   2
   -colorspace CMYK                        1.00  1.15   15.94   2
   -colorspace GRAY                        1.00  1.47    9.51   2
   -colorspace HSL                         1.00  1.51    4.68   2
   -colorspace HWB                         1.00  1.67    5.53   2
   -colorspace OHTA                        1.00  1.45    9.32   2
   -colorspace YCbCr                       1.00  1.44    9.25   2
   -colorspace YIQ                         1.00  1.46    9.32   2
   -colorspace YUV                         1.00  1.48    9.41   2
   -contrast -contrast -contrast           1.00  1.89    1.39   2
   +contrast +contrast +contrast           1.00  1.92    1.44   2
   -convolve 1,1,1,1,4,1,1,1,1             1.00  1.73    3.33   2
   -colorize 30%/20%/50%                   1.00  1.42    7.81   2
   -despeckle                              1.00  1.48    0.24   2
   -edge 0x1                               1.00  1.73    3.40   2
   -emboss 0x1                             1.00  1.72    1.51   2
   -enhance                                1.00  1.90    0.59   2
   -gaussian 0x0.5                         1.00  1.85    1.80   2
   -gaussian 0x1.0                         1.00  1.95    0.69   2
   -implode 0.5                            1.00  1.60    1.77   2
   -implode -1                             1.00  1.50    2.13   2
   -lat 10x10-5%                           1.00  1.88    0.51   2
   -median 1                               1.00  1.16    0.40   2
   -median 2                               1.00  0.91    0.13   1
   -minify                                 1.00  1.61    6.79   2
   -modulate 110/100/95                    1.00  1.79    4.29   2
   +noise Uniform                          1.00  1.69    2.62   2
   +noise Gaussian                         1.00  1.87    0.87   2
   +noise Multiplicative                   1.00  1.86    1.16   2
   +noise Impulse                          1.00  1.71    2.49   2
   +noise Laplacian                        1.00  1.80    1.69   2
   +noise Poisson                          1.00  1.87    0.93   2
   -noise 1                                1.00  0.96    0.35   1
   -noise 2                                1.00  0.94    0.12   1
   -fill blue -fuzz 35% -opaque red        1.00  1.35   13.94   2
   -operator all Add 2%                    1.00  1.55    9.92   2
   -operator all And 233                   1.00  1.01   18.29   2
   -operator all Assign 50%                1.00  1.13   17.93   2
   -operator all Depth 6                   1.00  1.03   18.02   2
   -operator all Divide 2                  1.00  1.54    9.25   2
   -operator all Gamma 0.7                 1.00  1.00   15.45   2
   -operator all Negate 1.0                1.00  1.02   18.76   2
   -operator all LShift 2                  1.00  1.01   18.96   2
   -operator all Multiply 0.5              1.00  1.53    9.96   2
   -operator all Or 233                    1.00  1.02   18.53   2
   -operator all RShift 2                  1.00  1.01   18.65   2
   -operator all Subtract 10%              1.00  1.55   10.06   2
   -operator red Threshold 50%             1.00  0.97   18.76   1
   -operator gray Threshold 50%            1.00  1.04   18.53   2
   -operator all Threshold-White 80%       1.00  1.10   19.88   2
   -operator all Threshold-Black 10%       1.00  1.10   19.88   2
   -operator all Xor 233                   1.00  1.03   18.76   2
   -operator all Noise-Gaussian 30%        1.00  1.92    0.93   2
   -operator all Noise-Impulse 30%         1.00  1.85    2.98   2
   -operator all Noise-Laplacian 30%       1.00  1.82    1.83   2
   -operator all Noise-Multiplicative 30%  1.00  1.93    1.26   2
   -operator all Noise-Poisson 30%         1.00  1.96    1.02   2
   -operator all Noise-Uniform 30%         1.00  1.74    2.99   2
   -ordered-dither all 2x2                 1.00  1.09   14.97   2
   -ordered-dither all 3x3                 1.00  1.15   14.57   2
   -ordered-dither intensity 3x3           1.00  1.15   14.65   2
   -ordered-dither all 4x4                 1.00  1.08   14.74   2
   -paint 0x1                              1.00  1.69    3.89   2
   -random-threshold all 20x80             1.00  1.52    8.40   2
   -density 75x75 -resample 50x50          1.00  1.75    2.13   2
   -resize 10%                             1.00  1.69    5.36   2
   -resize 50%                             1.00  1.63    3.20   2
   -resize 150%                            1.00  1.63    0.53   2
   -rotate 45                              1.00  1.73    0.24   2
   -segment 0.5x0.25                       1.00  1.22    0.04   2
   -shade 30x30                            1.00  1.59    5.20   2
   -sharpen 0x0.5                          1.00  1.84    1.79   2
   -sharpen 0x1.0                          1.00  1.88    0.67   2
   -shear 45x45                            1.00  1.66    0.28   2
   -solarize 50%                           1.00  1.06   18.25   2
   -swirl 90                               1.00  1.68    1.93   2
   -fuzz 35% -transparent red              1.00  1.36   13.72   2
   -trim                                   1.00  1.70    6.24   2
   -fuzz 5% -trim                          1.00  1.78    3.80   2
   -unsharp 0x0.5+20+1                     1.00  1.62    1.54   2
   -unsharp 0x1.0+20+1                     1.00  1.69    1.19   2
   -wave 25x150                            1.00  1.40    1.82   2
   ====================================== ===== ===== ======= ====

FreeBSD / Intel Xeon
--------------------

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

   ====================================== ===== ===== ===== ===== ======= ====
   Operation                                1     2     3     4   iter/s  thds
   ====================================== ===== ===== ===== ===== ======= ====
   -noop                                   1.00  0.99  1.00  0.99   18.31   1
   -affine 1,0,0.785,1,0,0 -transform      1.00  1.27  1.33  1.33    2.53   4
   -blur 0x0.5                             1.00  1.24  1.38  1.42    2.81   4
   -blur 0x1.0                             1.00  1.34  1.47  1.49    2.39   4
   -charcoal 0x1                           1.00  1.35  1.62  1.70    0.99   4
   -colorspace CMYK                        1.00  0.88  0.89  0.85   11.13   1
   -colorspace GRAY                        1.00  1.34  1.47  1.48    7.88   4
   -colorspace HSL                         1.00  1.62  1.89  2.01    4.82   4
   -colorspace HWB                         1.00  1.55  1.90  2.09    6.71   4
   -colorspace OHTA                        1.00  1.37  1.53  1.57    8.06   4
   -colorspace YCbCr                       1.00  1.36  1.52  1.57    8.09   4
   -colorspace YIQ                         1.00  1.39  1.53  1.58    8.07   4
   -colorspace YUV                         1.00  1.37  1.53  1.57    8.06   4
   -contrast -contrast -contrast           1.00  1.77  2.25  2.56    1.21   4
   +contrast +contrast +contrast           1.00  1.79  2.24  2.52    1.24   4
   -convolve 1,1,1,1,4,1,1,1,1             1.00  1.39  1.81  1.95    3.98   4
   -colorize 30%/20%/50%                   1.00  1.34  1.50  1.59    6.62   4
   -despeckle                              1.00  1.39  2.14  2.12    0.39   3
   -edge 0x1                               1.00  1.46  1.85  2.02    3.96   4
   -emboss 0x1                             1.00  1.44  1.76  1.87    1.95   4
   -enhance                                1.00  1.75  2.01  2.08    0.66   4
   -gaussian 0x0.5                         1.00  1.51  1.95  2.09    2.59   4
   -gaussian 0x1.0                         1.00  1.51  1.99  2.06    1.20   4
   -implode 0.5                            1.00  1.60  1.99  2.31    1.19   4
   -implode -1                             1.00  1.48  1.73  2.03    1.70   4
   -lat 10x10-5%                           1.00  1.36  1.94  2.04    0.99   4
   -median 1                               1.00  0.67  1.61  0.96    0.27   3
   -median 2                               1.00  0.77  1.52  1.10    0.09   3
   -minify                                 1.00  1.39  1.59  1.66    8.09   4
   -modulate 110/100/95                    1.00  1.64  2.03  2.22    3.98   4
   +noise Uniform                          1.00  1.60  1.92  2.09    2.53   4
   +noise Gaussian                         1.00  1.78  2.28  2.64    0.84   4
   +noise Multiplicative                   1.00  1.77  2.24  2.60    1.16   4
   +noise Impulse                          1.00  1.51  1.80  1.89    2.55   4
   +noise Laplacian                        1.00  1.70  2.25  2.61    1.86   4
   +noise Poisson                          1.00  1.76  2.15  2.42    0.88   4
   -noise 1                                1.00  0.72  1.58  0.94    0.26   3
   -noise 2                                1.00  0.76  1.50  1.11    0.09   3
   -fill blue -fuzz 35% -opaque red        1.00  1.18  1.28  1.31   13.63   4
   -operator all Add 2%                    1.00  1.22  1.35  1.38   12.29   4
   -operator all And 233                   1.00  1.00  1.00  1.00   13.43   2
   -operator all Assign 50%                1.00  1.01  1.01  1.01   13.49   3
   -operator all Depth 6                   1.00  1.01  1.01  1.01   13.20   2
   -operator all Divide 2                  1.00  1.25  1.50  1.52    9.20   4
   -operator all Gamma 0.7                 1.00  1.00  1.00  1.00    8.86   2
   -operator all Negate 1.0                1.00  1.00  1.00  1.00   13.43   2
   -operator all LShift 2                  1.00  1.00  1.01  1.01   13.45   3
   -operator all Multiply 0.5              1.00  1.20  1.35  1.38   12.25   4
   -operator all Or 233                    1.00  1.01  1.00  1.00   13.47   2
   -operator all RShift 2                  1.00  1.00  1.01  1.00   13.52   3
   -operator all Subtract 10%              1.00  1.23  1.39  1.42   12.12   4
   -operator red Threshold 50%             1.00  0.99  1.00  0.99   13.47   3
   -operator gray Threshold 50%            1.00  1.03  1.05  1.03   13.56   3
   -operator all Threshold-White 80%       1.00  1.08  1.10  1.11   15.28   4
   -operator all Threshold-Black 10%       1.00  1.07  1.12  1.11   14.98   3
   -operator all Xor 233                   1.00  1.01  1.01  1.00   13.52   2
   -operator all Noise-Gaussian 30%        1.00  1.80  2.33  2.73    0.90   4
   -operator all Noise-Impulse 30%         1.00  1.61  1.92  2.01    3.21   4
   -operator all Noise-Laplacian 30%       1.00  1.82  2.39  2.84    2.19   4
   -operator all Noise-Multiplicative 30%  1.00  1.81  2.34  2.72    1.28   4
   -operator all Noise-Poisson 30%         1.00  1.71  2.22  2.53    0.95   4
   -operator all Noise-Uniform 30%         1.00  1.64  2.11  2.37    3.22   4
   -ordered-dither all 2x2                 1.00  0.94  0.94  0.89   10.44   1
   -ordered-dither all 3x3                 1.00  1.02  1.11  1.07    9.91   3
   -ordered-dither intensity 3x3           1.00  1.02  1.11  1.07    9.94   3
   -ordered-dither all 4x4                 1.00  0.94  0.93  0.88   10.50   1
   -paint 0x1                              1.00  1.62  2.04  2.20    1.87   4
   -random-threshold all 20x80             1.00  1.39  1.69  1.78    7.59   4
   -density 75x75 -resample 50x50          1.00  1.44  1.70  1.79    1.78   4
   -resize 10%                             1.00  1.47  1.53  1.65    3.92   4
   -resize 50%                             1.00  1.51  1.61  1.67    2.37   4
   -resize 150%                            1.00  1.46  1.71  1.73    0.57   4
   -rotate 45                              1.00  1.21  1.34  1.38    0.31   4
   -segment 0.5x0.25                       1.00  1.48  1.52  1.54    0.08   4
   -shade 30x30                            1.00  1.71  2.23  2.60    3.68   4
   -sharpen 0x0.5                          1.00  1.49  1.95  2.11    2.61   4
   -sharpen 0x1.0                          1.00  1.55  1.99  2.06    1.20   4
   -shear 45x45                            1.00  1.14  1.27  1.32    0.38   4
   -solarize 50%                           1.00  1.01  1.00  1.01   13.49   4
   -swirl 90                               1.00  1.54  1.82  2.17    1.69   4
   -fuzz 35% -transparent red              1.00  1.18  1.28  1.30   13.16   4
   -trim                                   1.00  0.95  1.19  1.32    6.00   4
   -fuzz 5% -trim                          1.00  1.25  1.61  1.89    4.96   4
   -unsharp 0x0.5+20+1                     1.00  1.32  1.54  1.60    2.40   4
   -unsharp 0x1.0+20+1                     1.00  1.36  1.60  1.68    2.12   4
   -wave 25x150                            1.00  1.24  1.35  1.42    1.32   4
   ====================================== ===== ===== ===== ===== ======= ====

Windows XP / MSVC / Intel Core 2 Quad
-------------------------------------

This system is Windows XP Professional (SP3) using the Visual Studio
2008 compiler and a Q8 build.  The system CPU is a 2.83 GHz Core 2
Quad Processor (Q9550).  This processor is a multi-chip module (MCM)
based on two Core 2 CPUs bonded to a L3 cache in the same chip
package.

The following shows the performance boost for a Q8 build:

.. table:: Performance Boost on an Intel Core 2 Quad (Q9550) system

   ====================================== ===== ===== ===== ===== ======= ====
   Operation                                1     2     3     4   iter/s  thds
   ====================================== ===== ===== ===== ===== ======= ====
   -noop                                   1.00  0.99  0.96  1.00  149.60   4
   -affine 1,0,0.785,1,0,0 -transform      1.00  1.46  1.98  2.35   12.29   4
   -blur 0x0.5                             1.00  1.72  2.29  2.71   11.65   4
   -blur 0x1.0                             1.00  1.77  2.33  2.83   10.14   4
   -charcoal 0x1                           1.00  1.79  2.46  3.00    5.12   4
   -colorspace CMYK                        1.00  1.25  1.43  1.41   81.80   3
   -colorspace GRAY                        1.00  1.80  2.50  3.05   34.30   4
   -colorspace HSL                         1.00  1.99  2.90  3.74   19.28   4
   -colorspace HWB                         1.00  1.88  2.68  3.40   25.00   4
   -colorspace OHTA                        1.00  1.82  2.53  3.05   34.20   4
   -colorspace YCbCr                       1.00  1.81  2.53  3.13   35.09   4
   -colorspace YIQ                         1.00  1.82  2.53  3.12   35.00   4
   -colorspace YUV                         1.00  1.81  2.53  3.07   34.49   4
   -contrast -contrast -contrast           1.00  1.99  2.96  3.90    4.17   4
   +contrast +contrast +contrast           1.00  1.97  2.94  3.85    4.16   4
   -convolve 1,1,1,1,4,1,1,1,1             1.00  1.89  2.71  3.41   17.63   4
   -colorize 30%/20%/50%                   1.00  1.84  2.56  3.20   29.71   4
   -despeckle                              1.00  1.47  2.89  2.88    0.94   3
   -edge 0x1                               1.00  1.88  2.73  3.46   16.45   4
   -emboss 0x1                             1.00  1.97  2.73  3.33   14.31   4
   -enhance                                1.00  1.98  2.95  3.90    2.91   4
   -gaussian 0x0.5                         1.00  2.05  2.96  3.72   17.69   4
   -gaussian 0x1.0                         1.00  1.99  2.82  3.81    6.91   4
   -implode 0.5                            1.00  1.88  2.83  3.39    6.63   4
   -implode -1                             1.00  1.81  2.49  3.20    6.24   4
   -lat 10x10-5%                           1.00  1.95  2.84  3.68    4.92   4
   -median 1                               1.00  1.99  1.93  2.21    0.90   4
   -median 2                               1.00  2.01  2.26  2.41    0.35   4
   -minify                                 1.00  1.84  2.51  3.07   42.80   4
   -modulate 110/100/95                    1.00  1.93  2.80  3.61   15.80   4
   +noise Uniform                          1.00  1.88  2.64  3.23   11.56   4
   +noise Gaussian                         1.00  1.98  2.91  3.82    2.93   4
   +noise Multiplicative                   1.00  1.94  2.85  3.71    3.83   4
   +noise Impulse                          1.00  1.90  2.69  3.34   11.13   4
   +noise Laplacian                        1.00  1.94  2.79  3.60    6.58   4
   +noise Poisson                          1.00  1.97  2.91  3.83    3.08   4
   -noise 1                                1.00  2.00  1.93  1.88    0.81   2
   -noise 2                                1.00  2.01  2.16  2.06    0.30   3
   -fill blue -fuzz 35% -opaque red        1.00  1.53  1.76  1.96   90.20   4
   -operator all Add 2%                    1.00  1.79  2.46  3.08   39.20   4
   -operator all And 233                   1.00  1.18  1.11  1.00   96.00   2
   -operator all Assign 50%                1.00  1.04  0.94  0.77   93.20   2
   -operator all Depth 6                   1.00  1.35  1.32  1.33   87.60   2
   -operator all Divide 2                  1.00  1.82  2.49  2.98   37.68   4
   -operator all Gamma 0.7                 1.00  1.31  1.31  1.28   88.80   3
   -operator all Negate 1.0                1.00  1.16  1.16  1.02   92.20   2
   -operator all LShift 2                  1.00  1.17  1.16  1.03   92.80   2
   -operator all Multiply 0.5              1.00  1.83  2.50  3.05   38.48   4
   -operator all Or 233                    1.00  1.16  1.15  1.02   92.80   2
   -operator all RShift 2                  1.00  1.20  1.19  1.08   92.60   2
   -operator all Subtract 10%              1.00  1.83  2.51  3.12   37.80   4
   -operator red Threshold 50%             1.00  1.05  1.03  0.85   91.60   2
   -operator gray Threshold 50%            1.00  1.24  1.25  1.21   90.60   3
   -operator all Threshold-White 80%       1.00  1.22  1.20  0.96   99.20   2
   -operator all Threshold-Black 10%       1.00  1.27  1.27  1.11   96.80   3
   -operator all Xor 233                   1.00  1.20  1.22  1.11   91.20   3
   -operator all Noise-Gaussian 30%        1.00  1.98  2.96  3.89    3.08   4
   -operator all Noise-Impulse 30%         1.00  1.99  2.93  3.81   12.68   4
   -operator all Noise-Laplacian 30%       1.00  1.94  2.92  3.82    7.13   4
   -operator all Noise-Multiplicative 30%  1.00  1.99  2.94  3.86    4.04   4
   -operator all Noise-Poisson 30%         1.00  1.96  2.93  3.81    3.17   4
   -operator all Noise-Uniform 30%         1.00  1.96  2.90  3.74   13.56   4
   -ordered-dither all 2x2                 1.00  1.32  1.25  1.33   78.00   4
   -ordered-dither all 3x3                 1.00  1.40  1.44  1.55   81.80   4
   -ordered-dither intensity 3x3           1.00  1.36  1.35  1.48   78.60   4
   -ordered-dither all 4x4                 1.00  1.27  1.25  1.34   82.00   4
   -paint 0x1                              1.00  1.96  2.86  3.73    8.07   4
   -random-threshold all 20x80             1.00  1.76  2.41  3.00   39.80   4
   -density 75x75 -resample 50x50          1.00  1.87  2.19  2.43   11.29   4
   -resize 10%                             1.00  1.85  2.56  3.19   36.40   4
   -resize 50%                             1.00  1.86  2.13  2.48   14.90   4
   -resize 150%                            1.00  1.91  2.29  2.68    5.04   4
   -rotate 45                              1.00  1.61  1.99  2.16    2.10   4
   -segment 0.5x0.25                       1.00  1.39  1.64  1.78    0.18   4
   -shade 30x30                            1.00  1.82  2.46  3.02   29.40   4
   -sharpen 0x0.5                          1.00  2.05  2.94  3.75   17.89   4
   -sharpen 0x1.0                          1.00  1.92  2.80  3.76    6.89   4
   -shear 45x45                            1.00  1.51  1.56  1.53    3.18   3
   -solarize 50%                           1.00  1.32  1.35  1.41   90.20   4
   -swirl 90                               1.00  1.84  2.86  3.48    7.46   4
   -fuzz 35% -transparent red              1.00  1.45  1.66  1.86   79.00   4
   -trim                                   1.00  1.71  2.24  2.69   67.60   4
   -fuzz 5% -trim                          1.00  1.82  2.53  3.18   37.68   4
   -unsharp 0x0.5+20+1                     1.00  1.79  2.43  2.93    9.57   4
   -unsharp 0x1.0+20+1                     1.00  1.80  2.48  3.02    8.57   4
   -wave 25x150                            1.00  1.71  2.25  2.78    6.66   4
   ====================================== ===== ===== ===== ===== ======= ====

Windows XP / MinGW / Intel Core 2 Quad
--------------------------------------

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

   ====================================== ===== ===== ===== ===== ======= ====
   Operation                                1     2     3     4   iter/s  thds
   ====================================== ===== ===== ===== ===== ======= ====
   -noop                                   1.00  1.02  0.99  1.00  146.60   2
   -affine 1,0,0.785,1,0,0 -transform      1.00  1.84  1.91  2.31   13.16   4
   -blur 0x0.5                             1.00  1.45  1.71  1.88   17.24   4
   -blur 0x1.0                             1.00  1.61  1.99  2.29   13.96   4
   -charcoal 0x1                           1.00  1.64  2.09  2.44    7.44   4
   -colorspace CMYK                        1.00  1.38  1.45  1.56   85.60   4
   -colorspace GRAY                        1.00  1.59  1.93  2.24   68.60   4
   -colorspace HSL                         1.00  1.96  2.82  3.64   22.40   4
   -colorspace HWB                         1.00  1.93  2.74  3.42   31.70   4
   -colorspace OHTA                        1.00  1.62  1.95  2.29   70.20   4
   -colorspace YCbCr                       1.00  1.58  1.94  2.25   68.79   4
   -colorspace YIQ                         1.00  1.58  1.97  2.30   70.20   4
   -colorspace YUV                         1.00  1.58  1.93  2.24   68.80   4
   -contrast -contrast -contrast           1.00  2.27  3.52  4.73    4.40   4
   +contrast +contrast +contrast           1.00  2.25  3.57  4.78    4.47   4
   -convolve 1,1,1,1,4,1,1,1,1             1.00  1.79  2.50  3.15   29.00   4
   -colorize 30%/20%/50%                   1.00  1.85  2.38  2.86   42.80   4
   -despeckle                              1.00  1.48  2.83  2.82    1.73   3
   -edge 0x1                               1.00  1.79  2.54  3.16   26.04   4
   -emboss 0x1                             1.00  1.72  2.21  2.65   21.00   4
   -enhance                                1.00  2.03  3.05  4.05    3.18   4
   -gaussian 0x0.5                         1.00  1.79  2.50  3.13   29.00   4
   -gaussian 0x1.0                         1.00  2.04  2.97  3.85   10.00   4
   -implode 0.5                            1.00  2.08  3.14  3.95    5.27   4
   -implode -1                             1.00  2.07  3.00  3.76    5.00   4
   -lat 10x10-5%                           1.00  1.94  2.86  3.70    4.82   4
   -median 1                               1.00  1.98  2.89  2.57    1.30   3
   -median 2                               1.00  1.99  2.96  2.79    0.48   3
   -minify                                 1.00  1.74  2.32  2.67   53.44   4
   -modulate 110/100/95                    1.00  1.94  2.82  3.65   18.14   4
   +noise Uniform                          1.00  1.87  2.65  3.28   13.20   4
   +noise Gaussian                         1.00  2.88  4.66  6.46    2.60   4
   +noise Multiplicative                   1.00  2.61  4.23  5.71    4.00   4
   +noise Impulse                          1.00  1.88  2.62  3.28   14.20   4
   +noise Laplacian                        1.00  2.51  3.93  5.18    6.58   4
   +noise Poisson                          1.00  1.96  2.92  3.85    2.79   4
   -noise 1                                1.00  1.98  2.89  2.58    1.27   3
   -noise 2                                1.00  1.99  2.96  2.81    0.47   3
   -fill blue -fuzz 35% -opaque red        1.00  1.53  1.88  2.04   90.18   4
   -operator all Add 2%                    1.00  1.45  1.83  2.04   76.20   4
   -operator all And 233                   1.00  1.13  1.24  1.23   99.58   3
   -operator all Assign 50%                1.00  1.04  1.13  1.16  100.98   4
   -operator all Depth 6                   1.00  1.23  1.38  1.40   95.20   4
   -operator all Divide 2                  1.00  1.47  1.86  2.10   73.80   4
   -operator all Gamma 0.7                 1.00  1.14  1.23  1.28   96.60   4
   -operator all Negate 1.0                1.00  1.03  1.09  1.13  101.98   4
   -operator all LShift 2                  1.00  1.17  1.29  1.28   98.40   3
   -operator all Multiply 0.5              1.00  1.48  1.82  2.10   74.00   4
   -operator all Or 233                    1.00  1.13  1.24  1.23   99.80   3
   -operator all RShift 2                  1.00  1.12  1.24  1.29   99.00   4
   -operator all Subtract 10%              1.00  1.54  1.92  2.07   70.59   4
   -operator red Threshold 50%             1.00  1.05  1.08  1.12  102.60   4
   -operator gray Threshold 50%            1.00  1.19  1.29  1.28   97.58   3
   -operator all Threshold-White 80%       1.00  1.16  1.23  1.22  106.00   3
   -operator all Threshold-Black 10%       1.00  1.28  1.34  1.43  108.60   4
   -operator all Xor 233                   1.00  1.19  1.19  1.23   98.80   4
   -operator all Noise-Gaussian 30%        1.00  2.92  4.75  6.51    2.63   4
   -operator all Noise-Impulse 30%         1.00  1.96  2.85  3.68   16.60   4
   -operator all Noise-Laplacian 30%       1.00  2.57  4.10  5.52    7.07   4
   -operator all Noise-Multiplicative 30%  1.00  2.69  4.34  5.89    4.16   4
   -operator all Noise-Poisson 30%         1.00  2.02  3.03  4.02    2.87   4
   -operator all Noise-Uniform 30%         1.00  1.95  2.84  3.67   15.40   4
   -ordered-dither all 2x2                 1.00  1.43  1.53  1.68   79.60   4
   -ordered-dither all 3x3                 1.00  1.50  1.56  1.76   76.20   4
   -ordered-dither intensity 3x3           1.00  1.52  1.63  1.83   79.00   4
   -ordered-dither all 4x4                 1.00  1.46  1.45  1.62   77.20   4
   -paint 0x1                              1.00  1.95  2.88  3.75    9.74   4
   -random-threshold all 20x80             1.00  1.77  2.32  2.55   42.80   4
   -density 75x75 -resample 50x50          1.00  1.85  2.17  2.42   12.09   4
   -resize 10%                             1.00  1.83  2.60  3.24   30.00   4
   -resize 50%                             1.00  1.86  2.20  2.57   15.60   4
   -resize 150%                            1.00  1.88  2.14  2.51    5.93   4
   -rotate 45                              1.00  1.32  1.64  1.68    2.34   4
   -segment 0.5x0.25                       1.00  1.47  1.72  1.88    0.18   4
   -shade 30x30                            1.00  1.90  2.74  3.51   17.00   4
   -sharpen 0x0.5                          1.00  1.83  2.53  3.18   28.20   4
   -sharpen 0x1.0                          1.00  2.05  2.98  3.79    9.77   4
   -shear 45x45                            1.00  1.16  1.32  1.26    3.16   3
   -solarize 50%                           1.00  1.27  1.31  1.36   94.60   4
   -swirl 90                               1.00  2.08  3.15  4.36    6.11   4
   -fuzz 35% -transparent red              1.00  1.45  1.70  1.89   80.20   4
   -trim                                   1.00  1.79  2.43  2.94   50.60   4
   -fuzz 5% -trim                          1.00  1.86  2.60  3.24   35.20   4
   -unsharp 0x0.5+20+1                     1.00  1.56  1.93  2.18   14.20   4
   -unsharp 0x1.0+20+1                     1.00  1.62  2.12  2.50   11.92   4
   -wave 25x150                            1.00  1.57  2.29  2.86    5.35   4
   ====================================== ===== ===== ===== ===== ======= ====

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

