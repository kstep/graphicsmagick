.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=============================================================
GraphicsMagick 1.3.8 vs ImageMagick 6.5.8-10 Benchmark Report
=============================================================

.. contents::
  :local:

Introduction
============

GraphicsMagick 1.3.8 has been benchmarked against the most recent
ImageMagick release available at time of release. ImageMagick has been
significantly updated to use OpenMP as has GraphicsMagick
1.3.8. Several disparate types of systems were used in our
testing. While last time around, ImageMagick performance had taken a
terrific nose-dive, in this series of tests ImageMagick is much more
competitive.  ImageMagick has clearly benefited from this benchmark
being available since November, 2008.

Executive Summary
=================

GraphicsMagick is usually considerably faster at executing image
processing operations from the command line than ImageMagick 6.5.8-10
is. One ImageMagick algorithm runs as much as 770 times slower.
GraphicsMagick clearly runs much more efficiently under Microsoft
Windows.

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

  GraphicsMagick 1.4 unreleased Q16 http://www.GraphicsMagick.org/

  ImageMagick 6.5.8-10 2010-01-07 Q16 http://www.imagemagick.org

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
  #convert1='/c/Program\ Files/GraphicsMagick-1.3.8-Q16/gm.exe convert'

  # ImageMagick
  convert2='/usr/local/bin/convert'
  #convert2='/c/Program\ Files/ImageMagick-6.5.9-Q16/convert.exe'

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
CPUs and running Solaris 10 U8. The compiler used identifies itself as
"gcc (GCC) 4.4.2".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:01.178   0:01.377    1.17
-affine 1,0,0.785,1,0,0 -transform               0:07.777  16:22.340  126.31
-black-threshold 20%                             0:01.305   0:01.675    1.28
-blur 0x0.5                                      0:06.076   0:12.666    2.08
-blur 0x1.0                                      0:06.972   0:15.007    2.15
-border 6x6                                      0:01.986   0:02.269    1.14
-charcoal 0x1                                    0:13.530   0:24.343    1.80
-chop 800x600+200+300                            0:01.364   0:01.880    1.38
-colorspace CMYK                                 0:01.610   0:03.859    2.40
-colorspace GRAY                                 0:01.908   0:02.568    1.35
-colorspace HSL                                  0:03.250   0:04.262    1.31
-colorspace HWB                                  0:02.445   0:04.014    1.64
-colorspace OHTA                                 0:01.884   0:03.616    1.92
-colorspace YCbCr                                0:01.947   0:03.573    1.84
-colorspace YIQ                                  0:01.878   0:03.631    1.93
-colorspace YUV                                  0:01.891   0:03.640    1.92
-contrast -contrast -contrast                    0:14.232   0:14.088    0.99
+contrast +contrast +contrast                    0:13.290   0:13.165    0.99
-convolve 1,1,1,1,4,1,1,1,1                      0:03.341   0:04.307    1.29
-colorize 30%/20%/50%                            0:02.367   0:04.835    2.04
-crop 1700x900+100+100                           0:01.741   0:02.056    1.18
-despeckle                                       0:29.337   1:43.441    3.53
-edge 0x1                                        0:04.774   0:06.027    1.26
-emboss 0x1                                      0:06.310   0:14.123    2.24
-enhance                                         0:18.500   0:15.313    0.83
-equalize                                        0:02.512   0:04.415    1.76
-flip                                            0:02.025   0:02.189    1.08
-flop                                            0:01.948   0:02.269    1.16
-frame 15x15+3+3                                 0:01.976   0:02.230    1.13
-gamma 1.6                                       0:01.466   0:02.074    1.41
-gaussian 0x0.5                                  0:04.628   0:05.722    1.24
-gaussian 0x1.0                                  0:09.062   0:10.515    1.16
-implode 0.5                                     0:08.754   0:13.985    1.60
-implode -1                                      0:07.412   0:12.415    1.67
-lat 10x10-5%                                    0:12.709   0:15.995    1.26
-level 10%,1.2,90%                               0:01.706   0:10.498    6.15
-median 1                                        0:27.066   0:32.835    1.21
-median 2                                        1:05.904   1:14.802    1.14
-modulate 110/100/95                             0:03.953   0:03.682    0.93
-motion-blur 0x3+30                              0:40.116   2:33.456    3.83
-negate                                          0:01.323   0:01.598    1.21
+noise Uniform                                   0:05.286   0:11.753    2.22
+noise Gaussian                                  0:24.222   1:26.128    3.56
+noise Multiplicative                            0:16.094   0:54.490    3.39
+noise Impulse                                   0:05.397   0:10.444    1.94
+noise Laplacian                                 0:08.857   0:26.220    2.96
+noise Poisson                                   0:10.688   0:40.122    3.75
-noise 1                                         0:27.430   0:33.008    1.20
-noise 2                                         1:07.936   1:14.809    1.10
-normalize                                       0:02.356   0:04.437    1.88
-fill blue -fuzz 35% -opaque red                 0:01.461   0:02.007    1.37
-paint 0x1                                       0:07.981   0:10.795    1.35
-raise 10x10                                     0:01.235   0:01.438    1.16
-density 75x75 -resample 50x50                   0:04.841   0:09.212    1.90
-recolor '1,0,0,0,1,0,0,0,1'                     0:01.128   0:03.285    2.91
-recolor '0,0,1,0,1,0,1,0,0'                     0:01.864   0:03.265    1.75
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:01.929   0:03.201    1.66
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:01.946   0:03.276    1.68
-resize 10%                                      0:02.585   0:03.081    1.19
-resize 50%                                      0:03.600   0:04.605    1.28
-resize 150%                                     0:15.096   0:14.621    0.97
-roll +20+10                                     0:02.071   0:02.331    1.13
-rotate 0                                        0:01.125   0:01.411    1.25
-rotate 15                                       0:13.394   0:34.523    2.58
-rotate 45                                       0:46.747   0:55.091    1.18
-rotate 90                                       0:02.952   0:03.417    1.16
-rotate 180                                      0:02.035   0:02.250    1.11
-rotate 270                                      0:02.901   0:03.485    1.20
-shade 30x30                                     0:03.266   0:04.167    1.28
-sharpen 0x0.5                                   0:04.628   0:05.735    1.24
-sharpen 0x1.0                                   0:09.073   0:10.304    1.14
-shave 10x10                                     0:01.922   0:02.134    1.11
-shear 45x45                                     0:36.908   1:17.646    2.10
-solarize 50%                                    0:01.318   0:01.600    1.21
-spread 1                                        0:02.010   0:14.009    6.97
-spread 3                                        0:02.090   0:14.401    6.89
-swirl 90                                        0:07.524   0:12.387    1.65
-threshold 35%                                   0:01.538   0:01.557    1.01
-fuzz 35% -transparent red                       0:01.416   0:02.227    1.57
-trim                                            0:01.723   0:02.532    1.47
-unsharp 0x0.5+20+1                              0:06.696   0:13.597    2.03
-unsharp 0x1.0+20+1                              0:07.596   0:15.803    2.08
-wave 25x150                                     0:12.730   0:28.316    2.22
-white-threshold 80%                             0:01.315   0:01.590    1.21
============================================== ========== ========== ===========

Sun SPARC/Solaris 10
--------------------

This system is a 2004 vintage Sun SPARC workstation with two 1.2GHz
CPUs and running Solaris 10 U7. The compiler used identifies itself as
"gcc (GCC) 4.3.4".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:02.709   0:03.656    1.35
-affine 1,0,0.785,1,0,0 -transform               0:52.316  17:57.509   20.60
-black-threshold 20%                             0:04.991   0:06.352    1.27
-blur 0x0.5                                      0:24.166   0:49.635    2.05
-blur 0x1.0                                      0:30.369   0:55.950    1.84
-border 6x6                                      0:04.660   0:05.512    1.18
-charcoal 0x1                                    1:09.717   1:44.719    1.50
-chop 800x600+200+300                            0:03.833   0:04.858    1.27
-colorspace CMYK                                 0:04.883   0:13.226    2.71
-colorspace GRAY                                 0:08.069   0:09.496    1.18
-colorspace HSL                                  0:15.010   0:15.858    1.06
-colorspace HWB                                  0:13.749   0:15.225    1.11
-colorspace OHTA                                 0:08.159   0:10.735    1.32
-colorspace YCbCr                                0:08.135   0:10.810    1.33
-colorspace YIQ                                  0:08.054   0:10.748    1.33
-colorspace YUV                                  0:08.027   0:10.763    1.34
-contrast -contrast -contrast                    1:33.320   1:28.127    0.94
+contrast +contrast +contrast                    1:29.213   1:24.948    0.95
-convolve 1,1,1,1,4,1,1,1,1                      0:18.011   0:19.750    1.10
-colorize 30%/20%/50%                            0:08.752   0:15.397    1.76
-crop 1700x900+100+100                           0:04.148   0:04.917    1.19
-despeckle                                       3:55.710   7:30.774    1.91
-edge 0x1                                        0:29.148   0:29.600    1.02
-emboss 0x1                                      0:39.189   1:24.575    2.16
-enhance                                         1:29.822   1:06.435    0.74
-equalize                                        0:09.343   0:17.670    1.89
-flip                                            0:04.572   0:05.356    1.17
-flop                                            0:05.871   0:06.303    1.07
-frame 15x15+3+3                                 0:04.694   0:05.630    1.20
-gamma 1.6                                       0:04.858   0:05.905    1.22
-gaussian 0x0.5                                  0:30.514   0:31.029    1.02
-gaussian 0x1.0                                  1:16.600   1:08.731    0.90
-implode 0.5                                     0:47.753   1:09.826    1.46
-implode -1                                      0:38.926   1:01.029    1.57
-lat 10x10-5%                                    1:13.266   1:31.907    1.25
-level 10%,1.2,90%                               0:05.840  10:41.916  109.92
-median 1                                        1:47.302   3:21.764    1.88
-median 2                                        4:23.818   7:38.972    1.74
-modulate 110/100/95                             0:26.764   0:28.943    1.08
-motion-blur 0x3+30                              2:11.420   6:07.715    2.80
-negate                                          0:04.212   0:05.283    1.25
+noise Uniform                                   0:26.128   0:44.786    1.71
+noise Gaussian                                  1:59.307   3:39.241    1.84
+noise Multiplicative                            1:19.406   2:31.356    1.91
+noise Impulse                                   0:26.146   0:45.240    1.73
+noise Laplacian                                 0:46.293   1:25.536    1.85
+noise Poisson                                   1:17.055   2:23.729    1.87
-noise 1                                         1:53.173   3:31.135    1.87
-noise 2                                         4:37.589   7:43.120    1.67
-normalize                                       0:08.867   0:17.635    1.99
-fill blue -fuzz 35% -opaque red                 0:06.869   0:08.560    1.25
-paint 0x1                                       0:52.714   0:54.845    1.04
-raise 10x10                                     0:02.928   0:03.972    1.36
-density 75x75 -resample 50x50                   0:26.941   0:35.301    1.31
-recolor '1,0,0,0,1,0,0,0,1'                     0:02.909   0:13.396    4.61
-recolor '0,0,1,0,1,0,1,0,0'                     0:09.174   0:13.405    1.46
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:10.952   0:13.376    1.22
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:11.036   0:13.487    1.22
-resize 10%                                      0:14.046   0:15.834    1.13
-resize 50%                                      0:20.490   0:23.745    1.16
-resize 150%                                     1:07.444   1:05.372    0.97
-roll +20+10                                     0:04.581   0:06.687    1.46
-rotate 0                                        0:02.702   0:03.653    1.35
-rotate 15                                       0:57.029   1:32.969    1.63
-rotate 45                                       2:00.493   2:24.184    1.20
-rotate 90                                       0:06.971   0:08.778    1.26
-rotate 180                                      0:05.920   0:06.741    1.14
-rotate 270                                      0:07.031   0:08.994    1.28
-shade 30x30                                     0:20.879   0:19.907    0.95
-sharpen 0x0.5                                   0:30.575   0:31.092    1.02
-sharpen 0x1.0                                   1:16.978   1:09.121    0.90
-shave 10x10                                     0:04.467   0:05.010    1.12
-shear 45x45                                     1:10.505   2:21.245    2.00
-solarize 50%                                    0:04.228   0:06.233    1.47
-spread 1                                        0:06.452   1:05.634   10.17
-spread 3                                        0:06.907   1:06.340    9.60
-swirl 90                                        0:38.610   1:00.303    1.56
-threshold 35%                                   0:05.579   0:08.509    1.53
-fuzz 35% -transparent red                       0:06.832   0:09.791    1.43
-trim                                            0:11.591   0:07.473    0.64
-unsharp 0x0.5+20+1                              0:29.871   1:00.088    2.01
-unsharp 0x1.0+20+1                              0:36.600   1:12.704    1.99
-wave 25x150                                     0:39.870   1:05.838    1.65
-white-threshold 80%                             0:04.806   0:06.248    1.30
============================================== ========== ========== ===========

Apple PowerPC G5/OS-X Tiger
---------------------------

This system is an Apple G5 system with two 2.5GHz CPUs and running the
Leopard release of OS-X. The compiler used identifies itself as
"gcc-4.4.0 (GCC) 4.4.0".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:04.857   0:04.801    0.99
-affine 1,0,0.785,1,0,0 -transform               0:26.769   8:18.592   18.63
-black-threshold 20%                             0:05.168   0:07.055    1.37
-blur 0x0.5                                      0:28.855   0:48.399    1.68
-blur 0x1.0                                      0:40.022   0:58.368    1.46
-border 6x6                                      0:06.973   0:07.072    1.01
-charcoal 0x1                                    1:17.480   1:48.558    1.40
-chop 800x600+200+300                            0:05.381   0:05.391    1.00
-colorspace CMYK                                 0:05.694   0:10.704    1.88
-colorspace GRAY                                 0:07.361   0:08.987    1.22
-colorspace HSL                                  0:10.304   0:11.742    1.14
-colorspace HWB                                  0:10.452   0:11.600    1.11
-colorspace OHTA                                 0:07.269   0:10.592    1.46
-colorspace YCbCr                                0:07.283   0:10.710    1.47
-colorspace YIQ                                  0:07.292   0:10.622    1.46
-colorspace YUV                                  0:07.379   0:10.615    1.44
-contrast -contrast -contrast                    0:31.329   0:41.765    1.33
+contrast +contrast +contrast                    0:30.762   0:40.587    1.32
-convolve 1,1,1,1,4,1,1,1,1                      0:18.642   0:20.481    1.10
-colorize 30%/20%/50%                            0:07.698   0:16.949    2.20
-crop 1700x900+100+100                           0:06.338   0:06.351    1.00
-despeckle                                       3:12.338   5:55.818    1.85
-edge 0x1                                        0:18.397   0:41.676    2.27
-emboss 0x1                                      0:38.262   2:05.150    3.27
-enhance                                         1:04.915   1:16.363    1.18
-equalize                                        0:06.514   0:09.544    1.47
-flip                                            0:06.888   0:06.975    1.01
-flop                                            0:07.047   0:07.085    1.01
-frame 15x15+3+3                                 0:07.031   0:07.202    1.02
-gamma 1.6                                       0:05.488   0:05.629    1.03
-gaussian 0x0.5                                  0:34.716   0:41.741    1.20
-gaussian 0x1.0                                  1:30.259   1:57.255    1.30
-implode 0.5                                     0:23.532   0:39.246    1.67
-implode -1                                      0:19.219   0:36.793    1.91
-lat 10x10-5%                                    1:33.668   2:49.748    1.81
-level 10%,1.2,90%                               0:05.645   0:19.288    3.42
-median 1                                        1:05.764   1:19.453    1.21
-median 2                                        2:41.285   3:00.425    1.12
-modulate 110/100/95                             0:12.872   0:12.409    0.96
-motion-blur 0x3+30                              1:28.800   4:22.206    2.95
-negate                                          0:05.220   0:05.263    1.01
+noise Uniform                                   0:14.980   0:25.401    1.70
+noise Gaussian                                  0:45.289   1:23.811    1.85
+noise Multiplicative                            0:31.283   0:57.160    1.83
+noise Impulse                                   0:15.176   0:21.868    1.44
+noise Laplacian                                 0:23.128   0:41.936    1.81
+noise Poisson                                   0:30.384   1:08.247    2.25
-noise 1                                         1:05.451   1:19.087    1.21
-noise 2                                         2:36.103   3:02.010    1.17
-normalize                                       0:06.315   0:10.978    1.74
-fill blue -fuzz 35% -opaque red                 0:05.567   0:06.769    1.22
-paint 0x1                                       0:13.307   0:21.479    1.61
-raise 10x10                                     0:05.067   0:05.337    1.05
-density 75x75 -resample 50x50                   0:27.349   0:38.198    1.40
-recolor '1,0,0,0,1,0,0,0,1'                     0:04.742   0:11.828    2.49
-recolor '0,0,1,0,1,0,1,0,0'                     0:08.992   0:11.823    1.31
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:09.540   0:11.844    1.24
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:09.490   0:12.411    1.31
-resize 10%                                      0:13.062   0:16.979    1.30
-resize 50%                                      0:20.049   0:25.518    1.27
-resize 150%                                     1:26.295   1:14.216    0.86
-roll +20+10                                     0:06.910   0:07.065    1.02
-rotate 0                                        0:04.778   0:04.805    1.01
-rotate 15                                       0:57.578   1:36.889    1.68
-rotate 45                                       2:50.294   2:49.535    1.00
-rotate 90                                       0:08.963   0:09.550    1.07
-rotate 180                                      0:07.026   0:07.095    1.01
-rotate 270                                      0:09.018   0:09.416    1.04
-shade 30x30                                     0:10.232   0:22.140    2.16
-sharpen 0x0.5                                   0:34.710   0:41.765    1.20
-sharpen 0x1.0                                   1:30.219   1:57.341    1.30
-shave 10x10                                     0:06.853   0:06.973    1.02
-shear 45x45                                     2:27.556   3:51.787    1.57
-solarize 50%                                    0:05.201   0:06.788    1.31
-spread 1                                        0:07.035   0:46.593    6.62
-spread 3                                        0:06.923   0:46.810    6.76
-swirl 90                                        0:21.505   0:39.508    1.84
-threshold 35%                                   0:05.559   0:05.705    1.03
-fuzz 35% -transparent red                       0:05.655   0:07.779    1.38
-trim                                            0:09.073   0:08.390    0.92
-unsharp 0x0.5+20+1                              0:33.049   0:54.405    1.65
-unsharp 0x1.0+20+1                              0:43.513   1:04.673    1.49
-wave 25x150                                     0:22.211   0:43.112    1.94
-white-threshold 80%                             0:05.071   0:06.812    1.34
============================================== ========== ========== ===========

Intel Xeon / FreeBSD 8.0
------------------------

This system is a 2003 vintage Intel Xeon system with two 2.4GHz CPUs
and running FreeBSD 8.0. The compiler used identifies itself as "gcc
(GCC) 4.2.1 20070719 [FreeBSD]".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:03.177   0:06.680    2.10
-affine 1,0,0.785,1,0,0 -transform               0:24.031   5:00.039   12.49
-black-threshold 20%                             0:03.934   0:07.566    1.92
-blur 0x0.5                                      0:23.950   0:37.977    1.59
-blur 0x1.0                                      0:25.728   0:39.373    1.53
-border 6x6                                      0:05.673   0:09.964    1.76
-charcoal 0x1                                    0:48.669   1:12.379    1.49
-chop 800x600+200+300                            0:03.891   0:07.357    1.89
-colorspace CMYK                                 0:05.142   0:12.824    2.49
-colorspace GRAY                                 0:05.839   0:12.578    2.15
-colorspace HSL                                  0:09.272   0:14.138    1.52
-colorspace HWB                                  0:06.999   0:13.505    1.93
-colorspace OHTA                                 0:05.917   0:14.988    2.53
-colorspace YCbCr                                0:05.860   0:15.052    2.57
-colorspace YIQ                                  0:05.849   0:15.048    2.57
-colorspace YUV                                  0:05.792   0:14.980    2.59
-contrast -contrast -contrast                    0:33.574   0:52.013    1.55
+contrast +contrast +contrast                    0:32.869   0:51.365    1.56
-convolve 1,1,1,1,4,1,1,1,1                      0:10.757   0:14.228    1.32
-colorize 30%/20%/50%                            0:07.522   0:17.437    2.32
-crop 1700x900+100+100                           0:05.026   0:09.100    1.81
-despeckle                                       1:51.714   6:22.366    3.42
-edge 0x1                                        0:10.847   0:17.962    1.66
-emboss 0x1                                      0:20.616   0:43.243    2.10
-enhance                                         1:03.846   0:56.680    0.89
-equalize                                        0:06.964   0:14.879    2.14
-flip                                            0:05.635   0:09.881    1.75
-flop                                            0:05.584   0:09.938    1.78
-frame 15x15+3+3                                 0:05.750   0:10.083    1.75
-gamma 1.6                                       0:04.608   0:08.242    1.79
-gaussian 0x0.5                                  0:15.472   0:18.685    1.21
-gaussian 0x1.0                                  0:32.413   0:32.944    1.02
-implode 0.5                                     0:31.106   0:48.765    1.57
-implode -1                                      0:19.217   0:37.754    1.96
-lat 10x10-5%                                    0:37.576   0:39.690    1.06
-level 10%,1.2,90%                               0:05.316   3:23.157   38.22
-median 1                                        1:39.682   2:40.029    1.61
-median 2                                        4:24.794   6:40.895    1.51
-modulate 110/100/95                             0:11.340   0:15.722    1.39
-motion-blur 0x3+30                              2:55.188   3:12.914    1.10
-negate                                          0:04.091   0:07.575    1.85
+noise Uniform                                   0:16.640   0:40.202    2.42
+noise Gaussian                                  0:43.222   1:36.953    2.24
+noise Multiplicative                            0:33.578   1:26.842    2.59
+noise Impulse                                   0:16.629   0:34.612    2.08
+noise Laplacian                                 0:23.023   0:59.406    2.58
+noise Poisson                                   0:37.688   2:13.866    3.55
-noise 1                                         1:41.152   2:34.051    1.52
-noise 2                                         4:27.027   6:49.683    1.53
-normalize                                       0:06.632   0:14.720    2.22
-fill blue -fuzz 35% -opaque red                 0:04.019   0:08.585    2.14
-paint 0x1                                       0:22.183   0:42.306    1.91
-raise 10x10                                     0:03.362   0:07.124    2.12
-density 75x75 -resample 50x50                   0:21.683   0:30.418    1.40
-recolor '1,0,0,0,1,0,0,0,1'                     0:03.277   0:13.651    4.17
-recolor '0,0,1,0,1,0,1,0,0'                     0:05.255   0:13.740    2.61
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:05.716   0:13.621    2.38
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:05.762   0:13.799    2.39
-resize 10%                                      0:11.807   0:15.142    1.28
-resize 50%                                      0:17.039   0:22.336    1.31
-resize 150%                                     1:05.138   0:55.981    0.86
-roll +20+10                                     0:06.544   0:10.143    1.55
-rotate 0                                        0:03.206   0:06.770    2.11
-rotate 15                                       0:44.553   1:21.571    1.83
-rotate 45                                       2:01.911   2:03.346    1.01
-rotate 90                                       0:11.581   0:15.309    1.32
-rotate 180                                      0:05.478   0:09.919    1.81
-rotate 270                                      0:11.248   0:15.360    1.37
-shade 30x30                                     0:11.884   0:13.802    1.16
-sharpen 0x0.5                                   0:15.478   0:18.661    1.21
-sharpen 0x1.0                                   0:31.906   0:32.956    1.03
-shave 10x10                                     0:05.617   0:09.833    1.75
-shear 45x45                                     1:43.165   2:44.866    1.60
-solarize 50%                                    0:04.074   0:07.415    1.82
-spread 1                                        0:06.284   0:44.468    7.08
-spread 3                                        0:06.215   0:44.812    7.21
-swirl 90                                        0:20.247   0:38.510    1.90
-threshold 35%                                   0:04.870   0:07.567    1.55
-fuzz 35% -transparent red                       0:04.097   0:09.360    2.28
-trim                                            0:06.245   0:10.589    1.70
-unsharp 0x0.5+20+1                              0:26.559   0:43.460    1.64
-unsharp 0x1.0+20+1                              0:28.696   0:44.927    1.57
-wave 25x150                                     0:19.748   0:41.727    2.11
-white-threshold 80%                             0:03.728   0:07.435    1.99
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
-noop                                            0:01.656   0:07.626    4.61
-affine 1,0,0.785,1,0,0 -transform               0:08.703   2:35.861   17.91
-black-threshold 20%                             0:01.921   0:07.797    4.06
-blur 0x0.5                                      0:05.719   0:14.391    2.52
-blur 0x1.0                                      0:06.141   0:15.093    2.46
-border 6x6                                      0:02.219   0:09.047    4.08
-charcoal 0x1                                    0:13.516   0:24.828    1.84
-chop 800x600+200+300                            0:01.812   0:07.703    4.25
-colorspace CMYK                                 0:02.047   0:09.344    4.56
-colorspace GRAY                                 0:03.234   0:09.718    3.00
-colorspace HSL                                  0:03.578   0:09.672    2.70
-colorspace HWB                                  0:03.047   0:09.406    3.09
-colorspace OHTA                                 0:03.250   0:10.235    3.15
-colorspace YCbCr                                0:03.234   0:10.234    3.16
-colorspace YIQ                                  0:03.328   0:10.219    3.07
-colorspace YUV                                  0:03.250   0:10.250    3.15
-contrast -contrast -contrast                    0:11.016   0:18.656    1.69
+contrast +contrast +contrast                    0:11.344   0:18.656    1.64
-convolve 1,1,1,1,4,1,1,1,1                      0:03.703   0:10.032    2.71
-colorize 30%/20%/50%                            0:02.782   0:11.547    4.15
-crop 1700x900+100+100                           0:02.063   0:08.610    4.17
-despeckle                                       0:46.141   2:43.596    3.55
-edge 0x1                                        0:05.500   0:11.250    2.05
-emboss 0x1                                      0:06.297   0:18.047    2.87
-enhance                                         0:15.266   0:19.360    1.27
-equalize                                        0:02.469   0:09.594    3.89
-flip                                            0:02.219   0:09.140    4.12
-flop                                            0:02.234   0:09.251    4.14
-frame 15x15+3+3                                 0:02.250   0:09.203    4.09
-gamma 1.6                                       0:02.000   0:08.016    4.01
-gaussian 0x0.5                                  0:05.265   0:11.344    2.15
-gaussian 0x1.0                                  0:09.828   0:15.750    1.60
-implode 0.5                                     0:06.829   0:15.844    2.32
-implode -1                                      0:06.828   0:15.906    2.33
-lat 10x10-5%                                    0:10.094   0:18.453    1.83
-level 10%,1.2,90%                               0:02.109   0:33.813   16.03
-median 1                                        0:27.094   1:41.157    3.73
-median 2                                        1:10.860   3:36.674    3.06
-modulate 110/100/95                             0:03.922   0:10.078    2.57
-motion-blur 0x3+30                              1:10.860   1:01.407    0.87
-negate                                          0:01.937   0:07.906    4.08
+noise Uniform                                   0:04.015   0:20.485    5.10
+noise Gaussian                                  0:13.110   0:55.220    4.21
+noise Multiplicative                            0:09.750   0:40.282    4.13
+noise Impulse                                   0:04.313   0:18.422    4.27
+noise Laplacian                                 0:06.250   0:28.891    4.62
+noise Poisson                                   0:09.359   0:42.625    4.55
-noise 1                                         0:26.812   1:04.016    2.39
-noise 2                                         1:11.016   2:19.486    1.96
-normalize                                       0:02.391   0:09.594    4.01
-fill blue -fuzz 35% -opaque red                 0:01.875   0:08.031    4.28
-paint 0x1                                       0:09.328   0:15.641    1.68
-raise 10x10                                     0:01.718   0:08.016    4.67
-density 75x75 -resample 50x50                   0:04.953   0:12.313    2.49
-recolor '1,0,0,0,1,0,0,0,1'                     0:01.625   0:09.094    5.60
-recolor '0,0,1,0,1,0,1,0,0'                     0:02.406   0:09.109    3.79
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:02.797   0:09.094    3.25
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:02.735   0:09.110    3.33
-resize 10%                                      0:02.796   0:09.000    3.22
-resize 50%                                      0:04.172   0:10.500    2.52
-resize 150%                                     0:10.594   0:16.954    1.60
-roll +20+10                                     0:03.125   0:09.031    2.89
-rotate 0                                        0:01.641   0:07.562    4.61
-rotate 15                                       0:09.781   0:27.828    2.85
-rotate 45                                       0:20.781   0:37.579    1.81
-rotate 90                                       0:02.422   0:08.438    3.48
-rotate 180                                      0:02.235   0:09.063    4.06
-rotate 270                                      0:02.391   0:08.484    3.55
-shade 30x30                                     0:02.891   0:09.359    3.24
-sharpen 0x0.5                                   0:05.328   0:11.344    2.13
-sharpen 0x1.0                                   0:09.844   0:15.641    1.59
-shave 10x10                                     0:02.188   0:08.953    4.09
-shear 45x45                                     0:15.172   0:40.563    2.67
-solarize 50%                                    0:01.938   0:07.781    4.01
-spread 1                                        0:02.391   0:18.657    7.80
-spread 3                                        0:02.453   0:18.407    7.50
-swirl 90                                        0:06.328   0:15.391    2.43
-threshold 35%                                   0:02.219   0:07.859    3.54
-fuzz 35% -transparent red                       0:01.953   0:08.391    4.30
-trim                                            0:02.000   0:09.406    4.70
-unsharp 0x0.5+20+1                              0:06.250   0:15.203    2.43
-unsharp 0x1.0+20+1                              0:06.985   0:15.734    2.25
-wave 25x150                                     0:07.063   0:16.688    2.36
-white-threshold 80%                             0:01.890   0:07.813    4.13
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
-noop                                            0:01.062   0:04.719    4.44
-affine 1,0,0.785,1,0,0 -transform               0:09.000   2:45.611   18.40
-black-threshold 20%                             0:01.313   0:05.000    3.81
-blur 0x0.5                                      0:04.000   0:11.266    2.82
-blur 0x1.0                                      0:04.625   0:12.000    2.59
-border 6x6                                      0:01.656   0:06.219    3.76
-charcoal 0x1                                    0:10.203   0:19.734    1.93
-chop 800x600+200+300                            0:01.234   0:04.859    3.94
-colorspace CMYK                                 0:01.469   0:06.031    4.11
-colorspace GRAY                                 0:02.187   0:05.890    2.69
-colorspace HSL                                  0:02.828   0:06.531    2.31
-colorspace HWB                                  0:02.109   0:06.156    2.92
-colorspace OHTA                                 0:02.156   0:06.453    2.99
-colorspace YCbCr                                0:02.172   0:06.422    2.96
-colorspace YIQ                                  0:02.187   0:06.438    2.94
-colorspace YUV                                  0:02.172   0:06.422    2.96
-contrast -contrast -contrast                    0:12.047   0:19.016    1.58
+contrast +contrast +contrast                    0:11.922   0:18.938    1.59
-convolve 1,1,1,1,4,1,1,1,1                      0:02.469   0:06.610    2.68
-colorize 30%/20%/50%                            0:01.875   0:07.203    3.84
-crop 1700x900+100+100                           0:01.500   0:05.781    3.85
-despeckle                                       0:25.781   1:26.766    3.37
-edge 0x1                                        0:02.531   0:08.032    3.17
-emboss 0x1                                      0:04.688   0:14.593    3.11
-enhance                                         0:13.172   0:14.859    1.13
-equalize                                        0:01.828   0:06.703    3.67
-flip                                            0:01.641   0:06.250    3.81
-flop                                            0:01.657   0:06.281    3.79
-frame 15x15+3+3                                 0:01.687   0:06.172    3.66
-gamma 1.6                                       0:01.437   0:05.579    3.88
-gaussian 0x0.5                                  0:03.750   0:07.906    2.11
-gaussian 0x1.0                                  0:08.234   0:12.063    1.47
-implode 0.5                                     0:07.828   0:14.625    1.87
-implode -1                                      0:07.828   0:14.609    1.87
-lat 10x10-5%                                    0:10.375   0:17.328    1.67
-level 10%,1.2,90%                               0:01.593   0:37.953   23.82
-median 1                                        0:26.125   1:04.641    2.47
-median 2                                        1:06.719   2:24.642    2.17
-modulate 110/100/95                             0:03.203   0:06.734    2.10
-motion-blur 0x3+30                              1:34.032   0:57.656    0.61
-negate                                          0:01.344   0:05.031    3.74
+noise Uniform                                   0:03.156   0:12.734    4.03
+noise Gaussian                                  0:25.422   1:40.579    3.96
+noise Multiplicative                            0:14.125   0:55.406    3.92
+noise Impulse                                   0:03.312   0:11.251    3.40
+noise Laplacian                                 0:08.953   0:35.313    3.94
+noise Poisson                                   0:11.859   0:51.406    4.33
-noise 1                                         0:25.813   0:55.406    2.15
-noise 2                                         1:06.110   2:21.922    2.15
-normalize                                       0:01.750   0:06.687    3.82
-fill blue -fuzz 35% -opaque red                 0:01.297   0:05.219    4.02
-paint 0x1                                       0:04.719   0:11.359    2.41
-raise 10x10                                     0:01.172   0:04.906    4.19
-density 75x75 -resample 50x50                   0:04.156   0:09.203    2.21
-recolor '1,0,0,0,1,0,0,0,1'                     0:01.062   0:06.063    5.71
-recolor '0,0,1,0,1,0,1,0,0'                     0:01.938   0:06.093    3.14
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:01.860   0:06.093    3.28
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:01.828   0:06.094    3.33
-resize 10%                                      0:02.312   0:06.141    2.66
-resize 50%                                      0:03.500   0:07.703    2.20
-resize 150%                                     0:08.609   0:12.828    1.49
-roll +20+10                                     0:02.500   0:06.141    2.46
-rotate 0                                        0:01.062   0:04.703    4.43
-rotate 15                                       0:08.422   0:21.657    2.57
-rotate 45                                       0:19.001   0:30.203    1.59
-rotate 90                                       0:01.890   0:05.906    3.12
-rotate 180                                      0:01.672   0:06.235    3.73
-rotate 270                                      0:01.890   0:05.891    3.12
-shade 30x30                                     0:02.282   0:06.485    2.84
-sharpen 0x0.5                                   0:03.750   0:07.906    2.11
-sharpen 0x1.0                                   0:08.312   0:12.063    1.45
-shave 10x10                                     0:01.641   0:06.171    3.76
-shear 45x45                                     0:14.797   0:34.094    2.30
-solarize 50%                                    0:01.328   0:04.906    3.69
-spread 1                                        0:01.797   0:14.062    7.83
-spread 3                                        0:01.890   0:14.141    7.48
-swirl 90                                        0:07.031   0:14.297    2.03
-threshold 35%                                   0:01.562   0:05.047    3.23
-fuzz 35% -transparent red                       0:01.375   0:05.454    3.97
-trim                                            0:01.531   0:06.656    4.35
-unsharp 0x0.5+20+1                              0:04.515   0:11.531    2.55
-unsharp 0x1.0+20+1                              0:05.141   0:11.969    2.33
-wave 25x150                                     0:07.578   0:14.782    1.95
-white-threshold 80%                             0:01.265   0:04.906    3.88
============================================== ========== ========== ===========


Windows XP / Cygwin / Intel Core 2 Quad
---------------------------------------

Cygwin 1.7.1 was installed on the same Windows XP system with the 2.83
GHz Core 2 Quad Processor (Q9550) as described above.  The compiler
identifies itself as "gcc (GCC) 4.3.4 20090804 (release) 1".  This
build supports OpenMP:

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:01.928   0:08.197    4.25
-affine 1,0,0.785,1,0,0 -transform               0:12.905 165:35.730  769.91
-black-threshold 20%                             0:02.266   0:08.521    3.76
-blur 0x0.5                                      0:06.421   0:16.578    2.58
-blur 0x1.0                                      0:06.976   0:17.301    2.48
-border 6x6                                      0:02.761   0:09.652    3.50
-charcoal 0x1                                    0:13.793   0:27.620    2.00
-chop 800x600+200+300                            0:02.207   0:08.325    3.77
-colorspace CMYK                                 0:02.406   0:09.997    4.16
-colorspace GRAY                                 0:03.588   0:10.377    2.89
-colorspace HSL                                  0:03.928   0:10.463    2.66
-colorspace HWB                                  0:03.473   0:10.031    2.89
-colorspace OHTA                                 0:03.582   0:10.945    3.06
-colorspace YCbCr                                0:03.577   0:10.877    3.04
-colorspace YIQ                                  0:03.582   0:10.897    3.04
-colorspace YUV                                  0:03.652   0:10.845    2.97
-contrast -contrast -contrast                    0:11.403   0:17.629    1.55
+contrast +contrast +contrast                    0:10.605   0:17.189    1.62
-convolve 1,1,1,1,4,1,1,1,1                      0:04.132   0:10.717    2.59
-colorize 30%/20%/50%                            0:03.213   0:11.686    3.64
-crop 1700x900+100+100                           0:02.569   0:09.206    3.58
-despeckle                                       0:33.497   1:51.932    3.34
-edge 0x1                                        0:03.988   0:11.719    2.94
-emboss 0x1                                      0:06.354   0:18.172    2.86
-enhance                                         0:14.415   0:19.149    1.33
-equalize                                        0:02.956   0:10.183    3.44
-flip                                            0:02.705   0:09.688    3.58
-flop                                            0:02.756   0:09.671    3.51
-frame 15x15+3+3                                 0:03.000   0:09.611    3.20
-gamma 1.6                                       0:02.432   0:08.730    3.59
-gaussian 0x0.5                                  0:05.312   0:11.819    2.22
-gaussian 0x1.0                                  0:09.502   0:15.750    1.66
-implode 0.5                                     0:09.004   0:21.601    2.40
-implode -1                                      0:06.632   0:18.941    2.86
-lat 10x10-5%                                    0:10.017   0:18.333    1.83
-level 10%,1.2,90%                               0:02.735   0:45.821   16.75
-median 1                                        0:25.805   0:43.627    1.69
-median 2                                        1:05.236   1:30.799    1.39
-modulate 110/100/95                             0:04.351   0:10.546    2.42
-motion-blur 0x3+30                              5:22.296   8:07.893    1.51
-negate                                          0:02.259   0:08.448    3.74
+noise Uniform                                   0:07.420   0:21.944    2.96
+noise Gaussian                                  0:22.922   1:18.283    3.42
+noise Multiplicative                            0:15.800   0:53.022    3.36
+noise Impulse                                   0:07.260   0:20.299    2.80
+noise Laplacian                                 0:11.288   0:36.685    3.25
+noise Poisson                                   0:14.427   0:52.164    3.62
-noise 1                                         0:27.148   0:41.649    1.53
-noise 2                                         1:07.402   1:30.261    1.34
-normalize                                       0:02.927   0:10.246    3.50
-fill blue -fuzz 35% -opaque red                 0:02.231   0:08.701    3.90
-paint 0x1                                       0:06.312   0:15.300    2.42
-raise 10x10                                     0:02.081   0:08.530    4.10
-density 75x75 -resample 50x50                   0:05.725   0:13.540    2.37
-recolor '1,0,0,0,1,0,0,0,1'                     0:01.911   0:09.518    4.98
-recolor '0,0,1,0,1,0,1,0,0'                     0:02.706   0:09.521    3.52
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:03.015   0:09.739    3.23
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:02.991   0:09.576    3.20
-resize 10%                                      0:03.237   0:09.364    2.89
-resize 50%                                      0:04.873   0:11.472    2.35
-resize 150%                                     0:13.869   0:19.895    1.43
-roll +20+10                                     0:03.489   0:09.533    2.73
-rotate 0                                        0:01.924   0:07.944    4.13
-rotate 15                                       0:11.410   0:29.659    2.60
-rotate 45                                       0:22.465   0:39.599    1.76
-rotate 90                                       0:03.222   0:10.516    3.26
-rotate 180                                      0:02.745   0:09.379    3.42
-rotate 270                                      0:03.196   0:10.911    3.41
-shade 30x30                                     0:04.260   0:09.986    2.34
-sharpen 0x0.5                                   0:05.298   0:11.716    2.21
-sharpen 0x1.0                                   0:09.476   0:15.528    1.64
-shave 10x10                                     0:02.736   0:09.476    3.46
-shear 45x45                                     0:16.381   0:50.045    3.06
-solarize 50%                                    0:02.270   0:08.177    3.60
-spread 1                                        0:04.427   0:23.959    5.41
-spread 3                                        0:04.625   0:24.148    5.22
-swirl 90                                        0:06.326   0:18.430    2.91
-threshold 35%                                   0:02.483   0:08.241    3.32
-fuzz 35% -transparent red                       0:02.352   0:08.988    3.82
-trim                                            0:02.494   0:09.961    3.99
-unsharp 0x0.5+20+1                              0:06.895   0:17.157    2.49
-unsharp 0x1.0+20+1                              0:07.493   0:17.920    2.39
-wave 25x150                                     1:19.003   2:51.214    2.17
-white-threshold 80%                             0:02.283   0:08.153    3.57
============================================== ========== ========== ===========

Intel Pentium IV / Cygwin
-------------------------

This system is a 2000 vintage 2.0GHz Pentium IV system with Windows XP
SP3 and Cygwin 1.5.25.  This system has only one CPU core and the
compiler (gcc version 3.4.4 (cygming special, gdc 0.12, using dmd
0.125) does not support OpenMP so this is a single-threaded
configuration.  It can be seen that ImageMagick takes much longer to
generate its tiled input image and to even get started (first -noop
entry).  GraphicsMagick is faster for all algorithms:

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:04.178   0:07.683    1.84
-affine 1,0,0.785,1,0,0 -transform               1:14.068  28:20.037   22.95
-black-threshold 20%                             0:05.673   0:09.997    1.76
-blur 0x0.5                                      0:34.525   1:58.798    3.44
-blur 0x1.0                                      0:41.269   2:06.509    3.07
-border 6x6                                      0:06.623   0:10.110    1.53
-charcoal 0x1                                    1:33.932   3:23.937    2.17
-chop 800x600+200+300                            0:04.855   0:08.589    1.77
-colorspace CMYK                                 0:06.039   0:22.370    3.70
-colorspace GRAY                                 0:11.963   0:17.510    1.46
-colorspace HSL                                  0:26.458   0:28.692    1.08
-colorspace HWB                                  0:19.366   0:24.247    1.25
-colorspace OHTA                                 0:12.123   0:19.885    1.64
-colorspace YCbCr                                0:12.437   0:19.869    1.60
-colorspace YIQ                                  0:12.294   0:19.906    1.62
-colorspace YUV                                  0:12.456   0:19.897    1.60
-contrast -contrast -contrast                    2:02.015   2:25.974    1.20
+contrast +contrast +contrast                    1:52.509   2:13.817    1.19
-convolve 1,1,1,1,4,1,1,1,1                      0:25.349   0:35.895    1.42
-colorize 30%/20%/50%                            0:13.520   0:17.376    1.29
-crop 1700x900+100+100                           0:05.929   0:09.521    1.61
-despeckle                                       4:19.391   5:24.702    1.25
-edge 0x1                                        0:24.386   0:52.759    2.16
-emboss 0x1                                      0:48.020   2:05.825    2.62
-enhance                                         2:55.869   2:32.214    0.87
-equalize                                        0:08.240   0:21.104    2.56
-flip                                            0:06.516   0:09.964    1.53
-flop                                            0:06.386   0:09.994    1.56
-frame 15x15+3+3                                 0:06.583   0:10.235    1.55
-gamma 1.6                                       0:07.958   0:11.753    1.48
-gaussian 0x0.5                                  0:42.076   0:53.634    1.27
-gaussian 0x1.0                                  1:46.263   1:48.271    1.02
-implode 0.5                                     2:02.628   2:56.145    1.44
-implode -1                                      1:16.693   2:04.869    1.63
-lat 10x10-5%                                    1:24.813   1:54.310    1.35
-level 10%,1.2,90%                               0:07.713  11:47.057   91.67
-median 1                                        5:27.383   6:27.662    1.18
-median 2                                       15:15.335  16:47.217    1.10
-modulate 110/100/95                             0:31.605   0:35.805    1.13
-motion-blur 0x3+30                              3:03.748  11:02.876    3.61
-negate                                          0:04.996   0:08.797    1.76
+noise Uniform                                   0:43.692   0:51.613    1.18
+noise Gaussian                                  4:31.673   4:34.014    1.01
+noise Multiplicative                            2:47.432   2:56.038    1.05
+noise Impulse                                   0:41.934   0:42.731    1.02
+noise Laplacian                                 1:35.802   1:37.336    1.02
+noise Poisson                                   2:42.987   2:39.194    0.98
-noise 1                                         5:34.284   6:39.437    1.19
-noise 2                                        15:28.059  17:01.218    1.10
-normalize                                       0:08.639   0:21.953    2.54
-fill blue -fuzz 35% -opaque red                 0:06.082   0:14.030    2.31
-paint 0x1                                       0:53.363   1:38.203    1.84
-raise 10x10                                     0:04.550   0:08.105    1.78
-density 75x75 -resample 50x50                   0:39.429   1:15.942    1.93
-recolor '1,0,0,0,1,0,0,0,1'                     0:04.141   0:20.278    4.90
-recolor '0,0,1,0,1,0,1,0,0'                     0:12.841   0:20.466    1.59
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:17.110   0:20.453    1.20
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:17.111   0:20.392    1.19
-resize 10%                                      0:21.119   0:37.593    1.78
-resize 50%                                      0:29.906   0:52.624    1.76
-resize 150%                                     1:35.072   2:31.977    1.60
-roll +20+10                                     0:06.662   0:10.019    1.50
-rotate 0                                        0:04.111   0:07.907    1.92
-rotate 15                                       1:01.096   2:13.312    2.18
-rotate 45                                       3:15.993   4:28.199    1.37
-rotate 90                                       0:09.776   0:14.513    1.48
-rotate 180                                      0:06.315   0:09.697    1.54
-rotate 270                                      0:09.819   0:14.594    1.49
-shade 30x30                                     0:34.224   0:25.400    0.74
-sharpen 0x0.5                                   0:41.902   0:53.673    1.28
-sharpen 0x1.0                                   1:46.532   1:48.107    1.01
-shave 10x10                                     0:06.379   0:09.751    1.53
-shear 45x45                                     2:10.209   5:53.412    2.71
-solarize 50%                                    0:05.004   0:09.211    1.84
-spread 1                                        0:11.297   2:03.593   10.94
-spread 3                                        0:11.024   2:03.900   11.24
-swirl 90                                        1:05.565   1:55.078    1.76
-threshold 35%                                   0:06.181   0:09.977    1.61
-fuzz 35% -transparent red                       0:06.400   0:14.938    2.33
-trim                                            0:08.227   0:10.946    1.33
-unsharp 0x0.5+20+1                              0:43.320   2:08.170    2.96
-unsharp 0x1.0+20+1                              0:50.079   2:15.643    2.71
-wave 25x150                                     1:05.935   2:10.486    1.98
-white-threshold 80%                             0:05.503   0:08.967    1.63
============================================== ========== ========== ===========


--------------------------------------------------------------------------

| Copyright (C) 2008-2010 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/Copyright.html.

