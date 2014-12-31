.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=============================================================
GraphicsMagick vs ImageMagick Benchmarks
=============================================================

.. contents::
  :local:

Foreword
=========

This page used to document benchmark results which compared
GraphicsMagick 1.3.8 performance to ImageMagick 6.5.8-10 performance.
We found that GraphicsMagick was usually considerably faster at
executing image processing operations from the command line than
ImageMagick 6.5.8-10 was. One ImageMagick algorithm ran as much as 770
times slower.  GraphicsMagick clearly ran much more efficiently under
Microsoft Windows.

We now find that a simple head to head performance comparison between
GraphicsMagick and recent ImageMagick is no longer possible because
ImageMagick has changed the meanings of the existing arguments.  In
particular, we discovered that ImageMagick filter arguments producing
convolution matrices are now producing lower-order convolution
matrices representing perhaps an order of magnitude less work given
the same arguments.  The resulting images are visually substantially
less filtered.  Using the same command-line filter arguments causes
GraphicsMagick to appear slower when it is actually doing far more
work than ImageMagick.

Due to it not being feasable to do a head to head performance
measurement between GraphicsMagick and ImageMagick, we have decided to
not post results at the moment.  However, the strategy and a simple
benchmark driver script are still provided for those who want to do
their own performance comparisons between GraphicsMagick and
ImageMagick.

Strategy
========

The benchmark focuses on the ability to process many medium sized
HD-resolution (1920x1080 pixels) images. In order to prevent disk I/O
from being a factor, we use a small input image and tile it to create
a larger input image via the "tile:" coder. The processed image is
sent to the "null:" coder so that file writes to a slow disk are also
not a factor.  Static executables are used and executed via full paths
in order to minimize variability from the execution environment. In
order to obtain accurate and useful timing, we use the bash shell to
execute the command 40 times and see how long it takes. This is a very
simple benchmark approach which is quite representative of the
performance that the typical user observes.

This new benchmark reveals the following performance criteria:

  * Basic execution overhead of the software.
  
  * Image processing algorithmic efficiency.
  
  * Efficacy of OpenMP enhancements.

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


--------------------------------------------------------------------------

| Copyright (C) 2008 - 2015 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/Copyright.html.

