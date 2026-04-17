;;; ex-graph-zeta.scm — The Riemann Zeta function on the real line
;;; Computes ζ(s) = Σ 1/n^s for s > 1 via partial sums, showing the
;;; pole at s=1 and decay toward 1 as s → ∞.  A central object in
;;; analytic number theory linking primes to complex analysis.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define (zeta-partial s terms)
  "Approximate ζ(s) by summing the first TERMS terms of 1/n^s."
  (let loop ((n 1) (acc 0.0))
    (if (> n terms)
        acc
        (loop (+ n 1) (+ acc (/ 1.0 (expt n s)))))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-zeta.svg"))
         (output-format (output-format-from-filename output-file))
         (n 400)
         (smin 1.05)
         (smax 8.0)
         (step (/ (- smax smin) n))
         (xs (iota n smin step))
         (ys (map (lambda (s) (zeta-partial s 5000)) xs)))
    (with-output-to-file output-file
      (lambda ()
        (graph xs ys
               #:output-format output-format
               #:top-label "Riemann Zeta Function \\zeta(s) for s > 1"
               #:x-label "s"
               #:y-label "\\zeta(s)"
               #:x-limits '(1.0 8.0)
               #:y-limits '(0.0 10.0)
               #:grid-style 3
               #:line-width 0.005
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
