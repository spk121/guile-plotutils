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

(test-assert "linemod! is exported"
  (procedure? linemod!))

(test-equal "closepath! returns 0 on an open plotter"
  0
  (let ((p (newpl "svg" (open-output-file "/dev/null")
                  (current-error-port) (newplparams))))
    (openpl! p)
    (move! p 0 0)
    (cont! p 1 0)
    (cont! p 1 1)
    (let ((ret (closepath! p)))
      (closepl! p)
      ret)))

(test-end "plot.scm")
