.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

========================================================
GraphicsMagick 1.3 vs ImageMagick 6.4.5 Benchmark Report
========================================================

.. _`GraphicsMagick 1.2 Benchmarks` : benchmarks-1.2.html

.. contents::
  :local:

Introduction
============

GraphicsMagick 1.3 has been benchmarked against the most recent
ImageMagick release available at time of release. ImageMagick has
recently been significantly updated to use OpenMP as has GraphicsMagick
1.3. Several disparate types of systems were used in our testing. We are
pleased to see that ImageMagick's performance has improved significantly
on the benchmark that we posted earlier (see `GraphicsMagick 1.2
Benchmarks`_). It seems that access to the useful benchmark and
GraphicsMagick source code has been quite helpful to ImageMagick in order
to resolve many of the performance issues.

Executive Summary
=================

GraphicsMagick is typically 1.5 to 5X faster at executing image
processing operations from the command line than ImageMagick 6.4.5 is.
Some ImageMagick algorithms run as much as 262X slower on three year old
hardware. ImageMagick 6.4.5 only runs adequately on the latest high-end
hardware with large caches, and runs poorly on most of the installed
computing base. Even on the latest high-end hardware, GraphicsMagick is
much more efficient than ImageMagick.

The often extreme difference in the results is difficult to believe until
one inspects the source code from both packages and sees that
GraphicsMagick uses streamlined straight-forward code paths while
ImageMagick uses convoluted and arcane code paths in an apparent attempt
to defeat human comprehension. While the ImageMagick approach does
successfully defeat human comprehension, it also seems to largely defeat
C compiler comprehension as well, resulting in slow code. If time and
money are important to you, then you should use GraphicsMagick rather
than ImageMagick. If time and money are not important to you, then you
should use ImageMagick.

Strategy
========

For this benchmark cycle, we used a completely different benchmark
approach. Instead of testing single-shot performance with large
uncompressed files, we focused on the ability to process many medium
sized (2048x1024 pixels) images. In order to prevent disk I/O from being
a factor, we used a small input image and tiled it to create a larger
input image via the "tile:" coder. The processed image is send to the
"null:" coder so that file writes to a slow disk are also not a factor.
Static executables are used and executed via full paths in order to
minimize variability from the execution environment. In order to obtain
accurate and useful timing, we used the bash shell to execute the command
20 times and see how long it took. This is a very simple benchmark
approach which is quite representative of the performance that the
typical user observes.

This new benchmark reveals the following performance criteria:

  * Basic execution overhead of the software.
  
  * Image processing algorithmic efficiency.
  
  * Efficacy of OpenMP enhancements.

The software involved in the testing identified itself as::

  GraphicsMagick 1.3 unreleased Q16 http://www.GraphicsMagick.org/

  ImageMagick 6.4.5 2008-11-03 Q16 OpenMP http://www.imagemagick.org

The Benchmark
=============

The benchmark is quite simple. It reads a list of commands to execute
from a file named "commands.txt" and times how long it takes to execute
each command 20 times using GraphicsMagick and ImageMagick.

Here is the simple benchmark script::

  #!/bin/bash
  #
  # Measure the performance between two 'convert' commands by
  # executing a subcommand through many iterations and seeing
  # the total time that it takes.
  #
  # Written by Bob Friesenhahn, October 2008
  #
  convert1='/usr/local/bin/gm convert'
  convert2='/usr/local/bin/convert'
  input_image="-size 1920x1080 tile:model.pnm"
  output_image="null:"
  iterations=20
  echo "Convert-1:   ${convert1}"
  echo "Version:     `${convert1} -version | head -1`"
  echo "Convert-2:   ${convert2}"
  echo "Version:     `${convert2} -version | head -1`"
  echo "Date:        `date`"
  echo "Host:        `uname -n`"
  echo "OS:          `uname -s`"
  echo "Release:     `uname -r`"
  echo "Arch:        `uname -p`"
  echo "Input File:  ${input_image}"
  echo "Output File: ${output_image}"
  echo "Threads:     ${OMP_NUM_THREADS:-1}"
  echo "Iterations:  ${iterations}"
  echo "========================================================================================"
  echo
  cat commands.txt | while read subcommand
  do
    echo ${subcommand}
  
    command1="${convert1} ${input_image} ${subcommand} ${output_image}"
    i=1
    count=$iterations
    time while test $i -lt $count
    do
      eval "${command1}"
      let i=i+1
    done
    sleep 1
  
    command2="${convert2} ${input_image} ${subcommand} ${output_image}"
    i=1
    count=$iterations
    time while test $i -lt $count
    do
      eval "${command2}"
      let i=i+1
    done
  
    echo
    sleep 1
  done

Test Results
============

The test results are expressed as the image processing operation
performed, the amount of wall-clock time it took for GraphicsMagick to
execute it twenty times, the amount of wall-clock time it took for
ImageMagick to execute it twenty times, and the ImageMagick time divided
by the GraphicsMagick time. In order to understand the "IM/GM ratio"
column, a value of 0.50 indicates that GM is half as fast as IM, a value
of 1.00 indicates that GM and IM are the same speed, and a value of 4.00
indicates that GM is four times as fast as IM.

AMD Opteron/Solaris 10
----------------------

This system is a modern AMD Opteron system with two dual-core 3.0GHz CPUs
and running Solaris 10 U5. The compiler used identifies itself as "gcc
(GCC) 4.2.3".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:00.749   0:01.273    1.70
-affine 1,0,0.785,1,0,0 -transform   0:02.310   4:08.401  107.53
-black-threshold 20%                 0:00.863   0:01.568    1.82
-blur 0x0.5                          0:04.185   0:08.070    1.93
-blur 0x1.0                          0:04.663   0:08.448    1.81
-border 6x6                          0:01.265   0:02.946    2.33
-charcoal 0x1                        0:08.167   0:12.700    1.56
-chop 800x600+200+300                0:00.923   0:01.532    1.66
-colorspace CMYK                     0:00.960   0:02.562    2.67
-colorspace GRAY                     0:01.117   0:02.672    2.39
-colorspace HSL                      0:01.740   0:02.821    1.62
-colorspace HWB                      0:01.384   0:02.603    1.88
-colorspace OHTA                     0:01.103   0:02.676    2.43
-colorspace YCbCr                    0:01.120   0:02.691    2.40
-colorspace YIQ                      0:01.100   0:02.698    2.45
-colorspace YUV                      0:01.109   0:02.664    2.40
-contrast -contrast -contrast        0:07.231   0:08.209    1.14
+contrast +contrast +contrast        0:06.668   0:07.739    1.16
-convolve 1,1,1,1,4,1,1,1,1          0:01.964   0:03.231    1.65
-colorize 30%/20%/50%                0:01.437   0:02.421    1.68
-crop 1700x900+100+100               0:01.153   0:01.969    1.71
-despeckle                           0:12.996   0:39.230    3.02
-edge 0x1                            0:02.831   0:04.415    1.56
-emboss 0x1                          0:03.617   0:10.060    2.78
-enhance                             0:08.398   0:08.606    1.02
-equalize                            0:01.441   0:02.658    1.84
-flip                                0:01.249   0:02.088    1.67
-flop                                0:01.280   0:02.051    1.60
-frame 15x15+3+3                     0:01.288   0:02.943    2.28
-gamma 1.6                           0:01.372   0:01.555    1.13
-gaussian 0x0.5                      0:02.806   0:04.340    1.55
-gaussian 0x1.0                      0:05.661   0:08.310    1.47
-implode 0.5                         0:05.468   0:09.718    1.78
-implode -1                          0:04.794   0:09.094    1.90
-lat 10x10-5%                        0:07.770   0:09.821    1.26
-level 10%,1.2,90%                   0:01.041   0:05.902    5.67
-median 1                            0:38.099   0:33.813    0.89
-median 2                            1:26.705   1:16.754    0.89
-modulate 110/100/95                 0:02.159   0:02.577    1.19
-motion-blur 0x3+30                  0:54.621   1:11.876    1.32
-negate                              0:00.861   0:01.344    1.56
+noise Uniform                       0:04.366   0:32.834    7.52
+noise Gaussian                      0:15.524   1:35.974    6.18
+noise Multiplicative                0:10.598   1:16.297    7.20
+noise Impulse                       0:04.381   0:31.372    7.16
+noise Laplacian                     0:06.020   0:40.159    6.67
+noise Poisson                       0:11.579   2:05.059   10.80
-noise 1                             0:38.586   0:33.877    0.88
-noise 2                             1:27.003   1:17.142    0.89
-normalize                           0:01.376   0:03.059    2.22
-fill blue -fuzz 35% -opaque red     0:00.983   0:01.657    1.69
-paint 0x1                           0:04.626   0:10.314    2.23
-raise 10x10                         0:00.809   0:01.342    1.66
-density 75x75 -resample 50x50       0:03.025   0:04.371    1.44
-resize 10%                          0:01.543   0:02.196    1.42
-resize 50%                          0:02.170   0:03.259    1.50
-resize 150%                         0:09.444   0:13.145    1.39
-roll +20+10                         0:01.272   0:31.839   25.03
-rotate 0                            0:01.275   0:02.098    1.65
-rotate 45                           0:25.886   0:29.334    1.13
-rotate 90                           0:02.032   0:02.584    1.27
-rotate 180                          0:01.242   0:02.109    1.70
-rotate 270                          0:02.109   0:02.603    1.23
-shade 30x30                         0:02.295   0:03.131    1.36
-sharpen 0x0.5                       0:02.806   0:04.274    1.52
-sharpen 0x1.0                       0:05.637   0:08.309    1.47
-shave 10x10                         0:01.217   0:02.189    1.80
-shear 45x45                         0:21.181   0:42.687    2.02
-solarize 50%                        0:00.857   0:01.351    1.58
-spread 1                            0:01.301   1:16.739   58.98
-spread 3                            0:01.317   1:15.766   57.53
-swirl 90                            0:04.751   0:09.107    1.92
-threshold 35%                       0:00.999   0:01.444    1.45
-fuzz 35% -transparent red           0:01.021   0:01.728    1.69
-trim                                0:01.156   0:02.238    1.94
-unsharp 0x0.5                       0:04.408   0:08.497    1.93
-unsharp 0x1.0                       0:04.981   0:08.928    1.79
-wave 25x150                         0:05.938   0:11.513    1.94
-white-threshold 80%                 0:00.853   0:01.546    1.81
================================== ========== ========== ===========


Sun SPARC/Solaris 10
--------------------

This system is a 2004 vintage Sun SPARC workstation with two 1.2GHz CPUs
and running Solaris 10 U5. The compiler used identifies itself as "gcc
(GCC) 4.2.4".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:01.526   0:07.578    4.97
-affine 1,0,0.785,1,0,0 -transform   0:11.329   5:15.568   27.85
-black-threshold 20%                 0:02.506   0:09.372    3.74
-blur 0x0.5                          0:12.971   0:33.334    2.57
-blur 0x1.0                          0:15.991   0:37.050    2.32
-border 6x6                          0:02.336   0:18.318    7.84
-charcoal 0x1                        0:36.199   1:09.599    1.92
-chop 800x600+200+300                0:02.024   0:08.183    4.04
-colorspace CMYK                     0:02.552   0:12.894    5.05
-colorspace GRAY                     0:04.225   0:12.964    3.07
-colorspace HSL                      0:08.005   0:14.341    1.79
-colorspace HWB                      0:07.083   0:13.906    1.96
-colorspace OHTA                     0:04.229   0:12.973    3.07
-colorspace YCbCr                    0:04.228   0:12.972    3.07
-colorspace YIQ                      0:04.224   0:12.876    3.05
-colorspace YUV                      0:04.244   0:12.913    3.04
-contrast -contrast -contrast        0:42.505   0:50.048    1.18
+contrast +contrast +contrast        0:40.600   0:48.062    1.18
-convolve 1,1,1,1,4,1,1,1,1          0:09.024   0:19.082    2.11
-colorize 30%/20%/50%                0:04.398   0:12.425    2.83
-crop 1700x900+100+100               0:02.193   0:08.409    3.83
-despeckle                           1:49.642   3:40.489    2.01
-edge 0x1                            0:14.888   0:26.428    1.78
-emboss 0x1                          0:19.709   1:03.768    3.24
-enhance                             0:42.950   0:42.707    0.99
-equalize                            0:04.744   0:14.854    3.13
-flip                                0:02.367   0:08.657    3.66
-flop                                0:02.984   0:09.343    3.13
-frame 15x15+3+3                     0:02.384   0:18.684    7.84
-gamma 1.6                           0:04.023   0:11.262    2.80
-gaussian 0x0.5                      0:15.795   0:27.357    1.73
-gaussian 0x1.0                      0:37.937   0:55.230    1.46
-implode 0.5                         0:22.553   0:42.850    1.90
-implode -1                          0:18.346   0:37.770    2.06
-lat 10x10-5%                        0:43.750   1:36.189    2.20
-level 10%,1.2,90%                   0:02.927   5:17.769  108.56
-median 1                            0:59.438   1:17.222    1.30
-median 2                            2:25.367   2:50.264    1.17
-modulate 110/100/95                 0:13.802   0:21.001    1.52
-motion-blur 0x3+30                  2:30.047   3:21.344    1.34
-negate                              0:02.248   0:08.389    3.73
+noise Uniform                       0:14.880   1:17.969    5.24
+noise Gaussian                      1:03.557   3:42.364    3.50
+noise Multiplicative                0:42.832   2:57.973    4.16
+noise Impulse                       0:15.328   1:17.282    5.04
+noise Laplacian                     0:25.185   1:40.490    3.99
+noise Poisson                       0:46.892   4:50.567    6.20
-noise 1                             1:01.259   1:19.243    1.29
-noise 2                             2:27.977   2:52.308    1.16
-normalize                           0:04.367   0:15.507    3.55
-fill blue -fuzz 35% -opaque red     0:03.790   0:26.564    7.01
-paint 0x1                           0:24.461   0:47.008    1.92
-raise 10x10                         0:01.637   0:07.766    4.74
-density 75x75 -resample 50x50       0:12.703   0:23.249    1.83
-resize 10%                          0:06.780   0:13.772    2.03
-resize 50%                          0:09.917   0:19.356    1.95
-resize 150%                         0:32.555   0:53.850    1.65
-roll +20+10                         0:02.347   0:30.410   12.96
-rotate 0                            0:02.388   0:08.613    3.61
-rotate 45                           0:58.035   1:22.708    1.43
-rotate 90                           0:04.085   0:09.689    2.37
-rotate 180                          0:02.987   0:09.448    3.16
-rotate 270                          0:04.074   0:09.552    2.34
-shade 30x30                         0:10.117   0:17.305    1.71
-sharpen 0x0.5                       0:15.744   0:27.288    1.73
-sharpen 0x1.0                       0:38.054   0:55.323    1.45
-shave 10x10                         0:02.331   0:08.658    3.71
-shear 45x45                         0:32.762   1:18.842    2.41
-solarize 50%                        0:02.306   0:09.034    3.92
-spread 1                            0:03.171   1:19.073   24.94
-spread 3                            0:03.297   1:19.369   24.07
-swirl 90                            0:18.545   0:36.082    1.95
-threshold 35%                       0:02.869   0:09.256    3.23
-fuzz 35% -transparent red           0:03.854   0:11.147    2.89
-trim                                0:05.808   0:09.636    1.66
-unsharp 0x0.5+20+1                  0:16.020   0:38.857    2.43
-unsharp 0x1.0+20+1                  0:18.996   0:42.441    2.23
-wave 25x150                         0:18.589   0:37.635    2.02
-white-threshold 80%                 0:02.487   0:09.405    3.78
================================== ========== ========== ===========

Apple PowerPC G5/OS-X Tiger
---------------------------

This system is an Apple G5 system with two 2.5GHz CPUs and running the
Tiger release of OS-X. The compiler used identifies itself as "gcc-4.3.2
(GCC) 4.3.2".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:02.141   0:05.006    2.34
-affine 1,0,0.785,1,0,0 -transform   0:07.154  31:15.331  262.14
-black-threshold 20%                 0:02.351   0:06.176    2.63
-blur 0x0.5                          0:09.133   0:28.061    3.07
-blur 0x1.0                          0:11.170   0:33.305    2.98
-border 6x6                          0:03.235   0:09.618    2.97
-charcoal 0x1                        0:23.033   0:59.141    2.57
-chop 800x600+200+300                0:02.460   0:05.401    2.20
-colorspace CMYK                     0:02.551   0:07.816    3.06
-colorspace GRAY                     0:03.371   0:07.975    2.37
-colorspace HSL                      0:05.102   0:08.414    1.65
-colorspace HWB                      0:04.695   0:08.404    1.79
-colorspace OHTA                     0:03.421   0:08.121    2.37
-colorspace YCbCr                    0:03.400   0:07.983    2.35
-colorspace YIQ                      0:03.369   0:08.037    2.39
-colorspace YUV                      0:03.380   0:07.966    2.36
-contrast -contrast -contrast        0:16.176   0:24.352    1.51
+contrast +contrast +contrast        0:15.800   0:24.236    1.53
-convolve 1,1,1,1,4,1,1,1,1          0:06.525   0:13.683    2.10
-colorize 30%/20%/50%                0:03.708   0:07.797    2.10
-crop 1700x900+100+100               0:02.953   0:05.784    1.96
-despeckle                           1:22.125   3:04.660    2.25
-edge 0x1                            0:06.472   0:25.053    3.87
-emboss 0x1                          0:12.320   1:09.507    5.64
-enhance                             0:32.979   0:46.402    1.41
-equalize                            0:03.033   0:08.155    2.69
-flip                                0:03.186   0:06.041    1.90
-flop                                0:03.216   0:06.064    1.89
-frame 15x15+3+3                     0:03.255   0:09.785    3.01
-gamma 1.6                           0:03.001   0:06.086    2.03
-gaussian 0x0.5                      0:10.551   0:25.374    2.40
-gaussian 0x1.0                      0:25.813   1:04.932    2.52
-implode 0.5                         0:10.902   0:22.598    2.07
-implode -1                          0:09.828   0:21.626    2.20
-lat 10x10-5%                        0:46.072   1:10.197    1.52
-level 10%,1.2,90%                   0:02.604   0:11.996    4.61
-median 1                            0:50.120   1:23.171    1.66
-median 2                            1:51.367   3:07.753    1.69
-modulate 110/100/95                 0:06.488   0:08.774    1.35
-motion-blur 0x3+30                  1:43.890   2:08.546    1.24
-negate                              0:02.397   0:05.246    2.19
+noise Uniform                       0:08.812   0:58.416    6.63
+noise Gaussian                      0:24.687   2:10.219    5.27
+noise Multiplicative                0:17.700   1:55.919    6.55
+noise Impulse                       0:08.959   0:55.778    6.23
+noise Laplacian                     0:12.668   1:10.429    5.56
+noise Poisson                       0:21.891   3:34.119    9.78
-noise 1                             1:07.811   1:29.053    1.31
-noise 2                             2:37.043   3:15.371    1.24
-normalize                           0:02.951   0:08.906    3.02
-fill blue -fuzz 35% -opaque red     0:02.883   0:06.302    2.19
-paint 0x1                           0:06.435   0:40.101    6.23
-raise 10x10                         0:02.226   0:05.067    2.28
-density 75x75 -resample 50x50       0:10.346   0:21.792    2.11
-resize 10%                          0:04.905   0:12.508    2.55
-resize 50%                          0:07.371   0:15.093    2.05
-resize 150%                         0:36.230   0:48.238    1.33
-roll +20+10                         0:03.240   6:29.392  120.18
-rotate 0                            0:03.129   0:06.056    1.94
-rotate 45                           1:22.108   1:27.433    1.06
-rotate 90                           0:03.669   0:06.440    1.76
-rotate 180                          0:03.203   0:06.036    1.88
-rotate 270                          0:03.671   0:06.484    1.77
-shade 30x30                         0:04.950   0:14.357    2.90
-sharpen 0x0.5                       0:10.704   0:25.082    2.34
-sharpen 0x1.0                       0:25.956   1:05.129    2.51
-shave 10x10                         0:03.162   0:06.028    1.91
-shear 45x45                         1:10.489   1:58.259    1.68
-solarize 50%                        0:02.367   0:05.966    2.52
-spread 1                            0:03.106  12:55.481  249.67
-spread 3                            0:03.162  13:05.018  248.27
-swirl 90                            0:11.516   0:23.322    2.03
-threshold 35%                       0:02.540   0:06.191    2.44
-fuzz 35% -transparent red           0:02.849   0:06.894    2.42
-trim                                0:04.384   0:06.751    1.54
-unsharp 0x0.5+20+1                  0:11.336   0:31.509    2.78
-unsharp 0x1.0+20+1                  0:12.989   0:36.978    2.85
-wave 25x150                         0:12.091   0:25.553    2.11
-white-threshold 80%                 0:02.295   0:06.228    2.71
================================== ========== ========== ===========

Intel Xeon / FreeBSD 7.0
------------------------

This system is a 2003 vintage Intel Xeon system with two 2.4GHz CPUs and
running FreeBSD 7.0. The compiler used identifies itself as "gcc (GCC)
4.2.1 20070719 [FreeBSD]".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:03.010   0:06.700    2.23
-affine 1,0,0.785,1,0,0 -transform   0:09.811  20:25.680  124.93
-black-threshold 20%                 0:03.329   0:07.208    2.17
-blur 0x0.5                          0:09.071   0:25.944    2.86
-blur 0x1.0                          0:10.336   0:27.158    2.63
-border 6x6                          0:05.676   0:11.523    2.03
-charcoal 0x1                        0:22.392   0:41.373    1.85
-chop 800x600+200+300                0:03.626   0:07.087    1.95
-colorspace CMYK                     0:03.869   0:09.694    2.51
-colorspace GRAY                     0:04.554   0:10.047    2.21
-colorspace HSL                      0:05.914   0:10.404    1.76
-colorspace HWB                      0:04.838   0:10.110    2.09
-colorspace OHTA                     0:04.615   0:10.064    2.18
-colorspace YCbCr                    0:04.482   0:10.058    2.24
-colorspace YIQ                      0:04.481   0:10.020    2.24
-colorspace YUV                      0:04.491   0:10.093    2.25
-contrast -contrast -contrast        0:18.364   0:30.375    1.65
+contrast +contrast +contrast        0:18.060   0:30.199    1.67
-convolve 1,1,1,1,4,1,1,1,1          0:06.893   0:10.962    1.59
-colorize 30%/20%/50%                0:04.811   0:09.779    2.03
-crop 1700x900+100+100               0:05.024   0:07.920    1.58
-despeckle                           0:51.850   3:04.778    3.56
-edge 0x1                            0:06.985   0:13.995    2.00
-emboss 0x1                          0:12.343   0:28.644    2.32
-enhance                             0:31.325   0:32.333    1.03
-equalize                            0:04.751   0:10.307    2.17
-flip                                0:05.662   0:08.471    1.50
-flop                                0:05.556   0:08.391    1.51
-frame 15x15+3+3                     0:05.693   0:11.275    1.98
-gamma 1.6                           0:05.541   0:08.331    1.50
-gaussian 0x0.5                      0:10.305   0:14.091    1.37
-gaussian 0x1.0                      0:20.015   0:23.821    1.19
-implode 0.5                         0:17.680   0:30.186    1.71
-implode -1                          0:12.920   0:25.140    1.95
-lat 10x10-5%                        0:23.660   0:23.226    0.98
-level 10%,1.2,90%                   0:03.986   1:45.078   26.36
-median 1                            1:20.912   1:09.754    0.86
-median 2                            3:22.398   2:57.052    0.87
-modulate 110/100/95                 0:07.388   0:11.225    1.52
-motion-blur 0x3+30                  2:24.969   3:01.534    1.25
-negate                              0:03.421   0:07.056    2.06
+noise Uniform                       0:09.654   1:03.113    6.54
+noise Gaussian                      0:24.779   2:30.811    6.09
+noise Multiplicative                0:18.474   2:12.502    7.17
+noise Impulse                       0:09.787   1:01.354    6.27
+noise Laplacian                     0:12.522   1:15.968    6.07
+noise Poisson                       0:23.621   4:15.570   10.82
-noise 1                             1:23.062   1:09.541    0.84
-noise 2                             3:19.800   2:55.989    0.88
-normalize                           0:04.590   0:11.292    2.46
-fill blue -fuzz 35% -opaque red     0:03.522   0:07.752    2.20
-paint 0x1                           0:12.565   0:31.864    2.54
-raise 10x10                         0:03.106   0:06.830    2.20
-density 75x75 -resample 50x50       0:12.371   0:16.312    1.32
-resize 10%                          0:06.852   0:10.760    1.57
-resize 50%                          0:09.951   0:13.566    1.36
-resize 150%                         0:34.780   0:38.619    1.11
-roll +20+10                         0:05.649   3:25.671   36.41
-rotate 0                            0:05.663   0:08.351    1.47
-rotate 45                           1:16.726   1:19.749    1.04
-rotate 90                           0:04.813   0:08.905    1.85
-rotate 180                          0:05.595   0:08.363    1.49
-rotate 270                          0:04.866   0:08.923    1.83
-shade 30x30                         0:07.087   0:10.833    1.53
-sharpen 0x0.5                       0:09.876   0:14.087    1.43
-sharpen 0x1.0                       0:20.033   0:23.786    1.19
-shave 10x10                         0:05.548   0:08.311    1.50
-shear 45x45                         1:06.440   1:48.575    1.63
-solarize 50%                        0:03.406   0:06.897    2.02
-spread 1                            0:05.427   5:26.353   60.14
-spread 3                            0:05.768   5:25.069   56.36
-swirl 90                            0:12.994   0:24.819    1.91
-threshold 35%                       0:03.964   0:07.203    1.82
-fuzz 35% -transparent red           0:03.528   0:08.060    2.28
-trim                                0:04.547   0:08.939    1.97
-unsharp 0x0.5+20+1                  0:10.401   0:28.620    2.75
-unsharp 0x1.0+20+1                  0:11.618   0:29.342    2.53
-wave 25x150                         0:16.272   0:32.563    2.00
-white-threshold 80%                 0:03.227   0:07.130    2.21
================================== ========== ========== ===========


--------------------------------------------------------------------------

| Copyright (C) 2008 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

