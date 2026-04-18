;;; graph-sine-wave.scm — Sine function over one full period
;;; y = sin(x), sampled densely on [0, 2π].
;;; This is the canonical periodic waveform used in trigonometry,
;;; signal processing, and Fourier analysis.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-sine-wave.svg"))
         (output-format (output-format-from-filename output-file))
         (n 600)
         (xmin 0.0)
         (xmax (* 2.0 pi))
         (step (/ (- xmax xmin) n))
         (xs (iota n xmin step))
         (ys (map sin xs)))
    (with-output-to-file output-file
      (lambda ()
        (graph xs ys
               #:output-format output-format
               #:bitmap-size "800x600"
               #:top-label "Sine Wave y = sin(x)"
               #:x-label "x"
               #:y-label "sin(x)"
               #:x-limits (list 0.0 (* 2.0 pi))
               #:y-limits '(-1.2 1.2)
               #:grid-style 3
               #:line-width 0.004
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
