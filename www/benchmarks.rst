.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

==========================================================
GraphicsMagick 1.3.1 vs ImageMagick 6.4.5 Benchmark Report
==========================================================

.. _`GraphicsMagick 1.2 Benchmarks` : benchmarks-1.2.html

.. contents::
  :local:

Introduction
============

GraphicsMagick 1.3.1 has been benchmarked against the most recent
ImageMagick release available at time of release. ImageMagick has
recently been significantly updated to use OpenMP as has GraphicsMagick
1.3.1. Several disparate types of systems were used in our testing. We are
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

The often extreme difference in the results is difficult to believe
until one inspects the source code from both packages and sees that
GraphicsMagick uses streamlined straight-forward code paths while
ImageMagick uses convoluted and arcane code paths in an apparent
attempt to defeat human comprehension. While the ImageMagick approach
does successfully defeat human comprehension, it also seems to largely
defeat C compiler comprehension (and the CPU's cache) as well,
resulting in slow code. If time and money are important to you, then
you should use GraphicsMagick rather than ImageMagick. If time and
money are not important to you, then you should use ImageMagick.

Strategy
========

For this benchmark cycle, we used a completely different benchmark
approach. Instead of testing single-shot performance with large
uncompressed files, we focused on the ability to process many medium
sized (1920x1080 pixels) images. In order to prevent disk I/O from being
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

  GraphicsMagick 1.3.1 unreleased Q16 http://www.GraphicsMagick.org/

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
-noop                                0:00.736   0:01.269    1.72
-affine 1,0,0.785,1,0,0 -transform   0:02.326   4:07.251  106.30
-black-threshold 20%                 0:00.840   0:01.537    1.83
-blur 0x0.5                          0:03.254   0:08.076    2.48
-blur 0x1.0                          0:03.820   0:08.569    2.24
-border 6x6                          0:01.196   0:02.949    2.47
-charcoal 0x1                        0:07.233   0:12.720    1.76
-chop 800x600+200+300                0:00.904   0:01.530    1.69
-colorspace CMYK                     0:00.952   0:02.543    2.67
-colorspace GRAY                     0:01.096   0:02.635    2.40
-colorspace HSL                      0:01.677   0:02.838    1.69
-colorspace HWB                      0:01.368   0:02.587    1.89
-colorspace OHTA                     0:01.077   0:02.645    2.46
-colorspace YCbCr                    0:01.101   0:02.619    2.38
-colorspace YIQ                      0:01.068   0:02.646    2.48
-colorspace YUV                      0:01.091   0:02.639    2.42
-contrast -contrast -contrast        0:07.085   0:08.332    1.18
+contrast +contrast +contrast        0:06.580   0:07.748    1.18
-convolve 1,1,1,1,4,1,1,1,1          0:01.944   0:03.168    1.63
-colorize 30%/20%/50%                0:01.382   0:02.341    1.69
-crop 1700x900+100+100               0:01.088   0:01.952    1.79
-despeckle                           0:12.892   0:39.312    3.05
-edge 0x1                            0:02.848   0:04.363    1.53
-emboss 0x1                          0:03.636   0:09.993    2.75
-enhance                             0:08.157   0:09.324    1.14
-equalize                            0:01.434   0:02.614    1.82
-flip                                0:01.196   0:02.054    1.72
-flop                                0:01.232   0:02.049    1.66
-frame 15x15+3+3                     0:01.243   0:02.932    2.36
-gamma 1.6                           0:01.346   0:01.559    1.16
-gaussian 0x0.5                      0:02.782   0:04.227    1.52
-gaussian 0x1.0                      0:05.635   0:08.219    1.46
-implode 0.5                         0:05.889   0:09.388    1.59
-implode -1                          0:05.230   0:08.784    1.68
-lat 10x10-5%                        0:07.761   0:09.705    1.25
-level 10%,1.2,90%                   0:01.017   0:05.903    5.80
-median 1                            0:34.255   0:33.785    0.99
-median 2                            1:18.256   1:16.800    0.98
-modulate 110/100/95                 0:02.105   0:02.576    1.22
-motion-blur 0x3+30                  1:07.478   1:35.953    1.42
-negate                              0:00.839   0:01.322    1.58
+noise Uniform                       0:04.364   0:33.349    7.64
+noise Gaussian                      0:15.340   1:36.815    6.31
+noise Multiplicative                0:10.494   1:17.915    7.42
+noise Impulse                       0:04.317   0:31.894    7.39
+noise Laplacian                     0:05.898   0:40.838    6.92
+noise Poisson                       0:11.311   2:05.923   11.13
-noise 1                             0:32.742   0:33.759    1.03
-noise 2                             1:14.809   1:16.895    1.03
-normalize                           0:01.320   0:03.012    2.28
-fill blue -fuzz 35% -opaque red     0:00.955   0:01.620    1.70
-paint 0x1                           0:04.342   0:10.323    2.38
-raise 10x10                         0:00.767   0:01.279    1.67
-density 75x75 -resample 50x50       0:02.757   0:04.318    1.57
-resize 10%                          0:01.522   0:02.134    1.40
-resize 50%                          0:02.058   0:03.198    1.55
-resize 150%                         0:08.699   0:12.722    1.46
-roll +20+10                         0:01.229   0:32.646   26.56
-rotate 0                            0:01.216   0:02.051    1.69
-rotate 45                           0:23.668   0:29.266    1.24
-rotate 90                           0:01.673   0:02.637    1.58
-rotate 180                          0:01.211   0:02.032    1.68
-rotate 270                          0:01.687   0:02.568    1.52
-shade 30x30                         0:02.287   0:03.185    1.39
-sharpen 0x0.5                       0:02.903   0:04.282    1.48
-sharpen 0x1.0                       0:05.709   0:08.228    1.44
-shave 10x10                         0:01.219   0:02.167    1.78
-shear 45x45                         0:18.835   0:42.231    2.24
-solarize 50%                        0:00.833   0:01.348    1.62
-spread 1                            0:01.235   1:16.682   62.09
-spread 3                            0:01.362   1:17.973   57.25
-swirl 90                            0:04.990   0:08.702    1.74
-threshold 35%                       0:00.970   0:01.447    1.49
-fuzz 35% -transparent red           0:00.975   0:01.731    1.78
-trim                                0:01.145   0:02.172    1.90
-unsharp 0x0.5+20+1                  0:03.428   0:08.365    2.44
-unsharp 0x1.0+20+1                  0:03.964   0:08.809    2.22
-wave 25x150                         0:05.830   0:11.381    1.95
-white-threshold 80%                 0:00.831   0:01.534    1.85
================================== ========== ========== ===========

Sun SPARC/Solaris 10
--------------------

This system is a 2004 vintage Sun SPARC workstation with two 1.2GHz CPUs
and running Solaris 10 U5. The compiler used identifies itself as "gcc
(GCC) 4.2.4".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:01.568   0:07.589    4.84
-affine 1,0,0.785,1,0,0 -transform   0:11.798   5:15.828   26.77
-black-threshold 20%                 0:02.462   0:09.356    3.80
-blur 0x0.5                          0:11.380   0:33.502    2.94
-blur 0x1.0                          0:14.582   0:36.937    2.53
-border 6x6                          0:02.377   0:18.054    7.60
-charcoal 0x1                        0:34.292   1:09.127    2.02
-chop 800x600+200+300                0:02.013   0:08.168    4.06
-colorspace CMYK                     0:02.410   0:12.921    5.36
-colorspace GRAY                     0:04.102   0:12.813    3.12
-colorspace HSL                      0:07.988   0:14.302    1.79
-colorspace HWB                      0:07.340   0:13.869    1.89
-colorspace OHTA                     0:04.114   0:12.919    3.14
-colorspace YCbCr                    0:04.103   0:12.738    3.10
-colorspace YIQ                      0:04.128   0:12.837    3.11
-colorspace YUV                      0:04.103   0:12.917    3.15
-contrast -contrast -contrast        0:42.407   0:49.965    1.18
+contrast +contrast +contrast        0:40.475   0:48.020    1.19
-convolve 1,1,1,1,4,1,1,1,1          0:09.254   0:19.168    2.07
-colorize 30%/20%/50%                0:04.494   0:12.240    2.72
-crop 1700x900+100+100               0:02.171   0:08.333    3.84
-despeckle                           1:49.368   3:38.388    2.00
-edge 0x1                            0:14.833   0:26.556    1.79
-emboss 0x1                          0:19.484   1:03.558    3.26
-enhance                             0:42.730   0:43.000    1.01
-equalize                            0:04.543   0:14.539    3.20
-flip                                0:02.373   0:08.643    3.64
-flop                                0:03.008   0:09.557    3.18
-frame 15x15+3+3                     0:02.410   0:18.543    7.69
-gamma 1.6                           0:03.939   0:11.092    2.82
-gaussian 0x0.5                      0:15.682   0:27.393    1.75
-gaussian 0x1.0                      0:37.851   0:54.938    1.45
-implode 0.5                         0:22.628   0:42.806    1.89
-implode -1                          0:18.455   0:37.726    2.04
-lat 10x10-5%                        0:39.769   1:36.573    2.43
-level 10%,1.2,90%                   0:02.859   5:20.154  111.98
-median 1                            1:15.361   1:17.136    1.02
-median 2                            2:56.672   2:50.296    0.96
-modulate 110/100/95                 0:13.694   0:20.879    1.52
-motion-blur 0x3+30                  2:21.951   3:18.113    1.40
-negate                              0:02.189   0:08.480    3.87
+noise Uniform                       0:14.847   1:17.781    5.24
+noise Gaussian                      1:03.014   3:42.374    3.53
+noise Multiplicative                0:42.599   2:58.011    4.18
+noise Impulse                       0:15.240   1:18.049    5.12
+noise Laplacian                     0:25.233   1:41.187    4.01
+noise Poisson                       0:46.670   4:52.140    6.26
-noise 1                             1:17.504   1:19.084    1.02
-noise 2                             2:59.061   2:52.232    0.96
-normalize                           0:04.233   0:15.509    3.66
-fill blue -fuzz 35% -opaque red     0:03.507   0:26.551    7.57
-paint 0x1                           0:24.353   0:47.092    1.93
-raise 10x10                         0:01.673   0:07.777    4.65
-density 75x75 -resample 50x50       0:12.631   0:23.225    1.84
-resize 10%                          0:06.733   0:13.767    2.04
-resize 50%                          0:09.798   0:19.219    1.96
-resize 150%                         0:32.331   0:53.710    1.66
-roll +20+10                         0:02.409   0:30.234   12.55
-rotate 0                            0:02.327   0:08.673    3.73
-rotate 45                           0:58.778   1:22.576    1.40
-rotate 90                           0:03.301   0:09.738    2.95
-rotate 180                          0:02.955   0:09.431    3.19
-rotate 270                          0:03.323   0:09.812    2.95
-shade 30x30                         0:10.043   0:17.376    1.73
-sharpen 0x0.5                       0:15.716   0:27.123    1.73
-sharpen 0x1.0                       0:37.969   0:54.999    1.45
-shave 10x10                         0:02.364   0:08.697    3.68
-shear 45x45                         0:34.908   1:19.305    2.27
-solarize 50%                        0:02.201   0:08.897    4.04
-spread 1                            0:03.302   1:19.018   23.93
-spread 3                            0:03.502   1:19.670   22.75
-swirl 90                            0:18.456   0:36.213    1.96
-threshold 35%                       0:02.779   0:09.431    3.39
-fuzz 35% -transparent red           0:03.674   0:11.203    3.05
-trim                                0:05.837   0:09.707    1.66
-unsharp 0x0.5+20+1                  0:13.865   0:38.966    2.81
-unsharp 0x1.0+20+1                  0:16.786   0:42.511    2.53
-wave 25x150                         0:18.974   0:37.838    1.99
-white-threshold 80%                 0:02.486   0:09.429    3.79
================================== ========== ========== ===========

Apple PowerPC G5/OS-X Tiger
---------------------------

This system is an Apple G5 system with two 2.5GHz CPUs and running the
Leopard release of OS-X. The compiler used identifies itself as "gcc-4.3.2
(GCC) 4.3.2".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:02.022   0:04.960    2.45
-affine 1,0,0.785,1,0,0 -transform   0:06.883  30:43.655  267.86
-black-threshold 20%                 0:03.467   0:06.982    2.01
-blur 0x0.5                          0:09.248   0:31.915    3.45
-blur 0x1.0                          0:11.508   0:37.316    3.24
-border 6x6                          0:03.157   0:10.530    3.34
-charcoal 0x1                        0:25.140   1:08.414    2.72
-chop 800x600+200+300                0:02.596   0:05.518    2.13
-colorspace CMYK                     0:02.494   0:08.131    3.26
-colorspace GRAY                     0:03.241   0:08.233    2.54
-colorspace HSL                      0:05.026   0:08.858    1.76
-colorspace HWB                      0:04.638   0:08.688    1.87
-colorspace OHTA                     0:03.206   0:08.276    2.58
-colorspace YCbCr                    0:03.408   0:08.350    2.45
-colorspace YIQ                      0:03.404   0:08.411    2.47
-colorspace YUV                      0:03.297   0:08.340    2.53
-contrast -contrast -contrast        0:16.890   0:27.220    1.61
+contrast +contrast +contrast        0:17.743   0:28.310    1.60
-convolve 1,1,1,1,4,1,1,1,1          0:08.263   0:15.515    1.88
-colorize 30%/20%/50%                0:03.953   0:08.429    2.13
-crop 1700x900+100+100               0:02.604   0:05.811    2.23
-despeckle                           1:21.989   3:04.578    2.25
-edge 0x1                            0:06.411   0:24.952    3.89
-emboss 0x1                          0:12.255   1:09.693    5.69
-enhance                             0:32.050   0:46.376    1.45
-equalize                            0:02.868   0:08.042    2.80
-flip                                0:02.839   0:06.096    2.15
-flop                                0:02.881   0:06.107    2.12
-frame 15x15+3+3                     0:02.864   0:09.594    3.35
-gamma 1.6                           0:02.827   0:06.096    2.16
-gaussian 0x0.5                      0:10.522   0:25.181    2.39
-gaussian 0x1.0                      0:25.330   1:05.189    2.57
-implode 0.5                         0:12.156   0:22.707    1.87
-implode -1                          0:08.586   0:21.671    2.52
-lat 10x10-5%                        0:44.989   1:09.993    1.56
-level 10%,1.2,90%                   0:02.431   0:12.173    5.01
-median 1                            1:09.370   1:22.857    1.19
-median 2                            2:46.354   3:07.653    1.13
-modulate 110/100/95                 0:06.201   0:08.827    1.42
-motion-blur 0x3+30                  1:38.525   2:08.928    1.31
-negate                              0:02.192   0:05.163    2.36
+noise Uniform                       0:08.530   0:58.352    6.84
+noise Gaussian                      0:22.906   2:10.226    5.69
+noise Multiplicative                0:17.390   1:55.859    6.66
+noise Impulse                       0:08.713   0:55.739    6.40
+noise Laplacian                     0:12.599   1:10.342    5.58
+noise Poisson                       0:21.506   3:34.477    9.97
-noise 1                             1:08.805   1:28.986    1.29
-noise 2                             2:39.773   3:15.389    1.22
-normalize                           0:02.765   0:08.914    3.22
-fill blue -fuzz 35% -opaque red     0:02.673   0:06.286    2.35
-paint 0x1                           0:05.731   0:39.947    6.97
-raise 10x10                         0:02.079   0:05.166    2.48
-density 75x75 -resample 50x50       0:10.017   0:21.748    2.17
-resize 10%                          0:04.727   0:11.976    2.53
-resize 50%                          0:07.146   0:15.122    2.12
-resize 150%                         0:35.528   0:47.838    1.35
-roll +20+10                         0:03.078   6:24.426  124.89
-rotate 0                            0:02.782   0:06.047    2.17
-rotate 45                           1:18.419   1:26.977    1.11
-rotate 90                           0:03.360   0:06.426    1.91
-rotate 180                          0:02.825   0:06.059    2.14
-rotate 270                          0:03.363   0:06.521    1.94
-shade 30x30                         0:04.748   0:14.180    2.99
-sharpen 0x0.5                       0:10.534   0:25.122    2.38
-sharpen 0x1.0                       0:25.289   1:05.088    2.57
-shave 10x10                         0:02.809   0:06.005    2.14
-shear 45x45                         1:07.169   1:57.570    1.75
-solarize 50%                        0:02.218   0:05.908    2.66
-spread 1                            0:03.002  12:50.601  256.70
-spread 3                            0:03.122  13:06.375  251.88
-swirl 90                            0:10.412   0:23.170    2.23
-threshold 35%                       0:02.528   0:06.270    2.48
-fuzz 35% -transparent red           0:02.680   0:06.867    2.56
-trim                                0:04.096   0:06.752    1.65
-unsharp 0x0.5+20+1                  0:10.698   0:31.634    2.96
-unsharp 0x1.0+20+1                  0:12.680   0:37.070    2.92
-wave 25x150                         0:09.668   0:25.747    2.66
-white-threshold 80%                 0:02.145   0:06.270    2.92
================================== ========== ========== ===========

Intel Xeon / FreeBSD 7.0
------------------------

This system is a 2003 vintage Intel Xeon system with two 2.4GHz CPUs and
running FreeBSD 7.0. The compiler used identifies itself as "gcc (GCC)
4.2.1 20070719 [FreeBSD]".

================================== ========== ========== ===========
Operation                          GM real    IM real    IM/GM ratio
================================== ========== ========== ===========
-noop                                0:01.575   0:06.729    4.27
-affine 1,0,0.785,1,0,0 -transform   0:08.414  20:26.384  145.76
-black-threshold 20%                 0:01.888   0:07.232    3.83
-blur 0x0.5                          0:07.689   0:25.976    3.38
-blur 0x1.0                          0:08.920   0:26.720    3.00
-border 6x6                          0:02.827   0:11.598    4.10
-charcoal 0x1                        0:20.800   0:41.973    2.02
-chop 800x600+200+300                0:01.895   0:07.300    3.85
-colorspace CMYK                     0:02.563   0:09.861    3.85
-colorspace GRAY                     0:03.146   0:10.237    3.25
-colorspace HSL                      0:04.478   0:10.414    2.33
-colorspace HWB                      0:03.393   0:10.111    2.98
-colorspace OHTA                     0:03.077   0:10.122    3.29
-colorspace YCbCr                    0:03.100   0:10.170    3.28
-colorspace YIQ                      0:03.068   0:10.137    3.30
-colorspace YUV                      0:03.102   0:10.182    3.28
-contrast -contrast -contrast        0:16.936   0:30.461    1.80
+contrast +contrast +contrast        0:16.592   0:30.291    1.83
-convolve 1,1,1,1,4,1,1,1,1          0:05.485   0:11.023    2.01
-colorize 30%/20%/50%                0:03.402   0:09.849    2.90
-crop 1700x900+100+100               0:02.468   0:07.984    3.24
-despeckle                           0:51.741   3:04.871    3.57
-edge 0x1                            0:05.581   0:13.908    2.49
-emboss 0x1                          0:10.720   0:28.741    2.68
-enhance                             0:29.384   0:32.385    1.10
-equalize                            0:03.327   0:10.392    3.12
-flip                                0:02.724   0:08.420    3.09
-flop                                0:02.700   0:08.450    3.13
-frame 15x15+3+3                     0:02.807   0:11.337    4.04
-gamma 1.6                           0:04.138   0:08.447    2.04
-gaussian 0x0.5                      0:08.408   0:14.191    1.69
-gaussian 0x1.0                      0:18.204   0:24.125    1.33
-implode 0.5                         0:15.799   0:30.206    1.91
-implode -1                          0:10.734   0:25.254    2.35
-lat 10x10-5%                        0:21.068   0:23.295    1.11
-level 10%,1.2,90%                   0:02.576   1:45.138   40.81
-median 1                            1:57.267   1:09.904    0.60
-median 2                            4:32.584   2:56.154    0.65
-modulate 110/100/95                 0:05.965   0:11.339    1.90
-motion-blur 0x3+30                  3:02.254   3:03.829    1.01
-negate                              0:01.975   0:07.151    3.62
+noise Uniform                       0:08.031   1:03.146    7.86
+noise Gaussian                      0:23.215   2:30.842    6.50
+noise Multiplicative                0:17.001   2:12.535    7.80
+noise Impulse                       0:08.029   1:01.383    7.65
+noise Laplacian                     0:10.925   1:16.020    6.96
+noise Poisson                       0:22.518   4:15.572   11.35
-noise 1                             1:58.910   1:08.945    0.58
-noise 2                             4:35.641   2:54.957    0.63
-normalize                           0:03.166   0:11.422    3.61
-fill blue -fuzz 35% -opaque red     0:02.056   0:07.857    3.82
-paint 0x1                           0:10.874   0:32.299    2.97
-raise 10x10                         0:01.656   0:06.795    4.10
-density 75x75 -resample 50x50       0:10.816   0:16.277    1.50
-resize 10%                          0:05.437   0:10.868    2.00
-resize 50%                          0:08.403   0:13.642    1.62
-resize 150%                         0:32.995   0:38.774    1.18
-roll +20+10                         0:04.223   3:27.736   49.19
-rotate 0                            0:02.766   0:08.454    3.06
-rotate 45                           1:01.114   1:19.892    1.31
-rotate 90                           0:03.405   0:08.957    2.63
-rotate 180                          0:02.693   0:08.437    3.13
-rotate 270                          0:03.314   0:08.944    2.70
-shade 30x30                         0:05.761   0:10.767    1.87
-sharpen 0x0.5                       0:08.406   0:14.197    1.69
-sharpen 0x1.0                       0:18.233   0:24.260    1.33
-shave 10x10                         0:02.713   0:08.369    3.08
-shear 45x45                         0:51.964   1:48.776    2.09
-solarize 50%                        0:01.968   0:06.992    3.55
-spread 1                            0:03.109   5:34.925  107.73
-spread 3                            0:03.075   5:31.330  107.75
-swirl 90                            0:10.836   0:25.069    2.31
-threshold 35%                       0:02.467   0:07.273    2.95
-fuzz 35% -transparent red           0:02.099   0:08.221    3.92
-trim                                0:03.767   0:09.037    2.40
-unsharp 0x0.5+20+1                  0:08.806   0:28.404    3.23
-unsharp 0x1.0+20+1                  0:10.089   0:29.624    2.94
-wave 25x150                         0:13.477   0:32.566    2.42
-white-threshold 80%                 0:01.798   0:07.193    4.00
================================== ========== ========== ===========


--------------------------------------------------------------------------

| Copyright (C) 2008 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

