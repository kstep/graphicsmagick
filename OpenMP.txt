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
available to the masses. Microsoft Visual Studio 2008 supports OpenMP 2.0
so Windows users should benefit as well. Any multi-CPU and/or multi-core
system is potentially a good candidate for use with OpenMP.
Unfortunately, some older multi-CPU hardware is more suitable for
multi-processing than multi-threading. Recent multi-core chipsets from
Intel and AMD perform very well with OpenMP. The operating system makes a
difference when it comes to OpenMP acceleration, with Linux and Solaris
working exceptionally well, and FreeBSD and Apple's OS-X working poorly.

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

The initial approach used in GraphicsMagick is to recognize the various
access patterns in the existing code, and re-write the algorithms
(sometimes from scratch) to be based on a framework that we call "pixel
iterators". With this approach, the computation is restricted to a small
unit (a callback function) with very well defined properties, and no
knowledge as to how it is executed or where the data comes from. This
approach removes the loops from the code and puts the loops in the
framework, which may be adjusted based on experience. The continuing
strategy will be to recognize design patterns and build frameworks which
support those patterns. Sometimes algorithms are special/exotic enough
that it is much easier to instrument the code for OpenMP rather than to
attempt to fit the algorithm into a framework.

Since OpenMP is based on multi-threading, multiple threads access the
underlying pixel storage at once. The interface to this underlying
storage is called the "pixel cache". The original pixel cache code
(derived from ImageMagick) was thread safe only to the extent that it
allowed one thread per image. This code has now been updated so that
multiple threads may safely and efficiently work on the pixels in one
image.

The following is an example of per-core speed-up due to OpenMP on a
four-core system.  All the pixel quantum values are divided by 2::

  % for threads in 1 2 3 4
  do
    env OMP_NUM_THREADS=$threads gm benchmark -duration 10 convert \
      -size 2048x1080 -type truecolor 'xc:#f00' -operator all divide 2.0 null:
  done
  Results: 211 iter 9.99s user 10.00s total 21.10 iter/s (21.12 iter/s cpu)
  Results: 372 iter 18.69s user 10.02s total 37.13 iter/s (19.90 iter/s cpu)
  Results: 512 iter 26.62s user 10.00s total 51.20 iter/s (19.23 iter/s cpu)
  Results: 582 iter 30.63s user 10.00s total 58.20 iter/s (19.00 iter/s cpu)

Note that the "iter/s cpu" value is a measure of the number of iterations
given the amount of reported CPU time consumed. It is an effective
measure of relative efficacy.

--------------------------------------------------------------------------

| Copyright (C) 2008 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

