(use-modules (plotutils plot)
             (srfi srfi-1)
             (srfi srfi-64))

(test-begin "plot.scm")
(test-assert "newplparams returns a <plparams>"
  (plparams? (newplparams)))
(test-assert "<plparams> equal themselves"
  (let ((p (newplparams)))
    (eq? p p)))

(test-assert "newpl returns a <plotter>"
  (plotter? (newpl "png" (current-output-port) (current-error-port) (newplparams))))
(test-assert "<plotter> equals itself"
  (let ((p (newpl "png" (current-output-port) (current-error-port) (newplparams))))
    (eq? p p)))

(test-end "plot.scm")
