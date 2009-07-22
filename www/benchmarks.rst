.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

==========================================================
GraphicsMagick 1.3.6 vs ImageMagick 6.5.4 Benchmark Report
==========================================================

.. _`GraphicsMagick 1.2 Benchmarks` : benchmarks-1.2.html

.. contents::
  :local:

Introduction
============

GraphicsMagick 1.3.6 has been benchmarked against the most recent
ImageMagick release available at time of release. ImageMagick has
recently been significantly updated to use OpenMP as has
GraphicsMagick 1.3.6. Several disparate types of systems were used in
our testing. We are pleased to see that ImageMagick's performance has
improved significantly on the benchmark that we posted earlier (see
`GraphicsMagick 1.2 Benchmarks`_). It seems that access to the useful
benchmark and GraphicsMagick source code has been quite helpful to
ImageMagick in order to resolve many of the performance issues.

Executive Summary
=================

GraphicsMagick is typically 1.5 to 5X faster at executing image
processing operations from the command line than ImageMagick 6.5.4 is.
Some ImageMagick algorithms run as much as 97X slower. ImageMagick
6.5.4 only runs adequately on the latest high-end hardware with large
caches, and runs poorly on most of the installed computing base. Even
on the latest high-end hardware, GraphicsMagick is much more efficient
and uses much less CPU than ImageMagick.

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
sized (1920x1080 pixels) images. In order to prevent disk I/O from
being a factor, we used a small input image and tiled it to create a
larger input image via the "tile:" coder. The processed image is sent
to the "null:" coder so that file writes to a slow disk are also not a
factor.  Static executables are used and executed via full paths in
order to minimize variability from the execution environment. In order
to obtain accurate and useful timing, we used the bash shell to
execute the command 20 times and see how long it took. This is a very
simple benchmark approach which is quite representative of the
performance that the typical user observes.

This new benchmark reveals the following performance criteria:

  * Basic execution overhead of the software.
  
  * Image processing algorithmic efficiency.
  
  * Efficacy of OpenMP enhancements.

The software involved in the testing identified itself as::

  GraphicsMagick 1.3.6 unreleased Q16 http://www.GraphicsMagick.org/

  ImageMagick 6.5.4-3 2009-07-19 Q16 OpenMP http://www.imagemagick.org

The Benchmark
=============

The benchmark is quite simple. It reads a list of commands to execute
from a file named "commands.txt" and times how long it takes to
execute each command 20 times using GraphicsMagick and ImageMagick.

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
ImageMagick to execute it twenty times, and the ImageMagick time
divided by the GraphicsMagick time. In order to understand the "IM/GM
ratio" column, a value of 0.50 indicates that GM is half as fast as
IM, a value of 1.00 indicates that GM and IM are the same speed, and a
value of 4.00 indicates that GM is four times as fast as IM.

AMD Opteron/Solaris 10
----------------------

This system is a modern AMD Opteron system with two dual-core 3.0GHz
CPUs and running Solaris 10 U5. The compiler used identifies itself as
"gcc (GCC) 4.3.3".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:00.614   0:00.763    1.24
-affine 1,0,0.785,1,0,0 -transform               0:04.958   8:01.232   97.06
-black-threshold 20%                             0:00.698   0:00.981    1.41
-blur 0x0.5                                      0:02.982   0:06.721    2.25
-blur 0x1.0                                      0:03.459   0:06.613    1.91
-border 6x6                                      0:00.981   0:02.989    3.05
-charcoal 0x1                                    0:06.634   0:12.480    1.88
-chop 800x600+200+300                            0:00.693   0:01.071    1.55
-colorspace CMYK                                 0:00.747   0:01.988    2.66
-colorspace GRAY                                 0:00.915   0:01.445    1.58
-colorspace HSL                                  0:01.527   0:02.150    1.41
-colorspace HWB                                  0:01.189   0:01.930    1.62
-colorspace OHTA                                 0:00.914   0:01.898    2.08
-colorspace YCbCr                                0:00.907   0:01.904    2.10
-colorspace YIQ                                  0:00.918   0:01.881    2.05
-colorspace YUV                                  0:00.914   0:01.900    2.08
-contrast -contrast -contrast                    0:06.654   0:06.800    1.02
+contrast +contrast +contrast                    0:06.156   0:06.304    1.02
-convolve 1,1,1,1,4,1,1,1,1                      0:01.654   0:02.305    1.39
-colorize 30%/20%/50%                            0:01.210   0:02.489    2.06
-crop 1700x900+100+100                           0:00.860   0:01.461    1.70
-despeckle                                       0:12.261   0:12.800    1.04
-edge 0x1                                        0:02.324   0:03.016    1.30
-emboss 0x1                                      0:03.161   0:07.010    2.22
-enhance                                         0:08.161   0:07.222    0.88
-equalize                                        0:01.288   0:02.299    1.78
-flip                                            0:00.953   0:01.559    1.64
-flop                                            0:01.040   0:01.480    1.42
-frame 15x15+3+3                                 0:01.047   0:02.370    2.26
-gamma 1.6                                       0:00.862   0:00.928    1.08
-gaussian 0x0.5                                  0:02.257   0:02.981    1.32
-gaussian 0x1.0                                  0:04.436   0:05.095    1.15
-implode 0.5                                     0:04.992   0:06.498    1.30
-implode -1                                      0:04.409   0:05.856    1.33
-lat 10x10-5%                                    0:06.194   0:07.648    1.23
-level 10%,1.2,90%                               0:00.865   0:04.918    5.69
-median 1                                        0:13.637   0:36.669    2.69
-median 2                                        0:33.319   1:19.413    2.38
-modulate 110/100/95                             0:01.809   0:01.875    1.04
-motion-blur 0x3+30                              0:20.037   1:21.030    4.04
-negate                                          0:00.656   0:00.882    1.34
+noise Uniform                                   0:02.643   0:05.889    2.23
+noise Gaussian                                  0:12.013   0:41.193    3.43
+noise Multiplicative                            0:08.062   0:27.656    3.43
+noise Impulse                                   0:02.639   0:05.110    1.94
+noise Laplacian                                 0:04.397   0:14.492    3.30
+noise Poisson                                   0:06.073   0:20.020    3.30
-noise 1                                         0:13.681   0:37.003    2.70
-noise 2                                         0:33.581   1:19.499    2.37
-normalize                                       0:01.148   0:02.136    1.86
-fill blue -fuzz 35% -opaque red                 0:00.736   0:01.207    1.64
-paint 0x1                                       0:04.093   0:05.946    1.45
-raise 10x10                                     0:00.607   0:00.839    1.38
-density 75x75 -resample 50x50                   0:02.408   0:04.989    2.07
-recolor '1,0,0,0,1,0,0,0,1'                     0:00.575   0:01.716    2.98
-recolor '0,0,1,0,1,0,1,0,0'                     0:00.845   0:01.694    2.00
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:00.935   0:01.620    1.73
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:00.940   0:01.625    1.73
-resize 10%                                      0:01.305   0:01.680    1.29
-resize 50%                                      0:01.788   0:02.587    1.45
-resize 150%                                     0:06.742   0:08.638    1.28
-roll +20+10                                     0:00.976   0:01.603    1.64
-rotate 0                                        0:01.027   0:01.481    1.44
-rotate 45                                       0:14.756   0:19.704    1.34
-rotate 90                                       0:01.504   0:01.861    1.24
-rotate 180                                      0:01.040   0:01.494    1.44
-rotate 270                                      0:01.503   0:01.910    1.27
-shade 30x30                                     0:02.127   0:02.577    1.21
-sharpen 0x0.5                                   0:02.252   0:02.962    1.32
-sharpen 0x1.0                                   0:04.509   0:05.094    1.13
-shave 10x10                                     0:00.954   0:01.632    1.71
-shear 45x45                                     0:10.692   0:26.365    2.47
-solarize 50%                                    0:00.706   0:00.850    1.20
-spread 1                                        0:01.068   0:06.055    5.67
-spread 3                                        0:01.117   0:06.116    5.48
-swirl 90                                        0:04.403   0:05.827    1.32
-threshold 35%                                   0:00.792   0:00.909    1.15
-fuzz 35% -transparent red                       0:00.762   0:01.343    1.76
-trim                                            0:00.910   0:01.710    1.88
-unsharp 0x0.5+20+1                              0:03.224   0:07.099    2.20
-unsharp 0x1.0+20+1                              0:03.738   0:07.161    1.92
-wave 25x150                                     0:06.158   0:12.154    1.97
-white-threshold 80%                             0:00.655   0:00.902    1.38
============================================== ========== ========== ===========

Sun SPARC/Solaris 10
--------------------

This system is a 2004 vintage Sun SPARC workstation with two 1.2GHz
CPUs and running Solaris 10 U5. The compiler used identifies itself as
"gcc (GCC) 4.2.4".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:02.139   0:03.632    1.70
-affine 1,0,0.785,1,0,0 -transform               0:24.924   8:12.578   19.76
-black-threshold 20%                             0:03.133   0:05.070    1.62
-blur 0x0.5                                      0:16.626   0:29.739    1.79
-blur 0x1.0                                      0:19.570   0:32.860    1.68
-border 6x6                                      0:04.187   0:16.900    4.04
-charcoal 0x1                                    0:42.807   1:07.120    1.57
-chop 800x600+200+300                            0:02.959   0:04.559    1.54
-colorspace CMYK                                 0:03.611   0:10.008    2.77
-colorspace GRAY                                 0:05.124   0:07.412    1.45
-colorspace HSL                                  0:08.583   0:11.069    1.29
-colorspace HWB                                  0:07.654   0:10.658    1.39
-colorspace OHTA                                 0:05.123   0:08.867    1.73
-colorspace YCbCr                                0:05.143   0:08.768    1.70
-colorspace YIQ                                  0:05.204   0:08.708    1.67
-colorspace YUV                                  0:05.105   0:08.765    1.72
-contrast -contrast -contrast                    0:42.214   0:44.926    1.06
+contrast +contrast +contrast                    0:40.406   0:43.281    1.07
-convolve 1,1,1,1,4,1,1,1,1                      0:11.025   0:15.909    1.44
-colorize 30%/20%/50%                            0:05.949   0:12.202    2.05
-crop 1700x900+100+100                           0:03.692   0:05.654    1.53
-despeckle                                       1:58.066   2:02.395    1.04
-edge 0x1                                        0:17.056   0:22.059    1.29
-emboss 0x1                                      0:22.330   0:54.474    2.44
-enhance                                         0:44.200   0:39.563    0.90
-equalize                                        0:05.904   0:11.443    1.94
-flip                                            0:04.155   0:06.330    1.52
-flop                                            0:04.684   0:06.822    1.46
-frame 15x15+3+3                                 0:04.404   0:15.307    3.48
-gamma 1.6                                       0:04.374   0:04.872    1.11
-gaussian 0x0.5                                  0:17.712   0:22.766    1.29
-gaussian 0x1.0                                  0:41.183   0:45.582    1.11
-implode 0.5                                     0:23.730   0:35.386    1.49
-implode -1                                      0:19.569   0:31.643    1.62
-lat 10x10-5%                                    0:40.424   1:32.463    2.29
-level 10%,1.2,90%                               0:04.289   5:11.062   72.53
-median 1                                        1:01.983   1:42.939    1.66
-median 2                                        2:27.795   3:48.450    1.55
-modulate 110/100/95                             0:12.601   0:16.079    1.28
-motion-blur 0x3+30                              1:03.390   3:06.800    2.95
-negate                                          0:03.248   0:04.544    1.40
+noise Uniform                                   0:13.673   0:25.271    1.85
+noise Gaussian                                  1:02.651   1:59.479    1.91
+noise Multiplicative                            0:40.231   1:14.469    1.85
+noise Impulse                                   0:13.992   0:26.110    1.87
+noise Laplacian                                 0:23.465   0:48.318    2.06
+noise Poisson                                   0:37.368   1:12.388    1.94
-noise 1                                         1:03.703   1:44.714    1.64
-noise 2                                         2:30.084   3:50.108    1.53
-normalize                                       0:05.480   0:11.091    2.02
-fill blue -fuzz 35% -opaque red                 0:04.263   0:06.147    1.44
-paint 0x1                                       0:27.532   0:30.058    1.09
-raise 10x10                                     0:02.222   0:03.798    1.71
-density 75x75 -resample 50x50                   0:14.966   0:25.654    1.71
-recolor '1,0,0,0,1,0,0,0,1'                     0:02.147   0:09.774    4.55
-recolor '0,0,1,0,1,0,1,0,0'                     0:05.216   0:09.781    1.88
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:06.191   0:09.841    1.59
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:05.998   0:09.889    1.65
-resize 10%                                      0:07.993   0:10.222    1.28
-resize 50%                                      0:11.460   0:16.420    1.43
-resize 150%                                     0:38.023   0:47.927    1.26
-roll +20+10                                     0:04.342   0:06.925    1.59
-rotate 0                                        0:04.291   0:06.446    1.50
-rotate 45                                       1:12.913   1:34.116    1.29
-rotate 90                                       0:05.236   0:07.177    1.37
-rotate 180                                      0:04.684   0:06.852    1.46
-rotate 270                                      0:05.298   0:07.284    1.37
-shade 30x30                                     0:12.515   0:14.671    1.17
-sharpen 0x0.5                                   0:17.984   0:22.726    1.26
-sharpen 0x1.0                                   0:41.637   0:45.964    1.10
-shave 10x10                                     0:04.144   0:06.073    1.47
-shear 45x45                                     0:47.275   1:38.130    2.08
-solarize 50%                                    0:03.225   0:05.173    1.60
-spread 1                                        0:04.860   0:34.943    7.19
-spread 3                                        0:05.189   0:35.059    6.76
-swirl 90                                        0:19.597   0:30.428    1.55
-threshold 35%                                   0:03.766   0:04.868    1.29
-fuzz 35% -transparent red                       0:04.554   0:07.368    1.62
-trim                                            0:06.492   0:07.307    1.13
-unsharp 0x0.5+20+1                              0:19.423   0:35.032    1.80
-unsharp 0x1.0+20+1                              0:22.229   0:37.837    1.70
-wave 25x150                                     0:19.587   0:35.062    1.79
-white-threshold 80%                             0:03.078   0:05.107    1.66
============================================== ========== ========== ===========

Apple PowerPC G5/OS-X Tiger
---------------------------

This system is an Apple G5 system with two 2.5GHz CPUs and running the
Leopard release of OS-X. The compiler used identifies itself as
"gcc-4.4.0 (GCC) 4.4.0".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:02.083   0:02.633    1.26
-affine 1,0,0.785,1,0,0 -transform               0:12.942   3:53.603   18.05
-black-threshold 20%                             0:02.277   0:04.811    2.11
-blur 0x0.5                                      0:12.732   0:24.634    1.93
-blur 0x1.0                                      0:17.979   0:30.334    1.69
-border 6x6                                      0:02.952   0:07.884    2.67
-charcoal 0x1                                    0:35.721   0:52.884    1.48
-chop 800x600+200+300                            0:02.394   0:02.974    1.24
-colorspace CMYK                                 0:02.542   0:05.322    2.09
-colorspace GRAY                                 0:03.388   0:04.695    1.39
-colorspace HSL                                  0:04.878   0:05.970    1.22
-colorspace HWB                                  0:04.762   0:05.985    1.26
-colorspace OHTA                                 0:03.526   0:06.725    1.91
-colorspace YCbCr                                0:03.400   0:05.549    1.63
-colorspace YIQ                                  0:03.386   0:05.643    1.67
-colorspace YUV                                  0:03.500   0:05.560    1.59
-contrast -contrast -contrast                    0:14.963   0:20.936    1.40
+contrast +contrast +contrast                    0:14.519   0:20.502    1.41
-convolve 1,1,1,1,4,1,1,1,1                      0:08.827   0:10.169    1.15
-colorize 30%/20%/50%                            0:03.596   0:08.617    2.40
-crop 1700x900+100+100                           0:02.755   0:03.489    1.27
-despeckle                                       1:31.885   1:36.527    1.05
-edge 0x1                                        0:08.932   0:20.395    2.28
-emboss 0x1                                      0:18.385   1:00.396    3.29
-enhance                                         0:31.414   0:36.208    1.15
-equalize                                        0:02.959   0:04.926    1.66
-flip                                            0:02.942   0:03.769    1.28
-flop                                            0:03.018   0:03.797    1.26
-frame 15x15+3+3                                 0:02.988   0:07.060    2.36
-gamma 1.6                                       0:02.670   0:03.011    1.13
-gaussian 0x0.5                                  0:16.388   0:20.536    1.25
-gaussian 0x1.0                                  0:43.288   0:56.324    1.30
-implode 0.5                                     0:11.018   0:18.306    1.66
-implode -1                                      0:09.951   0:17.416    1.75
-lat 10x10-5%                                    0:46.117   1:20.379    1.74
-level 10%,1.2,90%                               0:02.583   0:09.701    3.76
-median 1                                        0:30.872   1:23.094    2.69
-median 2                                        1:14.108   3:06.731    2.52
-modulate 110/100/95                             0:05.958   0:06.174    1.04
-motion-blur 0x3+30                              1:02.270   2:05.290    2.01
-negate                                          0:02.295   0:02.846    1.24
+noise Uniform                                   0:06.874   0:12.123    1.76
+noise Gaussian                                  0:20.555   0:42.578    2.07
+noise Multiplicative                            0:14.761   0:29.132    1.97
+noise Impulse                                   0:07.300   0:10.839    1.48
+noise Laplacian                                 0:10.972   0:22.978    2.09
+noise Poisson                                   0:14.189   0:32.897    2.32
-noise 1                                         0:32.071   1:21.996    2.56
-noise 2                                         1:18.371   3:06.169    2.38
-normalize                                       0:02.847   0:05.565    1.95
-fill blue -fuzz 35% -opaque red                 0:02.678   0:03.816    1.42
-paint 0x1                                       0:06.556   0:10.921    1.67
-raise 10x10                                     0:02.191   0:02.871    1.31
-density 75x75 -resample 50x50                   0:12.676   0:19.063    1.50
-recolor '1,0,0,0,1,0,0,0,1'                     0:02.113   0:04.870    2.30
-recolor '0,0,1,0,1,0,1,0,0'                     0:04.348   0:04.911    1.13
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:04.204   0:04.833    1.15
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:04.210   0:04.887    1.16
-resize 10%                                      0:06.311   0:08.433    1.34
-resize 50%                                      0:09.372   0:12.492    1.33
-resize 150%                                     0:41.387   0:36.367    0.88
-roll +20+10                                     0:03.247   0:04.008    1.23
-rotate 0                                        0:03.013   0:03.763    1.25
-rotate 45                                       1:20.219   1:25.895    1.07
-rotate 90                                       0:03.501   0:06.096    1.74
-rotate 180                                      0:02.974   0:03.754    1.26
-rotate 270                                      0:03.521   0:06.206    1.76
-shade 30x30                                     0:04.781   0:11.316    2.37
-sharpen 0x0.5                                   0:16.570   0:20.580    1.24
-sharpen 0x1.0                                   0:44.614   0:59.067    1.32
-shave 10x10                                     0:02.954   0:03.778    1.28
-shear 45x45                                     1:25.798   2:01.872    1.42
-solarize 50%                                    0:02.370   0:03.754    1.58
-spread 1                                        0:03.188   0:22.535    7.07
-spread 3                                        0:03.480   0:23.020    6.61
-swirl 90                                        0:10.908   0:19.083    1.75
-threshold 35%                                   0:02.771   0:03.223    1.16
-fuzz 35% -transparent red                       0:02.643   0:04.450    1.68
-trim                                            0:04.319   0:04.550    1.05
-unsharp 0x0.5+20+1                              0:15.246   0:28.876    1.89
-unsharp 0x1.0+20+1                              0:20.400   0:34.547    1.69
-wave 25x150                                     0:11.531   0:21.076    1.83
-white-threshold 80%                             0:02.300   0:03.845    1.67
============================================== ========== ========== ===========

Intel Xeon / FreeBSD 7.0
------------------------

This system is a 2003 vintage Intel Xeon system with two 2.4GHz CPUs
and running FreeBSD 7.2. The compiler used identifies itself as "gcc
(GCC) 4.2.1 20070719 [FreeBSD]".

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:01.551   0:04.627    2.98
-affine 1,0,0.785,1,0,0 -transform               0:13.708   4:03.230   17.74
-black-threshold 20%                             0:01.882   0:05.005    2.66
-blur 0x0.5                                      0:07.407   0:21.603    2.92
-blur 0x1.0                                      0:08.400   0:22.682    2.70
-border 6x6                                      0:02.737   0:10.916    3.99
-charcoal 0x1                                    0:19.984   0:37.167    1.86
-chop 800x600+200+300                            0:01.874   0:05.058    2.70
-colorspace CMYK                                 0:02.471   0:07.491    3.03
-colorspace GRAY                                 0:03.065   0:07.760    2.53
-colorspace HSL                                  0:04.451   0:08.137    1.83
-colorspace HWB                                  0:03.386   0:07.812    2.31
-colorspace OHTA                                 0:03.103   0:08.821    2.84
-colorspace YCbCr                                0:03.042   0:08.833    2.90
-colorspace YIQ                                  0:03.032   0:08.896    2.93
-colorspace YUV                                  0:03.044   0:08.845    2.91
-contrast -contrast -contrast                    0:16.149   0:26.986    1.67
+contrast +contrast +contrast                    0:15.745   0:26.670    1.69
-convolve 1,1,1,1,4,1,1,1,1                      0:05.276   0:08.136    1.54
-colorize 30%/20%/50%                            0:03.359   0:10.214    3.04
-crop 1700x900+100+100                           0:02.412   0:05.908    2.45
-despeckle                                       0:52.760   1:17.377    1.47
-edge 0x1                                        0:05.348   0:09.946    1.86
-emboss 0x1                                      0:10.249   0:22.091    2.16
-enhance                                         0:29.246   0:28.948    0.99
-equalize                                        0:03.283   0:08.469    2.58
-flip                                            0:02.739   0:06.304    2.30
-flop                                            0:02.665   0:06.289    2.36
-frame 15x15+3+3                                 0:02.740   0:09.907    3.62
-gamma 1.6                                       0:02.696   0:05.288    1.96
-gaussian 0x0.5                                  0:07.819   0:10.276    1.31
-gaussian 0x1.0                                  0:16.524   0:17.132    1.04
-implode 0.5                                     0:15.215   0:25.348    1.67
-implode -1                                      0:09.463   0:19.789    2.09
-lat 10x10-5%                                    0:19.631   0:20.297    1.03
-level 10%,1.2,90%                               0:02.537   1:38.073   38.66
-median 1                                        0:50.405   1:19.104    1.57
-median 2                                        2:15.739   3:13.792    1.43
-modulate 110/100/95                             0:05.356   0:08.949    1.67
-motion-blur 0x3+30                              1:33.139   2:03.672    1.33
-negate                                          0:01.939   0:04.999    2.58
+noise Uniform                                   0:07.713   0:20.301    2.63
+noise Gaussian                                  0:20.851   1:00.360    2.89
+noise Multiplicative                            0:15.773   0:41.714    2.64
+noise Impulse                                   0:07.425   0:19.045    2.56
+noise Laplacian                                 0:10.294   0:29.619    2.88
+noise Poisson                                   0:17.386   1:05.555    3.77
-noise 1                                         0:50.786   1:10.328    1.38
-noise 2                                         2:13.231   2:56.097    1.32
-normalize                                       0:03.134   0:08.582    2.74
-fill blue -fuzz 35% -opaque red                 0:02.011   0:05.686    2.83
-paint 0x1                                       0:10.916   0:22.060    2.02
-raise 10x10                                     0:01.615   0:04.723    2.92
-density 75x75 -resample 50x50                   0:10.821   0:16.923    1.56
-recolor '1,0,0,0,1,0,0,0,1'                     0:01.541   0:07.340    4.76
-recolor '0,0,1,0,1,0,1,0,0'                     0:02.348   0:07.336    3.12
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:02.560   0:07.321    2.86
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:02.571   0:07.369    2.87
-resize 10%                                      0:05.428   0:08.613    1.59
-resize 50%                                      0:08.447   0:11.416    1.35
-resize 150%                                     0:32.875   0:28.984    0.88
-roll +20+10                                     0:04.013   0:06.425    1.60
-rotate 0                                        0:02.704   0:06.338    2.34
-rotate 45                                       0:59.341   1:12.567    1.22
-rotate 90                                       0:03.359   0:08.126    2.42
-rotate 180                                      0:02.629   0:06.343    2.41
-rotate 270                                      0:03.277   0:08.181    2.50
-shade 30x30                                     0:05.770   0:08.446    1.46
-sharpen 0x0.5                                   0:07.840   0:10.188    1.30
-sharpen 0x1.0                                   0:16.499   0:17.043    1.03
-shave 10x10                                     0:02.676   0:06.321    2.36
-shear 45x45                                     0:49.748   1:37.466    1.96
-solarize 50%                                    0:01.935   0:04.875    2.52
-spread 1                                        0:03.066   0:24.506    7.99
-spread 3                                        0:03.068   0:23.976    7.81
-swirl 90                                        0:09.685   0:20.119    2.08
-threshold 35%                                   0:02.458   0:04.998    2.03
-fuzz 35% -transparent red                       0:02.072   0:05.995    2.89
-trim                                            0:03.741   0:06.627    1.77
-unsharp 0x0.5+20+1                              0:08.489   0:23.983    2.83
-unsharp 0x1.0+20+1                              0:09.705   0:24.755    2.55
-wave 25x150                                     0:11.415   0:24.194    2.12
-white-threshold 80%                             0:01.764   0:04.938    2.80
============================================== ========== ========== ===========

Windows XP / MinGW / Intel Core 2 Quad
--------------------------------------

TDM's GCC/mingw32 build 1.902.0 was downloaded from
http://www.tdragon.net/recentgcc/ and installed on the same Windows XP
system with the 2.83 GHz Core 2 Quad Processor (Q9550) as described
above.  This processor is a multi-chip module (MCM) based on two Core
2 CPUs bonded to a L3 cache in the same chip package.  This build
supports OpenMP:

============================================== ========== ========== ===========
Operation                                      GM real    IM real    IM/GM ratio
============================================== ========== ========== ===========
-noop                                            0:00.500   0:02.016    4.03
-affine 1,0,0.785,1,0,0 -transform               0:04.937   1:19.625   16.13
-black-threshold 20%                             0:00.640   0:02.297    3.59
-blur 0x0.5                                      0:01.922   0:05.204    2.71
-blur 0x1.0                                      0:02.157   0:05.078    2.35
-border 6x6                                      0:00.797   0:03.375    4.23
-charcoal 0x1                                    0:04.704   0:09.031    1.92
-chop 800x600+200+300                            0:00.578   0:02.297    3.97
-colorspace CMYK                                 0:00.687   0:02.750    4.00
-colorspace GRAY                                 0:01.031   0:02.687    2.61
-colorspace HSL                                  0:01.234   0:02.985    2.42
-colorspace HWB                                  0:01.016   0:02.875    2.83
-colorspace OHTA                                 0:01.015   0:02.938    2.89
-colorspace YCbCr                                0:01.015   0:02.953    2.91
-colorspace YIQ                                  0:01.047   0:02.953    2.82
-colorspace YUV                                  0:01.032   0:02.954    2.86
-contrast -contrast -contrast                    0:04.750   0:08.703    1.83
+contrast +contrast +contrast                    0:04.687   0:08.625    1.84
-convolve 1,1,1,1,4,1,1,1,1                      0:01.109   0:03.172    2.86
-colorize 30%/20%/50%                            0:00.906   0:03.562    3.93
-crop 1700x900+100+100                           0:00.703   0:02.468    3.51
-despeckle                                       0:12.297   0:14.734    1.20
-edge 0x1                                        0:01.672   0:03.797    2.27
-emboss 0x1                                      0:02.062   0:06.890    3.34
-enhance                                         0:06.313   0:07.218    1.14
-equalize                                        0:00.875   0:03.125    3.57
-flip                                            0:00.781   0:02.594    3.32
-flop                                            0:00.797   0:02.625    3.29
-frame 15x15+3+3                                 0:00.796   0:03.141    3.95
-gamma 1.6                                       0:00.750   0:02.282    3.04
-gaussian 0x0.5                                  0:01.594   0:03.703    2.32
-gaussian 0x1.0                                  0:03.422   0:05.641    1.65
-implode 0.5                                     0:03.657   0:07.031    1.92
-implode -1                                      0:03.719   0:06.985    1.88
-lat 10x10-5%                                    0:04.578   0:07.609    1.66
-level 10%,1.2,90%                               0:00.750   0:17.907   23.88
-median 1                                        0:11.766   0:25.219    2.14
-median 2                                        0:30.453   0:54.547    1.79
-modulate 110/100/95                             0:01.421   0:02.906    2.05
-motion-blur 0x3+30                              0:50.031   1:18.047    1.56
-negate                                          0:00.625   0:02.141    3.43
+noise Uniform                                   0:01.968   0:06.141    3.12
+noise Gaussian                                  0:07.781   0:48.047    6.17
+noise Multiplicative                            0:05.375   0:28.547    5.31
+noise Impulse                                   0:01.828   0:05.390    2.95
+noise Laplacian                                 0:03.500   0:18.282    5.22
+noise Poisson                                   0:06.485   0:25.406    3.92
-noise 1                                         0:11.484   0:25.000    2.18
-noise 2                                         0:30.141   0:54.766    1.82
-normalize                                       0:00.812   0:02.985    3.68
-fill blue -fuzz 35% -opaque red                 0:00.641   0:02.422    3.78
-paint 0x1                                       0:03.421   0:05.609    1.64
-raise 10x10                                     0:00.516   0:02.063    4.00
-density 75x75 -resample 50x50                   0:01.969   0:03.703    1.88
-recolor '1,0,0,0,1,0,0,0,1'                     0:00.484   0:02.485    5.13
-recolor '0,0,1,0,1,0,1,0,0'                     0:00.922   0:02.500    2.71
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:00.796   0:02.485    3.12
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:00.828   0:02.593    3.13
-resize 10%                                      0:01.110   0:02.704    2.44
-resize 50%                                      0:01.672   0:03.250    1.94
-resize 150%                                     0:04.047   0:05.656    1.40
-roll +20+10                                     0:01.203   0:02.531    2.10
-rotate 0                                        0:00.781   0:02.500    3.20
-rotate 45                                       0:10.453   0:14.156    1.35
-rotate 90                                       0:00.937   0:03.360    3.59
-rotate 180                                      0:00.797   0:02.453    3.08
-rotate 270                                      0:00.906   0:03.453    3.81
-shade 30x30                                     0:01.547   0:03.140    2.03
-sharpen 0x0.5                                   0:01.594   0:03.562    2.23
-sharpen 0x1.0                                   0:03.406   0:05.515    1.62
-shave 10x10                                     0:00.750   0:02.437    3.25
-shear 45x45                                     0:07.672   0:17.110    2.23
-solarize 50%                                    0:00.625   0:02.078    3.32
-spread 1                                        0:00.922   0:06.344    6.88
-spread 3                                        0:00.968   0:06.422    6.63
-swirl 90                                        0:03.266   0:06.391    1.96
-threshold 35%                                   0:00.735   0:02.109    2.87
-fuzz 35% -transparent red                       0:00.672   0:02.609    3.88
-trim                                            0:00.750   0:02.547    3.40
-unsharp 0x0.5+20+1                              0:02.125   0:04.797    2.26
-unsharp 0x1.0+20+1                              0:02.469   0:05.375    2.18
-wave 25x150                                     0:03.469   0:07.063    2.04
-white-threshold 80%                             0:00.578   0:02.094    3.62
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
-noop                                            0:02.083   0:04.096    1.97
-affine 1,0,0.785,1,0,0 -transform               0:36.099  13:40.806   22.74
-black-threshold 20%                             0:02.821   0:05.421    1.92
-blur 0x0.5                                      0:15.687   0:57.408    3.66
-blur 0x1.0                                      0:18.853   1:01.198    3.25
-border 6x6                                      0:03.293   0:26.667    8.10
-charcoal 0x1                                    0:43.705   1:36.394    2.21
-chop 800x600+200+300                            0:02.409   0:04.490    1.86
-colorspace CMYK                                 0:02.796   0:11.135    3.98
-colorspace GRAY                                 0:05.756   0:08.693    1.51
-colorspace HSL                                  0:12.670   0:14.219    1.12
-colorspace HWB                                  0:09.280   0:12.011    1.29
-colorspace OHTA                                 0:05.832   0:09.935    1.70
-colorspace YCbCr                                0:05.905   0:10.111    1.71
-colorspace YIQ                                  0:05.949   0:09.911    1.67
-colorspace YUV                                  0:05.893   0:09.932    1.69
-contrast -contrast -contrast                    0:58.031   1:09.836    1.20
+contrast +contrast +contrast                    0:53.681   1:03.939    1.19
-convolve 1,1,1,1,4,1,1,1,1                      0:12.086   0:17.575    1.45
-colorize 30%/20%/50%                            0:06.523   0:09.256    1.42
-crop 1700x900+100+100                           0:02.982   0:05.282    1.77
-despeckle                                       2:03.191   2:35.697    1.26
-edge 0x1                                        0:11.716   0:25.628    2.19
-emboss 0x1                                      0:22.654   1:00.486    2.67
-enhance                                         1:23.819   1:13.702    0.88
-equalize                                        0:04.056   0:10.473    2.58
-flip                                            0:03.216   0:05.682    1.77
-flop                                            0:03.137   0:05.700    1.82
-frame 15x15+3+3                                 0:03.231   0:21.970    6.80
-gamma 1.6                                       0:06.720   0:06.532    0.97
-gaussian 0x0.5                                  0:20.016   0:26.075    1.30
-gaussian 0x1.0                                  0:50.701   0:52.114    1.03
-implode 0.5                                     0:57.616   1:21.087    1.41
-implode -1                                      0:35.434   0:57.035    1.61
-lat 10x10-5%                                    0:40.747   0:55.071    1.35
-level 10%,1.2,90%                               0:03.735   5:36.166   90.00
-median 1                                        2:36.231   3:00.091    1.15
-median 2                                        7:15.913   7:46.614    1.07
-modulate 110/100/95                             0:15.161   0:18.475    1.22
-motion-blur 0x3+30                              1:27.640   5:14.333    3.59
-negate                                          0:02.453   0:04.585    1.87
+noise Uniform                                   0:20.843   0:31.502    1.51
+noise Gaussian                                  2:10.280   2:20.798    1.08
+noise Multiplicative                            1:20.215   1:33.245    1.16
+noise Impulse                                   0:20.096   0:31.876    1.59
+noise Laplacian                                 0:44.644   0:59.629    1.34
+noise Poisson                                   1:16.605   1:28.139    1.15
-noise 1                                         2:40.234   3:02.573    1.14
-noise 2                                         7:20.511   7:50.247    1.07
-normalize                                       0:04.020   0:10.731    2.67
-fill blue -fuzz 35% -opaque red                 0:03.233   0:07.390    2.29
-paint 0x1                                       0:25.547   0:50.312    1.97
-raise 10x10                                     0:02.234   0:04.342    1.94
-density 75x75 -resample 50x50                   0:19.065   0:37.399    1.96
-recolor '1,0,0,0,1,0,0,0,1'                     0:02.046   0:09.780    4.78
-recolor '0,0,1,0,1,0,1,0,0'                     0:06.023   0:09.975    1.66
-recolor '0.9,0,0,0,0.9,0,0,0,1.2'               0:08.207   0:09.761    1.19
-recolor '.22,.72,.07,.22,.72,.07,.22,.72,.07'   0:08.265   0:09.959    1.20
-resize 10%                                      0:10.240   0:18.388    1.80
-resize 50%                                      0:14.496   0:25.924    1.79
-resize 150%                                     0:46.798   1:14.778    1.60
-roll +20+10                                     0:03.287   0:05.813    1.77
-rotate 0                                        0:03.198   0:05.899    1.84
-rotate 45                                       1:53.323   2:22.718    1.26
-rotate 90                                       0:04.259   0:07.964    1.87
-rotate 180                                      0:03.034   0:05.561    1.83
-rotate 270                                      0:04.207   0:07.967    1.89
-shade 30x30                                     0:16.312   0:12.930    0.79
-sharpen 0x0.5                                   0:19.762   0:25.834    1.31
-sharpen 0x1.0                                   0:50.607   0:51.849    1.02
-shave 10x10                                     0:03.075   0:05.494    1.79
-shear 45x45                                     1:03.011   3:12.797    3.06
-solarize 50%                                    0:03.037   0:04.847    1.60
-spread 1                                        0:05.365   1:03.050   11.75
-spread 3                                        0:05.219   1:03.800   12.22
-swirl 90                                        0:31.609   0:51.134    1.62
-threshold 35%                                   0:02.944   0:05.275    1.79
-fuzz 35% -transparent red                       0:03.219   0:07.943    2.47
-trim                                            0:03.903   0:06.108    1.56
-unsharp 0x0.5+20+1                              0:19.818   1:01.871    3.12
-unsharp 0x1.0+20+1                              0:23.308   1:05.507    2.81
-wave 25x150                                     0:28.974   1:01.460    2.12
-white-threshold 80%                             0:02.450   0:04.819    1.97
============================================== ========== ========== ===========


--------------------------------------------------------------------------

| Copyright (C) 2008-2009 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

