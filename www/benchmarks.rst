.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

==========================================================
GraphicsMagick 1.3.7 vs ImageMagick 6.5.6 Benchmark Report
==========================================================

.. _`GraphicsMagick 1.2 Benchmarks` : benchmarks-1.2.html

.. contents::
  :local:

Introduction
============

GraphicsMagick 1.3.7 has been benchmarked against the most recent
ImageMagick release available at time of release. ImageMagick has been
significantly updated to use OpenMP as has GraphicsMagick
1.3.7. Several disparate types of systems were used in our testing. We
are pleased to see that ImageMagick's performance has improved
significantly on the benchmark that we posted earlier (see
`GraphicsMagick 1.2 Benchmarks`_) but also notice that in recent
months, ImageMagick performance has taken a serious nose-dive with
these newer benchmarks.

Executive Summary
=================

GraphicsMagick is typically 1.5 to 5 times faster at executing image
processing operations from the command line than ImageMagick 6.5.6 is.
Some ImageMagick algorithms run as much as 107 times
slower. ImageMagick 6.5.6 only runs adequately on the latest high-end
hardware with large caches, and runs poorly on most of the installed
computing base. Even on the latest high-end hardware, GraphicsMagick
is much more efficient and uses much less CPU than ImageMagick.

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

Since the 1.2.X benchmark cycle, we used a completely different
benchmark approach. Instead of testing single-shot performance with
large uncompressed files, we focused on the ability to process many
medium sized HD-resolution (1920x1080 pixels) images. In order to
prevent disk I/O from being a factor, we used a small input image and
tiled it to create a larger input image via the "tile:" coder. The
processed image is sent to the "null:" coder so that file writes to a
slow disk are also not a factor.  Static executables are used and
executed via full paths in order to minimize variability from the
execution environment. In order to obtain accurate and useful timing,
we used the bash shell to execute the command 40 times and see how
long it took. This is a very simple benchmark approach which is quite
representative of the performance that the typical user observes.

This new benchmark reveals the following performance criteria:

  * Basic execution overhead of the software.
  
  * Image processing algorithmic efficiency.
  
  * Efficacy of OpenMP enhancements.

The software involved in the testing identified itself as::

  GraphicsMagick 1.3.7 2009-09-17 Q16 http://www.GraphicsMagick.org/

  ImageMagick 6.5.6-1 2009-09-14 Q16 OpenMP http://www.imagemagick.org

The Benchmark
=============

The benchmark is quite simple. It reads a list of commands to execute
from a file named "commands.txt" and times how long it takes to
execute each command 40 times using GraphicsMagick and ImageMagick.

Here is the simple benchmark script::

  #!/usr/bin/env bash
  #
  # Measure the performance between two 'convert' commands by
  # executing a subcommand through many iterations and seeing
  # the total time that it takes.
  #
  # Written by Bob Friesenhahn, October 2008
  #

  # GraphicsMagick
  convert1='/usr/local/bin/gm convert'
  #convert1='/c/Program\ Files/GraphicsMagick-1.3.7-Q16/gm.exe convert'

  # ImageMagick
  convert2='/usr/local/bin/convert'
  #convert2='/c/Program\ Files/ImageMagick-6.5.6-Q16/convert.exe'

  # Input file specification
  input_image='-size 1920x1080 tile:model.pnm'

  # Ouput file specification
  output_image="null:"

  # Should not need to change any of the rest
  typeset -i iterations=40
  echo "Convert-1:   ${convert1}"
  echo "Version:     `eval "${convert1}" -version | head -1`"
  echo "Convert-2:   ${convert2}"
  echo "Version:     `eval "${convert2}" -version | head -1`"
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
  typeset -i count=0 i=0
  cat commands.txt | while read subcommand
  do
    echo ${subcommand}
  
    command1="${convert1} ${input_image} ${subcommand} ${output_image}"
    i=0
    count=$iterations
    time while ((i < count))
    do
      eval "${command1}"
      let i=i+1
    done
    sleep 1
  
    command2="${convert2} ${input_image} ${subcommand} ${output_image}"
    i=0
    count=$iterations
    time while ((i < count))
    do
      eval "${command2}"
      let i=i+1
    done
  
    echo
    sleep 1
  done 2>&1


Test Results
============

The test results are expressed as the image processing operation
performed, the amount of wall-clock time it took for GraphicsMagick to
execute it fourty times, the amount of wall-clock time it took for
ImageMagick to execute it fourty times, and the ImageMagick time
divided by the GraphicsMagick time. In order to understand the "IM/GM
ratio" column, a value of 0.50 indicates that GM is half as fast as
IM, a value of 1.00 indicates that GM and IM are the same speed, and a
value of 4.00 indicates that GM is four times as fast as IM.

AMD Opteron/Solaris 10
----------------------

This system is a modern AMD Opteron system with two dual-core 3.0GHz
CPUs and running Solaris 10 U7. The compiler used identifies itself as
"gcc (GCC) 4.3.4".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:01.233   0:04.262    3.46
-affine 1,0,0.785,1,0,0 -transform               0:08.229  13:48.194  100.64
-black-threshold 20%                             0:01.372   0:04.582    3.34
-blur 0x0.5                                      0:05.940   0:16.764    2.82
-blur 0x1.0                                      0:07.007   0:15.738    2.25
-border 6x6                                      0:01.897   0:06.521    3.44
-charcoal 0x1                                    0:13.339   0:26.571    1.99
-chop 800x600+200+300                            0:01.350   0:04.875    3.61
-colorspace CMYK                                 0:01.606   0:06.703    4.17
-colorspace GRAY                                 0:01.833   0:05.516    3.01
-colorspace HSL                                  0:03.302   0:06.885    2.09
-colorspace HWB                                  0:02.436   0:06.561    2.69
-colorspace OHTA                                 0:01.798   0:06.123    3.41
-colorspace YCbCr                                0:01.895   0:06.081    3.21
-colorspace YIQ                                  0:01.829   0:06.100    3.34
-colorspace YUV                                  0:01.915   0:06.068    3.17
-contrast -contrast -contrast                    0:14.605   0:17.562    1.20
+contrast +contrast +contrast                    0:12.724   0:16.606    1.31
-convolve 1,1,1,1,4,1,1,1,1                      0:03.216   0:07.679    2.39
-colorize 30%/20%/50%                            0:02.423   0:07.581    3.13
-crop 1700x900+100+100                           0:01.815   0:05.024    2.77
-despeckle                                       0:24.908   0:28.355    1.14
-edge 0x1                                        0:04.734   0:08.915    1.88
-emboss 0x1                                      0:06.260   0:16.705    2.67
-enhance                                         0:16.463   0:17.194    1.04
-equalize                                        0:02.470   0:07.025    2.84
-flip                                            0:01.856   0:05.172    2.79
-flop                                            0:01.996   0:05.121    2.57
-frame 15x15+3+3                                 0:01.994   0:06.513    3.27
-gamma 1.6                                       0:01.708   0:04.722    2.76
-gaussian 0x0.5                                  0:04.515   0:08.623    1.91
-gaussian 0x1.0                                  0:08.963   0:13.156    1.47
-implode 0.5                                     0:09.440   0:17.120    1.81
-implode -1                                      0:08.128   0:15.805    1.94
-lat 10x10-5%                                    0:12.365   0:18.592    1.50
-level 10%,1.2,90%                               0:01.712   0:12.768    7.46
-median 1                                        0:27.615   1:12.249    2.62
-median 2                                        1:08.010   2:33.849    2.26
-modulate 110/100/95                             0:03.781   0:06.759    1.79
-motion-blur 0x3+30                              0:41.481   1:44.971    2.53
-negate                                          0:01.368   0:04.401    3.22
+noise Uniform                                   0:05.299   0:14.600    2.76
+noise Gaussian                                  0:24.543   1:26.481    3.52
+noise Multiplicative                            0:16.805   0:59.392    3.53
+noise Impulse                                   0:05.346   0:13.012    2.43
+noise Laplacian                                 0:08.995   0:32.154    3.57
+noise Poisson                                   0:12.247   0:43.624    3.56
-noise 1                                         0:27.795   1:13.963    2.66
-noise 2                                         1:09.931   2:37.267    2.25
-normalize                                       0:02.298   0:06.534    2.84
-fill blue -fuzz 35% -opaque red                 0:01.433   0:05.046    3.52
-paint 0x1                                       0:08.509   0:15.816    1.86
-raise 10x10                                     0:01.202   0:04.528    3.77
-density 75x75 -resample 50x50                   0:04.990   0:12.155    2.44
-recolor '1,0,0,0,1,0,0,0,1'                     0:01.140   0:05.442    4.77
-recolor '0,0,1,0,1,0,1,0,0'                     0:01.656   0:05.367    3.24
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:01.849   0:05.487    2.97
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:01.778   0:05.575    3.14
-resize 10%                                      0:02.822   0:05.868    2.08
-resize 50%                                      0:03.534   0:07.286    2.06
-resize 150%                                     0:13.905   0:17.756    1.28
-roll +20+10                                     0:01.940   0:05.111    2.63
-rotate 0                                        0:01.109   0:04.339    3.91
-rotate 15                                       0:12.461   0:26.556    2.13
-rotate 45                                       0:28.965   0:39.220    1.35
-rotate 90                                       0:02.920   0:05.943    2.04
-rotate 180                                      0:02.029   0:05.187    2.56
-rotate 270                                      0:02.892   0:05.908    2.04
-shade 30x30                                     0:04.166   0:07.384    1.77
-sharpen 0x0.5                                   0:04.624   0:09.264    2.00
-sharpen 0x1.0                                   0:08.969   0:13.045    1.45
-shave 10x10                                     0:01.970   0:05.040    2.56
-shear 45x45                                     0:20.950   0:45.894    2.19
-solarize 50%                                    0:01.403   0:04.539    3.24
-spread 1                                        0:01.990   0:18.967    9.53
-spread 3                                        0:02.157   0:19.115    8.86
-swirl 90                                        0:07.613   0:16.016    2.10
-threshold 35%                                   0:01.625   0:04.603    2.83
-fuzz 35% -transparent red                       0:01.579   0:05.162    3.27
-trim                                            0:01.814   0:05.303    2.92
-unsharp 0x0.5+20+1                              0:06.629   0:16.876    2.55
-unsharp 0x1.0+20+1                              0:07.651   0:16.124    2.11
-wave 25x150                                     0:12.528   0:22.431    1.79
-white-threshold 80%                             0:01.353   0:04.642    3.43
============================================== ========== ========== ===========

Sun SPARC/Solaris 10
--------------------

This system is a 2004 vintage Sun SPARC workstation with two 1.2GHz
CPUs and running Solaris 10 U7. The compiler used identifies itself as
"gcc (GCC) 4.3.4".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:02.726   0:19.921    7.31
-affine 1,0,0.785,1,0,0 -transform               0:47.014  16:55.122   21.59
-black-threshold 20%                             0:04.761   0:22.841    4.80
-blur 0x0.5                                      0:23.663   1:01.740    2.61
-blur 0x1.0                                      0:29.685   1:07.337    2.27
-border 6x6                                      0:04.596   0:35.221    7.66
-charcoal 0x1                                    1:08.103   2:01.424    1.78
-chop 800x600+200+300                            0:03.840   0:21.388    5.57
-colorspace CMYK                                 0:05.004   0:29.290    5.85
-colorspace GRAY                                 0:08.014   0:26.471    3.30
-colorspace HSL                                  0:15.451   0:32.282    2.09
-colorspace HWB                                  0:13.790   0:32.128    2.33
-colorspace OHTA                                 0:07.950   0:27.762    3.49
-colorspace YCbCr                                0:08.221   0:27.884    3.39
-colorspace YIQ                                  0:08.006   0:27.524    3.44
-colorspace YUV                                  0:07.989   0:27.575    3.45
-contrast -contrast -contrast                    1:32.942   1:43.789    1.12
+contrast +contrast +contrast                    1:28.005   1:40.819    1.15
-convolve 1,1,1,1,4,1,1,1,1                      0:17.428   0:37.910    2.18
-colorize 30%/20%/50%                            0:08.830   0:32.052    3.63
-crop 1700x900+100+100                           0:04.147   0:22.446    5.41
-despeckle                                       3:03.554   4:08.218    1.35
-edge 0x1                                        0:28.116   0:46.252    1.65
-emboss 0x1                                      0:37.765   1:41.498    2.69
-enhance                                         1:27.306   1:23.153    0.95
-equalize                                        0:09.716   0:34.199    3.52
-flip                                            0:04.583   0:21.661    4.73
-flop                                            0:05.833   0:22.907    3.93
-frame 15x15+3+3                                 0:04.874   0:40.164    8.24
-gamma 1.6                                       0:06.800   0:22.626    3.33
-gaussian 0x0.5                                  0:30.110   0:48.680    1.62
-gaussian 0x1.0                                  1:14.885   1:26.188    1.15
-implode 0.5                                     0:46.630   1:26.249    1.85
-implode -1                                      0:37.939   1:17.315    2.04
-lat 10x10-5%                                    1:13.676   1:46.493    1.45
-level 10%,1.2,90%                               0:05.976  10:45.365  107.99
-median 1                                        1:46.913   3:03.629    1.72
-median 2                                        4:20.544   6:27.235    1.49
-modulate 110/100/95                             0:26.812   0:45.011    1.68
-motion-blur 0x3+30                              2:07.073   5:56.311    2.80
-negate                                          0:04.204   0:21.584    5.13
+noise Uniform                                   0:24.886   1:00.304    2.42
+noise Gaussian                                  1:57.531   4:07.297    2.10
+noise Multiplicative                            1:18.396   2:47.363    2.13
+noise Impulse                                   0:25.674   1:01.363    2.39
+noise Laplacian                                 0:45.174   1:47.453    2.38
+noise Poisson                                   1:14.742   2:35.050    2.07
-noise 1                                         1:51.628   3:07.612    1.68
-noise 2                                         4:29.597   6:30.426    1.45
-normalize                                       0:08.771   0:33.981    3.87
-fill blue -fuzz 35% -opaque red                 0:07.015   0:25.463    3.63
-paint 0x1                                       0:50.787   1:11.228    1.40
-raise 10x10                                     0:02.952   0:20.244    6.86
-density 75x75 -resample 50x50                   0:26.186   0:50.739    1.94
-recolor '1,0,0,0,1,0,0,0,1'                     0:02.726   0:29.296   10.75
-recolor '0,0,1,0,1,0,1,0,0'                     0:09.072   0:29.579    3.26
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:10.514   0:29.036    2.76
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:10.629   0:29.067    2.73
-resize 10%                                      0:13.924   0:32.420    2.33
-resize 50%                                      0:20.804   0:40.010    1.92
-resize 150%                                     1:03.759   1:21.577    1.28
-roll +20+10                                     0:04.627   0:23.094    4.99
-rotate 0                                        0:02.733   0:19.941    7.30
-rotate 15                                       0:56.017   1:52.949    2.02
-rotate 45                                       1:55.895   2:30.948    1.30
-rotate 90                                       0:06.925   0:23.835    3.44
-rotate 180                                      0:05.834   0:22.933    3.93
-rotate 270                                      0:06.966   0:23.876    3.43
-shade 30x30                                     0:20.671   0:36.207    1.75
-sharpen 0x0.5                                   0:29.778   0:48.072    1.61
-sharpen 0x1.0                                   1:14.540   1:26.616    1.16
-shave 10x10                                     0:04.516   0:21.726    4.81
-shear 45x45                                     1:07.862   2:25.320    2.14
-solarize 50%                                    0:04.304   0:22.645    5.26
-spread 1                                        0:06.298   1:18.131   12.41
-spread 3                                        0:06.716   1:18.527   11.69
-swirl 90                                        0:38.145   1:16.659    2.01
-threshold 35%                                   0:05.527   0:22.565    4.08
-fuzz 35% -transparent red                       0:07.201   0:26.807    3.72
-trim                                            0:11.584   0:23.568    2.03
-unsharp 0x0.5+20+1                              0:30.119   1:11.243    2.37
-unsharp 0x1.0+20+1                              0:35.591   1:17.194    2.17
-wave 25x150                                     0:38.944   1:18.420    2.01
-white-threshold 80%                             0:04.614   0:22.792    4.94
============================================== ========== ========== ===========

Apple PowerPC G5/OS-X Tiger
---------------------------

This system is an Apple G5 system with two 2.5GHz CPUs and running the
Leopard release of OS-X. The compiler used identifies itself as
"gcc-4.4.0 (GCC) 4.4.0".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:04.046   0:11.535    2.85
-affine 1,0,0.785,1,0,0 -transform               0:26.568   8:14.180   18.60
-black-threshold 20%                             0:04.484   0:13.801    3.08
-blur 0x0.5                                      0:28.426   0:57.533    2.02
-blur 0x1.0                                      0:39.727   1:06.861    1.68
-border 6x6                                      0:05.828   0:18.694    3.21
-charcoal 0x1                                    1:15.546   1:54.882    1.52
-chop 800x600+200+300                            0:04.597   0:12.140    2.64
-colorspace CMYK                                 0:05.006   0:17.208    3.44
-colorspace GRAY                                 0:06.465   0:14.524    2.25
-colorspace HSL                                  0:09.611   0:18.152    1.89
-colorspace HWB                                  0:09.582   0:17.982    1.88
-colorspace OHTA                                 0:06.503   0:16.371    2.52
-colorspace YCbCr                                0:06.512   0:16.264    2.50
-colorspace YIQ                                  0:06.485   0:16.318    2.52
-colorspace YUV                                  0:06.519   0:16.285    2.50
-contrast -contrast -contrast                    0:29.941   0:47.248    1.58
+contrast +contrast +contrast                    0:29.419   0:46.732    1.59
-convolve 1,1,1,1,4,1,1,1,1                      0:17.589   0:26.473    1.51
-colorize 30%/20%/50%                            0:07.004   0:23.465    3.35
-crop 1700x900+100+100                           0:05.306   0:12.997    2.45
-despeckle                                       3:07.100   3:13.137    1.03
-edge 0x1                                        0:17.687   0:46.779    2.64
-emboss 0x1                                      0:36.983   2:07.395    3.44
-enhance                                         1:02.686   1:19.160    1.26
-equalize                                        0:05.776   0:15.874    2.75
-flip                                            0:05.870   0:13.515    2.30
-flop                                            0:05.843   0:13.711    2.35
-frame 15x15+3+3                                 0:05.821   0:19.936    3.42
-gamma 1.6                                       0:05.340   0:12.174    2.28
-gaussian 0x0.5                                  0:33.276   0:47.120    1.42
-gaussian 0x1.0                                  1:27.306   1:59.602    1.37
-implode 0.5                                     0:22.295   0:47.518    2.13
-implode -1                                      0:18.401   0:43.679    2.37
-lat 10x10-5%                                    1:33.301   2:52.486    1.85
-level 10%,1.2,90%                               0:05.034   0:25.972    5.16
-median 1                                        1:02.092   1:51.250    1.79
-median 2                                        2:29.759   4:04.222    1.63
-modulate 110/100/95                             0:11.978   0:18.763    1.57
-motion-blur 0x3+30                              2:01.304   4:19.051    2.14
-negate                                          0:04.529   0:11.923    2.63
+noise Uniform                                   0:14.197   0:32.176    2.27
+noise Gaussian                                  0:42.626   1:33.405    2.19
+noise Multiplicative                            0:29.995   1:05.781    2.19
+noise Impulse                                   0:14.188   0:28.262    1.99
+noise Laplacian                                 0:22.468   0:53.045    2.36
+noise Poisson                                   0:28.520   1:12.433    2.54
-noise 1                                         1:04.815   2:48.166    2.59
-noise 2                                         2:38.154   6:22.876    2.42
-normalize                                       0:05.601   0:15.753    2.81
-fill blue -fuzz 35% -opaque red                 0:05.136   0:13.796    2.69
-paint 0x1                                       0:12.218   0:28.187    2.31
-raise 10x10                                     0:04.236   0:12.056    2.85
-density 75x75 -resample 50x50                   0:25.610   0:44.076    1.72
-recolor '1,0,0,0,1,0,0,0,1'                     0:04.117   0:15.524    3.77
-recolor '0,0,1,0,1,0,1,0,0'                     0:08.270   0:15.167    1.83
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:08.765   0:15.248    1.74
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:08.852   0:15.197    1.72
-resize 10%                                      0:12.772   0:23.112    1.81
-resize 50%                                      0:18.870   0:32.177    1.71
-resize 150%                                     1:22.337   1:18.413    0.95
-roll +20+10                                     0:06.205   0:13.820    2.23
-rotate 0                                        0:04.134   0:11.441    2.77
-rotate 15                                       0:52.266   1:47.691    2.06
-rotate 45                                       2:42.146   2:54.539    1.08
-rotate 90                                       0:08.277   0:16.109    1.95
-rotate 180                                      0:05.841   0:13.697    2.34
-rotate 270                                      0:08.273   0:16.109    1.95
-shade 30x30                                     0:09.365   0:28.269    3.02
-sharpen 0x0.5                                   0:33.267   0:47.248    1.42
-sharpen 0x1.0                                   1:27.577   1:59.689    1.37
-shave 10x10                                     0:05.719   0:13.519    2.36
-shear 45x45                                     2:20.366   3:57.406    1.69
-solarize 50%                                    0:04.562   0:13.620    2.99
-spread 1                                        0:06.180   0:54.340    8.79
-spread 3                                        0:06.554   0:55.006    8.39
-swirl 90                                        0:20.750   0:44.901    2.16
-threshold 35%                                   0:05.278   0:12.354    2.34
-fuzz 35% -transparent red                       0:05.207   0:14.686    2.82
-trim                                            0:08.357   0:15.058    1.80
-unsharp 0x0.5+20+1                              0:32.321   1:04.183    1.99
-unsharp 0x1.0+20+1                              0:43.134   1:14.151    1.72
-wave 25x150                                     0:19.622   0:54.330    2.77
-white-threshold 80%                             0:04.425   0:13.510    3.05
============================================== ========== ========== ===========

Intel Xeon / FreeBSD 7.0
------------------------

This system is a 2003 vintage Intel Xeon system with two 2.4GHz CPUs
and running FreeBSD 7.2. The compiler used identifies itself as "gcc
(GCC) 4.2.1 20070719 [FreeBSD]".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:03.251   0:14.314    4.40
-affine 1,0,0.785,1,0,0 -transform               0:29.349   9:13.637   18.86
-black-threshold 20%                             0:04.021   0:15.252    3.79
-blur 0x0.5                                      0:25.975   0:51.626    1.99
-blur 0x1.0                                      0:28.254   0:52.647    1.86
-border 6x6                                      0:05.810   0:25.175    4.33
-charcoal 0x1                                    0:51.488   1:24.116    1.63
-chop 800x600+200+300                            0:04.015   0:15.110    3.76
-colorspace CMYK                                 0:05.477   0:20.470    3.74
-colorspace GRAY                                 0:06.574   0:20.667    3.14
-colorspace HSL                                  0:09.505   0:21.925    2.31
-colorspace HWB                                  0:07.227   0:21.248    2.94
-colorspace OHTA                                 0:06.510   0:23.004    3.53
-colorspace YCbCr                                0:06.584   0:23.220    3.53
-colorspace YIQ                                  0:06.532   0:22.786    3.49
-colorspace YUV                                  0:06.524   0:22.812    3.50
-contrast -contrast -contrast                    0:34.332   1:01.471    1.79
+contrast +contrast +contrast                    0:33.642   0:59.341    1.76
-convolve 1,1,1,1,4,1,1,1,1                      0:11.292   0:22.176    1.96
-colorize 30%/20%/50%                            0:07.362   0:25.254    3.43
-crop 1700x900+100+100                           0:05.097   0:18.411    3.61
-despeckle                                       1:51.840   2:44.996    1.48
-edge 0x1                                        0:11.290   0:25.808    2.29
-emboss 0x1                                      0:21.763   0:51.035    2.35
-enhance                                         1:01.658   1:04.914    1.05
-equalize                                        0:07.072   0:22.127    3.13
-flip                                            0:05.675   0:19.972    3.52
-flop                                            0:05.646   0:19.566    3.47
-frame 15x15+3+3                                 0:05.839   0:24.860    4.26
-gamma 1.6                                       0:05.800   0:15.804    2.72
-gaussian 0x0.5                                  0:16.799   0:26.519    1.58
-gaussian 0x1.0                                  0:35.049   0:41.271    1.18
-implode 0.5                                     0:32.494   0:58.465    1.80
-implode -1                                      0:20.606   0:46.632    2.26
-lat 10x10-5%                                    0:41.122   0:47.378    1.15
-level 10%,1.2,90%                               0:05.456   3:31.813   38.82
-median 1                                        1:47.411   3:07.590    1.75
-median 2                                        4:43.409   7:37.138    1.61
-modulate 110/100/95                             0:11.237   0:23.307    2.07
-motion-blur 0x3+30                              3:37.289   3:31.539    0.97
-negate                                          0:04.187   0:15.236    3.64
+noise Uniform                                   0:16.352   0:52.892    3.23
+noise Gaussian                                  0:43.827   2:14.221    3.06
+noise Multiplicative                            0:33.780   1:34.087    2.79
+noise Impulse                                   0:15.949   0:51.279    3.22
+noise Laplacian                                 0:21.754   1:07.490    3.10
+noise Poisson                                   0:37.187   2:30.774    4.05
-noise 1                                         1:46.405   3:06.780    1.76
-noise 2                                         4:41.027   7:34.807    1.62
-normalize                                       0:06.831   0:22.319    3.27
-fill blue -fuzz 35% -opaque red                 0:04.374   0:16.837    3.85
-paint 0x1                                       0:22.624   0:55.343    2.45
-raise 10x10                                     0:03.489   0:14.683    4.21
-density 75x75 -resample 50x50                   0:23.665   0:39.634    1.67
-recolor '1,0,0,0,1,0,0,0,1'                     0:03.329   0:19.232    5.78
-recolor '0,0,1,0,1,0,1,0,0'                     0:05.052   0:19.238    3.81
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:05.490   0:19.371    3.53
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:05.486   0:19.057    3.47
-resize 10%                                      0:11.593   0:22.681    1.96
-resize 50%                                      0:18.214   0:29.136    1.60
-resize 150%                                     1:10.661   1:05.345    0.92
-roll +20+10                                     0:08.237   0:19.277    2.34
-rotate 0                                        0:03.337   0:14.469    4.34
-rotate 15                                       0:45.706   1:43.766    2.27
-rotate 45                                       2:02.318   2:38.752    1.30
-rotate 90                                       0:12.165   0:28.594    2.35
-rotate 180                                      0:05.622   0:19.601    3.49
-rotate 270                                      0:11.788   0:28.707    2.44
-shade 30x30                                     0:12.189   0:21.893    1.80
-sharpen 0x0.5                                   0:16.677   0:26.561    1.59
-sharpen 0x1.0                                   0:34.907   0:41.158    1.18
-shave 10x10                                     0:05.698   0:19.918    3.50
-shear 45x45                                     1:42.162   3:33.090    2.09
-solarize 50%                                    0:04.194   0:15.056    3.59
-spread 1                                        0:06.559   0:52.668    8.03
-spread 3                                        0:06.547   0:53.645    8.19
-swirl 90                                        0:20.989   0:47.446    2.26
-threshold 35%                                   0:05.221   0:15.314    2.93
-fuzz 35% -transparent red                       0:04.465   0:17.365    3.89
-trim                                            0:06.451   0:21.426    3.32
-unsharp 0x0.5+20+1                              0:27.754   0:56.153    2.02
-unsharp 0x1.0+20+1                              0:30.291   0:57.516    1.90
-wave 25x150                                     0:25.142   0:55.832    2.22
-white-threshold 80%                             0:03.808   0:15.072    3.96
============================================== ========== ========== ===========

Windows XP / MSVC / Intel Core 2 Quad
-------------------------------------

For this test, the GraphicsMagick Windows distribution packages were
used.  Both distribution packages included support for OpenMP.  The
Winodws system is Windows XP with a 2.83 GHz Core 2 Quad Processor
(Q9550).

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:01.812   0:08.782    4.85
-affine 1,0,0.785,1,0,0 -transform               0:10.188   2:46.861   16.38
-black-threshold 20%                             0:02.078   0:08.969    4.32
-blur 0x0.5                                      0:05.703   0:15.000    2.63
-blur 0x1.0                                      0:06.313   0:15.672    2.48
-border 6x6                                      0:02.390   0:12.000    5.02
-charcoal 0x1                                    0:13.547   0:25.251    1.86
-chop 800x600+200+300                            0:02.047   0:08.844    4.32
-colorspace CMYK                                 0:02.203   0:10.469    4.75
-colorspace GRAY                                 0:03.407   0:10.735    3.15
-colorspace HSL                                  0:03.735   0:10.828    2.90
-colorspace HWB                                  0:03.187   0:10.672    3.35
-colorspace OHTA                                 0:03.422   0:11.062    3.23
-colorspace YCbCr                                0:03.391   0:11.078    3.27
-colorspace YIQ                                  0:03.391   0:11.063    3.26
-colorspace YUV                                  0:03.390   0:11.234    3.31
-contrast -contrast -contrast                    0:11.062   0:19.766    1.79
+contrast +contrast +contrast                    0:11.079   0:19.984    1.80
-convolve 1,1,1,1,4,1,1,1,1                      0:03.890   0:11.266    2.90
-colorize 30%/20%/50%                            0:02.938   0:12.751    4.34
-crop 1700x900+100+100                           0:02.235   0:09.844    4.40
-despeckle                                       0:46.375   0:54.328    1.17
-edge 0x1                                        0:05.656   0:12.859    2.27
-emboss 0x1                                      0:06.453   0:19.406    3.01
-enhance                                         0:15.312   0:20.375    1.33
-equalize                                        0:02.656   0:10.578    3.98
-flip                                            0:02.406   0:10.390    4.32
-flop                                            0:02.375   0:10.406    4.38
-frame 15x15+3+3                                 0:02.406   0:11.297    4.70
-gamma 1.6                                       0:02.282   0:09.032    3.96
-gaussian 0x0.5                                  0:05.422   0:12.719    2.35
-gaussian 0x1.0                                  0:10.500   0:16.812    1.60
-implode 0.5                                     0:07.047   0:17.204    2.44
-implode -1                                      0:07.140   0:17.188    2.41
-lat 10x10-5%                                    0:10.109   0:19.579    1.94
-level 10%,1.2,90%                               0:02.281   0:35.688   15.65
-median 1                                        0:27.234   2:10.001    4.77
-median 2                                        1:10.438   5:13.861    4.46
-modulate 110/100/95                             0:04.079   0:11.172    2.74
-motion-blur 0x3+30                              1:10.188   1:08.953    0.98
-negate                                          0:02.125   0:09.000    4.24
+noise Uniform                                   0:04.172   0:21.344    5.12
+noise Gaussian                                  0:13.375   1:02.938    4.71
+noise Multiplicative                            0:10.047   0:46.047    4.58
+noise Impulse                                   0:04.328   0:19.406    4.48
+noise Laplacian                                 0:06.438   0:32.532    5.05
+noise Poisson                                   0:09.297   0:43.329    4.66
-noise 1                                         0:27.078   2:10.829    4.83
-noise 2                                         1:11.766   4:43.486    3.95
-normalize                                       0:02.594   0:10.672    4.11
-fill blue -fuzz 35% -opaque red                 0:05.844   0:13.563    2.32
-paint 0x1                                       0:09.515   0:17.406    1.83
-raise 10x10                                     0:01.891   0:09.156    4.84
-density 75x75 -resample 50x50                   0:05.343   0:13.422    2.51
-recolor '1,0,0,0,1,0,0,0,1'                     0:01.781   0:09.828    5.52
-recolor '0,0,1,0,1,0,1,0,0'                     0:02.641   0:09.750    3.69
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:02.891   0:09.735    3.37
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:02.875   0:09.750    3.39
-resize 10%                                      0:02.969   0:10.015    3.37
-resize 50%                                      0:04.391   0:11.906    2.71
-resize 150%                                     0:10.640   0:18.079    1.70
-roll +20+10                                     0:03.235   0:10.172    3.14
-rotate 0                                        0:01.797   0:08.875    4.94
-rotate 15                                       0:10.016   0:33.687    3.36
-rotate 45                                       0:21.016   0:46.751    2.22
-rotate 90                                       0:02.579   0:12.016    4.66
-rotate 180                                      0:02.390   0:10.391    4.35
-rotate 270                                      0:02.578   0:11.938    4.63
-shade 30x30                                     0:03.046   0:10.531    3.46
-sharpen 0x0.5                                   0:05.438   0:12.859    2.36
-sharpen 0x1.0                                   0:10.516   0:16.859    1.60
-shave 10x10                                     0:02.359   0:10.219    4.33
-shear 45x45                                     0:15.141   0:56.094    3.70
-solarize 50%                                    0:02.094   0:09.062    4.33
-spread 1                                        0:02.594   0:21.032    8.11
-spread 3                                        0:02.625   0:20.500    7.81
-swirl 90                                        0:06.641   0:17.078    2.57
-threshold 35%                                   0:02.328   0:09.015    3.87
-fuzz 35% -transparent red                       0:05.922   0:13.890    2.35
-trim                                            0:02.172   0:10.703    4.93
-unsharp 0x0.5+20+1                              0:06.718   0:16.610    2.47
-unsharp 0x1.0+20+1                              0:07.032   0:16.782    2.39
-wave 25x150                                     0:07.281   0:17.204    2.36
-white-threshold 80%                             0:02.047   0:08.937    4.37
============================================== ========== ========== ===========

Windows XP / MinGW / Intel Core 2 Quad
--------------------------------------

TDM's GCC/mingw32 build 1.905.0 (GCC 4.4.1) was downloaded from
http://www.tdragon.net/recentgcc/ and installed on the same Windows XP
system with the 2.83 GHz Core 2 Quad Processor (Q9550) as described
above.  This processor is a multi-chip module (MCM) based on two Core
2 CPUs bonded to a L3 cache in the same chip package.  This build
supports OpenMP:

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:01.047   0:05.422    5.18
-affine 1,0,0.785,1,0,0 -transform               0:09.860   2:50.751   17.32
-black-threshold 20%                             0:01.375   0:05.828    4.24
-blur 0x0.5                                      0:03.890   0:11.078    2.85
-blur 0x1.0                                      0:04.390   0:11.406    2.60
-border 6x6                                      0:01.625   0:08.063    4.96
-charcoal 0x1                                    0:09.937   0:19.641    1.98
-chop 800x600+200+300                            0:01.218   0:05.531    4.54
-colorspace CMYK                                 0:01.422   0:06.719    4.73
-colorspace GRAY                                 0:02.140   0:06.938    3.24
-colorspace HSL                                  0:02.688   0:07.203    2.68
-colorspace HWB                                  0:02.079   0:07.031    3.38
-colorspace OHTA                                 0:02.140   0:07.313    3.42
-colorspace YCbCr                                0:02.141   0:07.484    3.50
-colorspace YIQ                                  0:02.156   0:07.593    3.52
-colorspace YUV                                  0:02.141   0:07.406    3.46
-contrast -contrast -contrast                    0:11.797   0:21.422    1.82
+contrast +contrast +contrast                    0:11.640   0:21.157    1.82
-convolve 1,1,1,1,4,1,1,1,1                      0:02.375   0:07.344    3.09
-colorize 30%/20%/50%                            0:01.938   0:07.906    4.08
-crop 1700x900+100+100                           0:01.485   0:06.500    4.38
-despeckle                                       0:24.890   0:31.422    1.26
-edge 0x1                                        0:03.781   0:08.969    2.37
-emboss 0x1                                      0:04.563   0:14.610    3.20
-enhance                                         0:13.594   0:15.547    1.14
-equalize                                        0:01.828   0:06.953    3.80
-flip                                            0:01.640   0:07.032    4.29
-flop                                            0:01.625   0:07.047    4.34
-frame 15x15+3+3                                 0:01.640   0:07.969    4.86
-gamma 1.6                                       0:01.546   0:05.875    3.80
-gaussian 0x0.5                                  0:03.719   0:08.813    2.37
-gaussian 0x1.0                                  0:08.110   0:12.860    1.59
-implode 0.5                                     0:07.657   0:16.094    2.10
-implode -1                                      0:07.765   0:16.079    2.07
-lat 10x10-5%                                    0:10.140   0:17.703    1.75
-level 10%,1.2,90%                               0:01.578   0:38.438   24.36
-median 1                                        0:24.984   0:46.829    1.87
-median 2                                        1:04.172   1:43.345    1.61
-modulate 110/100/95                             0:03.156   0:07.546    2.39
-motion-blur 0x3+30                              1:39.938   1:39.157    0.99
-negate                                          0:01.328   0:05.796    4.36
+noise Uniform                                   0:03.078   0:13.391    4.35
+noise Gaussian                                  0:24.640   1:39.485    4.04
+noise Multiplicative                            0:13.657   0:57.032    4.18
+noise Impulse                                   0:03.234   0:11.734    3.63
+noise Laplacian                                 0:08.781   0:37.751    4.30
+noise Poisson                                   0:11.453   0:51.219    4.47
-noise 1                                         0:24.657   0:44.422    1.80
-noise 2                                         1:03.594   1:40.985    1.59
-normalize                                       0:01.735   0:06.953    4.01
-fill blue -fuzz 35% -opaque red                 0:01.359   0:06.500    4.78
-paint 0x1                                       0:07.672   0:12.375    1.61
-raise 10x10                                     0:01.125   0:05.813    5.17
-density 75x75 -resample 50x50                   0:04.078   0:10.016    2.46
-recolor '1,0,0,0,1,0,0,0,1'                     0:01.031   0:06.391    6.20
-recolor '0,0,1,0,1,0,1,0,0'                     0:01.922   0:06.219    3.24
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:01.829   0:06.282    3.43
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:01.812   0:06.187    3.41
-resize 10%                                      0:02.281   0:06.906    3.03
-resize 50%                                      0:03.454   0:08.266    2.39
-resize 150%                                     0:08.359   0:13.500    1.62
-roll +20+10                                     0:02.468   0:06.813    2.76
-rotate 0                                        0:01.046   0:05.484    5.24
-rotate 15                                       0:09.000   0:25.562    2.84
-rotate 45                                       0:19.844   0:37.969    1.91
-rotate 90                                       0:01.860   0:07.672    4.12
-rotate 180                                      0:01.688   0:07.203    4.27
-rotate 270                                      0:01.844   0:07.625    4.14
-shade 30x30                                     0:02.265   0:07.484    3.30
-sharpen 0x0.5                                   0:03.656   0:08.734    2.39
-sharpen 0x1.0                                   0:08.141   0:12.703    1.56
-shave 10x10                                     0:01.625   0:06.921    4.26
-shear 45x45                                     0:14.828   0:47.704    3.22
-solarize 50%                                    0:01.328   0:05.890    4.44
-spread 1                                        0:01.797   0:16.110    8.96
-spread 3                                        0:01.875   0:16.031    8.55
-swirl 90                                        0:07.047   0:16.109    2.29
-threshold 35%                                   0:01.547   0:05.860    3.79
-fuzz 35% -transparent red                       0:01.485   0:06.703    4.51
-trim                                            0:01.531   0:07.328    4.79
-unsharp 0x0.5+20+1                              0:04.422   0:11.703    2.65
-unsharp 0x1.0+20+1                              0:04.891   0:11.672    2.39
-wave 25x150                                     0:07.375   0:16.344    2.22
-white-threshold 80%                             0:01.235   0:05.625    4.55
============================================== ========== ========== ===========

Windows XP / MinGW / Intel Core 2 Quad
--------------------------------------

Cygwin was installed on the same Windows XP system with the 2.83 GHz
Core 2 Quad Processor (Q9550) as described above.  The compiler
identifies itself as "gcc (GCC) 3.4.4 (cygming special, gdc 0.12,
using dmd 0.125)".  This build does not support OpenMP:

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:01.674   0:13.637    8.15
-affine 1,0,0.785,1,0,0 -transform               0:23.444   7:32.770   19.31
-black-threshold 20%                             0:02.243   0:14.723    6.56
-blur 0x0.5                                      0:11.954   0:47.272    3.95
-blur 0x1.0                                      0:15.192   0:50.138    3.30
-border 6x6                                      0:02.498   0:22.661    9.07
-charcoal 0x1                                    0:33.791   1:17.981    2.31
-chop 800x600+200+300                            0:01.935   0:13.749    7.11
-colorspace CMYK                                 0:02.233   0:18.685    8.37
-colorspace GRAY                                 0:04.168   0:17.117    4.11
-colorspace HSL                                  0:09.673   0:20.896    2.16
-colorspace HWB                                  0:07.373   0:19.621    2.66
-colorspace OHTA                                 0:04.173   0:17.559    4.21
-colorspace YCbCr                                0:04.174   0:17.578    4.21
-colorspace YIQ                                  0:04.180   0:17.738    4.24
-colorspace YUV                                  0:04.175   0:17.551    4.20
-contrast -contrast -contrast                    0:38.538   0:52.218    1.35
+contrast +contrast +contrast                    0:36.886   0:50.116    1.36
-convolve 1,1,1,1,4,1,1,1,1                      0:10.502   0:24.278    2.31
-colorize 30%/20%/50%                            0:05.478   0:17.483    3.19
-crop 1700x900+100+100                           0:02.289   0:14.129    6.17
-despeckle                                       1:31.772   2:02.981    1.34
-edge 0x1                                        0:09.810   0:30.845    3.14
-emboss 0x1                                      0:17.727   1:03.705    3.59
-enhance                                         0:56.835   1:01.679    1.09
-equalize                                        0:02.578   0:17.886    6.94
-flip                                            0:02.436   0:14.552    5.97
-flop                                            0:02.455   0:14.402    5.87
-frame 15x15+3+3                                 0:02.481   0:25.481   10.27
-gamma 1.6                                       0:03.783   0:14.867    3.93
-gaussian 0x0.5                                  0:16.823   0:31.376    1.87
-gaussian 0x1.0                                  0:38.667   0:59.197    1.53
-implode 0.5                                     0:30.508   0:53.367    1.75
-implode -1                                      0:19.896   0:42.563    2.14
-lat 10x10-5%                                    0:35.550   0:56.598    1.59
-level 10%,1.2,90%                               0:02.540   2:36.615   61.66
-median 1                                        1:49.419   2:11.164    1.20
-median 2                                        4:48.004   5:14.274    1.09
-modulate 110/100/95                             0:11.456   0:24.595    2.15
-motion-blur 0x3+30                              1:05.598   3:35.775    3.29
-negate                                          0:02.073   0:14.212    6.86
+noise Uniform                                   0:14.705   0:27.403    1.86
+noise Gaussian                                  1:18.284   1:36.102    1.23
+noise Multiplicative                            0:48.461   1:00.968    1.26
+noise Impulse                                   0:14.525   0:26.239    1.81
+noise Laplacian                                 0:29.814   0:46.027    1.54
+noise Poisson                                   0:44.009   1:00.769    1.38
-noise 1                                         1:51.826   2:11.598    1.18
-noise 2                                         4:54.624   5:13.749    1.06
-normalize                                       0:02.651   0:18.406    6.94
-fill blue -fuzz 35% -opaque red                 0:02.567   0:15.935    6.21
-paint 0x1                                       0:14.652   0:40.489    2.76
-raise 10x10                                     0:01.825   0:13.747    7.53
-density 75x75 -resample 50x50                   0:12.903   0:32.136    2.49
-recolor '1,0,0,0,1,0,0,0,1'                     0:01.679   0:18.010   10.73
-recolor '0,0,1,0,1,0,1,0,0'                     0:04.684   0:18.013    3.85
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:05.963   0:18.044    3.03
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:05.871   0:18.007    3.07
-resize 10%                                      0:06.761   0:20.389    3.02
-resize 50%                                      0:10.459   0:26.129    2.50
-resize 150%                                     0:30.356   0:54.589    1.80
-roll +20+10                                     0:02.535   0:14.338    5.66
-rotate 0                                        0:01.668   0:13.555    8.13
-rotate 15                                       0:24.306   1:10.965    2.92
-rotate 45                                       0:50.960   1:33.959    1.84
-rotate 90                                       0:02.838   0:15.098    5.32
-rotate 180                                      0:02.470   0:14.356    5.81
-rotate 270                                      0:02.836   0:20.578    7.26
-shade 30x30                                     0:11.337   0:21.175    1.87
-sharpen 0x0.5                                   0:16.831   0:31.487    1.87
-sharpen 0x1.0                                   0:38.642   0:58.924    1.52
-shave 10x10                                     0:06.014   0:14.326    2.38
-shear 45x45                                     0:28.127   1:51.023    3.95
-solarize 50%                                    0:02.398   0:14.514    6.05
-spread 1                                        0:04.114   0:47.786   11.62
-spread 3                                        0:04.185   0:47.923   11.45
-swirl 90                                        0:18.969   0:42.025    2.22
-threshold 35%                                   0:02.292   0:14.353    6.26
-fuzz 35% -transparent red                       0:02.617   0:16.218    6.20
-trim                                            0:03.471   0:15.033    4.33
-unsharp 0x0.5+20+1                              0:14.258   0:50.623    3.55
-unsharp 0x1.0+20+1                              0:17.336   0:54.694    3.15
-wave 25x150                                     0:19.888   0:49.419    2.48
-white-threshold 80%                             0:01.994   0:14.213    7.13
============================================== ========== ========== ===========

Intel Pentium IV / Cygwin
-------------------------

This system is a 2000 vintage 2.0GHz Pentium IV system with Windows XP
SP3 and latest Cygwin.  This system has only one CPU core and the
compiler (gcc version 3.4.4 (cygming special, gdc 0.12, using dmd
0.125) does not support OpenMP so this is a single-threaded
configuration.  It can be seen that ImageMagick takes much longer to
generate its tiled input image and to even get started (first -noop
entry).  GraphicsMagick is faster for all algorithms:

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:04.512   0:41.131    9.12
-affine 1,0,0.785,1,0,0 -transform               1:14.401  29:00.690   23.40
-black-threshold 20%                             0:05.918   0:43.291    7.32
-blur 0x0.5                                      0:34.824   2:33.262    4.40
-blur 0x1.0                                      0:41.732   2:41.096    3.86
-border 6x6                                      0:06.813   1:10.549   10.36
-charcoal 0x1                                    1:33.863   3:57.569    2.53
-chop 800x600+200+300                            0:04.991   0:41.566    8.33
-colorspace CMYK                                 0:05.762   0:55.454    9.62
-colorspace GRAY                                 0:12.069   0:50.569    4.19
-colorspace HSL                                  0:26.534   1:01.787    2.33
-colorspace HWB                                  0:19.400   0:57.439    2.96
-colorspace OHTA                                 0:12.191   0:53.048    4.35
-colorspace YCbCr                                0:12.748   0:53.210    4.17
-colorspace YIQ                                  0:12.356   0:53.083    4.30
-colorspace YUV                                  0:12.380   0:53.025    4.28
-contrast -contrast -contrast                    2:01.570   2:59.583    1.48
+contrast +contrast +contrast                    1:52.208   2:47.550    1.49
-convolve 1,1,1,1,4,1,1,1,1                      0:25.636   1:09.530    2.71
-colorize 30%/20%/50%                            0:13.641   0:50.601    3.71
-crop 1700x900+100+100                           0:06.080   0:42.762    7.03
-despeckle                                       4:18.294   5:57.356    1.38
-edge 0x1                                        0:24.561   1:26.935    3.54
-emboss 0x1                                      0:47.520   2:41.918    3.41
-enhance                                         2:55.648   3:05.925    1.06
-equalize                                        0:08.394   0:54.447    6.49
-flip                                            0:06.697   0:43.274    6.46
-flop                                            0:06.570   0:43.139    6.57
-frame 15x15+3+3                                 0:06.763   1:16.531   11.32
-gamma 1.6                                       0:14.039   0:45.235    3.22
-gaussian 0x0.5                                  0:41.671   1:27.427    2.10
-gaussian 0x1.0                                  1:46.245   2:24.104    1.36
-implode 0.5                                     2:01.506   3:20.054    1.65
-implode -1                                      1:13.870   2:30.783    2.04
-lat 10x10-5%                                    1:24.427   2:29.528    1.77
-level 10%,1.2,90%                               0:07.871  12:18.750   93.86
-median 1                                        5:28.956   7:02.009    1.28
-median 2                                       15:16.730  17:18.391    1.13
-modulate 110/100/95                             0:31.982   1:09.186    2.16
-motion-blur 0x3+30                              3:04.725  11:38.141    3.78
-negate                                          0:05.156   0:42.058    8.16
+noise Uniform                                   0:43.778   1:39.521    2.27
+noise Gaussian                                  4:33.385   5:28.144    1.20
+noise Multiplicative                            2:48.559   3:48.636    1.36
+noise Impulse                                   0:41.870   1:39.284    2.37
+noise Laplacian                                 1:33.908   2:36.945    1.67
+noise Poisson                                   2:38.987   3:37.807    1.37
-noise 1                                         5:37.070   7:06.010    1.26
-noise 2                                        15:25.164  17:24.100    1.13
-normalize                                       0:08.385   0:54.978    6.56
-fill blue -fuzz 35% -opaque red                 0:06.805   0:47.789    7.02
-paint 0x1                                       0:53.607   2:11.397    2.45
-raise 10x10                                     0:04.702   0:41.412    8.81
-density 75x75 -resample 50x50                   0:39.947   1:49.252    2.73
-recolor '1,0,0,0,1,0,0,0,1'                     0:04.292   0:51.649   12.03
-recolor '0,0,1,0,1,0,1,0,0'                     0:12.692   0:51.578    4.06
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:17.231   0:51.637    3.00
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:17.319   0:51.665    2.98
-resize 10%                                      0:21.376   1:10.829    3.31
-resize 50%                                      0:30.334   1:28.011    2.90
-resize 150%                                     1:36.731   3:06.453    1.93
-roll +20+10                                     0:07.229   0:43.783    6.06
-rotate 0                                        0:04.284   0:40.744    9.51
-rotate 15                                       1:01.468   3:04.509    3.00
-rotate 45                                       3:13.576   5:15.498    1.63
-rotate 90                                       0:09.931   0:48.534    4.89
-rotate 180                                      0:06.508   0:43.521    6.69
-rotate 270                                      0:09.975   0:48.866    4.90
-shade 30x30                                     0:34.390   0:58.898    1.71
-sharpen 0x0.5                                   0:41.745   1:27.829    2.10
-sharpen 0x1.0                                   1:46.583   2:24.551    1.36
-shave 10x10                                     0:06.874   0:43.187    6.28
-shear 45x45                                     2:10.297   6:31.764    3.01
-solarize 50%                                    0:05.182   0:42.536    8.21
-spread 1                                        0:11.578   2:44.566   14.21
-spread 3                                        0:11.163   2:46.028   14.87
-swirl 90                                        1:08.346   2:19.522    2.04
-threshold 35%                                   0:06.402   0:43.831    6.85
-fuzz 35% -transparent red                       0:07.014   0:49.086    7.00
-trim                                            0:08.423   0:44.454    5.28
-unsharp 0x0.5+20+1                              0:43.551   2:42.554    3.73
-unsharp 0x1.0+20+1                              0:50.222   2:50.735    3.40
-wave 25x150                                     1:01.619   2:41.975    2.63
-white-threshold 80%                             0:05.339   0:42.792    8.01
============================================== ========== ========== ===========


--------------------------------------------------------------------------

| Copyright (C) 2008-2009 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

