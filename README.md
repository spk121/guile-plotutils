# Guile Plotutils

This is a library for [GNU Guile](https://gnu.org/software/guile) to
use the plotting and graphing functionality provided by
[GNU Plotutils](https://gnu.org/software/plotutils).

GNU Guile is an implementation of Scheme, which is a Lisp-like language.
This library allows Guile draw plots using a PCL-like plotting functions.
It also allows Guile to make plots of y or x,y data.

Guile Plotutils has two primary components.

* The `(plotutils plot)` and `(plotutils graph)` scheme modules

* `libguile-plotutils.so` or `libguile-plotutils.dll`: a compiled module that
  contains glue code to interface with GNU Plotutils's libplot library

## Requirements

To build `guile-plotutils`, one needs a fairly standard build
environment, a recent GNU Guile, plus the a current install of GNU
Plotutils that happens to also intstall a working version of its
libplot library.

This latter requirement, libplot, may end up being tricky for some
distrubtions and operating systems. GNU Plotutils is rarely used as an
upstream used by other programs, and some OSs and distros just don't
build or install libplot in a way that is usable by other program.

It appears to work on some GNU/Linux distros.

## Running from source

To run one of the examples or your own code with Guile-Plotutils built
directly from source, use `tools/uninstalled-env`.  For example

    $ tools/uninstalled-env tools/run-guile examples/ex-plot3.scm


