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
close to 3X with only a small loss of CPU efficiency as threads are added.

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

   ================================== ==== ==== ==== ====
   Operation                            1    2    3    4 
   ================================== ==== ==== ==== ====
   -noop                              1.00 1.46 1.67 1.82
   -affine 1,0,0.785,1,0,0 -transform 1.00 1.96 2.74 3.61
   -black-threshold 20%               1.00 1.62 1.94 2.35
   -blur 0x0.5                        1.00 1.53 1.89 2.13
   -blur 0x1.0                        1.00 1.65 2.16 2.52
   -border 6x6                        1.00 1.28 1.45 1.49
   -charcoal 0x1                      1.00 1.74 2.29 2.76
   -chop 800x600+200+300              1.00 1.45 1.56 1.81
   -colorspace CMYK                   1.00 1.62 1.85 2.20
   -colorspace GRAY                   1.00 1.76 2.37 2.99
   -colorspace HSL                    1.00 1.95 2.81 3.68
   -colorspace HWB                    1.00 1.95 2.75 3.61
   -colorspace OHTA                   1.00 1.79 2.39 2.98
   -colorspace YCbCr                  1.00 1.80 2.40 3.05
   -colorspace YIQ                    1.00 1.76 2.33 2.92
   -colorspace YUV                    1.00 1.76 2.34 2.80
   -contrast -contrast -contrast      1.00 1.98 2.88 3.82
   +contrast +contrast +contrast      1.00 2.00 2.90 3.83
   -convolve 1,1,1,1,4,1,1,1,1        1.00 1.92 2.76 3.51
   -colorize 30%/20%/50%              1.00 1.86 2.57 3.33
   -crop 1700x900+100+100             1.00 1.40 1.53 1.73
   -despeckle                         1.00 1.49 2.83 2.86
   -edge 0x1                          1.00 1.96 2.81 3.75
   -emboss 0x1                        1.00 1.85 2.50 3.18
   -enhance                           1.00 1.99 2.89 3.87
   -equalize                          1.00 1.27 1.39 1.48
   -flip                              1.00 1.57 1.62 1.96
   -flop                              1.00 1.51 1.69 1.97
   -frame 15x15+3+3                   1.00 1.46 1.61 1.85
   -gamma 1.6                         1.00 1.29 1.40 1.50
   -gaussian 0x0.5                    1.00 1.97 2.84 3.78
   -gaussian 0x1.0                    1.00 1.98 2.87 3.85
   -implode 0.5                       1.00 1.62 2.38 3.06
   -implode -1                        1.00 1.51 2.21 2.81
   -lat 10x10-5%                      1.00 1.97 2.83 3.78
   -level 10%,1.2,90%                 1.00 1.44 1.72 1.95
   -median 1                          1.00 1.10 1.67 1.99
   -median 2                          1.00 1.22 1.84 2.27
   -modulate 110/100/95               1.00 1.97 2.82 3.70
   -motion-blur 0x3+30                1.00 0.83 1.22 1.58
   -negate                            1.00 1.41 1.55 1.96
   +noise Uniform                     1.00 1.39 1.95 2.34
   +noise Gaussian                    1.00 1.82 2.50 3.22
   +noise Multiplicative              1.00 1.66 2.29 2.98
   +noise Impulse                     1.00 1.26 1.66 2.09
   +noise Laplacian                   1.00 1.73 2.27 2.75
   +noise Poisson                     1.00 1.34 1.81 2.51
   -noise 1                           1.00 1.10 1.70 1.93
   -noise 2                           1.00 1.22 1.85 2.29
   -normalize                         1.00 1.29 1.42 1.46
   -fill blue -fuzz 35% -opaque red   1.00 1.75 2.31 2.88
   -ordered-dither all 3x3            1.00 1.71 2.17 2.66
   -paint 0x1                         1.00 1.63 2.42 3.11
   -raise 10x10                       1.00 1.43 1.54 1.76
   -density 75x75 -resample 50x50     1.00 1.81 2.56 3.29
   -resize 10%                        1.00 1.72 2.35 2.92
   -resize 50%                        1.00 1.79 2.54 3.25
   -resize 150%                       1.00 1.83 2.51 3.15
   -rotate 0                          1.00 1.58 1.77 2.00
   -rotate 45                         1.00 1.62 2.18 2.88
   -rotate 180                        1.00 1.53 1.64 1.66
   -shade 30x30                       1.00 1.91 2.65 3.24
   -sharpen 0x0.5                     1.00 1.96 2.81 3.68
   -sharpen 0x1.0                     1.00 1.98 2.87 3.76
   -shave 10x10                       1.00 1.45 1.62 1.81
   -shear 45x45                       1.00 1.42 1.99 2.35
   -solarize 50%                      1.00 1.61 1.85 2.29
   -spread 1                          1.00 1.66 2.07 2.46
   -spread 3                          1.00 1.69 2.11 2.43
   -swirl 90                          1.00 1.49 2.18 2.72
   -threshold 35%                     1.00 1.75 2.22 2.72
   -fuzz 35% -transparent red         1.00 1.79 2.33 3.02
   -trim                              1.00 1.90 2.68 3.45
   -unsharp 0x0.5                     1.00 1.59 2.00 2.33
   -unsharp 0x1.0                     1.00 1.71 2.22 2.68
   -wave 25x150                       1.00 1.25 1.78 2.16
   -white-threshold 80%               1.00 1.55 1.83 2.31
   ================================== ==== ==== ==== ====


--------------------------------------------------------------------------

| Copyright (C) 2008 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

