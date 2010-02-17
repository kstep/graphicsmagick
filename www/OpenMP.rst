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
<http://openmp.org/>`_ for the 1.3 release series. OpenMP is a
portable framework for accelerating CPU-bound and memory-bound
operations using multiple threads. OpenMP originates in the
super-computing world and has been available in one form or another
since the late '90s.

Since GCC 4.2 has introduced excellent OpenMP support, OpenMP has
become available to the masses. Microsoft Visual Studio Professional
2005 and 2008 support OpenMP so Windows users can benefit as well. Any
multi-CPU and/or multi-core system is potentially a good candidate for
use with OpenMP. Unfortunately, some older multi-CPU hardware is more
suitable for multi-processing than multi-threading. Modern multi-core
chipsets from AMD, Intel and Sun/Oracle perform very well with
OpenMP. The operating system makes a difference when it comes to
OpenMP acceleration, with IBM's AIX, Linux, and Sun's Solaris working
exceptionally well, and FreeBSD and Apple's OS-X working less well.

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
executes the algorithm twice as fast, and a four core system executes
the algorithm four times as fast. Memory-bound algorithms scale based
on the memory bandwith available to the cores. For example,
memory-bound algorithms scale up to almost 1.5X on my four core
Opteron system due to its NUMA architecture. Some systems/CPUs are
able to immediately context switch to another thread if the core would
be blocked waiting for memory, allowing multiple memory accesses to be
pending at once, and thereby improving throughput.  For example,
typical speedup of 20-32X (average 24X) has been observed on the Sun
SPARC T2 CPU, which provides 8 cores, with 8 virtual CPUs per core (64
threads).

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

Sun Solaris / UltraSPARC-T2
---------------------------

.. Last update: Thu Jan 28 20:36:10 EST 2010

A GraphicsMagick user has submitted results from their `UltraSPARC-T2
<http://en.wikipedia.org/wiki/UltraSPARC_T2>`_ based Sun Enterprise
T5120 system (1167 MHz clock rate).  This is a 1U server chassis
containing a single CPU module.  Each CPU supports 8 cores, with two
integer ALUs per core, one floating point unit per core, and 8
concurrent threads per core.  This means that the one CPU is able to
perform 24 computing operations (integer and floating point)
simultaneously, while maintaining hardware context for 64 threads. The
end result is a CPU with rather poor single-threaded performance, but
which really wakes up and performs for a multi-threaded program like
GraphicsMagick.

The UltraSPARC-T2 tested here is a bottom end model.  The more recent
T2 Plus supports up to four-way SMP (32 cores, 256 threads), and the
forthcoming T3 CPU will support 16-cores per CPU and is rumored to
support up to eight-way SMP.  It will be facinating to see how
GraphicsMagick scales with so many more cores and threads.

The following results are abbreviated to produce a smaller table, and
for clarity.

.. table:: Performance Boost On Eight Core UltraSPARC-T2

   =====================================================================  =====  =====  =====  =====  =====  =====  =====  ===== ===== ===== ===== ===== ===== ===== ===== =====  ===== ======= ====
   Operation                                                                2      4      6      8     10     12     14     16    17    18    19    20    21    22    23    24     64   iter/s  thds
   =====================================================================  =====  =====  =====  =====  =====  =====  =====  ===== ===== ===== ===== ===== ===== ===== ===== =====  ===== ======= ====
   -noop                                                                   1.75   3.59   3.83   4.34   4.65   4.76   4.82   4.79  4.76  4.69  4.74  4.69  4.65  4.53  4.44  4.35   2.59  215.20   9
   -affine 1,0,0.785,1,0,0 -transform                                      1.96   3.87   5.73   7.57   9.23  10.87  12.42  13.61 14.36 14.84 15.59 16.09 16.76 17.15 17.79 18.11  28.35    7.14  64
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                 1.90   3.60   5.10   6.38   7.45   8.26   8.97   9.32  9.58  9.50  9.81  9.98  9.86  9.98  9.91  9.86   7.47   78.60  20
   -blur 0x0.5                                                             1.94   3.47   4.37   5.00   5.45   5.84   6.13   6.33  6.45  6.48  6.55  6.59  6.67  6.70  6.75  6.79   7.08    5.76  47
   -blur 0x1.0                                                             1.94   3.59   4.74   5.66   6.31   6.87   7.37   7.70  7.79  7.91  8.02  8.12  8.20  8.21  8.35  8.47   9.16    5.15  55
   -charcoal 0x1                                                           1.89   3.44   4.62   5.55   6.26   6.84   7.33   7.70  7.77  7.94  8.12  8.18  8.32  8.38  8.46  8.58   9.50    2.68  58
   -colorspace CMYK                                                        1.95   3.82   5.44   6.91   8.19   9.29  10.51  10.89 11.19 11.29 11.76 11.76 11.83 11.90 11.76 11.65   9.41   66.40  29
   -colorspace GRAY                                                        1.97   3.88   5.72   7.45   8.74   9.78   9.95  10.83 11.19 10.88 11.63 11.85 11.51 12.34 12.02 12.53  16.02   44.91  58
   -colorspace HSL                                                         1.99   3.96   5.86   7.80   9.52  11.09  12.35  13.27 14.08 14.40 14.61 15.39 15.73 16.36 16.50 16.87  24.74   25.95  61
   -colorspace HWB                                                         1.96   3.89   5.79   7.59   9.25  10.83  12.32  13.47 13.93 14.11 14.64 15.42 15.78 15.96 16.46 17.21  21.56   26.55  52
   -colorspace OHTA                                                        1.96   3.88   5.67   7.39   8.71   9.64  10.22  10.97 11.49 11.44 11.56 11.68 11.97 11.95 12.05 12.51  15.70   44.82  57
   -colorspace YCbCr                                                       1.97   3.88   5.63   7.40   8.78   9.56  10.20  11.13 11.19 10.83 11.37 11.90 11.92 12.07 12.48 12.51  16.02   44.71  62
   -colorspace YIQ                                                         1.96   3.86   5.72   7.40   8.65   9.71   9.93  11.00 11.24 10.81 11.44 11.68 12.07 12.22 11.97 12.31  16.35   44.91  58
   -colorspace YUV                                                         1.96   3.85   5.68   7.34   8.85   9.96   9.82  10.93 10.95 10.83 11.41 11.46 11.70 12.63 12.36 12.66  16.13   44.71  55
   -contrast -contrast -contrast                                           1.99   4.01   5.95   7.98   9.70  11.30  13.03  14.12 14.49 14.68 15.46 16.42 16.65 17.34 17.79 18.24  31.00    5.92  63
   +contrast +contrast +contrast                                           1.99   3.98   5.96   7.94   9.62  11.39  12.84  13.62 14.58 14.84 15.86 16.38 16.89 17.03 17.54 17.84  30.74    5.93  61
   -convolve 1,1,1,1,4,1,1,1,1                                             1.99   3.93   5.85   7.71   9.38  10.88  12.36  13.63 14.30 14.84 15.30 15.82 16.16 16.82 17.03 17.56  22.83   18.65  62
   -colorize 30%/20%/50%                                                   1.97   3.90   5.71   7.48   9.10  10.55  11.96  12.94 13.75 14.24 14.35 14.90 15.25 15.45 16.01 16.72  14.47   39.60  31
   -despeckle                                                              1.46   2.81   2.81   2.81   2.81   2.81   2.81   2.81  2.81  2.81  2.81  2.80  2.81  2.81  2.81  2.81   2.79    0.27   3
   -fill none -stroke gold -draw 'circle 800,500 1100,800'                 1.98   3.90   5.70   7.38   8.87  10.19  11.41  12.42 12.84 13.17 13.56 13.82 14.11 14.38 14.55 14.73  14.11   15.02  32
   -fill green -stroke gold -draw 'circle 800,500 1100,800'                2.04   4.03   5.92   7.68   9.33  10.77  12.15  13.16 13.80 14.17 14.60 14.96 15.30 15.56 15.88 16.13  16.45   13.72  37
   -fill none -stroke gold -draw 'rectangle 400,200 1100,800'              1.98   3.91   5.77   7.56   9.24  10.69  12.28  13.31 13.89 14.28 14.91 15.12 15.61 16.03 16.27 16.73  19.63   37.65  62
   -fill blue -stroke gold -draw 'rectangle 400,200 1100,800'              1.98   3.93   5.83   7.65   9.45  10.98  12.58  13.74 14.44 14.95 15.62 15.86 16.53 16.73 17.14 17.68  22.40   33.27  60
   -fill none -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'   2.06   4.05   5.88   7.70   9.39  10.75  12.19  13.06 13.43 13.84 14.33 14.47 14.90 15.28 15.57 15.88  16.57   39.52  33
   -fill blue -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'   2.04   4.03   5.94   7.75   9.53  10.99  12.50  13.51 14.23 14.61 14.93 15.48 15.92 16.38 16.61 16.88  19.60   34.00  53
   -fill none -stroke gold -draw 'polygon 400,200 1100,800 100,300'        1.98   3.90   5.77   7.65   9.42  10.96  12.53  13.83 14.36 14.80 15.38 15.90 16.41 16.71 17.01 17.55  21.75   32.27  61
   -fill blue -stroke gold -draw 'polygon 400,200 1100,800 100,300'        1.98   3.94   5.85   7.70   9.47  11.06  12.74  13.97 14.54 14.97 15.57 16.03 16.71 17.14 17.35 17.78  22.46   31.27  62
   -fill none -stroke gold -draw 'Bezier 400,200 1100,800 100,300'         2.02   3.87   5.52   7.02   8.34   9.31  10.28  10.81 11.11 11.25 11.50 11.76 11.82 11.85 11.90 11.90  10.30   41.00  31
   -fill blue -stroke gold -draw 'Bezier 400,200 1100,800 100,300'         2.03   3.91   5.64   7.17   8.56   9.51  10.63  11.18 11.57 11.68 11.97 12.19 12.32 12.45 12.52 12.69  11.20   39.32  28
   -edge 0x1                                                               1.98   3.93   5.82   7.70   9.26  10.87  12.35  13.65 14.17 14.79 15.29 15.62 16.20 16.61 17.01 17.41  22.25   18.96  63
   -emboss 0x1                                                             1.83   3.19   4.22   5.03   5.61   6.11   6.48   6.84  6.95  7.10  7.17  7.30  7.34  7.49  7.50  7.59   8.19    5.72  51
   -enhance                                                                1.99   3.97   5.95   7.86   9.61  11.17  12.67  14.25 14.68 15.29 15.81 16.15 16.83 17.23 17.72 18.13  22.38    4.37  61
   -gaussian 0x0.5                                                         1.99   3.93   5.85   7.73   9.36  10.91  12.41  13.65 14.14 14.73 15.12 15.75 16.21 16.69 17.18 17.42  22.87   18.69  64
   -gaussian 0x1.0                                                         1.99   3.94   5.89   7.79   9.42  10.99  12.51  14.00 14.59 15.11 15.43 15.90 16.44 16.89 17.36 17.90  23.55    5.66  54
   -hald-clut identity:8                                                   1.98   3.93   5.88   7.80   9.30  10.92  12.04  12.73 13.45 13.51 13.73 13.98 15.28 15.07 15.44 16.05  24.27   17.76  61
   -hald-clut identity:10                                                  1.98   3.94   5.85   7.76   9.20  10.76  12.00  13.13 12.93 13.49 13.84 14.37 15.01 15.25 15.28 15.66  24.24   16.73  61
   -hald-clut identity:14                                                  1.97   3.95   5.88   7.74   9.36  10.98  12.56  13.60 14.20 14.80 15.00 15.84 16.52 16.94 17.35 17.49  25.31   10.56  57
   -implode 0.5                                                            2.00   3.98   5.97   7.91   9.76  11.59  13.41  15.01 15.73 16.33 16.82 17.55 17.92 18.58 19.23 19.58  29.57    9.34  64
   -implode -1                                                             2.01   3.98   5.96   7.91   9.74  11.53  13.38  15.02 15.56 16.20 16.85 17.41 18.12 18.65 19.25 19.62  29.50   10.24  64
   -lat 10x10-5%                                                           1.99   3.95   5.88   7.79   9.60  11.36  13.04  14.61 15.23 15.85 16.46 17.10 17.64 18.17 18.66 19.31  28.65    3.87  64
   -median 1                                                               1.99   3.92   5.84   7.58   9.30  10.78  12.31  13.49 13.91 14.20 14.46 14.64 14.91 14.99 15.35 15.57  14.08    1.24  33
   -median 2                                                               2.03   3.97   5.93   7.83   9.57  11.27  12.80  14.43 14.80 15.00 15.47 15.73 16.03 16.30 16.63 16.93  17.63    0.56  47
   -minify                                                                 1.96   3.93   5.65   7.45   8.90  10.02  11.26  12.46 12.57 12.88 13.26 13.68 14.18 14.51 14.59 14.59  15.19   36.00  44
   -modulate 110/100/95                                                    2.00   3.99   5.93   7.91   9.60  11.37  12.70  13.88 14.53 14.81 15.30 16.07 16.49 16.73 17.75 18.09  28.75   18.92  62
   +noise Uniform                                                          1.96   3.81   5.59   7.27   8.78  10.12  11.23  11.64 12.14 12.48 12.87 13.27 13.59 13.78 14.12 14.29  19.99   13.55  63
   +noise Gaussian                                                         1.97   3.89   5.79   7.68   9.35  10.99  12.20  13.30 13.30 14.08 14.38 14.93 15.63 16.27 16.59 17.05  27.13    5.94  64
   +noise Multiplicative                                                   1.98   3.89   5.78   7.68   9.38  11.15  12.61  12.99 13.45 14.12 14.64 15.25 15.54 16.20 16.95 17.58  27.47    6.48  64
   +noise Impulse                                                          1.96   3.84   5.58   7.29   8.77  10.21  11.38  12.14 12.33 12.76 13.46 13.62 13.97 14.40 14.74 15.00  20.91   13.29  63
   +noise Laplacian                                                        1.99   3.90   5.79   7.60   9.28  10.86  12.18  13.35 13.11 13.84 14.21 14.60 14.92 15.66 16.05 16.48  25.52    9.90  64
   +noise Poisson                                                          2.00   3.97   5.84   7.72   9.45  11.14  12.18  13.08 13.21 13.85 14.33 14.82 15.17 16.09 16.27 16.72  24.74    6.43  63
   -noise 1                                                                2.00   3.93   5.85   7.56   9.32  10.81  12.32  13.42 13.96 14.16 14.47 14.70 14.95 15.14 15.33 15.56  13.67    1.23  32
   -noise 2                                                                2.00   3.97   5.87   7.80   9.53  11.13  12.70  14.40 14.67 14.90 15.33 15.60 15.87 16.17 16.50 16.73  17.47    0.56  40
   -fill blue -fuzz 35% -opaque red                                        1.96   3.84   5.58   7.31   8.88   9.96  11.26  11.37 11.63 12.14 12.51 12.65 12.99 12.91 13.29 13.29  14.38   56.69  49
   -operator all Add 2%                                                    1.97   3.83   5.70   7.41   9.11  10.51  11.89  13.00 13.26 13.60 14.12 14.27 15.13 15.02 15.77 15.60  17.08   48.20  52
   -operator all And 233                                                   1.90   3.49   5.11   6.21   7.43   8.19   8.89   9.00  8.93  9.12  9.16  9.16  9.23  9.20  9.20  9.09   6.91   81.20  21
   -operator all Assign 50%                                                1.79   3.07   4.42   4.79   5.37   5.56   5.70   5.66  5.56  5.56  5.58  5.47  5.41  5.39  5.38  5.29   4.11   84.60  14
   -operator all Depth 6                                                   1.90   3.47   5.12   6.15   7.44   8.20   8.80   9.12  8.98  9.25  9.16  9.25  9.18  9.14  9.18  9.07   6.85   82.60  18
   -operator all Divide 2                                                  1.97   3.82   5.70   7.35   9.11  10.60  11.85  13.17 13.31 13.86 14.26 14.69 14.74 15.35 15.57 15.67  17.72   48.10  47
   -operator all Gamma 0.7                                                 1.90   3.45   5.06   6.12   7.41   8.18   8.76   9.07  9.00  9.18  9.13  9.16  9.09  9.18  9.22  9.02   6.83   82.20  23
   -operator all Negate 1.0                                                1.91   3.52   5.18   6.33   7.64   8.56   9.19   9.60  9.60  9.85  9.91  9.99 10.01 10.06  9.99 10.09   7.37   83.23  24
   -operator all LShift 2                                                  1.90   3.49   5.11   6.17   7.44   8.25   8.71   9.00  8.98  9.16  9.14  9.11  9.18  9.32  9.23  9.14   6.92   82.00  22
   -operator all Multiply 0.5                                              1.97   3.84   5.69   7.37   8.99  10.57  11.91  12.97 13.03 13.60 13.93 14.45 14.98 14.98 15.52 15.76  17.39   47.80  44
   -operator all Or 233                                                    1.90   3.48   5.13   6.23   7.41   8.23   8.78   9.03  8.95  9.16  9.14  9.16  9.02  9.25  9.18  9.05   6.90   81.40  22
   -operator all RShift 2                                                  1.90   3.48   5.10   6.19   7.48   8.17   8.75   9.00  8.91  9.14  9.20  9.07  9.14  9.30  9.23  9.18   6.91   81.80  22
   -operator all Subtract 10%                                              1.97   3.85   5.72   7.40   9.03  10.49  11.81  12.79 12.99 13.40 13.91 14.24 14.67 15.19 15.19 15.11  16.84   48.50  42
   -operator red Threshold 50%                                             1.83   3.21   4.69   5.40   6.45   6.94   7.35   7.53  7.26  7.54  7.53  7.49  7.43  7.38  7.43  7.22   4.76  111.80  18
   -operator gray Threshold 50%                                            1.94   3.60   5.32   6.49   7.89   8.37   9.11   8.71  8.18  8.37  8.77  8.88  8.94  8.86  8.89  9.13   8.51   66.00  33
   -operator all Threshold-White 80%                                       1.94   3.56   5.31   6.49   7.78   7.95   7.83   7.20  7.10  7.80  7.58  7.74  7.88  7.90  8.05  8.05   7.76   67.27  42
   -operator all Threshold-Black 10%                                       1.92   3.57   5.26   6.40   7.52   7.93   7.69   7.88  7.37  7.78  7.78  7.92  8.04  8.06  8.25  8.09   8.09   67.00  40
   -operator all Xor 233                                                   1.90   3.49   5.13   6.17   7.45   8.20   8.73   8.94  8.98  9.23  9.14  9.16  9.11  9.23  9.30  9.00   6.93   81.80  23
   -operator all Noise-Gaussian 30%                                        1.99   3.95   5.93   7.87   9.61  11.43  12.60  13.32 13.99 14.55 14.86 15.89 16.36 16.58 17.27 17.35  30.13    6.63  64
   -operator all Noise-Impulse 30%                                         1.98   3.97   5.87   7.80   9.67  11.18  12.30  13.36 13.72 14.18 14.18 14.69 15.25 15.53 16.02 16.25  26.46   17.30  60
   -operator all Noise-Laplacian 30%                                       1.99   3.96   5.90   7.88   9.56  11.28  12.76  13.22 14.20 14.34 15.16 15.45 16.29 16.88 17.18 17.69  29.70   11.83  63
   -operator all Noise-Multiplicative 30%                                  1.98   3.98   5.96   7.92   9.70  11.30  13.08  14.32 13.82 14.81 14.86 16.11 16.61 17.27 17.17 17.54  29.84    7.23  63
   -operator all Noise-Poisson 30%                                         1.98   3.95   5.89   7.86   9.67  11.30  12.86  13.39 13.71 14.28 14.71 15.09 15.66 16.00 16.34 17.42  27.38    7.23  64
   -operator all Noise-Uniform 30%                                         1.99   4.00   5.90   7.86   9.60  11.19  12.32  13.17 13.52 13.31 13.71 14.52 15.41 15.32 15.67 15.94  25.43   17.62  61
   -ordered-dither all 2x2                                                 1.91   3.66   5.24   6.62   7.71   8.88   9.24   9.56  9.67  9.78 10.00 10.04 10.08 10.14 10.17 10.17   8.36   74.40  25
   -ordered-dither all 3x3                                                 1.93   3.77   5.41   6.98   8.28   9.47  10.17  10.71 11.04 11.34 11.41 11.59 11.83 11.96 11.93 11.99  10.53   66.07  25
   -ordered-dither intensity 3x3                                           1.93   3.77   5.40   6.98   8.26   9.58  10.20  10.90 11.15 11.28 11.48 11.63 11.75 11.97 11.92 12.03  10.50   66.20  27
   -ordered-dither all 4x4                                                 1.93   3.71   5.28   6.64   7.81   8.98   9.34   9.62  9.76  9.89 10.06 10.19 10.19 10.31 10.28 10.31   8.50   74.40  27
   -paint 0x1                                                              2.00   3.97   5.92   7.84   9.47  11.03  12.62  13.89 14.45 14.99 15.30 16.03 16.23 16.52 17.23 17.61  21.44    9.54  50
   -random-threshold all 20x80                                             1.96   3.83   5.64   7.34   8.81  10.28  11.19  11.92 12.43 12.67 12.90 13.11 13.31 13.42 13.54 14.27  13.22   49.60  29
   -recolor '1,0,0,0,1,0,0,0,1'                                            1.76   2.97   3.83   4.31   4.62   4.71   5.20   4.73  4.98  4.69  4.69  4.68  4.58  4.47  4.39  4.31   2.58  221.00  13
   -recolor '0,0,1,0,1,0,1,0,0'                                            1.98   3.93   5.80   7.62   9.02  10.90  11.67  11.74 11.92 11.74 12.47 12.70 12.70 13.40 13.40 13.72  20.72   31.20  63
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                      1.98   3.94   5.86   7.68   9.29  10.70  11.42  12.11 12.64 12.32 12.46 12.91 13.35 13.45 14.00 14.13  21.65   26.69  58
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                          1.98   3.94   5.84   7.73   9.22  10.70  11.53  12.16 12.42 12.32 12.67 12.86 13.70 13.40 13.51 13.94  21.57   26.89  63
   -density 75x75 -resample 50x50                                          1.98   3.90   5.81   7.59   8.70  10.38  11.58  11.44 11.89 11.89 12.40 12.23 13.07 13.28 13.20 13.76  20.29   12.01  63
   -resize 10%                                                             1.98   3.89   5.78   7.26   8.57  10.02  10.94  11.47 11.64 12.11 12.47 12.52 12.81 13.52 13.54 13.68  20.88   21.87  60
   -resize 50%                                                             1.98   3.88   5.77   7.49   8.72   9.69  10.39  10.86 11.72 11.84 12.26 12.19 12.59 12.74 13.09 13.37  19.38   14.20  62
   -resize 150%                                                            1.99   3.91   5.79   7.75   8.58  10.50  12.04  12.63 12.33 12.77 13.12 13.50 13.92 14.12 14.64 14.75  23.20    6.11  63
   -rotate 15                                                              1.87   3.38   4.59   5.60   6.40   6.97   7.51   7.56  7.64  7.77  7.83  7.78  7.86  7.88  7.88  7.83   7.80    2.35  45
   -rotate 45                                                              1.74   2.79   3.48   3.97   4.33   4.60   4.77   4.85  4.86  4.91  4.90  4.92  4.95  4.97  4.88  4.94   4.99    0.73  61
   -segment 0.5x0.25                                                       1.25   1.50   1.50   1.50   1.62   1.62   1.62   1.62  1.62  1.62  1.62  1.62  1.62  1.62  1.62  1.62   1.62    0.01  10
   -shade 30x30                                                            1.98   3.91   5.82   7.67   9.36  10.97  12.68  13.55 14.24 14.76 15.04 15.75 15.94 16.26 16.58 16.97  19.67   20.56  49
   -sharpen 0x0.5                                                          1.98   3.93   5.85   7.71   9.30  10.88  12.36  13.60 14.09 14.97 15.42 15.72 16.24 16.61 16.97 17.38  22.68   18.65  62
   -sharpen 0x1.0                                                          1.98   3.95   5.90   7.80   9.45  11.03  12.48  13.92 14.51 15.20 15.55 16.00 16.37 16.93 17.26 17.83  23.60    5.67  58
   -shear 45x45                                                            1.64   2.44   2.90   3.16   3.37   3.53   3.63   3.69  3.72  3.68  3.73  3.74  3.73  3.76  3.77  3.76   3.72    1.10  23
   -solarize 50%                                                           1.94   3.72   5.33   6.78   7.98   8.85   9.70  10.17 10.44 10.46 10.69 10.78 11.01 11.11 11.17 11.14   8.69   78.44  27
   -swirl 90                                                               1.96   3.93   5.86   7.78   9.55  11.34  13.04  14.37 15.37 15.83 16.42 16.95 17.62 18.13 18.55 19.25  28.70   11.22  64
   -fuzz 35% -transparent red                                              1.92   3.78   5.64   7.21   8.73   9.84  10.89  11.49 11.56 11.56 12.61 12.29 12.67 12.77 12.45 12.80  13.74   54.20  52
   -trim                                                                   1.98   3.94   5.81   7.64   9.25  10.87  12.21  13.48 14.08 14.49 14.98 15.44 16.00 16.27 16.68 17.29  20.92   34.06  52
   -fuzz 5% -trim                                                          2.00   3.95   5.93   7.78   9.60  11.23  12.76  14.47 15.10 15.58 16.38 17.08 17.36 18.01 18.51 19.04  27.01   28.14  59
   -unsharp 0x0.5+20+1                                                     1.95   3.57   4.66   5.58   6.20   6.73   7.21   7.48  7.65  7.74  7.92  7.98  8.04  8.17  8.25  8.33   8.45    5.07  30
   -unsharp 0x1.0+20+1                                                     1.95   3.67   4.93   5.97   6.75   7.47   8.11   8.58  8.74  8.87  9.05  9.08  9.33  9.46  9.55  9.67  10.22    4.54  45
   -wave 25x150                                                            1.98   3.85   5.28   6.49   7.48   8.51   9.51  10.29 10.67 11.04 11.46 11.75 11.98 12.33 12.61 12.84  15.14    5.48  48
   =====================================================================  =====  =====  =====  =====  =====  =====  =====  ===== ===== ===== ===== ===== ===== ===== ===== =====  ===== ======= ====


Sun Solaris / AMD Opteron
-------------------------

.. Last update: Mon Jul 20 16:39:33 CDT 2009

The following table shows the performance boost in GraphicsMagick
1.4 as threads are added on a four-core AMD Opteron 3.0GHz system
running Sun Solaris 10:

.. table:: Performance Boost On Four Core AMD Operon System

   ======================================================= ===== ===== ===== ===== ======= ====
   Operation                                                 1     2     3     4   iter/s  thds
   ======================================================= ===== ===== ===== ===== ======= ====
   -noop                                                    1.00  1.14  1.31  1.33  178.60   4
   -affine 1,0,0.785,1,0,0 -transform                       1.00  1.53  2.16  2.65    4.62   4
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01  1.00  1.79  2.44  2.75   51.20   4
   -blur 0x0.5                                              1.00  1.69  2.16  2.34   11.55   4
   -blur 0x1.0                                              1.00  1.81  2.42  2.74    8.73   4
   -charcoal 0x1                                            1.00  1.78  2.45  2.90    4.08   4
   -colorspace CMYK                                         1.00  1.44  1.70  1.81   88.60   4
   -colorspace GRAY                                         1.00  1.74  2.39  2.94   55.09   4
   -colorspace HSL                                          1.00  1.94  2.83  3.66   17.93   4
   -colorspace HWB                                          1.00  1.91  2.78  3.56   27.69   4
   -colorspace OHTA                                         1.00  1.77  2.43  3.01   54.20   4
   -colorspace YCbCr                                        1.00  1.77  2.46  2.97   53.49   4
   -colorspace YIQ                                          1.00  1.76  2.43  2.98   53.69   4
   -colorspace YUV                                          1.00  1.77  2.45  2.98   53.20   4
   -contrast -contrast -contrast                            1.00  1.99  2.96  3.88    3.08   4
   +contrast +contrast +contrast                            1.00  1.98  2.95  3.88    3.37   4
   -convolve 1,1,1,1,4,1,1,1,1                              1.00  1.87  2.68  3.19   20.64   4
   -colorize 30%/20%/50%                                    1.00  1.83  2.56  3.22   49.00   4
   -despeckle                                               1.00  1.49  2.83  2.84    1.73   4
   -edge 0x1                                                1.00  1.94  2.85  3.71   12.35   4
   -emboss 0x1                                              1.00  1.79  2.44  2.99    8.75   4
   -enhance                                                 1.00  1.99  2.96  3.86    2.59   4
   -gaussian 0x0.5                                          1.00  1.94  2.84  3.70   12.85   4
   -gaussian 0x1.0                                          1.00  1.96  2.91  3.73    5.13   4
   -hald-clut identity:8                                    1.00  1.96  2.89  3.76   12.87   4
   -hald-clut identity:10                                   1.00  1.95  2.86  3.73   12.35   4
   -hald-clut identity:14                                   1.00  1.97  2.90  3.80    7.63   4
   -implode 0.5                                             1.00  1.61  2.35  3.02    4.15   4
   -implode -1                                              1.00  1.59  2.24  2.79    4.76   4
   -lat 10x10-5%                                            1.00  1.97  2.88  3.77    3.86   4
   -median 1                                                1.00  1.99  2.95  3.85    1.62   4
   -median 2                                                1.00  1.99  2.96  3.87    0.60   4
   -minify                                                  1.00  1.77  2.48  3.02   48.60   4
   -modulate 110/100/95                                     1.00  1.95  2.84  3.69   14.54   4
   +noise Uniform                                           1.00  1.91  2.72  3.40   13.32   4
   +noise Gaussian                                          1.00  1.99  2.96  3.89    1.75   4
   +noise Multiplicative                                    1.00  2.02  2.96  3.88    2.76   4
   +noise Impulse                                           1.00  1.88  2.64  3.41   13.83   4
   +noise Laplacian                                         1.00  1.96  2.87  3.73    6.04   4
   +noise Poisson                                           1.00  1.96  2.86  3.75    3.90   4
   -noise 1                                                 1.00  1.98  2.93  3.85    1.62   4
   -noise 2                                                 1.00  1.99  2.97  3.92    0.60   4
   -fill blue -fuzz 35% -opaque red                         1.00  1.52  2.03  2.40   95.80   4
   -operator all Add 2%                                     1.00  1.69  2.29  2.24   60.00   3
   -operator all And 233                                    1.00  1.24  1.57  1.54   98.20   3
   -operator all Assign 50%                                 1.00  1.20  1.44  1.48  101.80   4
   -operator all Depth 6                                    1.00  1.55  1.97  2.27   82.00   4
   -operator all Divide 2                                   1.00  1.74  2.36  2.82   66.20   4
   -operator all Gamma 0.7                                  1.00  1.37  1.59  1.71   52.10   4
   -operator all Negate 1.0                                 1.00  1.22  1.51  1.47   96.60   3
   -operator all LShift 2                                   1.00  1.33  1.61  1.76  100.80   4
   -operator all Multiply 0.5                               1.00  1.70  2.32  2.69   70.80   4
   -operator all Or 233                                     1.00  1.24  1.62  1.64  100.40   4
   -operator all RShift 2                                   1.00  1.27  1.54  1.62   96.20   4
   -operator all Subtract 10%                               1.00  1.72  2.35  2.82   70.20   4
   -operator red Threshold 50%                              1.00  1.16  1.41  1.51  100.80   4
   -operator gray Threshold 50%                             1.00  1.33  1.68  1.72   98.00   4
   -operator all Threshold-White 80%                        1.00  1.29  1.67  1.79  112.20   4
   -operator all Threshold-Black 10%                        1.00  1.39  1.81  1.89  107.80   4
   -operator all Xor 233                                    1.00  1.20  1.41  1.53  100.40   4
   -operator all Noise-Gaussian 30%                         1.00  1.99  2.98  3.93    1.79   4
   -operator all Noise-Impulse 30%                          1.00  1.94  2.86  3.62   15.00   4
   -operator all Noise-Laplacian 30%                        1.00  1.97  2.92  3.83    6.35   4
   -operator all Noise-Multiplicative 30%                   1.00  2.00  3.02  3.97    2.85   4
   -operator all Noise-Poisson 30%                          1.00  1.99  2.95  3.84    4.01   4
   -operator all Noise-Uniform 30%                          1.00  1.94  2.81  3.66   15.17   4
   -ordered-dither all 2x2                                  1.00  1.50  1.85  2.02   84.20   4
   -ordered-dither all 3x3                                  1.00  1.61  2.10  2.40   81.00   4
   -ordered-dither intensity 3x3                            1.00  1.60  2.07  2.37   81.20   4
   -ordered-dither all 4x4                                  1.00  1.48  1.93  1.91   79.20   3
   -paint 0x1                                               1.00  1.97  2.93  3.84    5.69   4
   -random-threshold all 20x80                              1.00  1.75  2.46  3.01   50.60   4
   -recolor '1,0,0,0,1,0,0,0,1'                             1.00  1.22  1.42  1.29  184.60   3
   -recolor '0,0,1,0,1,0,1,0,0'                             1.00  1.80  2.49  3.10   51.60   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                       1.00  1.83  2.58  2.92   40.44   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'           1.00  1.83  2.55  3.22   44.71   4
   -density 75x75 -resample 50x50                           1.00  1.88  2.74  3.42   12.60   4
   -resize 10%                                              1.00  1.86  2.66  3.37   24.40   4
   -resize 50%                                              1.00  1.91  2.76  3.52   18.20   4
   -resize 150%                                             1.00  1.84  2.67  3.52    3.67   4
   -rotate 45                                               1.00  1.58  2.29  2.87    2.13   4
   -segment 0.5x0.25                                        1.00  1.42  1.64  1.75    0.17   4
   -shade 30x30                                             1.00  1.91  2.76  3.57   14.60   4
   -sharpen 0x0.5                                           1.00  1.94  2.85  3.70   12.80   4
   -sharpen 0x1.0                                           1.00  1.97  2.92  3.75    5.13   4
   -shear 45x45                                             1.00  1.40  1.99  2.46    3.24   4
   -solarize 50%                                            1.00  1.41  1.78  1.86   98.20   4
   -swirl 90                                                1.00  1.64  2.36  3.01    5.08   4
   -fuzz 35% -transparent red                               1.00  1.60  2.10  2.50   87.20   4
   -trim                                                    1.00  1.83  2.60  3.18   45.11   4
   -fuzz 5% -trim                                           1.00  1.89  2.71  3.17   29.48   4
   -unsharp 0x0.5+20+1                                      1.00  1.75  2.28  2.57    9.92   4
   -unsharp 0x1.0+20+1                                      1.00  1.82  2.50  2.92    7.91   4
   -wave 25x150                                             1.00  1.53  1.63  1.78    3.54   4
   ======================================================= ===== ===== ===== ===== ======= ====

Sun Solaris / UltraSPARC III
----------------------------

.. Last update: Mon Jul 20 16:41:33 CDT 2009

The following table shows the performance boost as threads are added
on 2 CPU Sun SPARC 1.2GHz workstation running Sun Solaris 10.  This
system obtains quite substantial benefit for most key algorithms:

.. table:: Performance Boost On Two CPU SPARC System

   ======================================================= ===== ===== ======= ====
   Operation                                                 1     2   iter/s  thds
   ======================================================= ===== ===== ======= ====
   -noop                                                    1.00  0.99   27.83   1
   -affine 1,0,0.785,1,0,0 -transform                       1.00  1.84    0.84   2
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01  1.00  1.41    6.79   2
   -blur 0x0.5                                              1.00  1.53    1.35   2
   -blur 0x1.0                                              1.00  1.68    1.16   2
   -charcoal 0x1                                            1.00  1.61    0.46   2
   -colorspace CMYK                                         1.00  1.20    9.47   2
   -colorspace GRAY                                         1.00  1.67    5.72   2
   -colorspace HSL                                          1.00  1.83    2.58   2
   -colorspace HWB                                          1.00  1.85    3.03   2
   -colorspace OHTA                                         1.00  1.59    5.52   2
   -colorspace YCbCr                                        1.00  1.68    5.73   2
   -colorspace YIQ                                          1.00  1.72    5.73   2
   -colorspace YUV                                          1.00  1.67    5.71   2
   -contrast -contrast -contrast                            1.00  1.91    0.45   2
   +contrast +contrast +contrast                            1.00  1.96    0.49   2
   -convolve 1,1,1,1,4,1,1,1,1                              1.00  1.83    2.16   2
   -colorize 30%/20%/50%                                    1.00  1.55    5.30   2
   -despeckle                                               1.00  1.48    0.16   2
   -edge 0x1                                                1.00  1.88    1.29   2
   -emboss 0x1                                              1.00  1.72    0.95   2
   -enhance                                                 1.00  1.91    0.44   2
   -gaussian 0x0.5                                          1.00  1.89    1.21   2
   -gaussian 0x1.0                                          1.00  1.96    0.48   2
   -hald-clut identity:8                                    1.00  1.87    1.98   2
   -hald-clut identity:10                                   1.00  1.89    1.83   2
   -hald-clut identity:14                                   1.00  1.90    0.91   2
   -implode 0.5                                             1.00  1.93    0.89   2
   -implode -1                                              1.00  1.92    1.12   2
   -lat 10x10-5%                                            1.00  2.00    0.49   2
   -median 1                                                1.00  1.95    0.32   2
   -median 2                                                1.00  1.97    0.13   2
   -minify                                                  1.00  1.35    4.59   2
   -modulate 110/100/95                                     1.00  1.89    1.68   2
   +noise Uniform                                           1.00  1.79    1.71   2
   +noise Gaussian                                          1.00  1.85    0.30   2
   +noise Multiplicative                                    1.00  1.92    0.50   2
   +noise Impulse                                           1.00  1.71    1.55   2
   +noise Laplacian                                         1.00  1.85    0.88   2
   +noise Poisson                                           1.00  1.91    0.54   2
   -noise 1                                                 1.00  1.91    0.31   2
   -noise 2                                                 1.00  1.91    0.13   2
   -fill blue -fuzz 35% -opaque red                         1.00  1.77    7.44   2
   -operator all Add 2%                                     1.00  1.74    5.93   2
   -operator all And 233                                    1.00  1.06   10.93   2
   -operator all Assign 50%                                 1.00  1.03   11.11   2
   -operator all Depth 6                                    1.00  1.14   10.52   2
   -operator all Divide 2                                   1.00  1.79    4.77   2
   -operator all Gamma 0.7                                  1.00  1.09    8.33   2
   -operator all Negate 1.0                                 1.00  1.11   11.04   2
   -operator all LShift 2                                   1.00  1.03   10.74   2
   -operator all Multiply 0.5                               1.00  1.77    5.94   2
   -operator all Or 233                                     1.00  1.10   11.02   2
   -operator all RShift 2                                   1.00  1.09   11.00   2
   -operator all Subtract 10%                               1.00  1.72    6.10   2
   -operator red Threshold 50%                              1.00  1.06   11.00   2
   -operator gray Threshold 50%                             1.00  1.30   10.80   2
   -operator all Threshold-White 80%                        1.00  1.46   11.78   2
   -operator all Threshold-Black 10%                        1.00  1.46   11.78   2
   -operator all Xor 233                                    1.00  1.09   11.00   2
   -operator all Noise-Gaussian 30%                         1.00  1.98    0.32   2
   -operator all Noise-Impulse 30%                          1.00  1.85    1.84   2
   -operator all Noise-Laplacian 30%                        1.00  1.95    0.96   2
   -operator all Noise-Multiplicative 30%                   1.00  1.97    0.52   2
   -operator all Noise-Poisson 30%                          1.00  1.93    0.56   2
   -operator all Noise-Uniform 30%                          1.00  1.86    1.93   2
   -ordered-dither all 2x2                                  1.00  1.40    9.16   2
   -ordered-dither all 3x3                                  1.00  1.45    8.78   2
   -ordered-dither intensity 3x3                            1.00  1.30    8.00   2
   -ordered-dither all 4x4                                  1.00  1.36    9.11   2
   -paint 0x1                                               1.00  1.94    0.75   2
   -random-threshold all 20x80                              1.00  1.64    5.94   2
   -recolor '1,0,0,0,1,0,0,0,1'                             1.00  0.99   27.69   1
   -recolor '0,0,1,0,1,0,1,0,0'                             1.00  1.76    5.01   2
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                       1.00  1.80    4.15   2
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'           1.00  1.81    4.14   2
   -density 75x75 -resample 50x50                           1.00  1.77    1.54   2
   -resize 10%                                              1.00  1.73    3.03   2
   -resize 50%                                              1.00  1.79    2.08   2
   -resize 150%                                             1.00  1.71    0.55   2
   -rotate 45                                               1.00  1.54    0.30   2
   -segment 0.5x0.25                                        1.00  1.41    0.02   2
   -shade 30x30                                             1.00  1.87    1.96   2
   -sharpen 0x0.5                                           1.00  1.85    1.19   2
   -sharpen 0x1.0                                           1.00  1.94    0.48   2
   -shear 45x45                                             1.00  1.22    0.48   2
   -solarize 50%                                            1.00  1.11   10.78   2
   -swirl 90                                                1.00  1.90    1.09   2
   -fuzz 35% -transparent red                               1.00  1.63    6.94   2
   -trim                                                    1.00  1.83    3.73   2
   -fuzz 5% -trim                                           1.00  1.98    2.99   2
   -unsharp 0x0.5+20+1                                      1.00  1.64    1.18   2
   -unsharp 0x1.0+20+1                                      1.00  1.70    0.99   2
   -wave 25x150                                             1.00  1.83    1.11   2
   ======================================================= ===== ===== ======= ====

IBM AIX / IBM Power5+
---------------------

.. Last update: Mon Jul 20 19:15:49 CDT 2009

The following table shows the boost on a four core IBM P5+ server
system (IBM System p5 505 Express with (2) 2.1Ghz CPUs) running AIX:

.. table:: Performance Boost On Four Core IBM P5+ System

   ======================================================= ===== ===== ===== ===== ======= ====
   Operation                                                 1     2     3     4   iter/s  thds
   ======================================================= ===== ===== ===== ===== ======= ====
   -noop                                                    1.00  1.56  1.66  1.75  290.60   4
   -affine 1,0,0.785,1,0,0 -transform                       1.00  1.96  2.54  3.13    2.48   4
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01  1.00  1.64  2.41  2.78   49.70   4
   -blur 0x0.5                                              1.00  1.97  2.50  3.03    2.71   4
   -blur 0x1.0                                              1.00  1.95  2.67  3.30    2.13   4
   -charcoal 0x1                                            1.00  1.95  2.68  3.35    0.96   4
   -colorspace CMYK                                         1.00  1.52  1.48  1.59   67.00   4
   -colorspace GRAY                                         1.00  1.70  2.47  2.93   17.17   4
   -colorspace HSL                                          1.00  1.59  2.38  2.97    8.20   4
   -colorspace HWB                                          1.00  1.94  2.56  2.88    7.84   4
   -colorspace OHTA                                         1.00  1.87  2.42  2.94   17.20   4
   -colorspace YCbCr                                        1.00  1.30  2.49  3.04   17.20   4
   -colorspace YIQ                                          1.00  1.90  2.29  2.92   17.10   4
   -colorspace YUV                                          1.00  1.92  2.50  3.04   17.20   4
   -contrast -contrast -contrast                            1.00  1.99  2.50  2.97    2.09   4
   +contrast +contrast +contrast                            1.00  1.99  2.50  2.99    2.15   4
   -convolve 1,1,1,1,4,1,1,1,1                              1.00  2.03  2.84  3.60    3.79   4
   -colorize 30%/20%/50%                                    1.00  1.99  2.77  3.55   11.75   4
   -despeckle                                               1.00  1.49  2.79  2.81    0.27   4
   -edge 0x1                                                1.00  0.42  3.31  3.07    3.00   3
   -emboss 0x1                                              1.00  1.81  2.39  2.27    1.20   3
   -enhance                                                 1.00  2.10  2.93  3.08    0.74   4
   -gaussian 0x0.5                                          1.00  2.05  2.91  3.40    1.79   4
   -gaussian 0x1.0                                          1.00  2.00  2.86  3.70    0.79   4
   -implode 0.5                                             1.00  2.19  2.42  3.17    2.83   4
   -implode -1                                              1.00  1.72  2.20  3.08    3.00   4
   -lat 10x10-5%                                            1.00  2.00  2.26  2.42    1.13   4
   -median 1                                                1.00  2.03  1.32  2.86    0.79   4
   -median 2                                                1.00  1.99  2.40  2.81    0.30   4
   -minify                                                  1.00  1.97  2.83  3.67    9.72   4
   -modulate 110/100/95                                     1.00  2.56  3.33  3.81    6.57   4
   +noise Uniform                                           1.00  2.06  2.61  3.43    5.19   4
   +noise Gaussian                                          1.00  1.99  2.53  2.75    1.83   4
   +noise Multiplicative                                    1.00  1.99  2.72  3.46    2.32   4
   +noise Impulse                                           1.00  1.79  2.60  3.23    5.27   4
   +noise Laplacian                                         1.00  1.82  2.55  3.44    3.51   4
   +noise Poisson                                           1.00  1.78  2.74  3.32    2.11   4
   -noise 1                                                 1.00  1.99  2.41  2.81    0.79   4
   -noise 2                                                 1.00  2.00  2.39  2.69    0.29   4
   -fill blue -fuzz 35% -opaque red                         1.00  1.03  2.20  2.72   36.40   4
   -operator all Add 2%                                     1.00  2.69  1.49  5.30   14.82   4
   -operator all And 233                                    1.00  1.70  1.64  1.92  160.20   4
   -operator all Assign 50%                                 1.00  1.30  1.87  1.84  166.40   3
   -operator all Depth 6                                    1.00  1.75  1.81  1.88  134.00   4
   -operator all Divide 2                                   1.00  1.98  2.79  3.59   15.64   4
   -operator all Gamma 0.7                                  1.00  1.27  1.64  1.63  102.20   3
   -operator all Negate 1.0                                 1.00  1.68  1.50  1.70  149.00   4
   -operator all LShift 2                                   1.00  1.68  1.75  1.77  145.20   4
   -operator all Multiply 0.5                               1.00  1.98  2.71  3.59   15.57   4
   -operator all Or 233                                     1.00  1.71  1.73  1.88  165.20   4
   -operator all RShift 2                                   1.00  1.25  1.79  1.84  163.00   4
   -operator all Subtract 10%                               1.00  1.96  2.72  3.49   16.73   4
   -operator red Threshold 50%                              1.00  1.70  1.93  2.05  163.40   4
   -operator gray Threshold 50%                             1.00  1.82  1.90  2.03  113.00   4
   -operator all Threshold-White 80%                        1.00  1.85  1.97  2.08  117.60   4
   -operator all Threshold-Black 10%                        1.00  1.78  1.97  2.17  117.00   4
   -operator all Xor 233                                    1.00  1.71  1.74  1.86  164.00   4
   -operator all Noise-Gaussian 30%                         1.00  1.95  2.56  3.11    2.09   4
   -operator all Noise-Impulse 30%                          1.00  1.97  2.65  3.36    5.54   4
   -operator all Noise-Laplacian 30%                        1.00  2.00  2.80  3.60    3.70   4
   -operator all Noise-Multiplicative 30%                   1.00  1.95  2.73  3.49    2.35   4
   -operator all Noise-Poisson 30%                          1.00  2.00  2.74  3.33    2.12   4
   -operator all Noise-Uniform 30%                          1.00  1.95  2.69  3.52    5.40   4
   -ordered-dither all 2x2                                  1.00  1.50  1.62  1.39   53.49   3
   -ordered-dither all 3x3                                  1.00  1.17  1.53  1.54   42.60   4
   -ordered-dither intensity 3x3                            1.00  1.06  1.93  2.20   48.00   4
   -ordered-dither all 4x4                                  1.00  1.67  0.31  1.29   53.20   2
   -paint 0x1                                               1.00  1.71  2.18  2.18    5.64   3
   -random-threshold all 20x80                              1.00  1.90  2.36  2.43   19.40   4
   -recolor '1,0,0,0,1,0,0,0,1'                             1.00  1.49  1.72  1.44  226.80   3
   -recolor '0,0,1,0,1,0,1,0,0'                             1.00  1.89  2.60  3.26   10.63   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                       1.00  1.74  2.58  3.19    7.16   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'           1.00  1.81  2.02  3.01    6.57   4
   -density 75x75 -resample 50x50                           1.00  1.82  2.33  2.86    3.42   4
   -resize 10%                                              1.00  2.41  3.17  3.43    7.50   4
   -resize 50%                                              1.00  3.15  4.35  5.30    4.93   4
   -resize 150%                                             1.00  1.62  2.29  2.69    1.28   4
   -rotate 45                                               1.00  1.68  0.24  1.60    0.45   2
   -segment 0.5x0.25                                        1.00  1.12  1.21  1.25    0.03   4
   -shade 30x30                                             1.00  2.30  2.65  2.95    8.10   4
   -sharpen 0x0.5                                           1.00  1.91  2.81  3.53    1.89   4
   -sharpen 0x1.0                                           1.00  1.85  2.79  3.66    0.78   4
   -shear 45x45                                             1.00  1.61  2.06  2.03    1.16   3
   -solarize 50%                                            1.00  1.73  2.18  2.32   96.40   4
   -swirl 90                                                1.00  1.86  2.38  2.88    3.09   4
   -fuzz 35% -transparent red                               1.00  2.14  2.29  2.59   31.20   4
   -trim                                                    1.00  1.92  2.30  2.57   23.06   4
   -fuzz 5% -trim                                           1.00  1.91  2.61  3.21    8.96   4
   -unsharp 0x0.5+20+1                                      1.00  1.92  1.16  0.12    1.35   2
   -unsharp 0x1.0+20+1                                      1.00  1.85  2.65  3.39    1.83   4
   -wave 25x150                                             1.00  1.71  1.84  2.83    2.62   4
   ======================================================= ===== ===== ===== ===== ======= ====

Apple OS-X/IBM G5
-----------------

.. Last update: Mon Jul 20 16:46:35 CDT 2009

The following table shows the boost on a two core Apple PowerPC G5
system (2.5GHz) running OS-X Leopard:

.. table:: Performance Boost On Two Core PowerPC G5 System

   ======================================================= ===== ===== ======= ====
   Operation                                                 1     2   iter/s  thds
   ======================================================= ===== ===== ======= ====
   -noop                                                    1.00  1.03   24.25   2
   -affine 1,0,0.785,1,0,0 -transform                       1.00  1.68    1.76   2
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01  1.00  1.25   14.17   2
   -blur 0x0.5                                              1.00  1.62    1.70   2
   -blur 0x1.0                                              1.00  1.71    1.14   2
   -charcoal 0x1                                            1.00  1.76    0.56   2
   -colorspace CMYK                                         1.00  1.11   16.07   2
   -colorspace GRAY                                         1.00  1.47    9.38   2
   -colorspace HSL                                          1.00  1.72    5.53   2
   -colorspace HWB                                          1.00  1.72    5.61   2
   -colorspace OHTA                                         1.00  1.46    9.36   2
   -colorspace YCbCr                                        1.00  1.42    9.07   2
   -colorspace YIQ                                          1.00  1.47    9.34   2
   -colorspace YUV                                          1.00  1.48    9.38   2
   -contrast -contrast -contrast                            1.00  1.89    1.42   2
   +contrast +contrast +contrast                            1.00  1.84    1.41   2
   -convolve 1,1,1,1,4,1,1,1,1                              1.00  1.75    2.51   2
   -colorize 30%/20%/50%                                    1.00  1.40    8.43   2
   -despeckle                                               1.00  1.48    0.21   2
   -edge 0x1                                                1.00  1.79    2.60   2
   -emboss 0x1                                              1.00  1.80    1.14   2
   -enhance                                                 1.00  1.93    0.65   2
   -gaussian 0x0.5                                          1.00  1.88    1.26   2
   -gaussian 0x1.0                                          1.00  1.92    0.46   2
   -hald-clut identity:8                                    1.00  1.74    3.20   2
   -hald-clut identity:10                                   1.00  1.74    2.95   2
   -hald-clut identity:14                                   1.00  1.74    1.55   2
   -implode 0.5                                             1.00  1.79    1.92   2
   -implode -1                                              1.00  1.71    2.35   2
   -lat 10x10-5%                                            1.00  1.88    0.42   2
   -median 1                                                1.00  1.39    0.48   2
   -median 2                                                1.00  1.91    0.26   2
   -minify                                                  1.00  1.59    7.77   2
   -modulate 110/100/95                                     1.00  1.75    4.04   2
   +noise Uniform                                           1.00  1.60    3.44   2
   +noise Gaussian                                          1.00  1.85    0.99   2
   +noise Multiplicative                                    1.00  1.82    1.43   2
   +noise Impulse                                           1.00  1.62    3.23   2
   +noise Laplacian                                         1.00  1.76    1.98   2
   +noise Poisson                                           1.00  1.81    1.48   2
   -noise 1                                                 1.00  1.78    0.59   2
   -noise 2                                                 1.00  1.90    0.25   2
   -fill blue -fuzz 35% -opaque red                         1.00  1.23   15.11   2
   -operator all Add 2%                                     1.00  1.56    8.28   2
   -operator all And 233                                    1.00  0.98   19.16   1
   -operator all Assign 50%                                 1.00  1.11   17.82   2
   -operator all Depth 6                                    1.00  1.01   18.00   2
   -operator all Divide 2                                   1.00  1.59    8.43   2
   -operator all Gamma 0.7                                  1.00  1.00   15.45   1
   -operator all Negate 1.0                                 1.00  1.01   18.69   2
   -operator all LShift 2                                   1.00  1.03   18.80   2
   -operator all Multiply 0.5                               1.00  1.56    8.05   2
   -operator all Or 233                                     1.00  1.03   19.00   2
   -operator all RShift 2                                   1.00  0.99   19.16   1
   -operator all Subtract 10%                               1.00  1.59    8.37   2
   -operator red Threshold 50%                              1.00  1.00   18.56   1
   -operator gray Threshold 50%                             1.00  1.02   18.33   2
   -operator all Threshold-White 80%                        1.00  1.09   20.20   2
   -operator all Threshold-Black 10%                        1.00  1.04   19.28   2
   -operator all Xor 233                                    1.00  0.98   19.16   1
   -operator all Noise-Gaussian 30%                         1.00  1.92    1.07   2
   -operator all Noise-Impulse 30%                          1.00  1.72    3.99   2
   -operator all Noise-Laplacian 30%                        1.00  1.86    2.26   2
   -operator all Noise-Multiplicative 30%                   1.00  1.90    1.59   2
   -operator all Noise-Poisson 30%                          1.00  1.89    1.66   2
   -operator all Noise-Uniform 30%                          1.00  1.71    4.31   2
   -ordered-dither all 2x2                                  1.00  1.06   15.11   2
   -ordered-dither all 3x3                                  1.00  1.14   14.97   2
   -ordered-dither intensity 3x3                            1.00  1.10   14.77   2
   -ordered-dither all 4x4                                  1.00  1.08   15.25   2
   -paint 0x1                                               1.00  1.71    3.77   2
   -random-threshold all 20x80                              1.00  1.30   11.90   2
   -recolor '1,0,0,0,1,0,0,0,1'                             1.00  1.02   23.90   2
   -recolor '0,0,1,0,1,0,1,0,0'                             1.00  1.67    6.68   2
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                       1.00  1.68    6.65   2
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'           1.00  1.66    6.59   2
   -density 75x75 -resample 50x50                           1.00  1.56    1.55   2
   -resize 10%                                              1.00  1.76    3.73   2
   -resize 50%                                              1.00  1.70    2.34   2
   -resize 150%                                             1.00  1.67    0.49   2
   -rotate 45                                               1.00  1.67    0.24   2
   -segment 0.5x0.25                                        1.00  1.26    0.05   2
   -shade 30x30                                             1.00  1.57    5.49   2
   -sharpen 0x0.5                                           1.00  1.84    1.26   2
   -sharpen 0x1.0                                           1.00  1.90    0.45   2
   -shear 45x45                                             1.00  1.68    0.28   2
   -solarize 50%                                            1.00  1.09   19.05   2
   -swirl 90                                                1.00  1.79    2.10   2
   -fuzz 35% -transparent red                               1.00  1.27   15.71   2
   -trim                                                    1.00  1.68    6.55   2
   -fuzz 5% -trim                                           1.00  1.77    4.50   2
   -unsharp 0x0.5+20+1                                      1.00  1.66    1.46   2
   -unsharp 0x1.0+20+1                                      1.00  1.75    1.04   2
   -wave 25x150                                             1.00  1.59    2.17   2
   ======================================================= ===== ===== ======= ====

FreeBSD / Intel Xeon
--------------------

.. Last update: Mon Jul 20 16:43:39 CDT 2009

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

   ======================================================= ===== ===== ===== ===== ======= ====
   Operation                                                 1     2     3     4   iter/s  thds
   ======================================================= ===== ===== ===== ===== ======= ====
   -noop                                                    1.00  1.01  1.00  1.00   19.22   2
   -affine 1,0,0.785,1,0,0 -transform                       1.00  1.48  1.71  1.71    1.46   3
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01  1.00  1.27  1.55  1.68    8.85   4
   -blur 0x0.5                                              1.00  1.15  1.37  1.42    2.82   4
   -blur 0x1.0                                              1.00  1.38  1.48  1.52    2.44   4
   -charcoal 0x1                                            1.00  1.50  1.66  1.70    0.99   4
   -colorspace CMYK                                         1.00  0.90  0.90  0.86   11.56   1
   -colorspace GRAY                                         1.00  1.20  1.50  1.51    8.05   4
   -colorspace HSL                                          1.00  1.41  1.90  2.01    4.83   4
   -colorspace HWB                                          1.00  1.45  1.91  2.09    6.72   4
   -colorspace OHTA                                         1.00  1.19  1.55  1.59    8.17   4
   -colorspace YCbCr                                        1.00  1.19  1.56  1.58    8.15   4
   -colorspace YIQ                                          1.00  1.39  1.57  1.59    8.19   4
   -colorspace YUV                                          1.00  1.46  1.53  1.59    8.17   4
   -contrast -contrast -contrast                            1.00  1.53  2.28  2.60    1.22   4
   +contrast +contrast +contrast                            1.00  1.81  2.25  2.55    1.25   4
   -convolve 1,1,1,1,4,1,1,1,1                              1.00  1.63  1.88  2.04    4.06   4
   -colorize 30%/20%/50%                                    1.00  1.37  1.49  1.62    6.78   4
   -despeckle                                               1.00  1.42  2.04  2.09    0.37   4
   -edge 0x1                                                1.00  1.48  1.92  2.04    3.93   4
   -emboss 0x1                                              1.00  1.60  1.81  1.92    1.96   4
   -enhance                                                 1.00  1.80  2.04  2.11    0.66   4
   -gaussian 0x0.5                                          1.00  1.61  2.01  2.17    2.64   4
   -gaussian 0x1.0                                          1.00  1.90  1.99  2.06    1.19   4
   -hald-clut identity:8                                    1.00  1.79  2.16  2.45    3.41   4
   -hald-clut identity:10                                   1.00  1.66  2.20  2.58    2.90   4
   -hald-clut identity:14                                   1.00  1.84  2.37  2.93    1.32   4
   -implode 0.5                                             1.00  1.73  2.17  2.41    1.29   4
   -implode -1                                              1.00  1.71  2.09  2.27    2.12   4
   -lat 10x10-5%                                            1.00  1.69  1.99  2.13    1.00   4
   -median 1                                                1.00  1.84  2.11  2.25    0.37   4
   -median 2                                                1.00  1.76  2.10  2.29    0.14   4
   -minify                                                  1.00  1.42  1.60  1.67    8.17   4
   -modulate 110/100/95                                     1.00  1.61  2.02  2.19    3.95   4
   +noise Uniform                                           1.00  1.50  1.71  1.83    2.69   4
   +noise Gaussian                                          1.00  1.66  2.37  2.81    0.94   4
   +noise Multiplicative                                    1.00  1.83  2.46  2.80    1.17   4
   +noise Impulse                                           1.00  1.53  1.66  1.73    2.77   4
   +noise Laplacian                                         1.00  1.65  2.04  2.45    1.96   4
   +noise Poisson                                           1.00  1.86  2.33  2.74    1.13   4
   -noise 1                                                 1.00  1.86  2.08  2.32    0.38   4
   -noise 2                                                 1.00  1.89  2.13  2.33    0.14   4
   -fill blue -fuzz 35% -opaque red                         1.00  1.13  1.29  1.31   13.74   4
   -operator all Add 2%                                     1.00  1.32  1.35  1.38   12.52   4
   -operator all And 233                                    1.00  1.01  1.01  1.01   13.65   3
   -operator all Assign 50%                                 1.00  1.00  1.01  1.01   13.69   4
   -operator all Depth 6                                    1.00  1.00  1.01  1.01   13.36   3
   -operator all Divide 2                                   1.00  1.24  1.50  1.51    9.17   4
   -operator all Gamma 0.7                                  1.00  1.00  1.00  1.00    8.88   3
   -operator all Negate 1.0                                 1.00  1.00  1.00  1.01   13.63   4
   -operator all LShift 2                                   1.00  1.00  1.01  1.01   13.63   3
   -operator all Multiply 0.5                               1.00  1.33  1.37  1.39   12.43   4
   -operator all Or 233                                     1.00  1.02  1.01  1.02   13.63   2
   -operator all RShift 2                                   1.00  1.02  1.01  1.02   13.65   2
   -operator all Subtract 10%                               1.00  1.26  1.40  1.45   12.38   4
   -operator red Threshold 50%                              1.00  1.02  1.01  1.01   13.69   2
   -operator gray Threshold 50%                             1.00  1.03  1.04  1.05   13.76   4
   -operator all Threshold-White 80%                        1.00  1.06  1.10  1.12   15.63   4
   -operator all Threshold-Black 10%                        1.00  1.10  1.11  1.12   15.23   4
   -operator all Xor 233                                    1.00  1.02  1.01  1.00   13.74   2
   -operator all Noise-Gaussian 30%                         1.00  1.75  2.48  2.95    1.01   4
   -operator all Noise-Impulse 30%                          1.00  1.62  1.98  2.12    3.58   4
   -operator all Noise-Laplacian 30%                        1.00  1.86  2.35  2.78    2.33   4
   -operator all Noise-Multiplicative 30%                   1.00  1.84  2.61  3.22    1.39   4
   -operator all Noise-Poisson 30%                          1.00  1.66  2.44  2.93    1.24   4
   -operator all Noise-Uniform 30%                          1.00  1.44  2.07  2.28    3.46   4
   -ordered-dither all 2x2                                  1.00  0.95  0.90  0.85   11.04   1
   -ordered-dither all 3x3                                  1.00  1.09  1.07  1.02   10.14   2
   -ordered-dither intensity 3x3                            1.00  1.08  1.06  1.01   10.19   2
   -ordered-dither all 4x4                                  1.00  0.93  0.91  0.85   11.08   1
   -paint 0x1                                               1.00  1.59  1.98  2.18    1.83   4
   -random-threshold all 20x80                              1.00  1.45  1.74  1.89    8.35   4
   -recolor '1,0,0,0,1,0,0,0,1'                             1.00  0.99  1.00  0.99   18.65   1
   -recolor '0,0,1,0,1,0,1,0,0'                             1.00  1.44  1.52  1.34   10.18   3
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                       1.00  1.25  1.56  1.61    9.37   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'           1.00  1.28  1.56  1.61    9.34   4
   -density 75x75 -resample 50x50                           1.00  1.42  1.70  1.80    1.85   4
   -resize 10%                                              1.00  1.18  1.45  1.71    3.90   4
   -resize 50%                                              1.00  1.58  1.57  1.66    2.41   4
   -resize 150%                                             1.00  1.45  1.68  1.74    0.59   4
   -rotate 45                                               1.00  1.15  1.32  1.40    0.32   4
   -segment 0.5x0.25                                        1.00  1.27  1.51  1.51    0.07   3
   -shade 30x30                                             1.00  1.75  2.24  2.60    3.67   4
   -sharpen 0x0.5                                           1.00  1.29  2.00  2.18    2.65   4
   -sharpen 0x1.0                                           1.00  1.14  1.99  2.06    1.19   4
   -shear 45x45                                             1.00  1.16  1.26  1.34    0.39   4
   -solarize 50%                                            1.00  1.01  1.01  1.01   13.71   2
   -swirl 90                                                1.00  1.67  2.22  2.53    2.08   4
   -fuzz 35% -transparent red                               1.00  1.10  1.29  1.32   13.40   4
   -trim                                                    1.00  0.88  1.30  1.33    6.08   4
   -fuzz 5% -trim                                           1.00  1.29  1.45  1.68    4.38   4
   -unsharp 0x0.5+20+1                                      1.00  1.46  1.54  1.60    2.37   4
   -unsharp 0x1.0+20+1                                      1.00  1.45  1.60  1.67    2.10   4
   -wave 25x150                                             1.00  1.50  1.68  1.83    1.79   4
   ======================================================= ===== ===== ===== ===== ======= ====

Windows XP / MSVC / Intel Core 2 Quad
-------------------------------------

.. Last update: Mon Jul 20 19:15:49 CDT 2009

This system is Windows XP Professional (SP3) using the Visual Studio
2008 compiler and a Q8 build.  The system CPU is a 2.83 GHz Core 2
Quad Processor (Q9550).  This processor is a multi-chip module (MCM)
based on two Core 2 CPUs bonded to a L3 cache in the same chip
package.

The following shows the performance boost for a Q16 build:

.. table:: Performance Boost on an Intel Core 2 Quad (Q9550) system

   ======================================================= ===== ===== ===== ===== ======= ====
   Operation                                                 1     2     3     4   iter/s  thds
   ======================================================= ===== ===== ===== ===== ======= ====
   -noop                                                    1.00  1.01  1.01  0.98   73.40   3
   -affine 1,0,0.785,1,0,0 -transform                       1.00  1.60  2.13  2.61    5.24   4
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01  1.00  1.37  1.50  1.57   39.80   4
   -blur 0x0.5                                              1.00  1.65  2.10  2.39    8.60   4
   -blur 0x1.0                                              1.00  1.68  2.21  2.58    7.66   4
   -charcoal 0x1                                            1.00  1.75  2.29  2.61    3.13   4
   -colorspace CMYK                                         1.00  1.13  1.17  1.17   41.80   3
   -colorspace GRAY                                         1.00  1.71  1.87  1.88   18.94   4
   -colorspace HSL                                          1.00  1.94  2.76  3.36   16.60   4
   -colorspace HWB                                          1.00  1.82  2.50  3.07   20.80   4
   -colorspace OHTA                                         1.00  1.70  1.86  1.85   18.74   3
   -colorspace YCbCr                                        1.00  1.70  1.89  1.90   19.14   4
   -colorspace YIQ                                          1.00  1.71  1.91  1.91   19.00   3
   -colorspace YUV                                          1.00  1.70  1.94  1.90   19.54   3
   -contrast -contrast -contrast                            1.00  1.97  2.91  3.78    4.04   4
   +contrast +contrast +contrast                            1.00  1.99  2.94  3.84    4.05   4
   -convolve 1,1,1,1,4,1,1,1,1                              1.00  1.99  2.77  3.36   14.76   4
   -colorize 30%/20%/50%                                    1.00  1.69  2.25  2.66   23.26   4
   -despeckle                                               1.00  1.50  2.84  2.86    0.89   4
   -edge 0x1                                                1.00  1.83  2.71  3.44    8.97   4
   -emboss 0x1                                              1.00  1.79  2.45  2.97    7.51   4
   -enhance                                                 1.00  2.01  2.95  3.90    2.83   4
   -gaussian 0x0.5                                          1.00  1.88  2.69  3.40    9.45   4
   -gaussian 0x1.0                                          1.00  2.01  2.93  3.77    4.50   4
   -hald-clut identity:8                                    1.00  1.88  2.59  3.20   11.92   4
   -hald-clut identity:10                                   1.00  1.70  2.21  2.61    9.03   4
   -hald-clut identity:14                                   1.00  1.32  1.47  1.56    2.86   4
   -implode 0.5                                             1.00  1.97  2.87  3.65    6.85   4
   -implode -1                                              1.00  1.92  2.80  3.60    6.91   4
   -lat 10x10-5%                                            1.00  1.89  2.69  3.43    4.47   4
   -median 1                                                1.00  2.01  2.67  3.68    1.50   4
   -median 2                                                1.00  1.97  2.79  3.83    0.57   4
   -minify                                                  1.00  1.67  2.20  2.43   29.60   4
   -modulate 110/100/95                                     1.00  1.89  2.66  3.35   14.22   4
   +noise Uniform                                           1.00  1.69  2.19  2.55   13.20   4
   +noise Gaussian                                          1.00  1.93  2.82  3.55    3.20   4
   +noise Multiplicative                                    1.00  1.92  2.75  3.52    4.54   4
   +noise Impulse                                           1.00  1.70  2.21  2.61   12.72   4
   +noise Laplacian                                         1.00  1.85  2.57  3.17    7.51   4
   +noise Poisson                                           1.00  1.91  2.73  3.46    4.98   4
   -noise 1                                                 1.00  1.95  2.51  3.60    1.52   4
   -noise 2                                                 1.00  1.98  2.77  3.79    0.56   4
   -fill blue -fuzz 35% -opaque red                         1.00  1.34  1.50  1.58   55.00   4
   -operator all Add 2%                                     1.00  1.72  2.25  2.64   30.40   4
   -operator all And 233                                    1.00  1.09  1.11  1.10   48.59   3
   -operator all Assign 50%                                 1.00  1.09  1.05  1.06   49.80   2
   -operator all Depth 6                                    1.00  1.11  1.13  1.15   48.40   4
   -operator all Divide 2                                   1.00  1.72  2.21  2.55   29.60   4
   -operator all Gamma 0.7                                  1.00  1.09  1.13  1.13   39.40   4
   -operator all Negate 1.0                                 1.00  1.11  1.09  1.09   49.60   2
   -operator all LShift 2                                   1.00  1.06  1.09  1.10   49.00   4
   -operator all Multiply 0.5                               1.00  1.76  2.28  2.68   30.40   4
   -operator all Or 233                                     1.00  1.05  1.09  1.10   48.00   4
   -operator all RShift 2                                   1.00  1.05  1.03  1.10   48.20   4
   -operator all Subtract 10%                               1.00  1.78  2.33  2.63   28.40   4
   -operator red Threshold 50%                              1.00  1.05  1.09  1.09   48.80   3
   -operator gray Threshold 50%                             1.00  1.07  1.13  1.13   48.45   3
   -operator all Threshold-White 80%                        1.00  1.06  1.08  1.05   54.20   3
   -operator all Threshold-Black 10%                        1.00  1.17  1.09  1.04   56.60   2
   -operator all Xor 233                                    1.00  1.12  1.13  1.12   48.60   3
   -operator all Noise-Gaussian 30%                         1.00  1.92  2.91  3.82    3.61   4
   -operator all Noise-Impulse 30%                          1.00  1.87  2.60  3.21   17.29   4
   -operator all Noise-Laplacian 30%                        1.00  1.93  2.79  3.56    9.03   4
   -operator all Noise-Multiplicative 30%                   1.00  1.95  3.04  3.87    4.99   4
   -operator all Noise-Poisson 30%                          1.00  1.96  2.88  3.74    5.66   4
   -operator all Noise-Uniform 30%                          1.00  1.86  2.58  3.19   18.34   4
   -ordered-dither all 2x2                                  1.00  1.07  1.08  1.16   41.00   4
   -ordered-dither all 3x3                                  1.00  1.21  1.19  1.21   39.60   4
   -ordered-dither intensity 3x3                            1.00  1.19  1.20  1.17   39.20   3
   -ordered-dither all 4x4                                  1.00  1.14  1.10  1.13   40.00   2
   -paint 0x1                                               1.00  1.97  2.86  3.72    4.80   4
   -random-threshold all 20x80                              1.00  1.56  1.92  2.22   34.70   4
   -recolor '1,0,0,0,1,0,0,0,1'                             1.00  0.99  0.97  0.99   70.80   1
   -recolor '0,0,1,0,1,0,1,0,0'                             1.00  1.71  2.25  2.53   29.62   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                       1.00  1.79  2.42  2.93   24.52   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'           1.00  1.79  2.43  2.86   23.85   4
   -density 75x75 -resample 50x50                           1.00  1.70  2.15  2.41    9.68   4
   -resize 10%                                              1.00  1.73  2.31  2.78   24.00   4
   -resize 50%                                              1.00  1.71  2.09  2.33   12.52   4
   -resize 150%                                             1.00  1.88  2.21  2.71    4.43   4
   -rotate 45                                               1.00  1.57  1.89  2.11    1.94   4
   -segment 0.5x0.25                                        1.00  1.44  1.62  1.79    0.17   4
   -shade 30x30                                             1.00  1.73  2.16  2.55   22.00   4
   -sharpen 0x0.5                                           1.00  1.89  2.68  3.42    9.48   4
   -sharpen 0x1.0                                           1.00  2.02  2.92  3.79    4.50   4
   -shear 45x45                                             1.00  1.35  1.57  1.67    2.67   4
   -solarize 50%                                            1.00  1.10  1.15  1.15   48.80   3
   -swirl 90                                                1.00  1.93  2.77  3.54    7.50   4
   -fuzz 35% -transparent red                               1.00  1.35  1.47  1.55   48.20   4
   -trim                                                    1.00  1.57  1.91  2.14   45.80   4
   -fuzz 5% -trim                                           1.00  1.75  2.33  2.73   29.71   4
   -unsharp 0x0.5+20+1                                      1.00  1.72  2.24  2.63    7.51   4
   -unsharp 0x1.0+20+1                                      1.00  1.77  2.36  2.80    6.78   4
   -wave 25x150                                             1.00  1.80  2.41  2.71    6.54   4
   ======================================================= ===== ===== ===== ===== ======= ====

Windows XP / MinGW / Intel Core 2 Quad
--------------------------------------

.. Last update: Mon Jul 20 16:32:57 CDT 2009

TDM's GCC/mingw32 4.4.1-tdm-2 (build 1.908.0) was downloaded from
http://www.tdragon.net/recentgcc/ and installed on the same Windows XP
system with the 2.83 GHz Core 2 Quad Processor (Q9550) as described
above.  This processor is a multi-chip module (MCM) based on two Core
2 CPUs bonded to a L3 cache in the same chip package.

The following shows the performance boost for a Q16 build:

.. table:: Performance Boost on an Intel Core 2 Quad (Q9550) system

   ======================================================= ===== ===== ===== ===== ======= ====
   Operation                                                 1     2     3     4   iter/s  thds
   ======================================================= ===== ===== ===== ===== ======= ====
   -noop                                                    1.00  0.98  1.01  1.01   74.60   4
   -affine 1,0,0.785,1,0,0 -transform                       1.00  1.81  2.22  2.68    4.43   4
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01  1.00  1.46  1.64  1.80   39.80   4
   -blur 0x0.5                                              1.00  1.40  1.64  1.81   11.06   4
   -blur 0x1.0                                              1.00  1.56  1.89  2.09    9.60   4
   -charcoal 0x1                                            1.00  1.63  2.00  2.22    4.21   4
   -colorspace CMYK                                         1.00  1.10  1.09  1.14   42.20   4
   -colorspace GRAY                                         1.00  1.41  1.20  1.07   32.60   2
   -colorspace HSL                                          1.00  1.90  2.63  3.26   19.20   4
   -colorspace HWB                                          1.00  1.82  2.43  2.95   25.72   4
   -colorspace OHTA                                         1.00  1.42  1.15  1.05   33.09   2
   -colorspace YCbCr                                        1.00  1.40  1.19  1.06   32.50   2
   -colorspace YIQ                                          1.00  1.42  1.15  1.06   33.09   2
   -colorspace YUV                                          1.00  1.41  1.19  1.06   32.50   2
   -contrast -contrast -contrast                            1.00  2.26  3.46  4.62    4.21   4
   +contrast +contrast +contrast                            1.00  2.27  3.47  4.60    4.24   4
   -convolve 1,1,1,1,4,1,1,1,1                              1.00  1.67  2.24  2.67   21.53   4
   -colorize 30%/20%/50%                                    1.00  1.69  1.99  2.30   29.71   4
   -despeckle                                               1.00  1.48  2.59  2.61    1.57   4
   -edge 0x1                                                1.00  1.82  2.56  3.22   13.24   4
   -emboss 0x1                                              1.00  1.69  2.26  2.69   10.27   4
   -enhance                                                 1.00  2.03  3.05  4.01    3.15   4
   -gaussian 0x0.5                                          1.00  1.85  2.57  3.18   13.67   4
   -gaussian 0x1.0                                          1.00  1.97  2.86  3.65    5.91   4
   -hald-clut identity:8                                    1.00  1.90  2.67  3.35   14.66   4
   -hald-clut identity:10                                   1.00  1.93  2.73  3.46   13.56   4
   -hald-clut identity:14                                   1.00  1.88  2.66  3.21    7.46   4
   -implode 0.5                                             1.00  2.05  2.97  4.07    5.50   4
   -implode -1                                              1.00  2.09  3.13  4.10    5.43   4
   -lat 10x10-5%                                            1.00  1.91  2.74  3.49    4.36   4
   -median 1                                                1.00  1.96  2.90  3.64    1.58   4
   -median 2                                                1.00  2.04  3.05  3.94    0.61   4
   -minify                                                  1.00  1.59  1.99  2.23   36.00   4
   -modulate 110/100/95                                     1.00  1.90  2.68  3.32   15.85   4
   +noise Uniform                                           1.00  1.55  1.98  2.33   11.13   4
   +noise Gaussian                                          1.00  2.82  4.53  6.15    2.58   4
   +noise Multiplicative                                    1.00  2.52  3.90  5.12    3.66   4
   +noise Impulse                                           1.00  1.51  1.91  2.22   11.85   4
   +noise Laplacian                                         1.00  2.39  3.60  4.61    5.78   4
   +noise Poisson                                           1.00  1.89  2.74  3.49    3.00   4
   -noise 1                                                 1.00  1.98  2.91  3.72    1.66   4
   -noise 2                                                 1.00  1.98  2.94  3.84    0.63   4
   -fill blue -fuzz 35% -opaque red                         1.00  1.34  1.49  1.51   53.20   4
   -operator all Add 2%                                     1.00  1.38  1.56  1.69   48.60   4
   -operator all And 233                                    1.00  1.08  1.11  1.11   50.64   3
   -operator all Assign 50%                                 1.00  1.07  1.06  1.05   50.00   2
   -operator all Depth 6                                    1.00  1.10  1.14  1.14   49.45   3
   -operator all Divide 2                                   1.00  1.41  1.61  1.76   48.20   4
   -operator all Gamma 0.7                                  1.00  1.10  1.14  1.12   37.48   3
   -operator all Negate 1.0                                 1.00  1.13  1.12  1.14   50.60   4
   -operator all LShift 2                                   1.00  1.07  1.11  1.10   50.80   3
   -operator all Multiply 0.5                               1.00  1.42  1.62  1.75   48.06   4
   -operator all Or 233                                     1.00  1.12  1.18  1.16   50.80   3
   -operator all RShift 2                                   1.00  1.09  1.09  1.11   50.60   4
   -operator all Subtract 10%                               1.00  1.43  1.67  1.76   47.60   4
   -operator red Threshold 50%                              1.00  1.04  1.07  1.08   49.80   4
   -operator gray Threshold 50%                             1.00  1.09  1.14  1.11   51.00   3
   -operator all Threshold-White 80%                        1.00  1.08  1.08  1.09   56.60   4
   -operator all Threshold-Black 10%                        1.00  1.10  1.14  1.13   56.00   3
   -operator all Xor 233                                    1.00  1.10  1.09  1.11   50.60   4
   -operator all Noise-Gaussian 30%                         1.00  2.85  4.65  6.36    2.71   4
   -operator all Noise-Impulse 30%                          1.00  1.58  2.14  2.63   16.00   4
   -operator all Noise-Laplacian 30%                        1.00  2.49  3.88  5.17    6.67   4
   -operator all Noise-Multiplicative 30%                   1.00  2.57  4.09  5.55    4.04   4
   -operator all Noise-Poisson 30%                          1.00  1.91  2.87  3.76    3.26   4
   -operator all Noise-Uniform 30%                          1.00  1.62  2.16  2.65   14.80   4
   -ordered-dither all 2x2                                  1.00  1.10  1.12  1.12   40.60   3
   -ordered-dither all 3x3                                  1.00  1.13  1.14  1.20   41.00   4
   -ordered-dither intensity 3x3                            1.00  1.14  1.18  1.19   41.00   4
   -ordered-dither all 4x4                                  1.00  1.08  1.09  1.12   40.87   4
   -paint 0x1                                               1.00  1.93  2.78  3.60    5.89   4
   -random-threshold all 20x80                              1.00  1.55  1.92  2.20   36.68   4
   -recolor '1,0,0,0,1,0,0,0,1'                             1.00  1.01  1.02  0.99   76.00   3
   -recolor '0,0,1,0,1,0,1,0,0'                             1.00  1.76  2.33  2.72   26.91   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                       1.00  1.68  2.14  2.47   33.89   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'           1.00  1.67  2.12  2.49   34.20   4
   -density 75x75 -resample 50x50                           1.00  1.58  2.05  2.20   10.37   4
   -resize 10%                                              1.00  1.79  2.41  2.92   22.26   4
   -resize 50%                                              1.00  1.59  2.05  2.30   13.12   4
   -resize 150%                                             1.00  1.59  2.02  2.36    5.20   4
   -rotate 45                                               1.00  1.63  1.99  2.17    1.85   4
   -segment 0.5x0.25                                        1.00  1.46  1.70  1.87    0.18   4
   -shade 30x30                                             1.00  1.83  2.51  3.09   14.31   4
   -sharpen 0x0.5                                           1.00  1.80  2.52  3.13   13.76   4
   -sharpen 0x1.0                                           1.00  1.95  2.84  3.64    5.91   4
   -shear 45x45                                             1.00  1.30  1.59  1.65    2.51   4
   -solarize 50%                                            1.00  1.10  1.15  1.14   51.05   3
   -swirl 90                                                1.00  2.15  3.06  4.31    6.16   4
   -fuzz 35% -transparent red                               1.00  1.35  1.50  1.51   46.80   4
   -trim                                                    1.00  1.65  2.12  2.48   38.68   4
   -fuzz 5% -trim                                           1.00  1.75  2.35  2.79   28.80   4
   -unsharp 0x0.5+20+1                                      1.00  1.52  1.82  1.91    9.45   4
   -unsharp 0x1.0+20+1                                      1.00  1.58  2.07  2.29    8.72   4
   -wave 25x150                                             1.00  1.74  2.50  2.92    5.69   4
   ======================================================= ===== ===== ===== ===== ======= ====

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

| Copyright (C) 2008 - 2010 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/Copyright.html.

