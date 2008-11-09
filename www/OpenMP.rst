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
four-core system.  All the pixel quantum values are divided by 2::

  % for threads in 1 2 3 4
  do
    env OMP_NUM_THREADS=$threads gm benchmark -duration 10 convert \
      -size 2048x1080 pattern:granite -operator all divide 2.0 null:
  done
  Results: 220 iter 10.04s user 10.04s total 21.912 iter/s (21.912 iter/s cpu)
  Results: 392 iter 19.41s user 10.02s total 39.122 iter/s (20.196 iter/s cpu)
  Results: 531 iter 27.73s user 10.01s total 53.047 iter/s (19.149 iter/s cpu)
  Results: 701 iter 36.65s user 10.00s total 70.100 iter/s (19.127 iter/s cpu)

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
running  Sun  Solaris  10:

.. table:: Performance Boost On Four Core AMD Operon System

   ================================== ===== ===== ===== ===== ======= ====
   Operation                            1     2     3     4   iter/s  thds
   ================================== ===== ===== ===== ===== ======= ====
   -noop                               1.00  1.01  1.24  1.25  162.40   4
   -affine 1,0,0.785,1,0,0 -transform  1.00  1.86  2.66  3.25   14.29   4
   -black-threshold 20%                1.00  1.35  1.73  1.74   87.80   4
   -blur 0x0.5                         1.00  1.42  1.72  1.87    8.05   4
   -blur 0x1.0                         1.00  1.57  1.98  2.27    6.59   4
   -border 6x6                         1.00  0.98  1.23  1.24   72.00   4
   -charcoal 0x1                       1.00  1.68  2.22  2.67    3.35   4
   -chop 800x600+200+300               1.00  1.24  1.59  1.66  119.40   4
   -colorspace CMYK                    1.00  1.38  1.58  1.72   82.04   4
   -colorspace GRAY                    1.00  1.63  2.22  2.60   57.69   4
   -colorspace HSL                     1.00  1.93  2.79  3.52   17.73   4
   -colorspace HWB                     1.00  1.87  2.70  3.34   25.10   4
   -colorspace OHTA                    1.00  1.66  2.26  2.59   57.60   4
   -colorspace YCbCr                   1.00  1.64  2.24  2.59   57.28   4
   -colorspace YIQ                     1.00  1.65  2.25  2.56   56.69   4
   -colorspace YUV                     1.00  1.65  2.26  2.60   57.40   4
   -contrast -contrast -contrast       1.00  1.97  2.93  3.81    2.89   4
   +contrast +contrast +contrast       1.00  1.98  2.95  3.75    3.10   4
   -convolve 1,1,1,1,4,1,1,1,1         1.00  1.87  2.72  3.34   19.05   4
   -colorize 30%/20%/50%               1.00  1.75  2.48  2.95   44.71   4
   -crop 1700x900+100+100              1.00  1.04  1.28  1.24   84.43   3
   -despeckle                          1.00  1.49  2.85  2.85    1.64   3
   -edge 0x1                           1.00  1.91  2.81  3.60    9.94   4
   -emboss 0x1                         1.00  1.80  2.49  3.06    7.50   4
   -enhance                            1.00  1.98  2.91  3.87    2.58   4
   -equalize                           1.00  1.26  1.39  1.50   31.34   4
   -flip                               1.00  1.11  1.35  1.33   73.65   3
   -flop                               1.00  1.33  1.59  1.57   72.06   3
   -frame 15x15+3+3                    1.00  1.07  1.28  1.26   69.26   3
   -gamma 1.6                          1.00  1.20  1.33  1.39   27.09   4
   -gaussian 0x0.5                     1.00  1.92  2.81  3.62   10.28   4
   -gaussian 0x1.0                     1.00  1.95  2.87  3.79    4.05   4
   -implode 0.5                        1.00  1.51  2.19  2.50    3.52   4
   -implode -1                         1.00  1.39  2.03  2.29    3.98   4
   -lat 10x10-5%                       1.00  1.95  2.85  3.74    3.10   4
   -level 10%,1.2,90%                  1.00  1.35  1.67  1.69   50.30   4
   -median 1                           1.00  0.88  1.22  1.34    0.52   4
   -median 2                           1.00  0.96  1.30  1.53    0.23   4
   -modulate 110/100/95                1.00  1.94  2.84  3.64   12.50   4
   -motion-blur 0x3+30                 1.00  0.90  1.31  1.17    0.28   3
   -negate                             1.00  1.17  1.34  1.59  100.60   4
   +noise Uniform                      1.00  1.52  2.19  2.89    6.64   4
   +noise Gaussian                     1.00  1.79  2.65  3.50    1.34   4
   +noise Multiplicative               1.00  1.69  2.47  3.28    2.06   4
   +noise Impulse                      1.00  1.51  2.17  2.80    6.50   4
   +noise Laplacian                    1.00  1.62  2.36  3.06    4.13   4
   +noise Poisson                      1.00  1.45  2.06  2.78    1.87   4
   -noise 1                            1.00  0.90  1.17  1.36    0.53   4
   -noise 2                            1.00  0.95  1.31  1.55    0.23   4
   -normalize                          1.00  1.21  1.38  1.43   30.54   4
   -fill blue -fuzz 35% -opaque red    1.00  1.75  2.33  2.94   91.40   4
   -paint 0x1                          1.00  1.95  2.86  3.79    5.18   4
   -raise 10x10                        1.00  1.17  1.33  1.35  131.60   4
   -density 75x75 -resample 50x50      1.00  1.72  2.56  3.33    9.92   4
   -resize 10%                         1.00  1.83  2.63  3.26   23.35   4
   -resize 50%                         1.00  1.92  2.78  3.53   16.21   4
   -resize 150%                        1.00  1.72  2.42  3.49    2.72   4
   -roll +20+10                        1.00  1.04  1.23  1.10   68.86   3
   -rotate 0                           1.00  1.14  1.41  1.35   74.05   3
   -rotate 45                          1.00  1.68  2.28  2.82    0.95   4
   -rotate 90                          1.00  0.97  1.00  0.99   24.80   1
   -rotate 180                         1.00  1.12  1.39  1.33   71.00   3
   -rotate 270                         1.00  0.99  0.99  1.00   21.76   4
   -shade 30x30                        1.00  1.90  2.76  3.62   15.14   4
   -sharpen 0x0.5                      1.00  1.92  2.81  3.60   10.18   4
   -sharpen 0x1.0                      1.00  1.95  2.87  3.64    3.90   4
   -shave 10x10                        1.00  1.13  1.37  1.28   77.20   3
   -shear 45x45                        1.00  1.59  2.20  2.73    1.19   4
   -solarize 50%                       1.00  1.27  1.58  1.63   86.83   4
   -spread 1                           1.00  1.55  2.15  2.35   64.00   4
   -spread 3                           1.00  1.48  2.04  2.23   55.20   4
   -swirl 90                           1.00  1.51  2.18  2.29    3.98   4
   -threshold 35%                      1.00  1.58  2.07  2.31   72.60   4
   -fuzz 35% -transparent red          1.00  1.61  2.20  2.57   69.00   4
   -trim                               1.00  1.81  2.60  3.18   38.12   4
   -unsharp 0x0.5+20+1                 1.00  1.45  1.78  2.00    7.53   4
   -unsharp 0x1.0+20+1                 1.00  1.59  2.03  2.36    6.20   4
   -wave 25x150                        1.00  1.66  1.73  1.97    3.86   4
   -white-threshold 80%                1.00  1.29  1.60  1.81  102.60   4
   ================================== ===== ===== ===== ===== ======= ====

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

   ================================== ===== ===== ======= ====
   Operation                            1     2   iter/s  thds
   ================================== ===== ===== ======= ====
   -noop                               1.00  0.79   25.10   1
   -affine 1,0,0.785,1,0,0 -transform  1.00  1.53    3.36   2
   -black-threshold 20%                1.00  0.90   18.09   1
   -blur 0x0.5                         1.00  1.40    2.36   2
   -blur 0x1.0                         1.00  1.53    1.90   2
   -border 6x6                         1.00  0.80   12.00   1
   -charcoal 0x1                       1.00  1.61    0.86   2
   -chop 800x600+200+300               1.00  0.84   18.76   1
   -colorspace CMYK                    1.00  0.95   14.74   1
   -colorspace GRAY                    1.00  1.34    8.86   2
   -colorspace HSL                     1.00  1.65    5.16   2
   -colorspace HWB                     1.00  1.62    5.47   2
   -colorspace OHTA                    1.00  1.34    8.82   2
   -colorspace YCbCr                   1.00  1.35    8.82   2
   -colorspace YIQ                     1.00  1.35    8.91   2
   -colorspace YUV                     1.00  1.34    8.78   2
   -contrast -contrast -contrast       1.00  1.88    1.28   2
   +contrast +contrast +contrast       1.00  1.87    1.31   2
   -convolve 1,1,1,1,4,1,1,1,1         1.00  1.67    3.63   2
   -colorize 30%/20%/50%               1.00  1.33    7.83   2
   -crop 1700x900+100+100              1.00  0.80   13.92   1
   -despeckle                          1.00  1.48    0.24   2
   -edge 0x1                           1.00  1.66    3.65   2
   -emboss 0x1                         1.00  1.71    1.68   2
   -enhance                            1.00  1.92    0.60   2
   -equalize                           1.00  0.89   11.55   1
   -flip                               1.00  0.83   11.95   1
   -flop                               1.00  0.82   11.81   1
   -frame 15x15+3+3                    1.00  0.82   11.80   1
   -gamma 1.6                          1.00  0.91   11.60   1
   -gaussian 0x0.5                     1.00  1.81    2.00   2
   -gaussian 0x1.0                     1.00  1.90    0.78   2
   -implode 0.5                        1.00  1.57    1.68   2
   -implode -1                         1.00  1.47    2.03   2
   -lat 10x10-5%                       1.00  1.92    0.43   2
   -level 10%,1.2,90%                  1.00  0.85   15.40   1
   -median 1                           1.00  0.98    0.34   1
   -median 2                           1.00  0.93    0.14   1
   -modulate 110/100/95                1.00  1.75    3.68   2
   -motion-blur 0x3+30                 1.00  1.41    0.28   2
   -negate                             1.00  0.80   19.28   1
   +noise Uniform                      1.00  1.66    2.56   2
   +noise Gaussian                     1.00  1.87    0.85   2
   +noise Multiplicative               1.00  1.82    1.15   2
   +noise Impulse                      1.00  1.66    2.46   2
   +noise Laplacian                    1.00  1.76    1.68   2
   +noise Poisson                      1.00  1.85    0.94   2
   -noise 1                            1.00  0.87    0.34   1
   -noise 2                            1.00  0.91    0.13   1
   -normalize                          1.00  0.89   11.86   1
   -fill blue -fuzz 35% -opaque red    1.00  1.19   12.53   2
   -paint 0x1                          1.00  1.61    3.94   2
   -raise 10x10                        1.00  0.80   23.21   1
   -density 75x75 -resample 50x50      1.00  1.57    2.08   2
   -resize 10%                         1.00  1.62    5.14   2
   -resize 50%                         1.00  1.58    3.09   2
   -resize 150%                        1.00  1.65    0.55   2
   -roll +20+10                        1.00  0.86   11.13   1
   -rotate 0                           1.00  0.81   12.08   1
   -rotate 45                          1.00  1.66    0.24   2
   -rotate 90                          1.00  0.88    8.64   1
   -rotate 180                         1.00  0.83   11.71   1
   -rotate 270                         1.00  0.88    8.66   1
   -shade 30x30                        1.00  1.55    5.13   2
   -sharpen 0x0.5                      1.00  1.80    2.00   2
   -sharpen 0x1.0                      1.00  1.90    0.78   2
   -shave 10x10                        1.00  0.80   12.28   1
   -shear 45x45                        1.00  1.61    0.27   2
   -solarize 50%                       1.00  0.90   17.73   1
   -spread 1                           1.00  0.88   10.87   1
   -spread 3                           1.00  0.98    9.96   1
   -swirl 90                           1.00  1.71    2.05   2
   -threshold 35%                      1.00  0.98   14.14   1
   -fuzz 35% -transparent red          1.00  1.19   12.40   2
   -trim                               1.00  1.58    6.28   2
   -unsharp 0x0.5+20+1                 1.00  1.52    1.92   2
   -unsharp 0x1.0+20+1                 1.00  1.59    1.58   2
   -wave 25x150                        1.00  1.32    1.76   2
   -white-threshold 80%                1.00  0.88   19.44   1
   ================================== ===== ===== ======= ====
   

--------------------------------------------------------------------------

| Copyright (C) 2008 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

