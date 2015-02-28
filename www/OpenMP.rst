.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

.. _`Amdahl's law` : http://en.wikipedia.org/wiki/Amdahl%27s_law
.. _`GOMP` : http://gcc.gnu.org/onlinedocs/libgomp/
.. _`OpenMP` : http://openmp.org/
.. _`Open64` : http://www.open64.net/home.html
.. _'MinGW-w64' : http://mingw-w64.sourceforge.net/

========================
OpenMP in GraphicsMagick
========================

.. contents::
  :local:

Overview
========

GraphicsMagick has been transformed to use OpenMP_ for the 1.3 release
series. OpenMP is a portable framework for accelerating CPU-bound and
memory-bound operations using multiple threads. OpenMP originates in
the super-computing world and has been available in one form or
another since the late '90s.

Since GCC 4.2 has introduced excellent OpenMP support via GOMP_,
OpenMP has become available to the masses. Microsoft Visual Studio
Professional 2005 and later support OpenMP so Windows users can
benefit as well. Any multi-CPU and/or multi-core system is potentially
a good candidate for use with OpenMP. Unfortunately, some older
multi-CPU hardware is more suitable for multi-processing than
multi-threading. Modern multi-core chipsets from AMD, Intel and
Sun/Oracle perform very well with OpenMP.

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
speed-up (within the constraints of `Amdahl's law`_) due to the number
of cores. For example, a two core system executes the algorithm twice
as fast, and a four core system executes the algorithm four times as
fast. Memory-bound algorithms scale based on the memory bandwith
available to the cores. For example, memory-bound algorithms scale up
to almost 1.5X on my four core Opteron system due to its NUMA
architecture. Some systems/CPUs are able to immediately context switch
to another thread if the core would be blocked waiting for memory,
allowing multiple memory accesses to be pending at once, and thereby
improving throughput.  For example, typical speedup of 20-32X (average
24X) has been observed on the Sun SPARC T2 CPU, which provides 8
cores, with 8 virtual CPUs per core (64 threads).

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

  % gm benchmark -stepthreads 1 -duration 10 convert \
    -size 2048x1080 pattern:granite -operator all Noise-Gaussian 30% null:
  Results: 1 threads 5 iter 11.07s user 11.07s total 0.452 iter/s (0.452 iter/s cpu) 1.00 speedup 1.000 karp-flatt
  Results: 2 threads 10 iter 22.16s user 11.11s total 0.900 iter/s (0.451 iter/s cpu) 1.99 speedup 0.004 karp-flatt
  Results: 3 threads 14 iter 31.06s user 10.47s total 1.337 iter/s (0.451 iter/s cpu) 2.96 speedup 0.007 karp-flatt
  Results: 4 threads 18 iter 40.01s user 10.24s total 1.758 iter/s (0.450 iter/s cpu) 3.89 speedup 0.009 karp-flatt

Note that the "iter/s cpu" value is a measure of the number of
iterations given the amount of reported CPU time consumed. It is an
effective measure of relative efficacy since its value should ideally
not drop as iterations are added.  The karp-flatt ratio is another
useful metric for evaluating thread-speedup efficiency. In the above
example, the total speedup was about 3.9X with only a slight loss of
CPU efficiency as threads are added.

According to the OpenMP specification, the OMP_NUM_THREADS evironment
variable may be used to specify the number of threads available to the
application. Typically this is set to the number of processor cores on
the system but may be set lower to limit resource consumption or (in
some cases) to improve execution efficiency.  The GraphicsMagick
commands also accept a ``-limit threads limit`` type option for
specifying the maximum number of threads to use.

Results
=======

A simple scheme was developed in order to evaluate the performance
boost with varying numbers of threads.  GraphicsMagick's built-in
benchmark facility is used.  The selected algorithm is executed
repeatedly until a specified amount of time has elapsed.  The input
image is generated on the fly by tiling a small image over a large
area using a specification like `-size 4000x3000 tile:model.pnm`.  It
is important to note that the time to generate the input image is
included in the benchmark timings so that even if an algorithm
achieves perfect linear scaling, the measured difference is likely to
be less than the number of cores used and the impact could be
substantial if image generation is slow.  Many modern CPUs increase
the core frequency substantially ("turbo mode") when only a few cores
are being used and this unfairly penalizes the calculated per-thread
speedup results which are based on the time to run with just one
thread.

A typical benchmark command using the built-in benchmark facility
(-stepthreads requires GraphicsMagick 1.3.13 or later) looks like::

  gm benchmark -stepthreads 1 -duration 5 convert \
    -size 4000x3000 tile:model.pnm -median 2 null:

The first test executed is `-noop` since it does no work other than
to generate the input image.  This represents how fast it is possible
to go based on how fast the input image may be generated.

OpenIndiana oi_151a7 / Intel Xeon E5-2680 2.70GHz / GCC 4.7.1 x86-64
--------------------------------------------------------------------

.. Last update: Sat Feb 16 15:53:21 CST 2013

The following results were obtained from an Intel Xeon E5-2680 at
2.70GHz.  This CPU has 16 cores and 32 threads.  GCC 4.7.1 was used to
build the software.  Please note that this CPU has a turbo-boost
feature which clocks the CPU at 3.9GHz when only a few cores are
active so the calculated speedup (based on performance with one
thread) is reported at considerably less (e.g 60% less) than it would
be based on all cores active:

.. table:: Performance Boost On 16 core Intel Xeon E5-2680 CPU:

   ============================================== ===== ===== ===== ===== ===== ===== ===== ===== ===== ======== ====
   Operation                                        1     4     8    12    16    20    24    25    32    iter/s  thds
   ============================================== ===== ===== ===== ===== ===== ===== ===== ===== ===== ======== ====
   -affine 1,0,0.785,1,0,0 -transform              1.00  3.76  6.50  8.76 10.62 10.68 12.23 13.61 15.00   6.496   9
   -blur 0x0.5                                     1.00  2.13  2.58  2.82  2.67  2.75  2.85  2.91  2.94   7.186   9
   -blur 0x1.0                                     1.00  2.47  3.12  3.64  3.33  3.57  3.69  3.79  3.87   6.587   9
   -blur 0x2.0                                     1.00  2.98  4.29  5.02  4.63  5.00  5.27  5.50  5.71   5.416   9
   -charcoal 0x1                                   1.00  2.64  3.48  3.90  3.72  3.85  4.00  4.07  4.09   3.435   9
   -colorspace GRAY                                1.00  2.91  4.08  4.77  4.46  3.67  3.73  3.63  3.50  41.800   4
   -colorspace HSL                                 1.00  3.59  6.24  8.27  9.10  7.23  8.12  9.06  9.05  18.200   5
   -colorspace HWB                                 1.00  3.51  5.59  7.37  5.86  6.49  7.07  7.82  7.57  24.303   8
   -colorspace OHTA                                1.00  2.77  3.72  4.24  4.39  3.75  3.74  3.68  3.81  37.800   5
   -colorspace YCbCr                               1.00  2.74  3.68  4.01  3.36  3.86  3.84  3.81  3.57  34.263   4
   -colorspace YIQ                                 1.00  2.82  3.83  4.31  3.78  3.82  4.00  3.78  3.74  36.200   4
   -colorspace YUV                                 1.00  2.67  3.97  4.24  4.01  3.60  4.05  3.86  3.69  35.657   4
   -contrast -contrast -contrast                   1.00  3.93  7.18 10.52 12.85 13.94 16.18 17.63 19.09   4.239   9
   +contrast +contrast +contrast                   1.00  3.96  7.19 10.54 13.38 14.19 16.09 17.56 18.37   4.078   9
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  3.54  5.93  7.92  9.27  9.01  9.36  9.44  9.55  25.697   9
   -despeckle                                      1.00  3.09  4.81  5.45  5.87  5.89  5.93  5.86  5.66   3.605   7
   -edge 0x1                                       1.00  3.56  5.88  7.82  9.31  8.91  9.18  9.48  9.45  26.243   8
   -emboss 0x1                                     1.00  3.35  5.23  6.65  7.43  7.73  7.97  8.24  8.53   8.858   9
   -enhance                                        1.00  3.96  7.18 10.61 12.83 11.58 12.49 13.27 14.07   3.448   9
   -gaussian 0x0.5                                 1.00  3.70  6.64  9.49 11.44 11.73 12.63 13.45 14.23  16.168   9
   -gaussian 0x1.0                                 1.00  3.92  7.07 10.12 12.00 13.70 14.98 16.50 18.01   6.809   9
   -gaussian 0x2.0                                 1.00  3.98  7.26 10.79 12.57 14.19 15.91 17.95 19.60   2.136   9
   -hald-clut identity:8                           1.00  3.76  6.49  9.03 11.20  9.85 10.39 10.62 10.53  16.501   5
   -hald-clut identity:10                          1.00  3.74  6.50  9.07 10.68  9.78 11.05 11.48 11.88  17.600   9
   -hald-clut identity:14                          1.00  3.75  6.47  9.19 10.95  9.63 10.63 11.21 11.18  14.970   8
   -implode 0.5                                    1.00  3.93  7.07 10.42 12.80 13.83 16.23 18.33 20.46   5.523   9
   -implode -1                                     1.00  3.90  7.05 10.39 12.00 13.41 15.21 17.51 19.61   6.433   9
   -lat 10x10-5%                                   1.00  1.00  1.00  1.00  1.00  1.00  1.00  1.00  1.00   1.692   3
   -median 1                                       1.00  3.89  6.92 10.29 12.43 11.67 13.34 15.28 16.84   2.004   9
   -median 2                                       1.00  3.77  6.90 10.26 10.85 12.79 14.49 16.79 18.54   0.723   9
   -minify                                         1.00  3.46  5.56  7.30  8.27  8.72  9.00  8.82  8.68  33.600   7
   -modulate 110/100/95                            1.00  3.72  6.47  8.66 10.12  8.89 10.24 11.31 11.14  15.706   8
   -motion-blur 0x3+30                             1.00  1.72  1.45  1.48  1.56  1.57  1.63  1.62  1.68   0.376   2
   +noise Uniform                                  1.00  2.90  4.08  4.75  4.31  4.59  4.83  5.07  5.23  14.286   9
   +noise Gaussian                                 1.00  3.91  7.04  9.88 11.53 14.10 16.69 19.02 21.41   2.462   9
   +noise Multiplicative                           1.00  3.88  6.93 10.08 11.99 13.55 15.88 18.07 20.19   3.675   9
   +noise Impulse                                  1.00  2.99  4.23  5.04  4.63  5.02  5.36  5.63  5.85  14.427   9
   +noise Laplacian                                1.00  3.80  6.64  9.32 11.60 12.30 14.03 15.69 17.40   5.917   9
   +noise Poisson                                  1.00  3.87  6.85  9.86 11.97 13.35 15.51 17.48 19.42   4.582   9
   +noise Random                                   1.00  3.21  4.82  5.44  5.38  5.88  6.36  6.74  7.11  12.948   9
   -noise 1                                        1.00  3.90  6.96 10.03 12.31 11.52 13.19 15.22 16.81   1.984   9
   -noise 2                                        1.00  3.90  6.95 10.15 11.64 12.82 14.36 16.64 18.51   0.722   9
   -operator all Add 2%                            1.00  2.63  3.58  3.82  3.14  3.16  3.14  3.21  3.06  47.000   4
   -operator all Divide 2                          1.00  2.71  3.70  4.69  4.02  4.20  4.28  4.19  4.54  37.400   4
   -operator all Multiply 0.5                      1.00  2.39  3.00  3.15  3.21  3.23  3.16  3.07  2.93  39.521   6
   -operator all Subtract 10%                      1.00  2.48  3.09  3.26  3.21  3.40  3.31  3.22  3.05  40.519   6
   -operator all Noise-Gaussian 30%                1.00  3.99  7.27 10.81 12.16 14.98 17.81 20.17 23.13   2.637   9
   -operator all Noise-Impulse 30%                 1.00  3.57  6.00  7.98  8.32  7.48  8.11  8.56  8.77  23.400   9
   -operator all Noise-Laplacian 30%               1.00  3.92  7.06 10.36 13.32 13.81 16.09 18.24 19.57   6.733   9
   -operator all Noise-Multiplicative 30%          1.00  3.96  7.20 10.63 13.40 14.49 17.11 19.93 22.50   4.118   9
   -operator all Noise-Poisson 30%                 1.00  3.94  7.14 10.51 13.07 14.56 17.02 19.79 22.00   5.325   9
   -operator all Noise-Uniform 30%                 1.00  3.57  5.78  7.55  7.92  6.65  7.32  7.56  7.41  23.658   5
   -ordered-dither all 2x2                         1.00  3.01  4.25  4.95  5.04  5.28  5.32  5.05  4.84  37.649   7
   -ordered-dither all 3x3                         1.00  3.00  4.27  4.88  4.74  5.44  5.46  5.27  5.07  37.600   7
   -ordered-dither intensity 3x3                   1.00  2.86  4.33  5.03  4.83  5.03  5.52  5.29  4.96  38.048   7
   -ordered-dither all 4x4                         1.00  2.89  4.11  5.22  5.46  5.09  5.02  5.48  4.99  37.924   8
   -paint 0x1                                      1.00  3.72  6.37  9.20 11.02  9.32 10.39 11.41 12.51  15.415   9
   -random-threshold all 20x80                     1.00  2.71  3.50  3.64  3.71  3.35  3.32  3.22  3.24  41.916   5
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  2.94  4.06  4.57  3.95  4.79  5.05  5.02  4.86  31.800   7
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  3.21  4.51  4.99  4.83  4.68  5.48  5.70  5.81  30.938   9
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  3.09  4.57  5.16  4.98  4.70  5.27  5.81  5.35  31.000   8
   -density 75x75 -resample 50x50                  1.00  3.60  6.10  8.71 10.39 10.02 10.43 11.06 10.85  18.000   8
   -resize 10%                                     1.00  3.35  5.38  6.78  7.28  6.94  6.83  6.80  7.03  25.050   5
   -resize 50%                                     1.00  3.60  5.89  7.79  9.64  9.20  9.49  9.61  9.62  21.756   5
   -resize 150%                                    1.00  4.03  7.13 10.06 12.30 11.74 12.21 12.52 10.46   8.203   8
   -rotate 15                                      1.00  3.33  5.50  7.13  8.14  8.70  9.15  9.65  9.86   4.960   9
   -rotate 45                                      1.00  3.44  5.42  6.99  7.95  8.42  8.81  9.06  9.26   2.103   9
   -shade 30x30                                    1.00  3.43  5.46  7.17  6.99  6.47  6.90  7.12  7.09  24.551   4
   -sharpen 0x0.5                                  1.00  3.63  6.56  9.24 10.87 11.70 12.60 13.62 14.29  16.238   9
   -sharpen 0x1.0                                  1.00  3.83  7.09 10.40 13.24 13.52 15.05 16.48 17.93   6.759   9
   -sharpen 0x2.0                                  1.00  3.98  7.26 10.77 13.44 14.56 16.13 17.78 19.63   2.140   9
   -shear 45x45                                    1.00  3.02  4.78  5.94  6.63  6.91  7.09  7.29  7.42   3.036   9
   -swirl 90                                       1.00  3.87  7.01 10.29 12.22 12.93 14.80 17.04 17.45   6.667   9
   -fuzz 5% -trim                                  1.00  2.34  2.50  2.55  2.77  3.13  3.19  3.47  4.06  21.074   9
   -unsharp 0x0.5+20+1                             1.00  2.37  2.98  3.36  3.19  3.31  3.40  3.47  3.52   6.574   9
   -unsharp 0x1.0+20+1                             1.00  2.65  3.47  3.95  4.24  3.93  4.12  4.26  4.28   6.000   9
   -wave 25x150                                    1.00  2.33  3.55  4.29  4.72  4.49  4.37  4.59  4.52   1.988   5
   ============================================== ===== ===== ===== ===== ===== ===== ===== ===== ===== ======== ====


Ubuntu Linux 11.10 / Intel Xeon E5649 / Open64 Compiler
--------------------------------------------------------

.. Last update: Wed Jan 18 22:04:57 CST 2012

The following results were obtained from an Intel Xeon E5649 CPU at
2.53GHz.  This CPU has 12 cores and 24 threads.  The free open source
`Open64`_ 5.0 compiler was used to build the software.  The `Open64`_
compiler produces very high performance code which exceeds GCC
performance in most cases, and often quite dramatically so:

.. table:: Performance Boost On 12 core Intel Xeon E5649 CPU:

   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ========
   Operation                                                               1     4     8    12     16   20    24    iter/s 
   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ========
   -noop                                                                  1.00  1.31  1.41  1.35  1.44  1.46  1.43  82.635 
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  2.74  5.59  5.35  5.78  5.95  7.56   4.175 
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  2.03  2.50  2.58  2.83  2.86  2.89  39.641 
   -blur 0x0.5                                                            1.00  3.15  4.35  5.05  4.94  5.22  5.30   5.631 
   -blur 0x1.0                                                            1.00  3.35  5.17  6.14  5.79  6.32  6.63   4.873 
   -blur 0x2.0                                                            1.00  3.54  5.90  7.48  6.90  7.84  8.56   3.937 
   -charcoal 0x1                                                          1.00  2.97  4.26  5.04  4.72  5.15  5.51   2.303 
   -colorspace CMYK                                                       1.00  2.25  2.72  2.60  2.85  2.81  2.76  40.319 
   -colorspace GRAY                                                       1.00  3.07  4.67  5.69  5.69  6.31  6.90  33.600 
   -colorspace HSL                                                        1.00  3.71  6.71  9.20  7.66  9.07 10.41  14.343 
   -colorspace HWB                                                        1.00  3.59  6.13  8.09  6.65  7.80  8.82  19.960 
   -colorspace OHTA                                                       1.00  3.08  4.66  5.72  5.69  6.22  6.85  33.267 
   -colorspace YCbCr                                                      1.00  3.07  4.56  5.72  5.70  6.21  6.92  33.665 
   -colorspace YIQ                                                        1.00  3.08  4.65  5.72  5.72  6.27  6.95  33.800 
   -colorspace YUV                                                        1.00  3.03  4.68  5.60  5.47  6.29  6.95  33.800 
   -contrast -contrast -contrast                                          1.00  3.70  7.15 10.51  9.69 11.90 13.65   4.150 
   +contrast +contrast +contrast                                          1.00  3.69  7.13 10.42  9.41 11.49 13.38   4.348 
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  3.21  5.48  7.22  6.10  6.83  7.76  13.861 
   -colorize 30%/20%/50%                                                  1.00  3.52  6.00  7.68  6.48  7.64  8.46  23.000 
   -despeckle                                                             1.00  2.67  3.33  4.09  4.14  4.27  4.65   1.359 
   -edge 0x1                                                              1.00  3.46  5.74  7.97  7.33  8.46  9.36  13.000 
   -emboss 0x1                                                            1.00  3.26  4.87  6.02  4.75  5.46  5.90   4.615 
   -enhance                                                               1.00  3.55  7.00 10.30  7.83  9.73 11.39   1.731 
   -gaussian 0x0.5                                                        1.00  3.58  6.31  8.60  6.20  7.20  8.23   7.570 
   -gaussian 0x1.0                                                        1.00  3.61  6.79  9.73  5.94  7.48  8.73   3.220 
   -gaussian 0x2.0                                                        1.00  3.57  6.81  9.79  5.55  6.89  8.22   1.077 
   -hald-clut identity:8                                                  1.00  3.70  6.81  9.44  8.58 10.32 11.62  13.412 
   -hald-clut identity:10                                                 1.00  3.73  6.80  9.31  8.42 10.20 11.52  13.439 
   -hald-clut identity:14                                                 1.00  3.76  6.69  9.42  8.50 10.19 11.66  12.103 
   -implode 0.5                                                           1.00  3.80  7.28  6.16  9.46 11.91 12.17   5.088 
   -implode -1                                                            1.00  3.79  7.29  5.21  8.93 11.33 12.82   7.677 
   -lat 10x10-5%                                                          1.00  3.56  6.24  8.34  7.31  8.72  9.84   4.094 
   -median 1                                                              1.00  2.99  4.41  6.32  7.54  6.38  5.53   0.641 
   -median 2                                                              1.00  3.23  4.20  7.70  8.93  8.00  7.10   0.268 
   -minify                                                                1.00  3.30  4.97  6.36  5.95  6.46  7.12  24.400 
   -modulate 110/100/95                                                   1.00  3.79  6.86  9.62  8.55 10.16 11.63  13.147 
   +noise Uniform                                                         1.00  3.29  5.24  6.40  6.73  7.23  7.74  13.690 
   +noise Gaussian                                                        1.00  3.51  6.91 10.01  9.49 11.56 13.34   2.308 
   +noise Multiplicative                                                  1.00  3.56  6.92 10.00 11.08 13.47 15.35   3.393 
   +noise Impulse                                                         1.00  3.24  5.05  6.22  5.73  6.25  6.75  12.525 
   +noise Laplacian                                                       1.00  3.65  6.90  9.80 10.03 11.93 13.53   4.762 
   +noise Poisson                                                         1.00  3.67  6.91  9.78 10.24 12.07 13.74   4.960 
   -noise 1                                                               1.00  3.06  3.40  6.48  7.08  6.79  8.25   0.701 
   -noise 2                                                               1.00  3.23  5.30  7.80  8.73  8.63 10.03   0.301 
   -fill blue -fuzz 35% -opaque red                                       1.00  2.60  3.45  3.93  4.43  4.59  4.69  50.000 
   -operator all Add 2%                                                   1.00  3.21  4.71  5.73  7.13  7.75  7.92  41.118 
   -operator all And 233                                                  1.00  1.87  2.12  2.07  2.25  2.21  2.17  44.821 
   -operator all Assign 50%                                               1.00  1.48  1.62  1.53  1.66  1.59  1.31  44.910 
   -operator all Depth 6                                                  1.00  2.34  2.95  2.98  3.17  3.08  2.54  42.400 
   -operator all Divide 2                                                 1.00  3.16  4.85  5.98  7.39  8.25  8.46  40.200 
   -operator all Gamma 0.7                                                1.00  1.95  2.11  2.13  2.29  2.21  2.17  30.279 
   -operator all Negate 1.0                                               1.00  1.49  1.62  1.42  1.40  1.38  1.33  43.114 
   -operator all LShift 2                                                 1.00  1.89  2.42  2.34  2.55  2.18  2.01  44.800 
   -operator all Multiply 0.5                                             1.00  3.24  4.94  6.28  7.72  8.84  8.89  39.400 
   -operator all Or 233                                                   1.00  1.90  2.03  2.02  2.55  2.52  2.20  44.800 
   -operator all RShift 2                                                 1.00  1.89  2.03  1.93  2.25  2.51  2.46  44.311 
   -operator all Subtract 10%                                             1.00  2.98  4.27  5.29  6.08  6.75  6.64  40.519 
   -operator red Threshold 50%                                            1.00  1.43  1.51  1.43  1.86  1.85  1.52  45.020 
   -operator gray Threshold 50%                                           1.00  1.95  2.10  1.99  2.43  2.61  2.54  44.511 
   -operator all Threshold-White 80%                                      1.00  2.13  2.60  2.68  3.28  3.36  3.39  52.695 
   -operator all Threshold-Black 10%                                      1.00  2.07  2.50  2.74  3.26  3.30  3.31  51.497 
   -operator all Xor 233                                                  1.00  1.81  1.94  1.83  1.99  2.32  2.34  43.200 
   -operator all Noise-Gaussian 30%                                       1.00  3.57  7.06 10.48  9.85 12.11 14.18   2.495 
   -operator all Noise-Impulse 30%                                        1.00  3.63  6.26  8.52  7.59  8.98  9.91  19.323 
   -operator all Noise-Laplacian 30%                                      1.00  3.73  7.24 10.53 10.67 13.09 15.17   5.400 
   -operator all Noise-Multiplicative 30%                                 1.00  3.63  7.15 10.59 11.83 14.61 16.87   3.762 
   -operator all Noise-Poisson 30%                                        1.00  3.75  7.27 10.70 11.10 13.66 15.89   5.894 
   -operator all Noise-Uniform 30%                                        1.00  3.64  6.39  8.75  9.50 10.98 12.11  22.465 
   -ordered-dither all 2x2                                                1.00  3.19  4.65  5.63  6.10  6.71  7.06  36.128 
   -ordered-dither all 3x3                                                1.00  3.19  4.65  5.58  6.08  6.57  6.93  35.458 
   -ordered-dither intensity 3x3                                          1.00  3.19  4.76  5.62  6.21  6.84  7.20  36.853 
   -ordered-dither all 4x4                                                1.00  3.19  4.65  5.54  5.93  6.03  6.06  31.076 
   -paint 0x1                                                             1.00  3.68  6.79  9.19  8.53 10.29 11.53   8.893 
   -random-threshold all 20x80                                            1.00  2.83  3.96  4.71  5.24  5.18  5.33  38.200 
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.32  1.35  1.13  1.22  1.23  1.19  76.248 
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  3.25  4.93  6.43  5.86  6.65  7.09  30.600 
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  3.37  5.41  7.00  6.53  7.53  8.16  27.745 
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  3.43  5.43  6.98  6.46  7.35  8.03  27.745 
   -density 75x75 -resample 50x50                                         1.00  3.68  6.66  9.16  7.26  7.96  8.36   8.023 
   -resize 10%                                                            1.00  3.31  5.37  6.72  5.78  6.72  6.58  17.460 
   -resize 50%                                                            1.00  3.65  6.34  8.81  6.86  8.14  9.21  12.375 
   -resize 150%                                                           1.00  3.46  5.58  7.34  5.40  5.41  5.28   2.890 
   -rotate 15                                                             1.00  3.26  5.36  7.05  7.34  7.95  8.82   2.724 
   -rotate 45                                                             1.00  2.94  4.55  5.50  5.17  5.21  5.28   0.786 
   -segment 0.5x0.25                                                      1.00  2.00  2.47  2.65  2.59  2.71  2.76   0.047 
   -shade 30x30                                                           1.00  3.30  5.00  6.51  6.62  7.06  8.07  17.659 
   -sharpen 0x0.5                                                         1.00  3.60  6.37  8.65  6.11  7.22  8.29   7.602 
   -sharpen 0x1.0                                                         1.00  3.71  6.73  9.75  5.99  7.39  8.82   3.208 
   -sharpen 0x2.0                                                         1.00  3.61  6.84  9.88  5.61  6.95  8.31   1.077 
   -shear 45x45                                                           1.00  2.82  4.14  4.99  4.34  4.06  4.16   1.213 
   -solarize 50%                                                          1.00  2.12  2.46  2.40  2.65  2.61  2.56  45.200 
   -swirl 90                                                              1.00  3.85  7.22  5.07  9.34 11.86 13.54   8.627 
   -fuzz 35% -transparent red                                             1.00  2.50  3.19  3.57  3.84  3.77  3.84  43.000 
   -trim                                                                  1.00  2.29  2.66  3.17  3.41  3.37  3.53  52.600 
   -fuzz 5% -trim                                                         1.00  3.56  5.92  7.83  7.37  8.71  9.69  23.260 
   -unsharp 0x0.5+20+1                                                    1.00  3.27  4.71  5.64  5.48  5.81  6.14   5.190 
   -unsharp 0x1.0+20+1                                                    1.00  3.42  5.33  6.52  6.19  6.94  7.36   4.600 
   -wave 25x150                                                           1.00  3.79  6.78  8.35  7.67  8.87 10.14   7.859 
   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ========


Ubuntu Linux 11.10 / Intel Xeon E5649 / GCC Compiler
-----------------------------------------------------

.. Last update: Fri Feb 15 08:54:04 CST 2013

The following results were obtained from an Intel Xeon E5649 CPU at
2.53GHz.  This CPU has 12 cores and 24 threads.  Ubtuntu's GCC 4.6.1
compiler was used to build the software:

.. table:: Performance Boost On 12 core Intel Xeon E5649 CPU:

   ============================================== ===== ===== ===== ===== ===== ===== ===== ======== ====
   Operation                                        1     4     8    12    16    20    24    iter/s  thds
   ============================================== ===== ===== ===== ===== ===== ===== ===== ======== ====
   -affine 1,0,0.785,1,0,0 -transform              1.00  2.73  3.59  4.29  5.00  5.33  5.65   3.220   7
   -blur 0x0.5                                     1.00  2.54  2.98  3.23  3.47  3.57  3.63   4.200   7
   -blur 0x1.0                                     1.00  2.72  3.30  3.89  4.25  4.56  4.57   3.557   7
   -blur 0x2.0                                     1.00  2.94  3.94  4.47  5.10  5.74  5.72   2.745   6
   -charcoal 0x1                                   1.00  2.69  3.42  4.10  4.46  4.75  4.86   1.761   7
   -colorspace GRAY                                1.00  2.77  3.34  3.68  3.97  4.14  4.15  17.000   7
   -colorspace HSL                                 1.00  3.05  4.09  5.44  6.37  6.82  7.46   8.661   7
   -colorspace HWB                                 1.00  3.08  3.49  4.52  5.32  6.09  5.97  11.776   6
   -colorspace OHTA                                1.00  2.83  3.09  3.65  3.99  4.11  4.10  16.865   6
   -colorspace YCbCr                               1.00  2.82  3.09  3.73  4.05  4.11  4.18  17.131   7
   -colorspace YIQ                                 1.00  2.80  3.33  3.73  4.04  4.10  3.85  16.832   6
   -colorspace YUV                                 1.00  2.78  3.33  3.75  4.05  4.14  4.16  17.063   7
   -contrast -contrast -contrast                   1.00  3.67  5.25  5.56  6.60  7.52  8.16   2.457   7
   +contrast +contrast +contrast                   1.00  3.67  5.24  5.58  6.59  7.89  8.16   2.481   7
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  3.16  3.90  5.32  5.94  6.51  6.58  10.600   7
   -despeckle                                      1.00  3.20  3.61  3.77  3.82  3.89  3.68   1.156   6
   -edge 0x1                                       1.00  3.15  5.15  5.57  6.26  7.01  7.07   9.742   7
   -emboss 0x1                                     1.00  3.02  4.24  5.24  5.88  6.52  6.64   4.215   7
   -enhance                                        1.00  3.56  4.12  6.23  7.41  9.38  9.04   1.501   6
   -gaussian 0x0.5                                 1.00  3.46  4.92  6.65  7.74  8.97  9.21   6.561   7
   -gaussian 0x1.0                                 1.00  3.66  5.00  7.53  8.74  9.29 10.30   2.677   7
   -gaussian 0x2.0                                 1.00  3.70  5.86  7.39  8.54  9.60 10.49   0.839   7
   -hald-clut identity:8                           1.00  2.97  4.03  5.55  6.50  7.30  7.65   7.540   7
   -hald-clut identity:10                          1.00  2.97  4.36  5.76  6.55  7.59  7.78   7.570   7
   -hald-clut identity:14                          1.00  3.18  4.11  5.39  6.28  7.33  7.40   6.445   7
   -implode 0.5                                    1.00  3.30  4.32  5.43  6.74  7.55  8.07   3.187   7
   -implode -1                                     1.00  3.04  5.07  5.38  6.47  7.62  7.83   4.348   7
   -lat 10x10-5%                                   1.00  1.03  1.03  1.03  1.03  1.02  1.02   1.211   3
   -median 1                                       1.00  3.53  4.95  5.80  6.55  6.90  7.62   0.655   7
   -median 2                                       1.00  3.68  5.32  5.32  7.35  9.90  9.10   0.307   6
   -minify                                         1.00  2.91  3.74  4.67  5.02  5.44  5.28  17.400   6
   -modulate 110/100/95                            1.00  3.18  3.89  5.41  6.40  7.50  7.64   7.255   7
   -motion-blur 0x3+30                             1.00  3.33  3.23  4.67  5.20  5.91  6.78   1.328   7
   +noise Uniform                                  1.00  2.22  2.61  3.00  3.21  3.43  3.44   5.871   7
   +noise Gaussian                                 1.00  3.39  4.29  5.67  6.63  7.17  8.20   1.230   7
   +noise Multiplicative                           1.00  3.44  4.29  5.41  6.34  7.22  7.78   1.689   7
   +noise Impulse                                  1.00  2.31  2.78  3.07  3.35  3.49  3.59   5.765   7
   +noise Laplacian                                1.00  3.42  4.91  5.29  5.99  6.64  7.08   2.534   7
   +noise Poisson                                  1.00  3.46  4.93  5.33  6.65  7.12  7.64   2.268   7
   +noise Random                                   1.00  2.67  3.14  3.71  4.13  4.33  4.51   5.029   7
   -noise 1                                        1.00  3.61  4.99  5.81  7.12  6.77  7.73   0.649   7
   -noise 2                                        1.00  3.77  5.70  5.43  7.60 10.10  9.40   0.303   6
   -operator all Add 2%                            1.00  2.97  4.02  4.40  4.84  4.93  4.79  21.956   6
   -operator all Divide 2                          1.00  2.92  3.90  4.99  5.52  5.60  5.56  19.960   6
   -operator all Multiply 0.5                      1.00  2.88  4.43  4.70  5.13  5.25  5.04  20.833   6
   -operator all Subtract 10%                      1.00  2.95  4.01  4.58  4.69  4.87  4.71  21.400   6
   -operator all Noise-Gaussian 30%                1.00  3.53  4.46  6.01  7.14  7.56  8.86   1.365   7
   -operator all Noise-Impulse 30%                 1.00  3.10  3.62  4.79  5.53  6.32  6.27  11.730   6
   -operator all Noise-Laplacian 30%               1.00  3.76  4.54  6.01  7.37  8.62  8.95   3.257   7
   -operator all Noise-Multiplicative 30%          1.00  3.62  4.26  6.14  7.10  7.67  8.98   2.011   7
   -operator all Noise-Poisson 30%                 1.00  3.68  4.36  5.69  7.33  7.64  8.80   2.772   7
   -operator all Noise-Uniform 30%                 1.00  3.18  3.52  4.65  5.34  5.70  6.10  12.079   7
   -ordered-dither all 2x2                         1.00  2.68  3.22  3.69  3.94  4.27  4.10  19.522   6
   -ordered-dither all 3x3                         1.00  2.69  3.21  3.80  4.08  4.10  4.14  18.887   7
   -ordered-dither intensity 3x3                   1.00  2.69  3.22  3.71  4.00  3.90  4.16  18.962   7
   -ordered-dither all 4x4                         1.00  2.69  3.21  3.78  4.01  4.07  4.06  18.563   6
   -paint 0x1                                      1.00  3.11  3.82  5.70  6.64  7.20  7.77   5.675   7
   -random-threshold all 20x80                     1.00  2.60  2.85  3.31  3.52  3.54  3.53  20.000   6
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  2.89  3.27  4.14  4.65  4.84  5.04  15.968   7
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  2.98  3.27  4.24  4.83  4.94  5.20  14.200   7
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  2.99  3.35  4.23  4.74  5.38  5.29  14.741   6
   -density 75x75 -resample 50x50                  1.00  3.15  4.28  5.25  5.48  5.37  5.18   5.039   5
   -resize 10%                                     1.00  3.15  4.11  4.71  5.32  5.69  5.78  10.651   7
   -resize 50%                                     1.00  3.05  3.92  5.00  5.70  6.12  6.08   7.466   6
   -resize 150%                                    1.00  3.20  5.15  5.59  6.00  5.64  5.30   2.544   5
   -rotate 15                                      1.00  3.27  4.81  6.14  7.29  8.04  8.48   2.662   7
   -rotate 45                                      1.00  3.03  4.07  5.29  6.14  6.75  7.12   1.047   7
   -shade 30x30                                    1.00  2.92  3.46  4.41  5.02  5.11  5.50  10.516   7
   -sharpen 0x0.5                                  1.00  3.37  5.01  6.63  7.76  8.42  9.20   6.535   7
   -sharpen 0x1.0                                  1.00  3.68  5.50  6.84  8.47 10.48 10.58   2.750   7
   -sharpen 0x2.0                                  1.00  3.65  5.88  7.40  8.65 10.23 11.06   0.896   7
   -shear 45x45                                    1.00  2.95  3.79  4.76  5.24  5.55  5.82   1.460   7
   -swirl 90                                       1.00  3.13  4.61  5.93  6.80  7.69  8.38   4.297   7
   -fuzz 5% -trim                                  1.00  2.99  3.49  4.46  5.04  5.59  5.51  14.343   6
   -unsharp 0x0.5+20+1                             1.00  2.61  3.31  3.67  3.98  4.10  4.18   3.725   7
   -unsharp 0x1.0+20+1                             1.00  2.90  3.61  4.14  4.59  4.96  4.93   3.214   6
   -wave 25x150                                    1.00  2.99  3.95  5.05  5.65  6.71  6.91   4.864   7
   ============================================== ===== ===== ===== ===== ===== ===== ===== ======== ====


Ubuntu Linux 11.10 / AMD Opteron 6220 / Open64 Compiler
--------------------------------------------------------

.. Last update: Wed Dec 21 15:40:08 CST 2011

The following results were obtained using an AMD Opteron 6220 CPU with
AMD's branch of the Open64 Compiler.  This system offered 16
processing cores with a clock rate of 3GHz.  This CPU agressively
increases its clock rate with just a few threads running.  This throws
off the naive per-thread speedup calculation, which is based on the
performance with just one thread.  In spite of relatively low reported
per-thread speed-up values, compare total performance with the test
run using the GCC compiler:

.. table:: Performance Boost On 16 core AMD Opteron 6220 CPU:

   ===================================================================== ===== ===== ===== ===== ===== ======== ====
   Operation                                                               1     4     8    12    16    iter/s  thds
   ===================================================================== ===== ===== ===== ===== ===== ======== ====
   -noop                                                                  1.00  1.20  1.14  1.10  0.97  55.100   4
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  2.07  4.65  5.74  4.81   3.540  15
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  1.93  2.02  2.07  1.88  25.050  12
   -blur 0x0.5                                                            1.00  2.17  3.12  3.39  3.26   3.674  13
   -blur 0x1.0                                                            1.00  2.38  3.66  4.21  4.46   3.431  16
   -blur 0x2.0                                                            1.00  2.24  3.70  4.51  4.93   2.843  16
   -charcoal 0x1                                                          1.00  2.20  2.88  3.43  3.60   1.488  13
   -colorspace CMYK                                                       1.00  1.78  1.77  1.75  1.59  24.076   5
   -colorspace GRAY                                                       1.00  2.40  3.48  4.14  4.18  22.732  15
   -colorspace HSL                                                        1.00  2.51  4.63  6.33  7.37  13.886  16
   -colorspace HWB                                                        1.00  3.02  5.11  6.81  8.13  16.617  16
   -colorspace OHTA                                                       1.00  2.41  3.49  4.16  4.21  22.700  15
   -colorspace YCbCr                                                      1.00  2.41  3.51  4.17  4.24  22.854  15
   -colorspace YIQ                                                        1.00  2.39  3.48  4.14  4.18  22.754  15
   -colorspace YUV                                                        1.00  2.40  3.47  4.14  4.19  22.732  15
   -contrast -contrast -contrast                                          1.00  2.98  5.76  8.47 10.76   3.766  16
   +contrast +contrast +contrast                                          1.00  2.99  5.74  8.45 10.86   3.953  16
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  2.13  3.66  4.72  5.46  10.568  16
   -colorize 30%/20%/50%                                                  1.00  1.78  2.84  3.51  3.24  19.522  12
   -despeckle                                                             1.00  2.33  2.34  2.34  2.34   0.293  12
   -edge 0x1                                                              1.00  2.35  4.20  5.61  6.32   9.633  16
   -emboss 0x1                                                            1.00  1.95  3.08  3.70  4.05   3.393  16
   -enhance                                                               1.00  2.56  5.06  7.51  9.89   1.602  16
   -gaussian 0x0.5                                                        1.00  2.16  4.06  5.53  6.62   7.065  16
   -gaussian 0x1.0                                                        1.00  2.31  4.36  6.16  8.32   3.287  16
   -gaussian 0x2.0                                                        1.00  2.32  4.60  6.69  8.94   1.117  16
   -hald-clut identity:8                                                  1.00  2.84  5.11  7.02  8.23  12.202  16
   -hald-clut identity:10                                                 1.00  2.83  5.04  6.97  8.24  11.817  16
   -hald-clut identity:14                                                 1.00  2.86  5.14  6.87  8.05  10.050  16
   -implode 0.5                                                           1.00  3.00  5.38  4.62  5.49   2.852  13
   -implode -1                                                            1.00  2.90  5.49  3.97  4.70   5.556  13
   -lat 10x10-5%                                                          1.00  2.45  4.44  5.25  6.43   3.370  16
   -median 1                                                              1.00  3.12  5.36  7.82  9.23   0.849  16
   -median 2                                                              1.00  3.28  6.21  9.21 12.10   0.351  16
   -minify                                                                1.00  2.04  3.42  4.34  4.74  16.200  15
   -modulate 110/100/95                                                   1.00  2.95  5.40  7.29  9.18  11.800  16
   +noise Uniform                                                         1.00  2.37  3.74  4.49  5.04  10.417  16
   +noise Gaussian                                                        1.00  3.35  6.53  9.55 12.39   1.722  16
   +noise Multiplicative                                                  1.00  3.28  6.28  8.92 11.48   2.584  16
   +noise Impulse                                                         1.00  2.67  4.37  5.50  6.16   9.335  16
   +noise Laplacian                                                       1.00  3.48  6.55  9.42 11.81   3.366  16
   +noise Poisson                                                         1.00  3.15  5.97  8.37 10.57   3.785  16
   -noise 1                                                               1.00  3.13  5.99  7.66  9.85   0.896  16
   -noise 2                                                               1.00  3.24  6.38  9.10 11.62   0.337  16
   -fill blue -fuzz 35% -opaque red                                       1.00  2.05  2.83  3.14  3.18  29.341  16
   -operator all Add 2%                                                   1.00  2.08  2.75  3.03  2.76  25.375  12
   -operator all And 233                                                  1.00  1.33  1.28  1.26  1.13  28.072   4
   -operator all Assign 50%                                               1.00  1.29  1.23  1.21  1.08  28.044   4
   -operator all Depth 6                                                  1.00  1.66  1.64  1.60  1.44  27.000   4
   -operator all Divide 2                                                 1.00  2.03  2.82  3.17  2.95  24.850  12
   -operator all Gamma 0.7                                                1.00  1.47  1.36  1.37  1.24  22.455   4
   -operator all Negate 1.0                                               1.00  1.34  1.30  1.28  1.15  27.672   4
   -operator all LShift 2                                                 1.00  1.34  1.29  1.27  1.13  28.000   4
   -operator all Multiply 0.5                                             1.00  2.06  2.72  3.00  2.72  25.424  12
   -operator all Or 233                                                   1.00  1.34  1.28  1.27  1.13  28.100   4
   -operator all RShift 2                                                 1.00  1.34  1.28  1.27  1.13  28.072   4
   -operator all Subtract 10%                                             1.00  2.24  3.09  3.49  3.22  24.850  12
   -operator red Threshold 50%                                            1.00  1.22  1.16  1.14  1.02  28.372   4
   -operator gray Threshold 50%                                           1.00  1.64  1.60  1.59  1.42  27.246   4
   -operator all Threshold-White 80%                                      1.00  1.95  2.06  2.06  1.87  33.500  11
   -operator all Threshold-Black 10%                                      1.00  1.94  2.05  2.06  1.85  32.900   9
   -operator all Xor 233                                                  1.00  1.33  1.28  1.26  1.13  28.144   4
   -operator all Noise-Gaussian 30%                                       1.00  3.38  6.65  9.85 12.97   1.829  16
   -operator all Noise-Impulse 30%                                        1.00  2.88  5.12  6.94  8.42  13.174  16
   -operator all Noise-Laplacian 30%                                      1.00  3.46  6.75  9.79 12.82   3.770  16
   -operator all Noise-Multiplicative 30%                                 1.00  3.32  6.49  9.40 12.31   2.794  16
   -operator all Noise-Poisson 30%                                        1.00  3.19  6.17  9.01 11.65   4.310  16
   -operator all Noise-Uniform 30%                                        1.00  2.55  4.39  5.88  6.97  15.085  16
   -ordered-dither all 2x2                                                1.00  2.43  2.97  3.05  2.84  22.832  12
   -ordered-dither all 3x3                                                1.00  2.47  3.05  3.14  2.86  23.529  12
   -ordered-dither intensity 3x3                                          1.00  2.42  2.96  3.04  2.84  22.877  12
   -ordered-dither all 4x4                                                1.00  2.46  3.01  3.11  2.89  23.207  12
   -paint 0x1                                                             1.00  2.41  4.70  6.53  7.88   5.373  16
   -random-threshold all 20x80                                            1.00  2.74  3.57  3.72  3.55  23.177  10
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.21  1.15  1.12  0.98  54.945   4
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  2.91  4.66  5.99  6.21  21.457  16
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  3.19  5.28  6.62  7.54  19.821  16
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  3.12  5.10  6.46  7.54  19.880  16
   -density 75x75 -resample 50x50                                         1.00  2.42  4.58  6.33  7.50   7.143  16
   -resize 10%                                                            1.00  2.51  4.22  5.49  6.23  13.686  16
   -resize 50%                                                            1.00  2.24  3.98  6.16  7.10  10.558  16
   -resize 150%                                                           1.00  2.58  4.82  6.56  7.39   2.778  15
   -rotate 15                                                             1.00  2.43  3.78  4.68  5.17   2.532  16
   -rotate 45                                                             1.00  1.98  3.01  3.55  3.79   0.856  16
   -segment 0.5x0.25                                                      1.00  1.86  2.21  2.43  2.50   0.035  14
   -shade 30x30                                                           1.00  2.86  4.85  6.38  6.93  10.789  15
   -sharpen 0x0.5                                                         1.00  2.14  4.01  5.46  6.52   6.944  16
   -sharpen 0x1.0                                                         1.00  2.33  4.37  6.23  8.34   3.284  16
   -sharpen 0x2.0                                                         1.00  2.33  4.58  6.71  8.96   1.120  16
   -shear 45x45                                                           1.00  1.97  2.93  3.48  3.72   1.157  16
   -solarize 50%                                                          1.00  1.82  1.82  1.80  1.63  27.073   9
   -swirl 90                                                              1.00  3.18  5.52  5.62  7.33   5.627  15
   -fuzz 35% -transparent red                                             1.00  2.06  2.72  3.02  2.76  25.449  12
   -trim                                                                  1.00  1.99  2.27  2.45  2.31  32.635  12
   -fuzz 5% -trim                                                         1.00  2.56  4.59  5.94  7.16  14.428  16
   -unsharp 0x0.5+20+1                                                    1.00  2.30  3.43  3.87  4.30   3.593  16
   -unsharp 0x1.0+20+1                                                    1.00  2.32  3.65  4.24  4.57   3.097  16
   -wave 25x150                                                           1.00  3.01  5.18  6.51  8.31   5.765  16
   ===================================================================== ===== ===== ===== ===== ===== ======== ====


Ubuntu Linux 11.10 / AMD Opteron 6220 / GCC Compiler
-----------------------------------------------------

.. Last update: Wed Dec 21 15:40:08 CST 2011

The following results were obtained using an AMD Opteron 6220 CPU.
Ubtuntu's GCC 4.6.1 compiler was used to build the software.
Ubtuntu's GCC has been found to offer less performance for this CPU
(and for Intel Xeon) than the Open64 compiler.  Compare these results
with the Open64 results above.  This system offers 16 processing cores
with a clock rate of 3GHz:

.. table:: Performance Boost On 16 core AMD Opteron 6220 CPU:

   ===================================================================== ===== ===== ===== ===== ===== ======== ====
   Operation                                                               1     4     8    12    16    iter/s  thds
   ===================================================================== ===== ===== ===== ===== ===== ======== ====
   -noop                                                                  1.00  1.97  2.05  1.95  2.07  29.341   7
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  3.73  6.65  6.56  6.63   3.868  15
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  2.37  2.97  2.82  3.18  23.752  16
   -blur 0x0.5                                                            1.00  2.75  3.36  3.55  3.22   3.661  14
   -blur 0x1.0                                                            1.00  2.89  3.69  4.11  4.07   3.156  15
   -blur 0x2.0                                                            1.00  3.21  4.54  4.81  4.95   2.569  15
   -charcoal 0x1                                                          1.00  2.30  3.07  3.32  3.47   1.252  16
   -colorspace CMYK                                                       1.00  2.03  2.39  2.21  2.36  21.627   8
   -colorspace GRAY                                                       1.00  2.70  3.82  3.61  4.27  19.721  16
   -colorspace HSL                                                        1.00  2.92  5.61  5.83  7.24  12.821  16
   -colorspace HWB                                                        1.00  3.42  5.28  6.30  7.79  13.861  16
   -colorspace OHTA                                                       1.00  2.81  3.86  3.59  4.14  19.200  16
   -colorspace YCbCr                                                      1.00  2.83  3.42  3.61  4.14  19.124  16
   -colorspace YIQ                                                        1.00  2.79  3.86  3.60  4.18  19.323  16
   -colorspace YUV                                                        1.00  2.83  3.43  3.62  4.24  19.522  16
   -contrast -contrast -contrast                                          1.00  3.93  6.78  8.59 11.24   3.360  16
   +contrast +contrast +contrast                                          1.00  3.89  7.11  8.65 11.17   3.429  16
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  3.10  5.10  5.65  6.71  10.379  16
   -colorize 30%/20%/50%                                                  1.00  2.68  3.80  3.49  4.15  14.371  16
   -despeckle                                                             1.00  2.95  2.88  2.95  2.91   0.271   4
   -edge 0x1                                                              1.00  3.37  5.61  6.39  7.68   9.182  16
   -emboss 0x1                                                            1.00  2.85  4.24  4.72  5.17   3.194  16
   -enhance                                                               1.00  3.73  7.18  8.20  9.45   1.597  16
   -gaussian 0x0.5                                                        1.00  3.24  6.07  7.01  8.17   6.000  16
   -gaussian 0x1.0                                                        1.00  3.80  6.83  7.92  8.97   2.358  16
   -gaussian 0x2.0                                                        1.00  3.94  6.63  8.44  9.69   0.843  16
   -hald-clut identity:8                                                  1.00  3.11  5.53  5.86  7.56   8.893  16
   -hald-clut identity:10                                                 1.00  3.67  4.40  5.77  7.57   8.583  16
   -hald-clut identity:14                                                 1.00  3.60  4.67  6.19  7.80   7.400  16
   -implode 0.5                                                           1.00  4.08  6.98  9.37 10.09   3.422  16
   -implode -1                                                            1.00  3.49  6.20  8.38  9.22   4.582  16
   -lat 10x10-5%                                                          1.00  3.01  4.29  4.72  5.04   1.685  16
   -median 1                                                              1.00  3.86  6.61  8.01  9.08   0.672  16
   -median 2                                                              1.00  3.96  7.25  9.54 11.54   0.277  16
   -minify                                                                1.00  3.01  4.46  4.58  5.17  15.200  16
   -modulate 110/100/95                                                   1.00  3.50  6.04  6.68  8.60   9.881  16
   +noise Uniform                                                         1.00  2.00  2.29  2.43  2.53   2.857  15
   +noise Gaussian                                                        1.00  4.55  7.58  9.16 10.87   1.152  16
   +noise Multiplicative                                                  1.00  3.74  5.98  7.58  8.95   1.378  16
   +noise Impulse                                                         1.00  2.10  2.48  2.64  2.79   2.846  16
   +noise Laplacian                                                       1.00  3.48  5.32  6.32  7.27   1.775  16
   +noise Poisson                                                         1.00  3.16  4.68  5.74  6.49   1.805  16
   -noise 1                                                               1.00  3.99  6.85  8.46  9.42   0.678  16
   -noise 2                                                               1.00  3.96  7.25  9.54 11.54   0.277  16
   -fill blue -fuzz 35% -opaque red                                       1.00  2.47  3.10  3.08  3.60  24.303  16
   -operator all Add 2%                                                   1.00  2.66  3.17  3.27  3.80  21.912  16
   -operator all And 233                                                  1.00  2.01  2.31  2.12  2.38  24.600  16
   -operator all Assign 50%                                               1.00  1.95  2.27  2.05  2.23  24.303   8
   -operator all Depth 6                                                  1.00  2.08  2.39  2.11  2.32  23.904   8
   -operator all Divide 2                                                 1.00  2.69  3.31  3.35  3.91  21.357  16
   -operator all Gamma 0.7                                                1.00  1.83  2.15  1.94  2.06  20.400   8
   -operator all Negate 1.0                                               1.00  2.00  2.36  2.11  2.37  24.600  16
   -operator all LShift 2                                                 1.00  1.98  2.29  2.08  2.34  24.701  16
   -operator all Multiply 0.5                                             1.00  2.65  3.12  3.27  3.90  22.510  16
   -operator all Or 233                                                   1.00  2.03  2.32  2.15  2.35  24.200  16
   -operator all RShift 2                                                 1.00  1.98  2.29  2.06  2.25  24.056   8
   -operator all Subtract 10%                                             1.00  2.71  3.35  3.58  4.10  20.758  15
   -operator red Threshold 50%                                            1.00  1.83  2.16  1.90  2.00  24.600   8
   -operator gray Threshold 50%                                           1.00  2.14  2.44  2.27  2.53  24.502  16
   -operator all Threshold-White 80%                                      1.00  2.16  2.54  2.36  2.47  24.206   9
   -operator all Threshold-Black 10%                                      1.00  2.20  2.63  2.40  2.57  24.254   8
   -operator all Xor 233                                                  1.00  1.95  2.37  2.13  2.35  24.400   8
   -operator all Noise-Gaussian 30%                                       1.00  3.87  6.25  9.03 11.74   1.667  16
   -operator all Noise-Impulse 30%                                        1.00  3.47  5.94  6.95  8.46  11.858  16
   -operator all Noise-Laplacian 30%                                      1.00  3.89  7.12  9.58 12.26   3.495  16
   -operator all Noise-Multiplicative 30%                                 1.00  4.01  7.40 10.12 12.99   2.196  16
   -operator all Noise-Poisson 30%                                        1.00  3.88  6.67  8.80 11.34   3.482  16
   -operator all Noise-Uniform 30%                                        1.00  3.49  5.22  6.51  8.17  13.069  16
   -ordered-dither all 2x2                                                1.00  2.46  3.36  3.23  3.64  20.717  16
   -ordered-dither all 3x3                                                1.00  2.51  3.24  3.24  3.67  20.833  16
   -ordered-dither intensity 3x3                                          1.00  2.55  3.27  3.26  3.67  20.800  16
   -ordered-dither all 4x4                                                1.00  2.53  3.40  3.28  3.70  20.875  16
   -paint 0x1                                                             1.00  3.60  6.18  6.97  7.99   5.976  16
   -random-threshold all 20x80                                            1.00  2.66  3.77  3.86  4.44  20.833  16
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.92  1.95  1.88  1.97  28.486   7
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  2.87  3.98  4.03  5.02  20.800  16
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  3.03  3.96  4.01  4.95  18.725  16
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  3.02  3.59  3.94  4.96  18.812  16
   -density 75x75 -resample 50x50                                         1.00  3.46  5.19  4.75  5.01   4.669   8
   -resize 10%                                                            1.00  3.42  4.51  5.62  6.53  10.609  16
   -resize 50%                                                            1.00  3.41  4.97  5.56  5.98   7.738  14
   -resize 150%                                                           1.00  3.50  6.01  6.48  7.37   2.820  15
   -rotate 15                                                             1.00  2.70  4.31  4.90  5.61   2.703  16
   -rotate 45                                                             1.00  2.51  3.58  3.94  4.17   0.958  16
   -segment 0.5x0.25                                                      1.00  1.63  1.79  1.84  1.89   0.036  14
   -shade 30x30                                                           1.00  3.38  5.44  6.78  8.08   9.722  16
   -sharpen 0x0.5                                                         1.00  3.12  6.12  7.06  8.28   6.055  16
   -sharpen 0x1.0                                                         1.00  3.74  6.71  7.83  8.86   2.339  16
   -sharpen 0x2.0                                                         1.00  3.68  7.10  8.36  9.69   0.843  16
   -shear 45x45                                                           1.00  2.32  3.29  3.64  3.93   1.308  16
   -solarize 50%                                                          1.00  2.15  2.48  2.26  2.52  24.351  16
   -swirl 90                                                              1.00  3.91  5.95  8.09  9.76   4.391  16
   -fuzz 35% -transparent red                                             1.00  2.40  3.01  3.00  3.49  24.200  16
   -trim                                                                  1.00  2.27  2.68  2.44  2.55  24.551   8
   -fuzz 5% -trim                                                         1.00  3.06  5.06  5.79  6.80  13.972  16
   -unsharp 0x0.5+20+1                                                    1.00  2.78  3.80  4.03  4.27   3.282  16
   -unsharp 0x1.0+20+1                                                    1.00  3.07  3.86  4.53  4.52   2.953  15
   -wave 25x150                                                           1.00  3.47  6.10  7.61  8.76   4.902  15
   ===================================================================== ===== ===== ===== ===== ===== ======== ====


Sun Solaris / AMD Opteron
-------------------------

.. Last update: Wed Dec 21 16:11:28 CST 2011

The following table shows the performance boost in GraphicsMagick
1.4 as threads are added on a four-core AMD Opteron 3.0GHz system
running Sun Solaris 10:

.. table:: Performance Boost On Four Core AMD Operon System

   ===================================================================== ===== ===== ===== ===== ======== ====
   Operation                                                               1     2     3     4    iter/s  thds
   ===================================================================== ===== ===== ===== ===== ======== ====
   -noop                                                                  1.00  1.23  1.22  1.18  32.635   2
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.78  2.92  3.12   0.971   4
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  1.62  1.94  2.07  12.762   4
   -blur 0x0.5                                                            1.00  1.74  2.26  2.56   2.284   4
   -blur 0x1.0                                                            1.00  1.84  2.47  2.99   1.777   4
   -blur 0x2.0                                                            1.00  1.89  2.63  3.22   1.386   4
   -charcoal 0x1                                                          1.00  1.85  2.56  3.08   0.763   4
   -colorspace CMYK                                                       1.00  1.39  1.53  1.49  13.772   3
   -colorspace GRAY                                                       1.00  1.80  2.38  2.84   9.690   4
   -colorspace HSL                                                        1.00  1.96  2.85  3.59   2.991   4
   -colorspace HWB                                                        1.00  1.94  2.77  3.46   4.469   4
   -colorspace OHTA                                                       1.00  1.81  2.44  2.93   9.980   4
   -colorspace YCbCr                                                      1.00  1.81  2.44  2.88   9.800   4
   -colorspace YIQ                                                        1.00  1.81  2.43  2.85   9.652   4
   -colorspace YUV                                                        1.00  1.81  2.44  2.92   9.930   4
   -contrast -contrast -contrast                                          1.00  2.00  2.94  3.88   0.539   4
   +contrast +contrast +contrast                                          1.00  1.99  2.93  3.86   0.591   4
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.92  2.73  3.45   3.689   4
   -colorize 30%/20%/50%                                                  1.00  1.85  2.50  3.04   8.111   4
   -despeckle                                                             1.00  1.51  2.84  2.85   0.254   4
   -edge 0x1                                                              1.00  1.97  2.87  3.68   2.000   4
   -emboss 0x1                                                            1.00  1.86  2.56  3.15   1.536   4
   -enhance                                                               1.00  2.02  3.01  3.95   0.411   4
   -gaussian 0x0.5                                                        1.00  1.97  2.84  3.67   2.109   4
   -gaussian 0x1.0                                                        1.00  2.01  2.96  3.86   0.725   4
   -gaussian 0x2.0                                                        1.00  2.02  3.00  3.95   0.253   4
   -hald-clut identity:8                                                  1.00  1.99  2.91  3.72   2.178   4
   -hald-clut identity:10                                                 1.00  1.98  2.87  3.71   2.240   4
   -hald-clut identity:14                                                 1.00  1.97  2.90  3.67   2.006   4
   -implode 0.5                                                           1.00  1.90  2.88  3.64   0.880   4
   -implode -1                                                            1.00  1.89  2.68  3.54   1.074   4
   -lat 10x10-5%                                                          1.00  1.99  2.91  3.75   1.015   4
   -median 1                                                              1.00  2.03  3.01  3.96   0.277   4
   -median 2                                                              1.00  1.96  2.93  3.89   0.105   4
   -minify                                                                1.00  1.83  2.50  3.08   7.662   4
   -modulate 110/100/95                                                   1.00  1.98  2.89  3.71   2.367   4
   +noise Uniform                                                         1.00  1.90  2.64  3.35   2.616   4
   +noise Gaussian                                                        1.00  2.04  3.01  3.98   0.322   4
   +noise Multiplicative                                                  1.00  2.01  2.97  3.87   0.518   4
   +noise Impulse                                                         1.00  1.90  2.66  3.36   2.449   4
   +noise Laplacian                                                       1.00  1.98  2.86  3.68   1.093   4
   +noise Poisson                                                         1.00  1.99  2.94  3.78   0.836   4
   -noise 1                                                               1.00  2.01  2.96  3.91   0.270   4
   -noise 2                                                               1.00  2.04  3.00  3.96   0.103   4
   -fill blue -fuzz 35% -opaque red                                       1.00  1.68  2.16  2.42  14.612   4
   -operator all Add 2%                                                   1.00  1.81  2.43  2.90  10.689   4
   -operator all And 233                                                  1.00  1.16  1.25  1.19  15.584   3
   -operator all Assign 50%                                               1.00  1.15  1.24  1.19  16.235   3
   -operator all Depth 6                                                  1.00  1.57  1.89  1.93  14.741   4
   -operator all Divide 2                                                 1.00  1.84  2.47  2.97   9.742   4
   -operator all Gamma 0.7                                                1.00  1.50  1.72  1.77  12.724   4
   -operator all Negate 1.0                                               1.00  1.17  1.20  1.20  14.841   4
   -operator all LShift 2                                                 1.00  1.25  1.34  1.34  14.770   4
   -operator all Multiply 0.5                                             1.00  1.81  2.42  2.86  10.558   4
   -operator all Or 233                                                   1.00  1.16  1.25  1.18  15.584   3
   -operator all RShift 2                                                 1.00  1.28  1.43  1.40  15.800   3
   -operator all Subtract 10%                                             1.00  1.82  2.45  2.90  10.338   4
   -operator red Threshold 50%                                            1.00  1.15  1.24  1.19  15.637   3
   -operator gray Threshold 50%                                           1.00  1.39  1.58  1.56  15.622   3
   -operator all Threshold-White 80%                                      1.00  1.28  1.43  1.44  16.783   4
   -operator all Threshold-Black 10%                                      1.00  1.31  1.46  1.56  17.313   4
   -operator all Xor 233                                                  1.00  1.17  1.27  1.22  16.168   3
   -operator all Noise-Gaussian 30%                                       1.00  2.01  3.00  3.93   0.326   4
   -operator all Noise-Impulse 30%                                        1.00  1.96  2.87  3.63   2.756   4
   -operator all Noise-Laplacian 30%                                      1.00  2.00  2.93  3.82   1.162   4
   -operator all Noise-Multiplicative 30%                                 1.00  1.99  2.95  3.88   0.531   4
   -operator all Noise-Poisson 30%                                        1.00  2.00  2.94  3.86   0.877   4
   -operator all Noise-Uniform 30%                                        1.00  1.95  2.82  3.60   2.962   4
   -ordered-dither all 2x2                                                1.00  1.83  2.46  2.95  10.479   4
   -ordered-dither all 3x3                                                1.00  1.81  2.40  2.88  10.259   4
   -ordered-dither intensity 3x3                                          1.00  1.82  2.44  2.89  10.317   4
   -ordered-dither all 4x4                                                1.00  1.82  2.45  2.94  10.479   4
   -paint 0x1                                                             1.00  2.00  2.93  3.85   1.139   4
   -random-threshold all 20x80                                            1.00  1.84  2.50  2.99   9.037   4
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.22  1.21  1.17  32.368   2
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  1.89  2.63  3.24   6.931   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  1.87  2.59  3.05   6.207   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  1.89  2.65  3.24   6.567   4
   -density 75x75 -resample 50x50                                         1.00  1.53  2.21  2.74   0.954   4
   -resize 10%                                                            1.00  1.64  2.38  3.03   3.210   4
   -resize 50%                                                            1.00  1.71  2.47  3.06   2.157   4
   -resize 150%                                                           1.00  1.48  2.12  2.61   0.384   4
   -rotate 15                                                             1.00  1.72  2.44  2.97   0.463   4
   -rotate 45                                                             1.00  1.76  2.34  2.93   0.205   4
   -segment 0.5x0.25                                                      1.00  1.46  1.77  2.08   0.027   4
   -shade 30x30                                                           1.00  1.90  2.69  3.37   3.575   4
   -sharpen 0x0.5                                                         1.00  1.97  2.87  3.68   2.115   4
   -sharpen 0x1.0                                                         1.00  2.01  2.97  3.87   0.735   4
   -sharpen 0x2.0                                                         1.00  2.02  2.98  3.94   0.252   4
   -shear 45x45                                                           1.00  1.68  2.20  2.71   0.244   4
   -solarize 50%                                                          1.00  1.40  1.59  1.58  15.569   3
   -swirl 90                                                              1.00  1.91  2.70  3.68   1.089   4
   -fuzz 35% -transparent red                                             1.00  1.75  2.28  2.64  12.961   4
   -trim                                                                  1.00  1.49  1.76  1.80  18.981   4
   -fuzz 5% -trim                                                         1.00  1.85  2.66  3.38   4.433   4
   -unsharp 0x0.5+20+1                                                    1.00  1.79  2.42  2.85   1.907   4
   -unsharp 0x1.0+20+1                                                    1.00  1.87  2.54  3.09   1.487   4
   -wave 25x150                                                           1.00  1.63  2.00  2.06   0.793   4
   ===================================================================== ===== ===== ===== ===== ======== ====


Sun Solaris / UltraSPARC III
----------------------------

.. Last update: Wed Dec 21 16:14:39 CST 2011

The following table shows the performance boost as threads are added
on 2 CPU Sun SPARC 1.2GHz workstation running Sun Solaris 10.  This
system obtains quite substantial benefit for most key algorithms:

.. table:: Performance Boost On Two CPU SPARC System

   ===================================================================== ===== ===== ======== ====
   Operation                                                               1     2    iter/s  thds
   ===================================================================== ===== ===== ======== ====
   -noop                                                                  1.00  1.14  13.917   2
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.95   0.158   2
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  1.51   2.852   2
   -blur 0x0.5                                                            1.00  1.77   0.347   2
   -blur 0x1.0                                                            1.00  1.83   0.267   2
   -blur 0x2.0                                                            1.00  1.87   0.187   2
   -charcoal 0x1                                                          1.00  1.76   0.123   2
   -colorspace CMYK                                                       1.00  1.30   2.338   2
   -colorspace GRAY                                                       1.00  1.80   1.275   2
   -colorspace HSL                                                        1.00  1.89   0.531   2
   -colorspace HWB                                                        1.00  1.90   0.607   2
   -colorspace OHTA                                                       1.00  1.78   1.289   2
   -colorspace YCbCr                                                      1.00  1.78   1.292   2
   -colorspace YIQ                                                        1.00  1.79   1.292   2
   -colorspace YUV                                                        1.00  1.78   1.287   2
   -contrast -contrast -contrast                                          1.00  1.97   0.077   2
   +contrast +contrast +contrast                                          1.00  1.95   0.080   2
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.90   0.521   2
   -colorize 30%/20%/50%                                                  1.00  1.77   1.304   2
   -despeckle                                                             1.00  1.50   0.030   2
   -edge 0x1                                                              1.00  1.90   0.295   2
   -emboss 0x1                                                            1.00  1.77   0.223   2
   -enhance                                                               1.00  2.00   0.080   2
   -gaussian 0x0.5                                                        1.00  1.92   0.278   2
   -gaussian 0x1.0                                                        1.00  1.96   0.110   2
   -gaussian 0x2.0                                                        1.00  2.00   0.034   2
   -hald-clut identity:8                                                  1.00  1.94   0.382   2
   -hald-clut identity:10                                                 1.00  1.93   0.382   2
   -hald-clut identity:14                                                 1.00  1.91   0.323   2
   -implode 0.5                                                           1.00  1.94   0.159   2
   -implode -1                                                            1.00  1.93   0.195   2
   -lat 10x10-5%                                                          1.00  1.90   0.201   2
   -median 1                                                              1.00  1.97   0.069   2
   -median 2                                                              1.00  1.93   0.027   2
   -minify                                                                1.00  1.81   1.625   2
   -modulate 110/100/95                                                   1.00  1.94   0.283   2
   +noise Uniform                                                         1.00  1.91   0.321   2
   +noise Gaussian                                                        1.00  2.00   0.056   2
   +noise Multiplicative                                                  1.00  1.96   0.090   2
   +noise Impulse                                                         1.00  1.89   0.305   2
   +noise Laplacian                                                       1.00  1.95   0.164   2
   +noise Poisson                                                         1.00  2.00   0.096   2
   -noise 1                                                               1.00  1.94   0.066   2
   -noise 2                                                               1.00  2.00   0.026   2
   -fill blue -fuzz 35% -opaque red                                       1.00  1.79   1.619   2
   -operator all Add 2%                                                   1.00  1.87   1.186   2
   -operator all And 233                                                  1.00  1.54   3.593   2
   -operator all Assign 50%                                               1.00  1.38   3.976   2
   -operator all Depth 6                                                  1.00  1.54   3.320   2
   -operator all Divide 2                                                 1.00  1.86   0.931   2
   -operator all Gamma 0.7                                                1.00  1.52   3.131   2
   -operator all Negate 1.0                                               1.00  1.61   3.605   2
   -operator all LShift 2                                                 1.00  1.59   3.626   2
   -operator all Multiply 0.5                                             1.00  1.88   1.190   2
   -operator all Or 233                                                   1.00  1.57   3.633   2
   -operator all RShift 2                                                 1.00  1.56   3.619   2
   -operator all Subtract 10%                                             1.00  1.83   1.228   2
   -operator red Threshold 50%                                            1.00  1.49   3.755   2
   -operator gray Threshold 50%                                           1.00  1.61   2.772   2
   -operator all Threshold-White 80%                                      1.00  1.73   2.879   2
   -operator all Threshold-Black 10%                                      1.00  1.68   2.841   2
   -operator all Xor 233                                                  1.00  1.54   3.640   2
   -operator all Noise-Gaussian 30%                                       1.00  2.00   0.058   2
   -operator all Noise-Impulse 30%                                        1.00  1.85   0.322   2
   -operator all Noise-Laplacian 30%                                      1.00  1.90   0.165   2
   -operator all Noise-Multiplicative 30%                                 1.00  1.96   0.090   2
   -operator all Noise-Poisson 30%                                        1.00  1.96   0.098   2
   -operator all Noise-Uniform 30%                                        1.00  1.84   0.340   2
   -ordered-dither all 2x2                                                1.00  1.76   1.468   2
   -ordered-dither all 3x3                                                1.00  1.77   1.498   2
   -ordered-dither intensity 3x3                                          1.00  1.78   1.493   2
   -ordered-dither all 4x4                                                1.00  1.78   1.501   2
   -paint 0x1                                                             1.00  1.97   0.140   2
   -random-threshold all 20x80                                            1.00  1.83   1.156   2
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.01  13.439   2
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  1.87   1.017   2
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  1.86   0.817   2
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  1.89   0.835   2
   -density 75x75 -resample 50x50                                         1.00  1.86   0.257   2
   -resize 10%                                                            1.00  1.80   0.527   2
   -resize 50%                                                            1.00  1.85   0.382   2
   -resize 150%                                                           1.00  1.86   0.108   2
   -rotate 15                                                             1.00  1.72   0.148   2
   -rotate 45                                                             1.00  1.62   0.063   2
   -segment 0.5x0.25                                                      1.00  1.33   0.004   2
   -shade 30x30                                                           1.00  1.92   0.383   2
   -sharpen 0x0.5                                                         1.00  1.93   0.278   2
   -sharpen 0x1.0                                                         1.00  1.96   0.110   2
   -sharpen 0x2.0                                                         1.00  2.00   0.034   2
   -shear 45x45                                                           1.00  1.47   0.103   2
   -solarize 50%                                                          1.00  1.51   3.288   2
   -swirl 90                                                              1.00  1.96   0.196   2
   -fuzz 35% -transparent red                                             1.00  1.91   1.487   2
   -trim                                                                  1.00  1.59   3.488   2
   -fuzz 5% -trim                                                         1.00  1.94   0.565   2
   -unsharp 0x0.5+20+1                                                    1.00  1.79   0.272   2
   -unsharp 0x1.0+20+1                                                    1.00  1.83   0.219   2
   -wave 25x150                                                           1.00  1.85   0.207   2
   ===================================================================== ===== ===== ======== ====


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

.. Last update: Wed Dec 21 16:16:35 CST 2011

The following shows the performance boost on a 2003 vintage 2-CPU
hyperthreaded Intel Xeon system running at 2.4GHz.  The operating
system used is FreeBSD 8.0.  Due to the hyperthreading support, this
system thinks it has four CPUs even though it really only has two
cores.  This can lead to very strange results since sometimes it seems
that the first two threads allocated may be from the same CPU,
resulting in much less boost than expected, but obtaining full boost
with four threads.  While the threading on this system behaves poorly
for "fast" algorithms, it is clear that OpenMP works well for "slow"
algorithms, and some algorithms show clear benefit from
hyperthreading:

.. table:: Performance Boost On Two CPU Xeon System

   ===================================================================== ===== ===== ===== ===== ======== ====
   Operation                                                               1     2     3     4    iter/s  thds
   ===================================================================== ===== ===== ===== ===== ======== ====
   -noop                                                                  1.00  1.00  1.00  1.00   3.253   2
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.73  1.54  1.85   0.300   4
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  1.08  1.15  1.09   2.023   3
   -blur 0x0.5                                                            1.00  1.24  1.23  1.28   0.354   4
   -blur 0x1.0                                                            1.00  1.24  1.30  1.39   0.329   4
   -blur 0x2.0                                                            1.00  1.43  1.35  1.52   0.275   4
   -charcoal 0x1                                                          1.00  1.39  1.32  1.35   0.171   2
   -colorspace CMYK                                                       1.00  0.91  0.89  0.87   1.925   1
   -colorspace GRAY                                                       1.00  1.39  1.34  1.49   1.571   4
   -colorspace HSL                                                        1.00  1.77  1.64  2.03   0.856   4
   -colorspace HWB                                                        1.00  1.68  1.72  2.04   1.187   4
   -colorspace OHTA                                                       1.00  1.39  1.34  1.53   1.602   4
   -colorspace YCbCr                                                      1.00  1.36  1.36  1.54   1.618   4
   -colorspace YIQ                                                        1.00  1.38  1.34  1.50   1.580   4
   -colorspace YUV                                                        1.00  1.38  1.35  1.54   1.616   4
   -contrast -contrast -contrast                                          1.00  1.95  2.05  2.61   0.214   4
   +contrast +contrast +contrast                                          1.00  1.93  1.99  2.57   0.221   4
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.57  1.52  1.57   0.812   4
   -colorize 30%/20%/50%                                                  1.00  1.46  1.37  1.46   1.082   2
   -despeckle                                                             1.00  1.44  1.94  1.97   0.063   4
   -edge 0x1                                                              1.00  1.24  1.56  1.64   0.812   4
   -emboss 0x1                                                            1.00  1.60  1.48  1.55   0.359   2
   -enhance                                                               1.00  1.96  1.69  2.09   0.115   4
   -gaussian 0x0.5                                                        1.00  1.75  1.56  1.69   0.477   2
   -gaussian 0x1.0                                                        1.00  1.86  1.62  1.93   0.205   4
   -gaussian 0x2.0                                                        1.00  1.91  1.88  2.21   0.075   4
   -hald-clut identity:8                                                  1.00  1.85  2.00  2.48   0.629   4
   -hald-clut identity:10                                                 1.00  1.85  2.01  2.52   0.602   4
   -hald-clut identity:14                                                 1.00  1.51  2.14  2.65   0.464   4
   -implode 0.5                                                           1.00  1.92  1.92  2.51   0.233   4
   -implode -1                                                            1.00  1.88  1.88  2.36   0.380   4
   -lat 10x10-5%                                                          1.00  1.59  1.23  1.34   0.305   2
   -median 1                                                              1.00  1.96  1.46  1.39   0.055   2
   -median 2                                                              1.00  2.00  1.80  2.20   0.022   4
   -minify                                                                1.00  1.50  1.39  1.47   1.329   2
   -modulate 110/100/95                                                   1.00  1.83  1.83  2.28   0.717   4
   +noise Uniform                                                         1.00  1.78  2.03  2.49   0.394   4
   +noise Gaussian                                                        1.00  1.53  2.37  2.86   0.163   4
   +noise Multiplicative                                                  1.00  1.94  2.76  2.98   0.185   4
   +noise Impulse                                                         1.00  1.79  2.43  2.58   0.400   4
   +noise Laplacian                                                       1.00  1.84  2.42  2.65   0.286   4
   +noise Poisson                                                         1.00  1.92  2.41  3.12   0.159   4
   -noise 1                                                               1.00  0.75  1.68  1.71   0.048   4
   -noise 2                                                               1.00  2.00  1.60  2.20   0.022   4
   -fill blue -fuzz 35% -opaque red                                       1.00  1.24  1.20  1.28   2.537   4
   -operator all Add 2%                                                   1.00  1.33  1.30  1.31   2.092   2
   -operator all And 233                                                  1.00  0.96  1.01  0.90   2.242   3
   -operator all Assign 50%                                               1.00  0.99  1.03  0.97   2.265   3
   -operator all Depth 6                                                  1.00  0.99  1.01  0.93   2.204   3
   -operator all Divide 2                                                 1.00  1.50  1.32  1.53   1.623   4
   -operator all Gamma 0.7                                                1.00  0.98  1.02  0.94   2.035   3
   -operator all Negate 1.0                                               1.00  0.97  1.01  0.92   2.239   3
   -operator all LShift 2                                                 1.00  0.97  1.00  0.91   2.249   3
   -operator all Multiply 0.5                                             1.00  1.34  1.27  1.33   2.095   2
   -operator all Or 233                                                   1.00  0.98  1.01  0.92   2.233   3
   -operator all RShift 2                                                 1.00  0.97  1.00  0.92   2.242   3
   -operator all Subtract 10%                                             1.00  1.35  1.33  1.39   2.101   4
   -operator red Threshold 50%                                            1.00  0.97  1.01  0.91   2.246   3
   -operator gray Threshold 50%                                           1.00  0.99  1.01  0.93   2.246   3
   -operator all Threshold-White 80%                                      1.00  1.08  1.09  1.07   2.655   3
   -operator all Threshold-Black 10%                                      1.00  1.10  1.12  1.10   2.576   3
   -operator all Xor 233                                                  1.00  0.96  1.02  0.91   2.276   3
   -operator all Noise-Gaussian 30%                                       1.00  1.48  2.20  2.86   0.189   4
   -operator all Noise-Impulse 30%                                        1.00  1.86  1.86  2.36   0.587   4
   -operator all Noise-Laplacian 30%                                      1.00  1.92  2.06  2.64   0.383   4
   -operator all Noise-Multiplicative 30%                                 1.00  1.96  2.24  2.92   0.242   4
   -operator all Noise-Poisson 30%                                        1.00  1.96  2.17  2.84   0.213   4
   -operator all Noise-Uniform 30%                                        1.00  1.86  1.83  2.28   0.578   4
   -ordered-dither all 2x2                                                1.00  1.46  1.48  1.56   1.613   4
   -ordered-dither all 3x3                                                1.00  1.47  1.46  1.58   1.643   4
   -ordered-dither intensity 3x3                                          1.00  1.48  1.48  1.60   1.653   4
   -ordered-dither all 4x4                                                1.00  1.19  1.50  1.61   1.665   4
   -paint 0x1                                                             1.00  1.24  1.80  2.30   0.324   4
   -random-threshold all 20x80                                            1.00  1.58  1.72  1.86   1.422   4
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.00  1.00  1.00   3.267   2
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  1.47  1.33  1.52   1.789   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  1.52  1.42  1.62   1.632   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  1.52  1.40  1.61   1.623   4
   -density 75x75 -resample 50x50                                         1.00  1.33  1.54  1.39   0.208   3
   -resize 10%                                                            1.00  1.61  1.31  1.53   0.658   2
   -resize 50%                                                            1.00  1.60  1.39  1.55   0.361   2
   -resize 150%                                                           1.00  1.48  1.58  1.65   0.086   4
   -rotate 15                                                             1.00  1.25  1.34  1.40   0.116   4
   -rotate 45                                                             1.00  1.20  1.34  1.37   0.048   4
   -segment 0.5x0.25                                                      1.00  1.33  1.33  1.33   0.012   2
   -shade 30x30                                                           1.00  1.80  2.18  2.47   0.621   4
   -sharpen 0x0.5                                                         1.00  1.80  1.62  1.72   0.473   2
   -sharpen 0x1.0                                                         1.00  1.12  1.76  1.90   0.201   4
   -sharpen 0x2.0                                                         1.00  1.91  1.76  2.21   0.075   4
   -shear 45x45                                                           1.00  1.14  1.33  1.40   0.060   4
   -solarize 50%                                                          1.00  0.97  1.01  0.90   2.272   3
   -swirl 90                                                              1.00  1.43  2.02  2.50   0.357   4
   -fuzz 35% -transparent red                                             1.00  1.19  1.20  1.12   2.220   3
   -trim                                                                  1.00  1.08  1.09  1.06   2.715   3
   -fuzz 5% -trim                                                         1.00  1.73  1.75  2.14   1.091   4
   -unsharp 0x0.5+20+1                                                    1.00  1.35  1.35  1.47   0.320   4
   -unsharp 0x1.0+20+1                                                    1.00  1.40  1.41  1.54   0.296   4
   -wave 25x150                                                           1.00  1.81  1.69  2.01   0.348   4
   ===================================================================== ===== ===== ===== ===== ======== ====

Windows XP / MSVC / Intel Core 2 Quad
-------------------------------------

.. Last update: Sun Jan 29 16:17:01 CST 2012

This system is Windows XP Professional (SP3) using the Visual Studio
2008 compiler and a Q16 build.  The system CPU is a 2.83 GHz Core 2
Quad Processor (Q9550).  This processor is a multi-chip module (MCM)
based on two Core 2 CPUs bonded to a L3 cache in the same chip
package.

The following shows the performance boost for a Q16 build:

.. table:: Performance Boost on an Intel Core 2 Quad (Q9550) system

   ===================================================================== ===== ===== ===== ===== ======== ====
   Operation                                                               1     2     3     4    iter/s  thds
   ===================================================================== ===== ===== ===== ===== ======== ====
   -noop                                                                  1.00  1.00  0.99  0.98  13.036   1
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.62  2.11  2.38   0.808   4
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  1.09  1.17  1.18   8.271   4
   -blur 0x0.5                                                            1.00  1.62  2.12  2.47   1.551   4
   -blur 0x1.0                                                            1.00  1.71  2.25  2.62   1.378   4
   -blur 0x2.0                                                            1.00  1.78  2.40  2.78   1.091   4
   -charcoal 0x1                                                          1.00  1.71  2.23  2.61   0.738   4
   -colorspace CMYK                                                       1.00  1.06  1.07  1.11   7.309   4
   -colorspace GRAY                                                       1.00  1.71  2.23  2.56   4.711   4
   -colorspace HSL                                                        1.00  1.83  2.56  3.15   3.418   4
   -colorspace HWB                                                        1.00  1.79  2.45  2.97   4.332   4
   -colorspace OHTA                                                       1.00  1.77  2.23  2.55   4.655   4
   -colorspace YCbCr                                                      1.00  1.72  2.19  2.52   4.711   4
   -colorspace YIQ                                                        1.00  1.73  2.21  2.48   4.613   4
   -colorspace YUV                                                        1.00  1.75  2.23  2.55   4.712   4
   -contrast -contrast -contrast                                          1.00  1.95  2.87  3.74   0.838   4
   +contrast +contrast +contrast                                          1.00  1.94  2.87  3.74   0.849   4
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.77  2.42  2.99   3.012   4
   -colorize 30%/20%/50%                                                  1.00  1.70  2.31  2.70   4.036   4
   -despeckle                                                             1.00  1.85  2.54  2.88   0.515   4
   -edge 0x1                                                              1.00  1.87  2.67  3.37   1.751   4
   -emboss 0x1                                                            1.00  1.79  2.43  2.97   1.519   4
   -enhance                                                               1.00  1.95  2.90  3.80   0.600   4
   -fill none -stroke gold -draw 'circle 800,500 1100,800'                1.00  1.34  1.48  1.59   9.938   4
   -fill green -stroke gold -draw 'circle 800,500 1100,800'               1.00  1.42  1.63  1.76   9.312   4
   -fill none -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.18  1.32  1.37  11.400   4
   -fill blue -stroke gold -draw 'rectangle 400,200 1100,800'             1.00  1.29  1.41  1.52  10.731   4
   -fill none -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.17  1.24  1.28  11.492   4
   -fill blue -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'  1.00  1.23  1.36  1.43  10.831   4
   -fill none -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.23  1.39  1.45  11.028   4
   -fill blue -stroke gold -draw 'polygon 400,200 1100,800 100,300'       1.00  1.24  1.40  1.49  10.731   4
   -fill none -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.09  1.13  1.18  11.655   4
   -fill blue -stroke gold -draw 'Bezier 400,200 1100,800 100,300'        1.00  1.13  1.18  1.22  11.563   4
   -gaussian 0x0.5                                                        1.00  1.88  2.65  3.36   1.883   4
   -gaussian 0x1.0                                                        1.00  1.98  2.90  3.76   0.884   4
   -gaussian 0x2.0                                                        1.00  1.92  2.91  3.82   0.298   4
   -hald-clut identity:8                                                  1.00  1.87  2.66  3.28   2.286   4
   -hald-clut identity:10                                                 1.00  1.84  2.57  3.22   2.193   4
   -hald-clut identity:14                                                 1.00  1.68  2.17  2.53   1.378   4
   -implode 0.5                                                           1.00  1.93  2.79  3.38   1.150   4
   -implode -1                                                            1.00  1.92  2.78  3.57   1.214   4
   -lat 10x10-5%                                                          1.00  1.86  2.60  3.24   1.185   4
   -median 1                                                              1.00  1.97  2.68  3.47   0.253   4
   -median 2                                                              1.00  1.96  2.77  3.65   0.095   4
   -minify                                                                1.00  1.69  2.17  2.55   5.415   4
   -modulate 110/100/95                                                   1.00  1.83  2.60  3.29   2.954   4
   +noise Uniform                                                         1.00  1.68  2.15  2.51   2.400   4
   +noise Gaussian                                                        1.00  1.91  2.75  3.44   0.713   4
   +noise Multiplicative                                                  1.00  1.89  2.71  3.42   0.899   4
   +noise Impulse                                                         1.00  1.70  2.20  2.53   2.239   4
   +noise Laplacian                                                       1.00  1.82  2.49  2.95   1.450   4
   +noise Poisson                                                         1.00  1.91  2.75  3.29   0.727   4
   -noise 1                                                               1.00  1.96  2.58  3.81   0.278   4
   -noise 2                                                               1.00  1.96  2.77  3.65   0.095   4
   -fill blue -fuzz 35% -opaque red                                       1.00  1.35  1.51  1.60   9.771   4
   -operator all Add 2%                                                   1.00  1.71  2.27  2.71   5.497   4
   -operator all And 233                                                  1.00  1.05  1.08  1.09   8.600   4
   -operator all Assign 50%                                               1.00  1.03  1.05  1.05   8.348   3
   -operator all Depth 6                                                  1.00  1.09  1.08  1.12   8.520   4
   -operator all Divide 2                                                 1.00  1.73  2.29  2.70   5.497   4
   -operator all Gamma 0.7                                                1.00  1.03  1.08  1.10   8.271   4
   -operator all Negate 1.0                                               1.00  1.02  1.04  1.06   8.398   4
   -operator all LShift 2                                                 1.00  1.08  1.07  1.09   8.600   4
   -operator all Multiply 0.5                                             1.00  1.73  2.29  2.71   5.498   4
   -operator all Or 233                                                   1.00  1.02  1.06  1.09   8.612   4
   -operator all RShift 2                                                 1.00  1.03  1.09  1.09   8.574   4
   -operator all Subtract 10%                                             1.00  1.73  2.31  2.73   5.333   4
   -operator red Threshold 50%                                            1.00  1.04  1.06  1.09   8.746   4
   -operator gray Threshold 50%                                           1.00  1.03  1.08  1.09   8.638   4
   -operator all Threshold-White 80%                                      1.00  1.13  1.14  1.15  10.208   4
   -operator all Threshold-Black 10%                                      1.00  1.16  1.13  1.18  10.000   4
   -operator all Xor 233                                                  1.00  1.06  1.10  1.10   8.692   3
   -operator all Noise-Gaussian 30%                                       1.00  1.97  2.90  3.54   0.757   4
   -operator all Noise-Impulse 30%                                        1.00  1.86  2.61  3.23   3.200   4
   -operator all Noise-Laplacian 30%                                      1.00  1.91  2.76  3.53   1.866   4
   -operator all Noise-Multiplicative 30%                                 1.00  1.96  2.90  3.48   0.944   4
   -operator all Noise-Poisson 30%                                        1.00  1.96  2.88  3.77   0.867   4
   -operator all Noise-Uniform 30%                                        1.00  1.80  2.57  3.18   3.429   4
   -ordered-dither all 2x2                                                1.00  1.26  1.28  1.32   7.308   4
   -ordered-dither all 3x3                                                1.00  1.27  1.30  1.35   7.438   4
   -ordered-dither intensity 3x3                                          1.00  1.25  1.30  1.35   7.400   4
   -ordered-dither all 4x4                                                1.00  1.24  1.29  1.33   7.354   4
   -paint 0x1                                                             1.00  1.93  2.83  3.65   0.836   4
   -random-threshold all 20x80                                            1.00  1.58  1.96  2.25   6.634   4
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.00  1.01  1.01  13.000   3
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  1.71  2.21  2.65   5.514   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  1.79  2.44  2.95   4.346   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  1.78  2.42  2.95   4.372   4
   -density 75x75 -resample 50x50                                         1.00  1.65  2.25  2.65   1.751   4
   -resize 10%                                                            1.00  1.71  2.23  2.63   4.834   4
   -resize 50%                                                            1.00  1.72  2.31  2.69   2.499   4
   -resize 150%                                                           1.00  1.86  2.36  2.58   0.715   4
   -rotate 15                                                             1.00  1.50  1.75  2.07   0.723   4
   -rotate 45                                                             1.00  1.49  1.55  1.76   0.311   4
   -segment 0.5x0.25                                                      1.00  1.44  1.62  1.69   0.027   4
   -shade 30x30                                                           1.00  1.67  2.20  2.55   4.085   4
   -sharpen 0x0.5                                                         1.00  1.89  2.64  3.36   1.877   4
   -sharpen 0x1.0                                                         1.00  1.99  2.90  3.76   0.879   4
   -sharpen 0x2.0                                                         1.00  1.94  2.91  3.83   0.299   4
   -shear 45x45                                                           1.00  1.05  1.30  1.49   0.441   4
   -solarize 50%                                                          1.00  1.06  1.12  1.12   8.322   4
   -swirl 90                                                              1.00  1.89  2.74  3.53   1.366   4
   -fuzz 35% -transparent red                                             1.00  1.32  1.42  1.43   8.322   4
   -trim                                                                  1.00  1.38  1.60  1.70   9.846   4
   -fuzz 5% -trim                                                         1.00  1.69  2.30  2.77   5.200   4
   -unsharp 0x0.5+20+1                                                    1.00  1.69  2.17  2.60   1.333   4
   -unsharp 0x1.0+20+1                                                    1.00  1.73  2.32  2.63   1.160   4
   -wave 25x150                                                           1.00  1.83  2.57  3.08   1.378   4
   ===================================================================== ===== ===== ===== ===== ======== ====


Windows 7 / MinGW / Intel Core 2 Quad
-------------------------------------

.. Last update: Sun Mar  9 10:39:14 CDT 2015

GCC 4.8.2 (x86_64-win32-sjlj) from a build of the 'MinGW-w64'_ project
was installed on the same Windows system with the 2.83 GHz Core 2 Quad
Processor (Q9550) as described above.  The build is a 32-bit build.
This processor is a multi-chip module (MCM) based on two Core 2 CPUs
bonded to a L3 cache in the same chip package.

The following shows the performance boost for a Q16 build:

.. table:: Performance Boost on an Intel Core 2 Quad (Q9550) system


   ============================================== ===== ===== ===== ===== ======== ====
   Operation                                        1     2     3     4    iter/s  thds
   ============================================== ===== ===== ===== ===== ======== ====
   -affine 1,0,0.785,1,0,0 -transform              1.00  1.78  2.46  3.01   1.447   4
   -blur 0x0.5                                     1.00  1.48  1.89  2.12   1.208   4
   -blur 0x1.0                                     1.00  1.68  2.18  2.53   0.955   4
   -blur 0x2.0                                     1.00  1.75  2.44  2.96   0.668   4
   -charcoal 0x1                                   1.00  1.67  2.16  2.53   0.613   4
   -colorspace GRAY                                1.00  1.38  1.60  1.71   7.689   4
   -colorspace HSL                                 1.00  1.80  2.44  3.00   3.631   4
   -colorspace HWB                                 1.00  1.72  2.27  2.70   4.568   4
   -colorspace OHTA                                1.00  1.31  1.63  1.72   7.673   4
   -colorspace YCbCr                               1.00  1.40  1.60  1.70   7.658   4
   -colorspace YIQ                                 1.00  1.34  1.65  1.73   7.764   4
   -colorspace YUV                                 1.00  1.37  1.62  1.72   7.692   4
   -contrast -contrast -contrast                   1.00  1.94  2.84  3.70   0.858   4
   +contrast +contrast +contrast                   1.00  1.92  2.84  3.69   0.872   4
   -convolve 1,1,1,1,4,1,1,1,1                     1.00  1.80  2.49  3.09   2.734   4
   -despeckle                                      1.00  1.64  2.09  2.30   0.561   4
   -edge 0x1                                       1.00  1.81  2.50  3.09   2.543   4
   -emboss 0x1                                     1.00  1.86  2.54  3.23   1.101   4
   -enhance                                        1.00  1.96  2.89  3.79   0.603   4
   -gaussian 0x0.5                                 1.00  1.92  2.70  3.56   1.295   4
   -gaussian 0x1.0                                 1.00  1.97  2.92  3.81   0.453   4
   -gaussian 0x2.0                                 1.00  1.97  2.97  3.94   0.134   4
   -hald-clut identity:8                           1.00  1.83  2.52  3.00   2.981   4
   -hald-clut identity:10                          1.00  1.83  2.52  3.10   3.046   4
   -hald-clut identity:14                          1.00  1.81  2.48  3.00   2.564   4
   -implode 0.5                                    1.00  1.87  2.79  3.56   1.098   4
   -implode -1                                     1.00  1.90  2.71  3.50   1.496   4
   -lat 10x10-5%                                   1.00  1.00  0.98  0.98   1.459   1
   -median 1                                       1.00  1.97  2.93  3.60   0.270   4
   -median 2                                       1.00  2.04  2.93  4.00   0.108   4
   -minify                                         1.00  1.69  2.20  2.59   4.828   4
   -modulate 110/100/95                            1.00  1.83  2.47  3.05   2.952   4
   -motion-blur 0x3+30                             1.00  1.98  2.91  3.73   0.351   4
   +noise Uniform                                  1.00  1.51  1.87  2.15   3.175   4
   +noise Gaussian                                 1.00  1.95  2.85  3.66   0.487   4
   +noise Multiplicative                           1.00  1.94  2.81  3.60   0.760   4
   +noise Impulse                                  1.00  1.50  1.86  2.11   3.189   4
   +noise Laplacian                                1.00  1.83  2.61  3.26   1.276   4
   +noise Poisson                                  1.00  1.92  2.77  3.53   0.774   4
   +noise Random                                   1.00  1.62  2.11  2.48   2.614   4
   -noise 1                                        1.00  2.00  2.92  3.89   0.292   4
   -noise 2                                        1.00  2.00  2.96  3.96   0.107   4
   -operator all Add 2%                            1.00  1.41  1.69  1.84   7.388   4
   -operator all Divide 2                          1.00  1.50  1.78  1.99   6.903   4
   -operator all Multiply 0.5                      1.00  1.43  1.75  1.94   7.077   4
   -operator all Subtract 10%                      1.00  1.47  1.70  1.86   7.239   4
   -operator all Noise-Gaussian 30%                1.00  1.99  2.96  3.89   0.521   4
   -operator all Noise-Impulse 30%                 1.00  1.66  2.18  2.59   4.603   4
   -operator all Noise-Laplacian 30%               1.00  1.86  2.78  3.57   1.460   4
   -operator all Noise-Multiplicative 30%          1.00  1.95  2.86  3.73   0.910   4
   -operator all Noise-Poisson 30%                 1.00  1.95  2.88  3.76   0.846   4
   -operator all Noise-Uniform 30%                 1.00  1.68  2.23  2.66   4.587   4
   -ordered-dither all 2x2                         1.00  1.20  1.21  1.25   6.655   4
   -ordered-dither all 3x3                         1.00  1.24  1.25  1.32   6.942   4
   -ordered-dither intensity 3x3                   1.00  1.20  1.20  1.27   6.768   4
   -ordered-dither all 4x4                         1.00  1.20  1.21  1.26   6.693   4
   -paint 0x1                                      1.00  1.86  2.63  3.31   1.867   4
   -random-threshold all 20x80                     1.00  1.35  1.46  1.55   6.865   4
   -recolor '0,0,1,0,1,0,1,0,0'                    1.00  1.68  2.13  2.45   5.251   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'              1.00  1.55  1.97  2.25   6.084   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'  1.00  1.59  1.97  2.24   6.090   4
   -density 75x75 -resample 50x50                  1.00  1.84  2.49  3.15   2.009   4
   -resize 10%                                     1.00  1.81  2.46  2.94   3.286   4
   -resize 50%                                     1.00  1.82  2.52  3.05   2.349   4
   -resize 150%                                    1.00  1.81  2.42  2.93   0.986   4
   -rotate 15                                      1.00  1.52  2.13  2.49   0.858   4
   -rotate 45                                      1.00  1.68  1.98  2.37   0.519   4
   -shade 30x30                                    1.00  1.64  2.12  2.49   4.081   4
   -sharpen 0x0.5                                  1.00  1.88  2.74  3.54   1.289   4
   -sharpen 0x1.0                                  1.00  1.97  2.92  3.84   0.457   4
   -sharpen 0x2.0                                  1.00  2.00  2.97  3.94   0.134   4
   -shear 45x45                                    1.00  1.16  1.43  1.63   0.460   4
   -swirl 90                                       1.00  1.92  2.73  3.50   1.448   4
   -fuzz 5% -trim                                  1.00  1.63  2.04  2.37   5.900   4
   -unsharp 0x0.5+20+1                             1.00  1.59  1.98  2.23   1.110   4
   -unsharp 0x1.0+20+1                             1.00  1.70  2.22  2.57   0.881   4
   -wave 25x150                                    1.00  1.84  2.60  3.27   1.918   4
   ============================================== ===== ===== ===== ===== ======== ====


--------------------------------------------------------------------------

| Copyright (C) 2008 - 2015 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/Copyright.html.

