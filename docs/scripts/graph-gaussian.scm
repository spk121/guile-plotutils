;;; ex-graph-gaussian.scm — Gaussian (Normal) probability density functions
;;; Plots the bell curve for three different standard deviations,
;;; illustrating how variance controls the spread of a distribution.
;;; f(x) = (1 / (σ√(2π))) · exp(−x²/(2σ²))

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

(define (gaussian sigma)
  (lambda (x)
    (/ (exp (- (/ (* x x) (* 2.0 sigma sigma))))
       (* sigma (sqrt (* 2.0 pi))))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-gaussian.svg"))
         (output-format (output-format-from-filename output-file))
         (n 500)
         (xmin -5.0)
         (xmax 5.0)
         (step (/ (- xmax xmin) n))
         (xs (iota n xmin step))
         (y1 (map (gaussian 0.5) xs))
         (y2 (map (gaussian 1.0) xs))
         (y3 (map (gaussian 2.0) xs)))
    (with-output-to-file output-file
      (lambda ()
        (graph (merge xs xs xs)
               (merge y1 y2 y3)
               #:output-format output-format
               #:bitmap-size "1000x1000"
               #:top-label "Gaussian PDF: \\*s=0.5, 1.0, 2.0"
               #:x-label "x"
               #:y-label "f(x)"
               #:x-limits '(-5.0 5.0)
               #:y-limits '(0.0 0.9)
               #:toggle-use-color #t
               #:grid-style 3
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
