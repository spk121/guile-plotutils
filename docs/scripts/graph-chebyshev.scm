;;; ex-graph-chebyshev.scm — Chebyshev polynomials of the first kind T_n(x)
;;; T_0(x) = 1,  T_1(x) = x,  T_{n+1}(x) = 2x·T_n(x) − T_{n−1}(x)
;;; Chebyshev polynomials are orthogonal on [−1,1] w.r.t. the weight
;;; 1/√(1−x²), minimize the maximum deviation among polynomials of the
;;; same degree (minimax property), and are the backbone of spectral
;;; methods for numerical PDEs and optimal polynomial interpolation.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define (chebyshev n x)
  "Compute T_n(x) via the three-term recurrence."
  (cond
   ((= n 0) 1.0)
   ((= n 1) x)
   (else
    (let loop ((k 2) (t-prev 1.0) (t-curr x))
      (let ((t-next (- (* 2.0 x t-curr) t-prev)))
        (if (= k n)
            t-next
            (loop (+ k 1) t-curr t-next)))))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-chebyshev.svg"))
         (output-format (output-format-from-filename output-file))
         (n 600)
         (xmin -1.0)
         (xmax 1.0)
         (step (/ (- xmax xmin) n))
         (xs (iota n xmin step))
         (t1 (map (lambda (x) (chebyshev 1 x)) xs))
         (t2 (map (lambda (x) (chebyshev 2 x)) xs))
         (t3 (map (lambda (x) (chebyshev 3 x)) xs))
         (t4 (map (lambda (x) (chebyshev 4 x)) xs))
         (t5 (map (lambda (x) (chebyshev 5 x)) xs))
         (t6 (map (lambda (x) (chebyshev 6 x)) xs)))
    (with-output-to-file output-file
      (lambda ()
        (graph (merge xs xs xs xs xs xs)
               (merge t1 t2 t3 t4 t5 t6)
               #:output-format output-format
               #:bitmap-size "800x600"
               #:top-label "Chebyshev Polynomials T\\sb1\\eb through T\\sb6\\eb"
               #:x-label "x"
               #:y-label "T\\sbn\\eb(x)"
               #:x-limits '(-1.0 1.0)
               #:y-limits '(-1.1 1.1)
               #:toggle-use-color #t
               #:grid-style 3
               #:line-width 0.004
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
