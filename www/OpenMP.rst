.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

.. _`Amdahl's law` : http://en.wikipedia.org/wiki/Amdahl%27s_law
.. _`GOMP` : http://gcc.gnu.org/onlinedocs/libgomp/
.. _`OpenMP` : http://openmp.org/

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
Professional 2005 and 2008 support OpenMP so Windows users can benefit
as well. Any multi-CPU and/or multi-core system is potentially a good
candidate for use with OpenMP. Unfortunately, some older multi-CPU
hardware is more suitable for multi-processing than
multi-threading. Modern multi-core chipsets from AMD, Intel and
Sun/Oracle perform very well with OpenMP. The operating system makes a
difference when it comes to OpenMP acceleration, with IBM's AIX,
Linux, and Sun's Solaris working exceptionally well, and FreeBSD and
Apple's OS-X working less well.

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

Ubuntu Linux 11.10 / Intel Xeon E5649 / Open64 Compiler
--------------------------------------------------------

.. Last update: Wed Dec 21 15:40:08 CST 2011

The following results were obtained from an Intel Xeon E5649 CPU at
2.53GHz.  This CPU has 12 cores and 24 threads.  The AMD Open64
4.2.5.2 compiler was used to build the software.  The Open64 compiler
produces very high performance code which exceeds GCC performance in
most cases, and often quite dramatically so:

.. table:: Performance Boost On 12 core Intel Xeon E5649 CPU:

   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ========
   Operation                                                               1     4     8    12    16    20    24    iter/s 
   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ========
   -noop                                                                  1.00  1.31  1.41  1.34  1.45  1.46  1.44  83.200 
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  2.53  4.85  5.07  5.43  5.52  6.46   3.613 
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  1.70  2.00  1.93  2.16  2.16  2.12  41.600 
   -blur 0x0.5                                                            1.00  3.14  4.34  4.91  4.87  5.05  5.20   5.506 
   -blur 0x1.0                                                            1.00  3.24  4.91  5.98  5.60  6.16  6.56   4.847 
   -blur 0x2.0                                                            1.00  3.51  5.84  7.25  6.78  7.64  8.28   3.800 
   -charcoal 0x1                                                          1.00  2.96  4.24  4.88  4.63  5.02  5.30   2.209 
   -colorspace CMYK                                                       1.00  2.24  2.64  2.51  2.76  2.70  2.68  40.818 
   -colorspace GRAY                                                       1.00  3.03  4.67  5.70  5.67  6.15  6.90  33.600 
   -colorspace HSL                                                        1.00  3.73  6.66  9.16  7.67  9.11 10.53  14.500 
   -colorspace HWB                                                        1.00  3.47  6.08  8.01  6.64  7.77  8.84  20.100 
   -colorspace OHTA                                                       1.00  3.07  4.67  5.70  5.69  6.13  6.89  33.533 
   -colorspace YCbCr                                                      1.00  3.10  4.66  5.70  5.67  6.04  6.94  33.766 
   -colorspace YIQ                                                        1.00  3.07  4.69  5.70  5.73  6.26  6.97  33.866 
   -colorspace YUV                                                        1.00  3.17  4.81  5.90  5.90  6.50  7.12  33.400 
   -contrast -contrast -contrast                                          1.00  3.66  7.10 10.29  9.48 11.57 13.51   4.122 
   +contrast +contrast +contrast                                          1.00  3.68  7.10 10.20  9.12 11.21 12.99   4.236 
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  3.34  5.47  6.70  5.90  6.65  7.38  13.307 
   -colorize 30%/20%/50%                                                  1.00  3.50  5.97  7.63  6.44  7.56  8.40  22.977 
   -despeckle                                                             1.00  2.66  2.67  2.68  2.67  2.67  2.68   0.459 
   -edge 0x1                                                              1.00  3.49  5.84  7.67  7.20  8.06  9.14  12.700 
   -emboss 0x1                                                            1.00  3.16  4.81  5.95  4.67  5.39  5.90   4.554 
   -enhance                                                               1.00  3.57  7.08 10.43  8.18 10.09 11.98   1.737 
   -gaussian 0x0.5                                                        1.00  3.60  6.22  8.32  5.97  7.12  8.25   7.327 
   -gaussian 0x1.0                                                        1.00  3.67  6.80  9.45  5.91  7.32  8.73   3.137 
   -gaussian 0x2.0                                                        1.00  3.54  6.79  9.84  5.55  6.91  8.20   1.082 
   -hald-clut identity:8                                                  1.00  3.70  6.77  9.32  8.49 10.10 11.57  13.759 
   -hald-clut identity:10                                                 1.00  3.66  6.66  9.29  8.46 10.16 11.52  13.473 
   -hald-clut identity:14                                                 1.00  3.68  6.68  9.25  8.44 10.16 11.54  12.004 
   -implode 0.5                                                           1.00  3.79  7.17  6.04  8.85 10.69 11.63   4.895 
   -implode -1                                                            1.00  3.60  7.03  5.04  8.72  9.78 10.25   6.281 
   -lat 10x10-5%                                                          1.00  3.55  5.90  7.97  6.98  7.97  9.36   4.024 
   -median 1                                                              1.00  3.02  4.45  6.46  7.20  6.66  8.25   0.718 
   -median 2                                                              1.00  3.27  4.27  7.87  9.40  8.17  7.37   0.282 
   -minify                                                                1.00  3.21  4.79  5.99  5.77  6.42  6.51  22.400 
   -modulate 110/100/95                                                   1.00  3.77  6.90  9.53  8.47 10.27 11.69  13.273 
   +noise Uniform                                                         1.00  3.28  5.18  6.36  6.72  7.23  7.79  13.772 
   +noise Gaussian                                                        1.00  3.54  6.88 10.02  9.43 11.43 13.29   2.246 
   +noise Multiplicative                                                  1.00  3.57  6.86  9.94 11.07 13.23 15.22   3.363 
   +noise Impulse                                                         1.00  3.24  4.97  6.18  5.71  6.27  6.74  12.662 
   +noise Laplacian                                                       1.00  3.55  6.74  9.54  9.86 11.57 13.34   4.577 
   +noise Poisson                                                         1.00  3.62  6.91  9.69 10.24 12.14 13.71   4.950 
   -noise 1                                                               1.00  3.14  3.54  6.69  8.46  6.96  8.62   0.733 
   -noise 2                                                               1.00  3.40  4.57  8.20 10.03  8.73  7.40   0.301 
   -fill blue -fuzz 35% -opaque red                                       1.00  2.61  3.50  3.90  4.45  4.52  4.68  50.150 
   -operator all Add 2%                                                   1.00  3.20  4.76  5.74  6.86  7.71  8.01  41.558 
   -operator all And 233                                                  1.00  1.89  2.16  1.99  1.88  1.85  1.79  42.900 
   -operator all Assign 50%                                               1.00  1.52  1.57  1.31  1.43  1.41  1.36  41.200 
   -operator all Depth 6                                                  1.00  2.37  2.82  2.36  2.51  2.47  2.57  40.400 
   -operator all Divide 2                                                 1.00  3.19  4.89  5.88  7.37  7.37  7.17  35.764 
   -operator all Gamma 0.7                                                1.00  2.05  2.24  2.08  2.26  2.17  2.15  30.669 
   -operator all Negate 1.0                                               1.00  1.47  1.37  1.28  1.66  1.53  1.33  44.555 
   -operator all LShift 2                                                 1.00  2.10  2.49  2.07  2.12  2.09  2.02  43.856 
   -operator all Multiply 0.5                                             1.00  3.25  4.97  6.13  7.67  8.40  7.62  37.962 
   -operator all Or 233                                                   1.00  2.02  2.35  2.02  2.02  1.99  2.04  43.357 
   -operator all RShift 2                                                 1.00  1.88  2.02  1.71  1.87  1.89  2.19  43.600 
   -operator all Subtract 10%                                             1.00  3.04  4.61  5.46  6.17  5.76  5.78  36.963 
   -operator red Threshold 50%                                            1.00  1.49  1.58  1.26  1.37  1.36  1.53  43.014 
   -operator gray Threshold 50%                                           1.00  1.80  2.29  2.23  2.02  1.99  1.92  42.300 
   -operator all Threshold-White 80%                                      1.00  2.18  2.76  2.98  2.95  2.74  2.71  49.100 
   -operator all Threshold-Black 10%                                      1.00  2.09  2.70  2.93  2.92  2.71  2.65  47.600 
   -operator all Xor 233                                                  1.00  1.88  2.14  1.74  1.87  1.85  2.05  42.500 
   -operator all Noise-Gaussian 30%                                       1.00  3.53  7.04 10.37  9.75 11.90 14.20   2.470 
   -operator all Noise-Impulse 30%                                        1.00  3.65  6.18  8.46  7.73  9.02 10.20  19.980 
   -operator all Noise-Laplacian 30%                                      1.00  3.62  7.05 10.33 10.90 13.42 15.40   5.484 
   -operator all Noise-Multiplicative 30%                                 1.00  3.64  7.19 10.59 11.98 14.78 17.18   3.796 
   -operator all Noise-Poisson 30%                                        1.00  3.75  7.29 10.63 11.01 13.58 15.73   5.853 
   -operator all Noise-Uniform 30%                                        1.00  3.59  6.35  8.81  9.47 11.08 12.11  22.455 
   -ordered-dither all 2x2                                                1.00  3.19  4.64  5.65  6.15  6.65  7.09  36.527 
   -ordered-dither all 3x3                                                1.00  3.15  4.65  5.54  6.09  6.68  6.47  34.366 
   -ordered-dither intensity 3x3                                          1.00  3.36  4.91  5.96  6.49  6.28  6.44  32.104 
   -ordered-dither all 4x4                                                1.00  3.27  4.72  5.77  6.06  6.13  6.26  31.705 
   -paint 0x1                                                             1.00  3.62  6.73  9.14  8.53  9.86 11.23   8.076 
   -random-threshold all 20x80                                            1.00  2.88  4.00  4.74  5.24  5.24  5.27  38.124 
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.51  1.68  1.59  1.51  1.46  1.42  80.300 
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  3.26  4.99  6.38  5.87  6.68  6.65  28.743 
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  3.45  5.51  6.93  6.47  7.44  8.11  27.800 
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  3.39  5.38  6.86  6.40  7.41  7.97  27.745 
   -density 75x75 -resample 50x50                                         1.00  3.71  6.61  9.23  7.21  7.85  8.28   8.192 
   -resize 10%                                                            1.00  3.34  5.34  6.69  5.01  5.38  5.94  17.448 
   -resize 50%                                                            1.00  3.62  6.34  8.78  6.73  6.16  6.93  11.741 
   -resize 150%                                                           1.00  3.45  5.56  7.26  5.37  5.35  5.23   2.860 
   -rotate 15                                                             1.00  3.24  5.28  6.89  7.14  7.73  8.39   2.584 
   -rotate 45                                                             1.00  2.92  4.55  5.64  5.37  5.50  5.78   0.826 
   -segment 0.5x0.25                                                      1.00  2.06  2.47  2.71  2.59  2.71  2.76   0.047 
   -shade 30x30                                                           1.00  3.21  5.12  6.39  6.26  6.81  7.63  16.600 
   -sharpen 0x0.5                                                         1.00  3.62  6.29  8.43  6.03  7.16  8.28   7.389 
   -sharpen 0x1.0                                                         1.00  3.67  6.81  9.40  5.94  7.40  8.72   3.110 
   -sharpen 0x2.0                                                         1.00  3.61  6.83  9.90  5.59  6.96  8.30   1.079 
   -shear 45x45                                                           1.00  2.82  4.11  4.92  4.60  4.53  4.79   1.190 
   -solarize 50%                                                          1.00  2.07  2.44  2.39  2.64  2.60  2.57  45.200 
   -swirl 90                                                              1.00  3.74  7.03  4.98  9.23 10.79 10.08   6.938 
   -fuzz 35% -transparent red                                             1.00  2.50  3.21  3.60  3.90  3.75  3.88  43.713 
   -trim                                                                  1.00  2.27  2.78  3.14  3.43  3.17  2.96  51.600 
   -fuzz 5% -trim                                                         1.00  3.52  5.95  7.87  7.39  8.76  9.78  23.330 
   -unsharp 0x0.5+20+1                                                    1.00  3.24  4.69  5.49  5.35  5.81  6.05   5.095 
   -unsharp 0x1.0+20+1                                                    1.00  3.42  5.23  6.34  6.09  6.76  7.26   4.514 
   -wave 25x150                                                           1.00  3.57  6.46  7.74  7.55  8.71  9.99   7.730 
   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ========


Ubuntu Linux 11.10 / Intel Xeon E5649 / GCC Compiler
-----------------------------------------------------

.. Last update: Wed Dec 21 15:40:08 CST 2011

The following results were obtained from an Intel Xeon E5649 CPU at
2.53GHz.  This CPU has 12 cores and 24 threads.  Ubtuntu's GCC 4.6.1
compiler was used to build the software:

.. table:: Performance Boost On 12 core Intel Xeon E5649 CPU:

   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ========
   Operation                                                               1     4     8     12    16    20    24   iter/s 
   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ========
   -noop                                                                  1.00  1.94  2.43  2.49  2.59  2.51  2.23  52.248 
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  3.49  5.74  6.15  5.60  6.65  8.01   4.536 
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  2.49  3.11  3.31  3.51  3.72  2.94  35.365 
   -blur 0x0.5                                                            1.00  3.34  5.27  6.65  7.43  7.86  7.62   4.970 
   -blur 0x1.0                                                            1.00  3.51  5.97  7.83  9.27  9.92  9.64   3.900 
   -blur 0x2.0                                                            1.00  3.47  6.35  8.71 10.68 12.14 12.03   2.805 
   -charcoal 0x1                                                          1.00  3.08  5.02  5.99  7.00  7.60  7.12   1.907 
   -colorspace CMYK                                                       1.00  2.13  2.40  2.38  2.56  2.69  2.41  32.136 
   -colorspace GRAY                                                       1.00  3.16  5.10  4.63  5.44  6.19  5.60  24.775 
   -colorspace HSL                                                        1.00  3.65  6.33  6.44  7.52  8.77  9.38  12.351 
   -colorspace HWB                                                        1.00  3.52  6.03  5.67  7.05  8.10  8.02  15.569 
   -colorspace OHTA                                                       1.00  3.15  4.43  4.50  5.26  6.00  5.78  24.152 
   -colorspace YCbCr                                                      1.00  3.19  4.88  4.81  5.55  6.23  5.72  25.175 
   -colorspace YIQ                                                        1.00  3.16  4.46  4.53  5.59  6.05  5.55  23.876 
   -colorspace YUV                                                        1.00  3.23  4.37  4.81  5.60  6.19  5.62  24.476 
   -contrast -contrast -contrast                                          1.00  3.63  7.09  7.45  9.23 11.37 11.98   3.582 
   +contrast +contrast +contrast                                          1.00  3.61  6.88  7.06  9.26 11.32 11.46   3.472 
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  3.68  6.75  8.83 10.77 12.23 12.03  10.469 
   -colorize 30%/20%/50%                                                  1.00  3.55  6.05  7.20  8.71  9.77  9.75  14.500 
   -despeckle                                                             1.00  2.65  2.69  2.65  2.67  2.69  2.68   0.430 
   -edge 0x1                                                              1.00  3.64  6.99  8.86 11.01 12.91 11.78   9.930 
   -emboss 0x1                                                            1.00  3.54  6.25  8.04  9.93 11.18 11.17   3.543 
   -enhance                                                               1.00  3.59  6.91  7.79  7.86  9.46 10.54   1.613 
   -gaussian 0x0.5                                                        1.00  3.61  6.94  9.58 12.32 14.62 14.74   5.248 
   -gaussian 0x1.0                                                        1.00  3.57  6.95 10.25 13.31 16.17 16.81   1.984 
   -gaussian 0x2.0                                                        1.00  3.88  7.29 10.53 13.74 16.88 18.76   0.638 
   -hald-clut identity:8                                                  1.00  3.72  7.05  7.73  9.80 11.49 11.49  10.689 
   -hald-clut identity:10                                                 1.00  3.76  7.11  7.68  9.67 11.55 12.00  11.067 
   -hald-clut identity:14                                                 1.00  3.65  6.78  7.49  9.45 11.10 11.02   8.856 
   -implode 0.5                                                           1.00  3.53  6.70  6.87  9.09 10.62 11.23   4.199 
   -implode -1                                                            1.00  3.36  6.74  7.53  8.72 10.73  6.99   5.677 
   -lat 10x10-5%                                                          1.00  3.40  5.32  7.28  6.66  7.72  8.63   2.579 
   -median 1                                                              1.00  2.33  3.06  4.88  5.05  5.41  7.64   0.634 
   -median 2                                                              1.00  2.86  3.24  5.93  6.69  7.86  9.69   0.281 
   -minify                                                                1.00  3.61  6.51  8.17  9.80 11.42 12.29  18.046 
   -modulate 110/100/95                                                   1.00  3.75  7.02  7.35  8.81 10.43 11.82  11.189 
   +noise Uniform                                                         1.00  2.52  3.40  3.37  3.50  3.76  3.91   6.753 
   +noise Gaussian                                                        1.00  3.41  6.32  8.12  9.48 11.26 12.71   1.868 
   +noise Multiplicative                                                  1.00  3.40  6.33  7.12  8.86 10.37 11.65   2.527 
   +noise Impulse                                                         1.00  2.55  3.49  3.68  3.73  3.92  4.13   6.739 
   +noise Laplacian                                                       1.00  3.41  5.92  7.77  7.89  9.01  9.90   3.516 
   +noise Poisson                                                         1.00  3.24  5.87  6.69  8.17  9.44 10.22   3.271 
   -noise 1                                                               1.00  2.36  3.21  4.35  5.78  5.59  6.64   0.538 
   -noise 2                                                               1.00  2.86  3.28  4.76  6.66  7.10  8.48   0.246 
   -fill blue -fuzz 35% -opaque red                                       1.00  2.77  4.00  4.25  4.62  5.01  5.13  40.000 
   -operator all Add 2%                                                   1.00  3.18  5.27  5.99  6.90  7.42  7.64  33.600 
   -operator all And 233                                                  1.00  2.21  2.75  2.69  2.84  2.99  3.03  38.124 
   -operator all Assign 50%                                               1.00  2.07  2.58  2.44  2.52  2.67  2.68  37.962 
   -operator all Depth 6                                                  1.00  2.30  2.79  2.76  3.10  3.13  3.12  36.327 
   -operator all Divide 2                                                 1.00  3.33  5.69  6.84  7.84  8.78  9.15  32.236 
   -operator all Gamma 0.7                                                1.00  1.94  2.46  2.37  2.44  2.52  2.50  27.246 
   -operator all Negate 1.0                                               1.00  2.21  2.84  2.67  2.82  3.07  3.07  38.262 
   -operator all LShift 2                                                 1.00  2.21  2.75  2.65  2.92  3.03  3.01  38.062 
   -operator all Multiply 0.5                                             1.00  3.27  5.35  6.45  7.27  8.18  8.42  33.000 
   -operator all Or 233                                                   1.00  2.20  2.90  2.78  2.89  3.05  3.01  38.262 
   -operator all RShift 2                                                 1.00  2.22  2.72  2.81  2.83  3.04  3.05  38.124 
   -operator all Subtract 10%                                             1.00  3.19  5.18  5.92  6.58  7.55  7.67  33.766 
   -operator red Threshold 50%                                            1.00  2.07  2.53  2.57  2.59  2.73  2.73  38.124 
   -operator gray Threshold 50%                                           1.00  2.43  3.04  3.18  3.31  3.41  3.43  37.900 
   -operator all Threshold-White 80%                                      1.00  2.41  3.13  2.99  3.26  3.55  3.50  40.900 
   -operator all Threshold-Black 10%                                      1.00  2.43  3.19  3.17  3.37  3.51  3.52  40.200 
   -operator all Xor 233                                                  1.00  2.22  2.79  2.74  2.81  3.03  3.02  38.100 
   -operator all Noise-Gaussian 30%                                       1.00  3.52  6.91  8.84 10.74 13.01 14.68   2.216 
   -operator all Noise-Impulse 30%                                        1.00  3.60  6.18  5.90  7.27  8.47  9.32  17.565 
   -operator all Noise-Laplacian 30%                                      1.00  3.77  7.29  8.82 10.27 12.34 14.35   5.279 
   -operator all Noise-Multiplicative 30%                                 1.00  3.64  7.13  9.90 10.85 12.86 14.87   3.287 
   -operator all Noise-Poisson 30%                                        1.00  3.53  6.87  9.53 10.24 12.46 14.47   4.847 
   -operator all Noise-Uniform 30%                                        1.00  3.56  4.97  5.58  6.95  8.13  8.97  18.227 
   -ordered-dither all 2x2                                                1.00  3.07  4.83  5.13  5.34  6.13  6.20  28.000 
   -ordered-dither all 3x3                                                1.00  3.04  4.64  5.33  5.49  6.09  6.17  27.844 
   -ordered-dither intensity 3x3                                          1.00  3.08  4.40  4.78  5.47  6.06  6.04  27.246 
   -ordered-dither all 4x4                                                1.00  3.08  4.89  4.84  5.59  6.09  6.08  27.445 
   -paint 0x1                                                             1.00  3.64  6.72  8.52  8.67 10.20 11.50   8.765 
   -random-threshold all 20x80                                            1.00  2.78  3.87  4.11  4.49  4.64  3.54  27.944 
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  2.04  2.52  2.63  2.67  2.74  2.75  51.800 
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  3.12  3.52  4.40  5.16  5.55  6.04  30.370 
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  3.22  5.24  4.63  5.51  6.23  6.50  27.300 
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  3.22  5.23  4.67  5.43  6.22  6.52  27.545 
   -density 75x75 -resample 50x50                                         1.00  3.73  6.76  8.48  8.58  8.61  8.53   4.985 
   -resize 10%                                                            1.00  3.66  6.50  8.24  9.65 10.91 11.69  12.375 
   -resize 50%                                                            1.00  3.69  6.58  8.04  9.45 10.25 11.17   7.952 
   -resize 150%                                                           1.00  3.55  6.50  7.69  8.17  8.00  7.32   2.655 
   -rotate 15                                                             1.00  3.23  5.61  6.46  7.44  8.14  9.14   2.933 
   -rotate 45                                                             1.00  3.14  5.04  5.89  6.45  7.05  7.32   1.040 
   -segment 0.5x0.25                                                      1.00  1.95  2.32  2.47  2.37  2.42  2.47   0.047 
   -shade 30x30                                                           1.00  3.35  4.49  5.69  6.42  7.30  8.02  15.522 
   -sharpen 0x0.5                                                         1.00  3.67  7.14  9.71 12.59 15.08 16.64   5.790 
   -sharpen 0x1.0                                                         1.00  3.59  6.94 10.07 13.23 16.22 18.50   2.183 
   -sharpen 0x2.0                                                         1.00  3.88  7.32 10.53 13.76 16.76 19.53   0.664 
   -shear 45x45                                                           1.00  2.95  4.37  4.74  5.34  5.62  5.75   1.444 
   -solarize 50%                                                          1.00  2.47  3.22  3.25  3.38  3.62  3.63  38.000 
   -swirl 90                                                              1.00  3.57  5.96  8.55  8.36 11.02 13.50   6.509 
   -fuzz 35% -transparent red                                             1.00  2.87  3.57  3.97  4.47  4.76  4.92  35.300 
   -trim                                                                  1.00  2.55  3.48  3.43  3.72  3.92  3.93  40.700 
   -fuzz 5% -trim                                                         1.00  3.41  5.07  5.51  6.64  7.71  8.16  21.058 
   -unsharp 0x0.5+20+1                                                    1.00  3.40  5.55  6.77  7.65  8.34  8.12   4.536 
   -unsharp 0x1.0+20+1                                                    1.00  3.44  6.03  7.74  9.14 10.06  9.84   3.663 
   -wave 25x150                                                           1.00  3.59  6.31  7.57  8.08  9.02  9.42   5.988 
   ===================================================================== ===== ===== ===== ===== ===== ===== ===== ========


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

   =====================================================================  =====  =====  =====  =====  =====  =====  =====  ===== ===== ===== ===== =====  ===== ======= ====
   Operation                                                                2      4      6      8     10     12     14     16    18    20    22    24     64   iter/s  thds
   =====================================================================  =====  =====  =====  =====  =====  =====  =====  ===== ===== ===== ===== =====  ===== ======= ====
   -noop                                                                   1.75   3.59   3.83   4.34   4.65   4.76   4.82   4.79  4.69  4.69  4.53  4.35   2.59  215.20   9
   -affine 1,0,0.785,1,0,0 -transform                                      1.96   3.87   5.73   7.57   9.23  10.87  12.42  13.61 14.84 16.09 17.15 18.11  28.35    7.14  64
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                 1.90   3.60   5.10   6.38   7.45   8.26   8.97   9.32  9.50  9.98  9.98  9.86   7.47   78.60  20
   -blur 0x0.5                                                             1.94   3.47   4.37   5.00   5.45   5.84   6.13   6.33  6.48  6.59  6.70  6.79   7.08    5.76  47
   -blur 0x1.0                                                             1.94   3.59   4.74   5.66   6.31   6.87   7.37   7.70  7.91  8.12  8.21  8.47   9.16    5.15  55
   -charcoal 0x1                                                           1.89   3.44   4.62   5.55   6.26   6.84   7.33   7.70  7.94  8.18  8.38  8.58   9.50    2.68  58
   -colorspace CMYK                                                        1.95   3.82   5.44   6.91   8.19   9.29  10.51  10.89 11.29 11.76 11.90 11.65   9.41   66.40  29
   -colorspace GRAY                                                        1.97   3.88   5.72   7.45   8.74   9.78   9.95  10.83 10.88 11.85 12.34 12.53  16.02   44.91  58
   -colorspace HSL                                                         1.99   3.96   5.86   7.80   9.52  11.09  12.35  13.27 14.40 15.39 16.36 16.87  24.74   25.95  61
   -colorspace HWB                                                         1.96   3.89   5.79   7.59   9.25  10.83  12.32  13.47 14.11 15.42 15.96 17.21  21.56   26.55  52
   -colorspace OHTA                                                        1.96   3.88   5.67   7.39   8.71   9.64  10.22  10.97 11.44 11.68 11.95 12.51  15.70   44.82  57
   -colorspace YCbCr                                                       1.97   3.88   5.63   7.40   8.78   9.56  10.20  11.13 10.83 11.90 12.07 12.51  16.02   44.71  62
   -colorspace YIQ                                                         1.96   3.86   5.72   7.40   8.65   9.71   9.93  11.00 10.81 11.68 12.22 12.31  16.35   44.91  58
   -colorspace YUV                                                         1.96   3.85   5.68   7.34   8.85   9.96   9.82  10.93 10.83 11.46 12.63 12.66  16.13   44.71  55
   -contrast -contrast -contrast                                           1.99   4.01   5.95   7.98   9.70  11.30  13.03  14.12 14.68 16.42 17.34 18.24  31.00    5.92  63
   +contrast +contrast +contrast                                           1.99   3.98   5.96   7.94   9.62  11.39  12.84  13.62 14.84 16.38 17.03 17.84  30.74    5.93  61
   -convolve 1,1,1,1,4,1,1,1,1                                             1.99   3.93   5.85   7.71   9.38  10.88  12.36  13.63 14.84 15.82 16.82 17.56  22.83   18.65  62
   -colorize 30%/20%/50%                                                   1.97   3.90   5.71   7.48   9.10  10.55  11.96  12.94 14.24 14.90 15.45 16.72  14.47   39.60  31
   -despeckle                                                              1.46   2.81   2.81   2.81   2.81   2.81   2.81   2.81  2.81  2.80  2.81  2.81   2.79    0.27   3
   -fill none -stroke gold -draw 'circle 800,500 1100,800'                 1.98   3.90   5.70   7.38   8.87  10.19  11.41  12.42 13.17 13.82 14.38 14.73  14.11   15.02  32
   -fill green -stroke gold -draw 'circle 800,500 1100,800'                2.04   4.03   5.92   7.68   9.33  10.77  12.15  13.16 14.17 14.96 15.56 16.13  16.45   13.72  37
   -fill none -stroke gold -draw 'rectangle 400,200 1100,800'              1.98   3.91   5.77   7.56   9.24  10.69  12.28  13.31 14.28 15.12 16.03 16.73  19.63   37.65  62
   -fill blue -stroke gold -draw 'rectangle 400,200 1100,800'              1.98   3.93   5.83   7.65   9.45  10.98  12.58  13.74 14.95 15.86 16.73 17.68  22.40   33.27  60
   -fill none -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'   2.06   4.05   5.88   7.70   9.39  10.75  12.19  13.06 13.84 14.47 15.28 15.88  16.57   39.52  33
   -fill blue -stroke gold -draw 'roundRectangle 400,200 1100,800 20,20'   2.04   4.03   5.94   7.75   9.53  10.99  12.50  13.51 14.61 15.48 16.38 16.88  19.60   34.00  53
   -fill none -stroke gold -draw 'polygon 400,200 1100,800 100,300'        1.98   3.90   5.77   7.65   9.42  10.96  12.53  13.83 14.80 15.90 16.71 17.55  21.75   32.27  61
   -fill blue -stroke gold -draw 'polygon 400,200 1100,800 100,300'        1.98   3.94   5.85   7.70   9.47  11.06  12.74  13.97 14.97 16.03 17.14 17.78  22.46   31.27  62
   -fill none -stroke gold -draw 'Bezier 400,200 1100,800 100,300'         2.02   3.87   5.52   7.02   8.34   9.31  10.28  10.81 11.25 11.76 11.85 11.90  10.30   41.00  31
   -fill blue -stroke gold -draw 'Bezier 400,200 1100,800 100,300'         2.03   3.91   5.64   7.17   8.56   9.51  10.63  11.18 11.68 12.19 12.45 12.69  11.20   39.32  28
   -edge 0x1                                                               1.98   3.93   5.82   7.70   9.26  10.87  12.35  13.65 14.79 15.62 16.61 17.41  22.25   18.96  63
   -emboss 0x1                                                             1.83   3.19   4.22   5.03   5.61   6.11   6.48   6.84  7.10  7.30  7.49  7.59   8.19    5.72  51
   -enhance                                                                1.99   3.97   5.95   7.86   9.61  11.17  12.67  14.25 15.29 16.15 17.23 18.13  22.38    4.37  61
   -gaussian 0x0.5                                                         1.99   3.93   5.85   7.73   9.36  10.91  12.41  13.65 14.73 15.75 16.69 17.42  22.87   18.69  64
   -gaussian 0x1.0                                                         1.99   3.94   5.89   7.79   9.42  10.99  12.51  14.00 15.11 15.90 16.89 17.90  23.55    5.66  54
   -hald-clut identity:8                                                   1.98   3.93   5.88   7.80   9.30  10.92  12.04  12.73 13.51 13.98 15.07 16.05  24.27   17.76  61
   -hald-clut identity:10                                                  1.98   3.94   5.85   7.76   9.20  10.76  12.00  13.13 13.49 14.37 15.25 15.66  24.24   16.73  61
   -hald-clut identity:14                                                  1.97   3.95   5.88   7.74   9.36  10.98  12.56  13.60 14.80 15.84 16.94 17.49  25.31   10.56  57
   -implode 0.5                                                            2.00   3.98   5.97   7.91   9.76  11.59  13.41  15.01 16.33 17.55 18.58 19.58  29.57    9.34  64
   -implode -1                                                             2.01   3.98   5.96   7.91   9.74  11.53  13.38  15.02 16.20 17.41 18.65 19.62  29.50   10.24  64
   -lat 10x10-5%                                                           1.99   3.95   5.88   7.79   9.60  11.36  13.04  14.61 15.85 17.10 18.17 19.31  28.65    3.87  64
   -median 1                                                               1.99   3.92   5.84   7.58   9.30  10.78  12.31  13.49 14.20 14.64 14.99 15.57  14.08    1.24  33
   -median 2                                                               2.03   3.97   5.93   7.83   9.57  11.27  12.80  14.43 15.00 15.73 16.30 16.93  17.63    0.56  47
   -minify                                                                 1.96   3.93   5.65   7.45   8.90  10.02  11.26  12.46 12.88 13.68 14.51 14.59  15.19   36.00  44
   -modulate 110/100/95                                                    2.00   3.99   5.93   7.91   9.60  11.37  12.70  13.88 14.81 16.07 16.73 18.09  28.75   18.92  62
   +noise Uniform                                                          1.96   3.81   5.59   7.27   8.78  10.12  11.23  11.64 12.48 13.27 13.78 14.29  19.99   13.55  63
   +noise Gaussian                                                         1.97   3.89   5.79   7.68   9.35  10.99  12.20  13.30 14.08 14.93 16.27 17.05  27.13    5.94  64
   +noise Multiplicative                                                   1.98   3.89   5.78   7.68   9.38  11.15  12.61  12.99 14.12 15.25 16.20 17.58  27.47    6.48  64
   +noise Impulse                                                          1.96   3.84   5.58   7.29   8.77  10.21  11.38  12.14 12.76 13.62 14.40 15.00  20.91   13.29  63
   +noise Laplacian                                                        1.99   3.90   5.79   7.60   9.28  10.86  12.18  13.35 13.84 14.60 15.66 16.48  25.52    9.90  64
   +noise Poisson                                                          2.00   3.97   5.84   7.72   9.45  11.14  12.18  13.08 13.85 14.82 16.09 16.72  24.74    6.43  63
   -noise 1                                                                2.00   3.93   5.85   7.56   9.32  10.81  12.32  13.42 14.16 14.70 15.14 15.56  13.67    1.23  32
   -noise 2                                                                2.00   3.97   5.87   7.80   9.53  11.13  12.70  14.40 14.90 15.60 16.17 16.73  17.47    0.56  40
   -fill blue -fuzz 35% -opaque red                                        1.96   3.84   5.58   7.31   8.88   9.96  11.26  11.37 12.14 12.65 12.91 13.29  14.38   56.69  49
   -operator all Add 2%                                                    1.97   3.83   5.70   7.41   9.11  10.51  11.89  13.00 13.60 14.27 15.02 15.60  17.08   48.20  52
   -operator all And 233                                                   1.90   3.49   5.11   6.21   7.43   8.19   8.89   9.00  9.12  9.16  9.20  9.09   6.91   81.20  21
   -operator all Assign 50%                                                1.79   3.07   4.42   4.79   5.37   5.56   5.70   5.66  5.56  5.47  5.39  5.29   4.11   84.60  14
   -operator all Depth 6                                                   1.90   3.47   5.12   6.15   7.44   8.20   8.80   9.12  9.25  9.25  9.14  9.07   6.85   82.60  18
   -operator all Divide 2                                                  1.97   3.82   5.70   7.35   9.11  10.60  11.85  13.17 13.86 14.69 15.35 15.67  17.72   48.10  47
   -operator all Gamma 0.7                                                 1.90   3.45   5.06   6.12   7.41   8.18   8.76   9.07  9.18  9.16  9.18  9.02   6.83   82.20  23
   -operator all Negate 1.0                                                1.91   3.52   5.18   6.33   7.64   8.56   9.19   9.60  9.85  9.99 10.06 10.09   7.37   83.23  24
   -operator all LShift 2                                                  1.90   3.49   5.11   6.17   7.44   8.25   8.71   9.00  9.16  9.11  9.32  9.14   6.92   82.00  22
   -operator all Multiply 0.5                                              1.97   3.84   5.69   7.37   8.99  10.57  11.91  12.97 13.60 14.45 14.98 15.76  17.39   47.80  44
   -operator all Or 233                                                    1.90   3.48   5.13   6.23   7.41   8.23   8.78   9.03  9.16  9.16  9.25  9.05   6.90   81.40  22
   -operator all RShift 2                                                  1.90   3.48   5.10   6.19   7.48   8.17   8.75   9.00  9.14  9.07  9.30  9.18   6.91   81.80  22
   -operator all Subtract 10%                                              1.97   3.85   5.72   7.40   9.03  10.49  11.81  12.79 13.40 14.24 15.19 15.11  16.84   48.50  42
   -operator red Threshold 50%                                             1.83   3.21   4.69   5.40   6.45   6.94   7.35   7.53  7.54  7.49  7.38  7.22   4.76  111.80  18
   -operator gray Threshold 50%                                            1.94   3.60   5.32   6.49   7.89   8.37   9.11   8.71  8.37  8.88  8.86  9.13   8.51   66.00  33
   -operator all Threshold-White 80%                                       1.94   3.56   5.31   6.49   7.78   7.95   7.83   7.20  7.80  7.74  7.90  8.05   7.76   67.27  42
   -operator all Threshold-Black 10%                                       1.92   3.57   5.26   6.40   7.52   7.93   7.69   7.88  7.78  7.92  8.06  8.09   8.09   67.00  40
   -operator all Xor 233                                                   1.90   3.49   5.13   6.17   7.45   8.20   8.73   8.94  9.23  9.16  9.23  9.00   6.93   81.80  23
   -operator all Noise-Gaussian 30%                                        1.99   3.95   5.93   7.87   9.61  11.43  12.60  13.32 14.55 15.89 16.58 17.35  30.13    6.63  64
   -operator all Noise-Impulse 30%                                         1.98   3.97   5.87   7.80   9.67  11.18  12.30  13.36 14.18 14.69 15.53 16.25  26.46   17.30  60
   -operator all Noise-Laplacian 30%                                       1.99   3.96   5.90   7.88   9.56  11.28  12.76  13.22 14.34 15.45 16.88 17.69  29.70   11.83  63
   -operator all Noise-Multiplicative 30%                                  1.98   3.98   5.96   7.92   9.70  11.30  13.08  14.32 14.81 16.11 17.27 17.54  29.84    7.23  63
   -operator all Noise-Poisson 30%                                         1.98   3.95   5.89   7.86   9.67  11.30  12.86  13.39 14.28 15.09 16.00 17.42  27.38    7.23  64
   -operator all Noise-Uniform 30%                                         1.99   4.00   5.90   7.86   9.60  11.19  12.32  13.17 13.31 14.52 15.32 15.94  25.43   17.62  61
   -ordered-dither all 2x2                                                 1.91   3.66   5.24   6.62   7.71   8.88   9.24   9.56  9.78 10.04 10.14 10.17   8.36   74.40  25
   -ordered-dither all 3x3                                                 1.93   3.77   5.41   6.98   8.28   9.47  10.17  10.71 11.34 11.59 11.96 11.99  10.53   66.07  25
   -ordered-dither intensity 3x3                                           1.93   3.77   5.40   6.98   8.26   9.58  10.20  10.90 11.28 11.63 11.97 12.03  10.50   66.20  27
   -ordered-dither all 4x4                                                 1.93   3.71   5.28   6.64   7.81   8.98   9.34   9.62  9.89 10.19 10.31 10.31   8.50   74.40  27
   -paint 0x1                                                              2.00   3.97   5.92   7.84   9.47  11.03  12.62  13.89 14.99 16.03 16.52 17.61  21.44    9.54  50
   -random-threshold all 20x80                                             1.96   3.83   5.64   7.34   8.81  10.28  11.19  11.92 12.67 13.11 13.42 14.27  13.22   49.60  29
   -recolor '1,0,0,0,1,0,0,0,1'                                            1.76   2.97   3.83   4.31   4.62   4.71   5.20   4.73  4.69  4.68  4.47  4.31   2.58  221.00  13
   -recolor '0,0,1,0,1,0,1,0,0'                                            1.98   3.93   5.80   7.62   9.02  10.90  11.67  11.74 11.74 12.70 13.40 13.72  20.72   31.20  63
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                      1.98   3.94   5.86   7.68   9.29  10.70  11.42  12.11 12.32 12.91 13.45 14.13  21.65   26.69  58
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                          1.98   3.94   5.84   7.73   9.22  10.70  11.53  12.16 12.32 12.86 13.40 13.94  21.57   26.89  63
   -density 75x75 -resample 50x50                                          1.98   3.90   5.81   7.59   8.70  10.38  11.58  11.44 11.89 12.23 13.28 13.76  20.29   12.01  63
   -resize 10%                                                             1.98   3.89   5.78   7.26   8.57  10.02  10.94  11.47 12.11 12.52 13.52 13.68  20.88   21.87  60
   -resize 50%                                                             1.98   3.88   5.77   7.49   8.72   9.69  10.39  10.86 11.84 12.19 12.74 13.37  19.38   14.20  62
   -resize 150%                                                            1.99   3.91   5.79   7.75   8.58  10.50  12.04  12.63 12.77 13.50 14.12 14.75  23.20    6.11  63
   -rotate 15                                                              1.87   3.38   4.59   5.60   6.40   6.97   7.51   7.56  7.77  7.78  7.88  7.83   7.80    2.35  45
   -rotate 45                                                              1.74   2.79   3.48   3.97   4.33   4.60   4.77   4.85  4.91  4.92  4.97  4.94   4.99    0.73  61
   -segment 0.5x0.25                                                       1.25   1.50   1.50   1.50   1.62   1.62   1.62   1.62  1.62  1.62  1.62  1.62   1.62    0.01  10
   -shade 30x30                                                            1.98   3.91   5.82   7.67   9.36  10.97  12.68  13.55 14.76 15.75 16.26 16.97  19.67   20.56  49
   -sharpen 0x0.5                                                          1.98   3.93   5.85   7.71   9.30  10.88  12.36  13.60 14.97 15.72 16.61 17.38  22.68   18.65  62
   -sharpen 0x1.0                                                          1.98   3.95   5.90   7.80   9.45  11.03  12.48  13.92 15.20 16.00 16.93 17.83  23.60    5.67  58
   -shear 45x45                                                            1.64   2.44   2.90   3.16   3.37   3.53   3.63   3.69  3.68  3.74  3.76  3.76   3.72    1.10  23
   -solarize 50%                                                           1.94   3.72   5.33   6.78   7.98   8.85   9.70  10.17 10.46 10.78 11.11 11.14   8.69   78.44  27
   -swirl 90                                                               1.96   3.93   5.86   7.78   9.55  11.34  13.04  14.37 15.83 16.95 18.13 19.25  28.70   11.22  64
   -fuzz 35% -transparent red                                              1.92   3.78   5.64   7.21   8.73   9.84  10.89  11.49 11.56 12.29 12.77 12.80  13.74   54.20  52
   -trim                                                                   1.98   3.94   5.81   7.64   9.25  10.87  12.21  13.48 14.49 15.44 16.27 17.29  20.92   34.06  52
   -fuzz 5% -trim                                                          2.00   3.95   5.93   7.78   9.60  11.23  12.76  14.47 15.58 17.08 18.01 19.04  27.01   28.14  59
   -unsharp 0x0.5+20+1                                                     1.95   3.57   4.66   5.58   6.20   6.73   7.21   7.48  7.74  7.98  8.17  8.33   8.45    5.07  30
   -unsharp 0x1.0+20+1                                                     1.95   3.67   4.93   5.97   6.75   7.47   8.11   8.58  8.87  9.08  9.46  9.67  10.22    4.54  45
   -wave 25x150                                                            1.98   3.85   5.28   6.49   7.48   8.51   9.51  10.29 11.04 11.75 12.33 12.84  15.14    5.48  48
   =====================================================================  =====  =====  =====  =====  =====  =====  =====  ===== ===== ===== ===== =====  ===== ======= ====


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

.. Last update: Sat Dec 24 19:37:08 CST 2011

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
   -noop                                                                  1.00  1.00  1.00  0.99  12.443   1
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.88  2.08  2.37   0.820   4
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  1.12  1.14  1.16   8.074   4
   -blur 0x0.5                                                            1.00  1.66  2.14  2.34   1.471   4
   -blur 0x1.0                                                            1.00  1.71  2.24  2.62   1.370   4
   -blur 0x2.0                                                            1.00  1.77  2.39  2.84   1.116   4
   -charcoal 0x1                                                          1.00  1.74  2.27  2.67   0.738   4
   -colorspace CMYK                                                       1.00  1.07  1.06  1.09   7.200   4
   -colorspace GRAY                                                       1.00  1.73  2.16  2.45   4.557   4
   -colorspace HSL                                                        1.00  1.83  2.53  3.12   3.400   4
   -colorspace HWB                                                        1.00  1.78  2.43  2.95   4.280   4
   -colorspace OHTA                                                       1.00  1.69  2.16  2.49   4.655   4
   -colorspace YCbCr                                                      1.00  1.72  2.16  2.47   4.600   4
   -colorspace YIQ                                                        1.00  1.73  2.18  2.52   4.655   4
   -colorspace YUV                                                        1.00  1.66  2.18  2.48   4.600   4
   -contrast -contrast -contrast                                          1.00  1.96  2.89  3.78   0.842   4
   +contrast +contrast +contrast                                          1.00  1.97  2.91  3.80   0.851   4
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.78  2.37  2.90   2.945   4
   -colorize 30%/20%/50%                                                  1.00  1.68  2.22  2.59   3.902   4
   -despeckle                                                             1.00  1.00  1.00  1.00   0.055   1
   -edge 0x1                                                              1.00  1.88  2.65  3.37   1.746   4
   -emboss 0x1                                                            1.00  1.80  2.40  2.96   1.506   4
   -enhance                                                               1.00  1.96  2.88  3.77   0.596   4
   -gaussian 0x0.5                                                        1.00  1.88  2.67  3.35   1.860   4
   -gaussian 0x1.0                                                        1.00  1.93  2.82  3.65   0.887   4
   -gaussian 0x2.0                                                        1.00  1.89  2.81  3.66   0.293   4
   -hald-clut identity:8                                                  1.00  1.87  2.68  3.36   2.313   4
   -hald-clut identity:10                                                 1.00  1.85  2.58  3.20   2.180   4
   -hald-clut identity:14                                                 1.00  1.67  2.15  2.47   1.345   4
   -implode 0.5                                                           1.00  1.92  2.79  3.60   1.214   4
   -implode -1                                                            1.00  1.86  2.77  3.54   1.200   4
   -lat 10x10-5%                                                          1.00  1.82  2.49  3.05   1.410   4
   -median 1                                                              1.00  1.92  2.70  3.22   0.235   4
   -median 2                                                              1.00  1.96  2.88  3.54   0.092   4
   -minify                                                                1.00  1.69  2.15  2.49   5.058   4
   -modulate 110/100/95                                                   1.00  1.85  2.60  3.24   2.909   4
   +noise Uniform                                                         1.00  1.67  2.14  2.43   2.327   4
   +noise Gaussian                                                        1.00  1.91  2.76  3.54   0.732   4
   +noise Multiplicative                                                  1.00  1.89  2.67  3.42   0.899   4
   +noise Impulse                                                         1.00  1.69  2.20  2.58   2.259   4
   +noise Laplacian                                                       1.00  1.82  2.50  3.01   1.488   4
   +noise Poisson                                                         1.00  1.92  2.75  3.00   0.661   4
   -noise 1                                                               1.00  2.00  2.86  3.85   0.277   4
   -noise 2                                                               1.00  1.96  2.85  3.46   0.090   4
   -fill blue -fuzz 35% -opaque red                                       1.00  1.34  1.50  1.60   9.619   4
   -operator all Add 2%                                                   1.00  1.72  2.24  2.66   5.367   4
   -operator all And 233                                                  1.00  1.04  1.06  1.08   8.400   4
   -operator all Assign 50%                                               1.00  1.06  1.06  1.06   8.520   3
   -operator all Depth 6                                                  1.00  1.06  1.10  1.11   8.400   4
   -operator all Divide 2                                                 1.00  1.67  2.23  2.66   5.384   4
   -operator all Gamma 0.7                                                1.00  1.08  1.07  1.08   8.025   2
   -operator all Negate 1.0                                               1.00  1.05  1.07  1.07   8.547   4
   -operator all LShift 2                                                 1.00  1.00  1.06  1.08   8.441   4
   -operator all Multiply 0.5                                             1.00  1.71  2.25  2.66   5.384   4
   -operator all Or 233                                                   1.00  1.07  1.08  1.09   8.573   4
   -operator all RShift 2                                                 1.00  1.01  1.06  1.08   8.468   4
   -operator all Subtract 10%                                             1.00  1.73  2.29  2.69   5.237   4
   -operator red Threshold 50%                                            1.00  1.07  1.06  1.07   8.415   2
   -operator gray Threshold 50%                                           1.00  1.04  1.06  1.09   8.545   4
   -operator all Threshold-White 80%                                      1.00  1.11  1.13  1.14  10.075   4
   -operator all Threshold-Black 10%                                      1.00  1.15  1.16  1.17   9.740   4
   -operator all Xor 233                                                  1.00  1.06  1.04  1.08   8.400   4
   -operator all Noise-Gaussian 30%                                       1.00  1.95  2.90  3.78   0.812   4
   -operator all Noise-Impulse 30%                                        1.00  1.86  2.56  3.21   3.160   4
   -operator all Noise-Laplacian 30%                                      1.00  1.92  2.76  3.52   1.855   4
   -operator all Noise-Multiplicative 30%                                 1.00  1.96  2.88  3.61   0.982   4
   -operator all Noise-Poisson 30%                                        1.00  1.97  2.90  3.77   0.863   4
   -operator all Noise-Uniform 30%                                        1.00  1.83  2.55  3.16   3.418   4
   -ordered-dither all 2x2                                                1.00  1.29  1.33  1.37   7.177   4
   -ordered-dither all 3x3                                                1.00  1.25  1.27  1.31   7.177   4
   -ordered-dither intensity 3x3                                          1.00  1.25  1.28  1.32   7.286   4
   -ordered-dither all 4x4                                                1.00  1.27  1.32  1.37   7.241   4
   -paint 0x1                                                             1.00  1.94  2.83  3.66   0.831   4
   -random-threshold all 20x80                                            1.00  1.54  1.95  2.23   6.458   4
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.01  1.01  1.00  12.838   2
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  1.71  2.21  2.60   5.384   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  1.72  2.42  2.89   4.254   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  1.78  2.42  2.93   4.306   4
   -density 75x75 -resample 50x50                                         1.00  1.83  2.27  2.61   1.724   4
   -resize 10%                                                            1.00  1.72  2.23  2.63   4.725   4
   -resize 50%                                                            1.00  1.81  2.25  2.65   2.454   4
   -resize 150%                                                           1.00  1.76  2.35  2.56   0.713   4
   -rotate 15                                                             1.00  1.51  2.09  2.44   0.860   4
   -rotate 45                                                             1.00  1.24  1.42  1.60   0.193   4
   -segment 0.5x0.25                                                      1.00  1.38  1.62  1.75   0.028   4
   -shade 30x30                                                           1.00  1.70  2.23  2.63   4.048   4
   -sharpen 0x0.5                                                         1.00  1.87  2.65  3.33   1.860   4
   -sharpen 0x1.0                                                         1.00  1.89  2.82  3.64   0.882   4
   -sharpen 0x2.0                                                         1.00  1.89  2.79  3.65   0.292   4
   -shear 45x45                                                           1.00  1.40  1.73  1.90   0.561   4
   -solarize 50%                                                          1.00  1.09  1.09  1.09   8.295   2
   -swirl 90                                                              1.00  1.93  2.49  3.06   1.167   4
   -fuzz 35% -transparent red                                             1.00  1.33  1.41  1.44   8.271   4
   -trim                                                                  1.00  1.35  1.53  1.67   9.481   4
   -fuzz 5% -trim                                                         1.00  1.71  2.25  2.66   5.105   4
   -unsharp 0x0.5+20+1                                                    1.00  1.68  2.22  2.61   1.337   4
   -unsharp 0x1.0+20+1                                                    1.00  1.78  2.38  2.84   1.211   4
   -wave 25x150                                                           1.00  1.83  2.57  3.16   1.396   4
   ===================================================================== ===== ===== ===== ===== ======== ====

Windows XP / MinGW / Intel Core 2 Quad
--------------------------------------

.. Last update: Sun Dec 25 09:09:20 CST 2011

TDM's GCC/mingw32 4.5.1 was downloaded from
http://www.tdragon.net/recentgcc/ and installed on the same Windows XP
system with the 2.83 GHz Core 2 Quad Processor (Q9550) as described
above.  This processor is a multi-chip module (MCM) based on two Core
2 CPUs bonded to a L3 cache in the same chip package.

The following shows the performance boost for a Q16 build:

.. table:: Performance Boost on an Intel Core 2 Quad (Q9550) system

   ===================================================================== ===== ===== ===== ===== ======== ====
   Operation                                                               1     2     3     4    iter/s  thds
   ===================================================================== ===== ===== ===== ===== ======== ====
   -noop                                                                  1.00  0.99  0.99  0.99  12.520   1
   -affine 1,0,0.785,1,0,0 -transform                                     1.00  1.53  2.05  2.62   0.769   4
   -asc-cdl 0.9,0.01,0.45:0.9,0.01,0.45:0.9,0.01,0.45:0.01                1.00  1.12  1.19  1.22   7.951   4
   -blur 0x0.5                                                            1.00  1.65  2.10  2.39   1.538   4
   -blur 0x1.0                                                            1.00  1.81  2.42  2.86   1.143   4
   -blur 0x2.0                                                            1.00  1.86  2.60  3.24   0.757   4
   -charcoal 0x1                                                          1.00  1.76  2.31  2.71   0.665   4
   -colorspace CMYK                                                       1.00  1.09  1.06  1.09   7.110   2
   -colorspace GRAY                                                       1.00  1.41  1.50  1.49   7.177   3
   -colorspace HSL                                                        1.00  1.83  2.54  3.11   3.555   4
   -colorspace HWB                                                        1.00  1.77  2.38  2.87   4.528   4
   -colorspace OHTA                                                       1.00  1.39  1.47  1.50   7.024   4
   -colorspace YCbCr                                                      1.00  1.42  1.56  1.51   7.376   3
   -colorspace YIQ                                                        1.00  1.42  1.50  1.50   7.046   3
   -colorspace YUV                                                        1.00  1.41  1.51  1.50   7.133   3
   -contrast -contrast -contrast                                          1.00  1.95  2.86  3.73   0.627   4
   +contrast +contrast +contrast                                          1.00  1.91  2.88  3.77   0.637   4
   -convolve 1,1,1,1,4,1,1,1,1                                            1.00  1.81  2.48  3.06   2.723   4
   -colorize 30%/20%/50%                                                  1.00  1.70  2.19  2.50   4.048   4
   -despeckle                                                             1.00  1.00  1.00  0.99   0.112   1
   -edge 0x1                                                              1.00  1.80  2.53  3.13   2.469   4
   -emboss 0x1                                                            1.00  1.85  2.62  3.23   1.094   4
   -enhance                                                               1.00  1.97  2.91  3.83   0.383   4
   -gaussian 0x0.5                                                        1.00  1.92  2.78  3.57   1.284   4
   -gaussian 0x1.0                                                        1.00  1.97  2.92  3.83   0.452   4
   -gaussian 0x2.0                                                        1.00  2.00  2.97  3.74   0.127   4
   -hald-clut identity:8                                                  1.00  1.91  2.72  3.45   2.095   4
   -hald-clut identity:10                                                 1.00  1.91  2.68  3.38   2.047   4
   -hald-clut identity:14                                                 1.00  1.86  2.40  2.95   1.655   4
   -implode 0.5                                                           1.00  1.93  2.77  3.65   0.836   4
   -implode -1                                                            1.00  1.97  2.80  3.62   0.814   4
   -lat 10x10-5%                                                          1.00  1.84  2.56  3.15   1.241   4
   -median 1                                                              1.00  1.99  2.66  3.87   0.259   4
   -median 2                                                              1.00  2.00  2.92  4.00   0.100   4
   -minify                                                                1.00  1.75  2.33  2.82   4.241   4
   -modulate 110/100/95                                                   1.00  1.84  2.58  3.25   2.945   4
   +noise Uniform                                                         1.00  1.60  2.00  2.30   2.659   4
   +noise Gaussian                                                        1.00  1.99  2.96  3.86   0.278   4
   +noise Multiplicative                                                  1.00  1.94  2.83  3.63   0.486   4
   +noise Impulse                                                         1.00  1.60  1.97  2.26   2.691   4
   +noise Laplacian                                                       1.00  1.91  2.71  3.47   0.842   4
   +noise Poisson                                                         1.00  1.93  2.81  3.63   0.607   4
   -noise 1                                                               1.00  2.01  2.90  3.88   0.260   4
   -noise 2                                                               1.00  2.00  2.96  3.96   0.099   4
   -fill blue -fuzz 35% -opaque red                                       1.00  1.33  1.50  1.58   9.511   4
   -operator all Add 2%                                                   1.00  1.54  1.88  2.09   7.704   4
   -operator all And 233                                                  1.00  1.02  1.08  1.09   8.322   4
   -operator all Assign 50%                                               1.00  1.02  1.05  1.06   8.323   4
   -operator all Depth 6                                                  1.00  1.06  1.07  1.07   8.322   4
   -operator all Divide 2                                                 1.00  1.56  1.97  2.23   7.285   4
   -operator all Gamma 0.7                                                1.00  1.04  1.07  1.08   7.900   4
   -operator all Negate 1.0                                               1.00  1.03  1.06  1.08   8.348   4
   -operator all LShift 2                                                 1.00  1.04  1.08  1.09   8.347   4
   -operator all Multiply 0.5                                             1.00  1.56  1.91  2.17   7.415   4
   -operator all Or 233                                                   1.00  1.03  1.06  1.08   8.375   4
   -operator all RShift 2                                                 1.00  0.99  1.06  1.07   8.322   4
   -operator all Subtract 10%                                             1.00  1.53  1.88  2.10   7.576   4
   -operator red Threshold 50%                                            1.00  1.06  1.06  1.06   8.468   2
   -operator gray Threshold 50%                                           1.00  1.01  1.07  1.08   8.348   4
   -operator all Threshold-White 80%                                      1.00  1.11  1.14  1.14   9.970   4
   -operator all Threshold-Black 10%                                      1.00  1.14  1.16  1.16   9.709   3
   -operator all Xor 233                                                  1.00  1.04  1.06  1.08   8.520   4
   -operator all Noise-Gaussian 30%                                       1.00  1.97  2.93  3.85   0.285   4
   -operator all Noise-Impulse 30%                                        1.00  1.79  2.44  2.96   4.149   4
   -operator all Noise-Laplacian 30%                                      1.00  1.95  2.86  3.71   0.930   4
   -operator all Noise-Multiplicative 30%                                 1.00  1.97  2.91  3.87   0.526   4
   -operator all Noise-Poisson 30%                                        1.00  1.95  2.92  3.81   0.663   4
   -operator all Noise-Uniform 30%                                        1.00  1.79  2.44  2.96   4.025   4
   -ordered-dither all 2x2                                                1.00  1.22  1.21  1.25   7.156   4
   -ordered-dither all 3x3                                                1.00  1.19  1.23  1.26   7.242   4
   -ordered-dither intensity 3x3                                          1.00  1.21  1.20  1.25   7.177   4
   -ordered-dither all 4x4                                                1.00  1.19  1.20  1.24   7.133   4
   -paint 0x1                                                             1.00  1.91  2.69  3.42   1.735   4
   -random-threshold all 20x80                                            1.00  1.43  1.60  1.67   7.220   4
   -recolor '1,0,0,0,1,0,0,0,1'                                           1.00  1.00  1.01  1.01  12.522   3
   -recolor '0,0,1,0,1,0,1,0,0'                                           1.00  1.75  2.29  2.73   5.400   4
   -recolor '0.9,0,0,0,0.9,0,0,0,1.2'                                     1.00  1.60  2.01  2.33   5.944   4
   -recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'                         1.00  1.60  2.04  2.38   6.181   4
   -density 75x75 -resample 50x50                                         1.00  1.76  2.39  2.78   1.605   4
   -resize 10%                                                            1.00  1.82  2.45  3.00   3.358   4
   -resize 50%                                                            1.00  1.76  2.39  2.82   2.114   4
   -resize 150%                                                           1.00  1.83  2.21  2.60   0.844   4
   -rotate 15                                                             1.00  1.59  2.18  2.48   0.771   4
   -rotate 45                                                             1.00  1.23  0.36  0.38   0.108   2
   -segment 0.5x0.25                                                      1.00  1.44  1.72  1.83   0.033   4
   -shade 30x30                                                           1.00  1.68  2.19  2.56   4.161   4
   -sharpen 0x0.5                                                         1.00  1.91  2.76  3.53   1.280   4
   -sharpen 0x1.0                                                         1.00  1.97  2.92  3.83   0.452   4
   -sharpen 0x2.0                                                         1.00  1.97  2.97  3.91   0.133   4
   -shear 45x45                                                           1.00  1.31  1.78  2.06   0.571   4
   -solarize 50%                                                          1.00  1.03  1.09  1.11   8.373   4
   -swirl 90                                                              1.00  1.91  2.74  3.52   0.838   4
   -fuzz 35% -transparent red                                             1.00  1.54  1.83  2.08   7.460   4
   -trim                                                                  1.00  1.29  1.43  1.48  10.073   4
   -fuzz 5% -trim                                                         1.00  1.74  2.32  2.77   4.544   4
   -unsharp 0x0.5+20+1                                                    1.00  1.67  2.21  2.59   1.374   4
   -unsharp 0x1.0+20+1                                                    1.00  1.78  2.42  2.91   1.046   4
   -wave 25x150                                                           1.00  1.88  2.67  3.29   1.143   4
   ===================================================================== ===== ===== ===== ===== ======== ====


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

| Copyright (C) 2008 - 2011 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/Copyright.html.

