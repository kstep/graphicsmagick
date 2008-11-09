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
-noop                                0:00.771   0:01.268    1.64
-affine 1,0,0.785,1,0,0 -transform   0:02.418   4:08.961  102.96
-black-threshold 20%                 0:00.865   0:01.564    1.81
-blur 0x0.5                          0:04.178   0:07.982    1.91
-blur 0x1.0                          0:04.710   0:08.464    1.80
-border 6x6                          0:01.277   0:02.886    2.26
-charcoal 0x1                        0:08.169   0:12.602    1.54
-chop 800x600+200+300                0:00.938   0:01.574    1.68
-colorspace CMYK                     0:00.980   0:02.538    2.59
-colorspace GRAY                     0:01.145   0:02.665    2.33
-colorspace HSL                      0:01.743   0:02.856    1.64
-colorspace HWB                      0:01.423   0:02.638    1.85
-colorspace OHTA                     0:01.134   0:02.659    2.34
-colorspace YCbCr                    0:01.123   0:02.674    2.38
-colorspace YIQ                      0:01.131   0:02.685    2.37
-colorspace YUV                      0:01.147   0:02.696    2.35
-contrast -contrast -contrast        0:07.199   0:08.193    1.14
+contrast +contrast +contrast        0:06.703   0:07.687    1.15
-convolve 1,1,1,1,4,1,1,1,1          0:01.939   0:03.236    1.67
-colorize 30%/20%/50%                0:01.422   0:02.398    1.69
-crop 1700x900+100+100               0:01.151   0:01.934    1.68
-despeckle                           0:12.933   0:39.125    3.03
-edge 0x1                            0:02.822   0:04.381    1.55
-emboss 0x1                          0:03.681   0:10.004    2.72
-enhance                             0:08.396   0:08.667    1.03
-equalize                            0:01.467   0:02.624    1.79
-flip                                0:01.261   0:02.073    1.64
-flop                                0:01.268   0:02.062    1.63
-frame 15x15+3+3                     0:01.294   0:02.944    2.28
-gamma 1.6                           0:01.387   0:01.579    1.14
-gaussian 0x0.5                      0:02.795   0:04.343    1.55
-gaussian 0x1.0                      0:05.652   0:08.287    1.47
-implode 0.5                         0:05.961   0:09.684    1.62
-implode -1                          0:05.337   0:09.117    1.71
-lat 10x10-5%                        0:07.884   0:09.727    1.23
-level 10%,1.2,90%                   0:01.046   0:05.877    5.62
-median 1                            0:37.968   0:33.680    0.89
-median 2                            1:26.258   1:16.682    0.89
-modulate 110/100/95                 0:02.171   0:02.601    1.20
-motion-blur 0x3+30                  1:16.428   1:10.434    0.92
-negate                              0:00.870   0:01.347    1.55
+noise Uniform                       0:04.391   0:32.861    7.48
+noise Gaussian                      0:15.442   1:36.048    6.22
+noise Multiplicative                0:10.623   1:16.379    7.19
+noise Impulse                       0:04.394   0:31.481    7.16
+noise Laplacian                     0:06.013   0:40.154    6.68
+noise Poisson                       0:11.523   2:05.476   10.89
-noise 1                             0:38.296   0:33.930    0.89
-noise 2                             1:27.124   1:17.289    0.89
-normalize                           0:01.383   0:03.084    2.23
-fill blue -fuzz 35% -opaque red     0:00.994   0:01.663    1.67
-paint 0x1                           0:04.643   0:10.331    2.23
-raise 10x10                         0:00.826   0:01.334    1.62
-density 75x75 -resample 50x50       0:03.010   0:04.279    1.42
-resize 10%                          0:01.562   0:02.220    1.42
-resize 50%                          0:02.157   0:03.309    1.53
-resize 150%                         0:09.212   0:13.249    1.44
-roll +20+10                         0:01.292   0:31.909   24.70
-rotate 0                            0:01.313   0:02.098    1.60
-rotate 45                           0:26.130   0:29.426    1.13
-rotate 90                           0:02.065   0:02.610    1.26
-rotate 180                          0:01.301   0:02.051    1.58
-rotate 270                          0:02.173   0:02.644    1.22
-shade 30x30                         0:02.300   0:03.190    1.39
-sharpen 0x0.5                       0:02.849   0:04.324    1.52
-sharpen 0x1.0                       0:05.703   0:08.308    1.46
-shave 10x10                         0:01.248   0:02.219    1.78
-shear 45x45                         0:21.007   0:42.378    2.02
-solarize 50%                        0:00.879   0:01.356    1.54
-spread 1                            0:01.313   1:16.925   58.59
-spread 3                            0:01.373   1:17.254   56.27
-swirl 90                            0:05.326   0:09.057    1.70
-threshold 35%                       0:01.009   0:01.482    1.47
-fuzz 35% -transparent red           0:01.024   0:01.758    1.72
-trim                                0:01.152   0:02.231    1.94
-unsharp 0x0.5+20+1                  0:04.312   0:08.341    1.93
-unsharp 0x1.0+20+1                  0:04.878   0:08.814    1.81
-wave 25x150                         0:06.168   0:11.686    1.89
-white-threshold 80%                 0:00.865   0:01.529    1.77
================================== ========== ========== ===========



Sun SPARC/Solaris 10
--------------------

This system is a 2004 vintage Sun SPARC workstation with two 1.2GHz CPUs
and running Solaris 10 U5. The compiler used identifies itself as "gcc
(GCC) 4.2.4".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:01.559   0:07.682    4.93
-affine 1,0,0.785,1,0,0 -transform   0:11.955   5:16.076   26.44
-black-threshold 20%                 0:02.533   0:09.371    3.70
-blur 0x0.5                          0:13.241   0:33.991    2.57
-blur 0x1.0                          0:16.528   0:37.005    2.24
-border 6x6                          0:02.360   0:18.196    7.71
-charcoal 0x1                        0:36.755   1:09.377    1.89
-chop 800x600+200+300                0:02.035   0:08.176    4.02
-colorspace CMYK                     0:02.516   0:12.927    5.14
-colorspace GRAY                     0:04.249   0:12.848    3.02
-colorspace HSL                      0:08.076   0:14.373    1.78
-colorspace HWB                      0:07.243   0:13.889    1.92
-colorspace OHTA                     0:04.230   0:12.784    3.02
-colorspace YCbCr                    0:04.238   0:12.844    3.03
-colorspace YIQ                      0:04.274   0:12.825    3.00
-colorspace YUV                      0:04.236   0:12.942    3.06
-contrast -contrast -contrast        0:42.309   0:49.859    1.18
+contrast +contrast +contrast        0:40.519   0:47.918    1.18
-convolve 1,1,1,1,4,1,1,1,1          0:09.011   0:19.148    2.12
-colorize 30%/20%/50%                0:04.434   0:12.208    2.75
-crop 1700x900+100+100               0:02.169   0:08.325    3.84
-despeckle                           1:49.201   3:39.506    2.01
-edge 0x1                            0:14.793   0:26.531    1.79
-emboss 0x1                          0:19.629   1:02.875    3.20
-enhance                             0:42.450   0:42.911    1.01
-equalize                            0:04.663   0:14.824    3.18
-flip                                0:02.356   0:08.622    3.66
-flop                                0:02.950   0:09.365    3.17
-frame 15x15+3+3                     0:02.435   0:18.431    7.57
-gamma 1.6                           0:04.051   0:11.224    2.77
-gaussian 0x0.5                      0:15.661   0:27.420    1.75
-gaussian 0x1.0                      0:37.816   0:55.493    1.47
-implode 0.5                         0:23.256   0:43.107    1.85
-implode -1                          0:19.071   0:37.880    1.99
-lat 10x10-5%                        0:48.049   1:37.183    2.02
-level 10%,1.2,90%                   0:02.931   5:21.276  109.61
-median 1                            1:20.197   1:17.407    0.97
-median 2                            3:12.218   2:50.628    0.89
-modulate 110/100/95                 0:13.765   0:20.993    1.53
-motion-blur 0x3+30                  2:56.312   3:19.321    1.13
-negate                              0:02.259   0:08.321    3.68
+noise Uniform                       0:15.522   1:17.669    5.00
+noise Gaussian                      1:04.580   3:41.353    3.43
+noise Multiplicative                0:42.506   2:58.412    4.20
+noise Impulse                       0:15.893   1:17.551    4.88
+noise Laplacian                     0:25.900   1:41.173    3.91
+noise Poisson                       0:47.410   4:52.765    6.18
-noise 1                             1:22.076   1:19.480    0.97
-noise 2                             3:14.165   2:52.093    0.89
-normalize                           0:04.315   0:15.490    3.59
-fill blue -fuzz 35% -opaque red     0:03.683   0:26.441    7.18
-paint 0x1                           0:24.394   0:47.091    1.93
-raise 10x10                         0:01.650   0:07.759    4.70
-density 75x75 -resample 50x50       0:12.610   0:23.159    1.84
-resize 10%                          0:06.737   0:13.820    2.05
-resize 50%                          0:09.930   0:19.380    1.95
-resize 150%                         0:32.559   0:53.872    1.65
-roll +20+10                         0:02.422   0:30.244   12.49
-rotate 0                            0:02.341   0:08.596    3.67
-rotate 45                           0:57.382   1:22.235    1.43
-rotate 90                           0:04.159   0:09.543    2.29
-rotate 180                          0:02.934   0:09.402    3.20
-rotate 270                          0:04.181   0:09.754    2.33
-shade 30x30                         0:09.911   0:17.263    1.74
-sharpen 0x0.5                       0:15.625   0:27.502    1.76
-sharpen 0x1.0                       0:37.826   0:55.054    1.46
-shave 10x10                         0:02.312   0:08.769    3.79
-shear 45x45                         0:32.595   1:19.643    2.44
-solarize 50%                        0:02.304   0:08.967    3.89
-spread 1                            0:03.253   1:19.224   24.35
-spread 3                            0:03.410   1:19.282   23.25
-swirl 90                            0:19.158   0:35.971    1.88
-threshold 35%                       0:02.854   0:09.409    3.30
-fuzz 35% -transparent red           0:03.882   0:11.185    2.88
-trim                                0:05.854   0:09.697    1.66
-unsharp 0x0.5+20+1                  0:15.696   0:38.765    2.47
-unsharp 0x1.0+20+1                  0:19.230   0:42.428    2.21
-wave 25x150                         0:19.527   0:37.623    1.93
-white-threshold 80%                 0:02.488   0:09.448    3.80
================================== ========== ========== ===========


Apple PowerPC G5/OS-X Tiger
---------------------------

This system is an Apple G5 system with two 2.5GHz CPUs and running the
Tiger release of OS-X. The compiler used identifies itself as "gcc-4.3.2
(GCC) 4.3.2".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:02.113   0:04.835    2.29
-affine 1,0,0.785,1,0,0 -transform   0:06.906  31:21.385  272.43
-black-threshold 20%                 0:02.345   0:06.078    2.59
-blur 0x0.5                          0:09.122   0:28.368    3.11
-blur 0x1.0                          0:11.150   0:33.660    3.02
-border 6x6                          0:03.145   0:09.460    3.01
-charcoal 0x1                        0:23.201   0:59.822    2.58
-chop 800x600+200+300                0:02.427   0:05.163    2.13
-colorspace CMYK                     0:02.562   0:07.694    3.00
-colorspace GRAY                     0:03.328   0:07.835    2.35
-colorspace HSL                      0:04.887   0:08.314    1.70
-colorspace HWB                      0:04.663   0:08.214    1.76
-colorspace OHTA                     0:03.349   0:07.807    2.33
-colorspace YCbCr                    0:03.343   0:07.836    2.34
-colorspace YIQ                      0:03.340   0:07.852    2.35
-colorspace YUV                      0:03.377   0:07.884    2.33
-contrast -contrast -contrast        0:16.037   0:24.245    1.51
+contrast +contrast +contrast        0:15.641   0:23.931    1.53
-convolve 1,1,1,1,4,1,1,1,1          0:06.481   0:13.546    2.09
-colorize 30%/20%/50%                0:03.619   0:07.725    2.13
-crop 1700x900+100+100               0:02.888   0:05.684    1.97
-despeckle                           1:19.996   3:04.596    2.31
-edge 0x1                            0:06.371   0:24.836    3.90
-emboss 0x1                          0:12.608   1:09.318    5.50
-enhance                             0:32.758   0:46.010    1.40
-equalize                            0:03.009   0:07.880    2.62
-flip                                0:03.116   0:05.940    1.91
-flop                                0:03.161   0:05.972    1.89
-frame 15x15+3+3                     0:03.147   0:09.491    3.02
-gamma 1.6                           0:02.995   0:05.977    2.00
-gaussian 0x0.5                      0:10.781   0:25.032    2.32
-gaussian 0x1.0                      0:25.533   1:04.499    2.53
-implode 0.5                         0:12.509   0:22.421    1.79
-implode -1                          0:08.896   0:21.422    2.41
-lat 10x10-5%                        0:45.599   1:09.555    1.53
-level 10%,1.2,90%                   0:02.627   0:11.884    4.52
-median 1                            1:07.882   1:23.062    1.22
-median 2                            2:37.578   3:07.499    1.19
-modulate 110/100/95                 0:06.312   0:08.662    1.37
-motion-blur 0x3+30                  1:43.441   2:10.654    1.26
-negate                              0:02.387   0:05.055    2.12
+noise Uniform                       0:08.629   0:58.280    6.75
+noise Gaussian                      0:23.570   2:10.043    5.52
+noise Multiplicative                0:17.679   1:55.541    6.54
+noise Impulse                       0:08.849   0:55.627    6.29
+noise Laplacian                     0:12.525   1:10.048    5.59
+noise Poisson                       0:21.462   3:33.880    9.97
-noise 1                             1:05.736   1:28.922    1.35
-noise 2                             2:34.986   3:15.250    1.26
-normalize                           0:02.997   0:08.789    2.93
-fill blue -fuzz 35% -opaque red     0:02.865   0:06.204    2.17
-paint 0x1                           0:06.013   0:39.713    6.60
-raise 10x10                         0:02.202   0:04.965    2.25
-density 75x75 -resample 50x50       0:10.351   0:21.290    2.06
-resize 10%                          0:04.895   0:11.801    2.41
-resize 50%                          0:07.391   0:14.959    2.02
-resize 150%                         0:36.102   0:47.760    1.32
-roll +20+10                         0:03.134   6:41.949  128.25
-rotate 0                            0:03.071   0:05.963    1.94
-rotate 45                           1:22.150   1:28.017    1.07
-rotate 90                           0:03.675   0:06.352    1.73
-rotate 180                          0:03.181   0:05.966    1.88
-rotate 270                          0:03.662   0:06.363    1.74
-shade 30x30                         0:04.847   0:13.986    2.89
-sharpen 0x0.5                       0:10.815   0:24.956    2.31
-sharpen 0x1.0                       0:25.466   1:04.602    2.54
-shave 10x10                         0:03.091   0:05.920    1.92
-shear 45x45                         1:10.711   1:58.775    1.68
-solarize 50%                        0:02.388   0:05.853    2.45
-spread 1                            0:03.174  13:06.351  247.75
-spread 3                            0:03.049  13:11.856  259.71
-swirl 90                            0:11.048   0:23.120    2.09
-threshold 35%                       0:02.573   0:06.111    2.38
-fuzz 35% -transparent red           0:02.853   0:06.742    2.36
-trim                                0:04.237   0:06.664    1.57
-unsharp 0x0.5+20+1                  0:11.155   0:31.857    2.86
-unsharp 0x1.0+20+1                  0:13.117   0:37.215    2.84
-wave 25x150                         0:09.968   0:25.263    2.53
-white-threshold 80%                 0:02.303   0:06.109    2.65
================================== ========== ========== ===========


Intel Xeon / FreeBSD 7.0
------------------------

This system is a 2003 vintage Intel Xeon system with two 2.4GHz CPUs and
running FreeBSD 7.0. The compiler used identifies itself as "gcc (GCC)
4.2.1 20070719 [FreeBSD]".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:03.062   0:06.686    2.18
-affine 1,0,0.785,1,0,0 -transform   0:09.984  20:20.902  122.29
-black-threshold 20%                 0:03.370   0:07.191    2.13
-blur 0x0.5                          0:09.223   0:26.028    2.82
-blur 0x1.0                          0:10.488   0:26.883    2.56
-border 6x6                          0:05.745   0:11.386    1.98
-charcoal 0x1                        0:22.435   0:41.492    1.85
-chop 800x600+200+300                0:03.702   0:07.125    1.92
-colorspace CMYK                     0:03.964   0:09.739    2.46
-colorspace GRAY                     0:04.715   0:10.096    2.14
-colorspace HSL                      0:05.981   0:10.400    1.74
-colorspace HWB                      0:05.033   0:09.996    1.99
-colorspace OHTA                     0:04.575   0:10.093    2.21
-colorspace YCbCr                    0:04.604   0:10.022    2.18
-colorspace YIQ                      0:04.644   0:10.090    2.17
-colorspace YUV                      0:04.644   0:10.125    2.18
-contrast -contrast -contrast        0:18.583   0:30.318    1.63
+contrast +contrast +contrast        0:18.469   0:29.994    1.62
-convolve 1,1,1,1,4,1,1,1,1          0:07.019   0:10.939    1.56
-colorize 30%/20%/50%                0:04.886   0:09.859    2.02
-crop 1700x900+100+100               0:05.013   0:07.939    1.58
-despeckle                           0:54.080   3:04.901    3.42
-edge 0x1                            0:07.031   0:14.044    2.00
-emboss 0x1                          0:12.291   0:28.677    2.33
-enhance                             0:31.499   0:32.292    1.03
-equalize                            0:04.789   0:10.343    2.16
-flip                                0:05.759   0:08.425    1.46
-flop                                0:05.685   0:08.386    1.48
-frame 15x15+3+3                     0:05.757   0:11.413    1.98
-gamma 1.6                           0:05.636   0:08.414    1.49
-gaussian 0x0.5                      0:09.874   0:14.185    1.44
-gaussian 0x1.0                      0:19.989   0:23.831    1.19
-implode 0.5                         0:17.646   0:30.169    1.71
-implode -1                          0:12.688   0:25.236    1.99
-lat 10x10-5%                        0:23.205   0:23.264    1.00
-level 10%,1.2,90%                   0:04.053   1:44.927   25.89
-median 1                            1:52.061   1:10.238    0.63
-median 2                            4:21.560   2:57.114    0.68
-modulate 110/100/95                 0:07.556   0:11.125    1.47
-motion-blur 0x3+30                  2:52.798   3:05.292    1.07
-negate                              0:03.449   0:07.057    2.05
+noise Uniform                       0:09.627   1:03.274    6.57
+noise Gaussian                      0:24.818   2:31.631    6.11
+noise Multiplicative                0:18.528   2:13.855    7.22
+noise Impulse                       0:09.630   1:03.560    6.60
+noise Laplacian                     0:12.425   1:16.709    6.17
+noise Poisson                       0:23.689   4:18.182   10.90
-noise 1                             1:52.276   1:09.558    0.62
-noise 2                             4:21.392   2:54.387    0.67
-normalize                           0:04.641   0:11.449    2.47
-fill blue -fuzz 35% -opaque red     0:03.555   0:07.730    2.17
-paint 0x1                           0:12.365   0:32.416    2.62
-raise 10x10                         0:03.193   0:06.878    2.15
-density 75x75 -resample 50x50       0:12.312   0:16.263    1.32
-resize 10%                          0:06.933   0:10.746    1.55
-resize 50%                          0:10.018   0:13.556    1.35
-resize 150%                         0:34.986   0:38.774    1.11
-roll +20+10                         0:05.676   3:26.954   36.46
-rotate 0                            0:05.746   0:08.375    1.46
-rotate 45                           1:16.289   1:19.710    1.04
-rotate 90                           0:04.844   0:08.918    1.84
-rotate 180                          0:05.666   0:08.327    1.47
-rotate 270                          0:04.933   0:08.912    1.81
-shade 30x30                         0:07.066   0:10.716    1.52
-sharpen 0x0.5                       0:09.926   0:14.088    1.42
-sharpen 0x1.0                       0:20.076   0:23.853    1.19
-shave 10x10                         0:05.632   0:08.311    1.48
-shear 45x45                         1:06.502   1:48.034    1.62
-solarize 50%                        0:03.441   0:06.946    2.02
-spread 1                            0:05.491   5:29.590   60.02
-spread 3                            0:05.838   5:25.804   55.81
-swirl 90                            0:12.811   0:24.858    1.94
-threshold 35%                       0:03.977   0:07.195    1.81
-fuzz 35% -transparent red           0:03.591   0:08.098    2.26
-trim                                0:04.534   0:09.102    2.01
-unsharp 0x0.5+20+1                  0:10.476   0:28.267    2.70
-unsharp 0x1.0+20+1                  0:11.669   0:29.142    2.50
-wave 25x150                         0:16.225   0:32.366    1.99
-white-threshold 80%                 0:03.352   0:07.238    2.16
================================== ========== ========== ===========



--------------------------------------------------------------------------

| Copyright (C) 2008 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

