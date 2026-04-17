;;; ex-graph-fourier.scm — Fourier series approximation of a square wave
;;; Demonstrates Gibbs phenomenon: partial sums of the Fourier series
;;; overshoot near the discontinuity by ~9%, no matter how many terms.
;;; S_N(x) = (4/π) Σ_{k=0}^{N} sin((2k+1)x) / (2k+1)

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

(define (fourier-square-wave n-terms)
  "Return a procedure computing the N-term Fourier partial sum of a square wave."
  (lambda (x)
    (let loop ((k 0) (acc 0.0))
      (if (> k n-terms)
          acc
          (let ((m (+ (* 2 k) 1)))
            (loop (+ k 1)
                  (+ acc (* (/ 4.0 pi) (/ (sin (* m x)) m)))))))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-fourier.svg"))
         (output-format (output-format-from-filename output-file))
         (n 800)
         (xmin (- pi))
         (xmax (* 3 pi))
         (step (/ (- xmax xmin) n))
         (xs (iota n xmin step))
         (y1 (map (fourier-square-wave 1) xs))
         (y2 (map (fourier-square-wave 5) xs))
         (y3 (map (fourier-square-wave 25) xs))
         (y4 (map (fourier-square-wave 100) xs)))
    (with-output-to-file output-file
      (lambda ()
        (graph (merge xs xs xs xs)
               (merge y1 y2 y3 y4)
               #:output-format output-format
               #:top-label "Fourier Series of Square Wave"
               #:x-label "x"
               #:y-label "S\\sbN\\eb(x)"
               #:x-limits (list (- pi) (* 3 pi))
               #:y-limits '(-1.4 1.4)
               #:toggle-use-color #t
               #:grid-style 2
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
