(use-modules (srfi srfi-1)
	         (plotutils graph))

(define xvalues (iota 101 0 0.1))
(define yvalues (map (lambda (x) (* x x)) xvalues))
(with-output-to-file "tmp.png"
  (lambda ()
    (graph xvalues yvalues
           #:output-format "png"
           #:top-label "Title"))
  #:binary #t)
