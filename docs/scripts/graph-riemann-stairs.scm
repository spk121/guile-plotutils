;;; ex-graph-riemann-stairs.scm — Prime-counting function and the logarithmic integral
;;; π(x) = number of primes ≤ x  (the prime-counting staircase)
;;; Li(x) = ∫₂ˣ dt/ln(t)       (the logarithmic integral)
;;; The Prime Number Theorem states π(x) ~ Li(x) ~ x/ln(x) as x → ∞.
;;; Also plots x/ln(x) to show Li(x) is a much better approximation.
;;; Uses symbols on π(x) to show discrete jumps at each prime.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define (prime? n)
  (cond
   ((<= n 1) #f)
   ((= n 2) #t)
   ((even? n) #f)
   (else
    (let loop ((d 3))
      (cond
       ((> (* d d) n) #t)
       ((zero? (remainder n d)) #f)
       (else (loop (+ d 2))))))))

(define (pi-count x)
  "Count primes up to x."
  (let loop ((n 2) (count 0))
    (if (> n (inexact->exact (floor x)))
        count
        (loop (+ n 1) (if (prime? n) (+ count 1) count)))))

(define (integrate-simpson f a b n)
  (let* ((h (/ (- b a) n))
         (sum (+ (f a) (f b))))
    (let loop ((i 1) (s sum))
      (if (>= i n)
          (* (/ h 3.0) s)
          (let* ((xv (+ a (* i h)))
                 (w (if (even? i) 2.0 4.0)))
            (loop (+ i 1) (+ s (* w (f xv)))))))))

(define (li x)
  "Logarithmic integral Li(x) = ∫₂ˣ dt/ln(t)."
  (if (<= x 2.0) 0.0
      (integrate-simpson (lambda (t) (/ 1.0 (log t)))
                         2.0 x 200)))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-prime-counting.svg"))
         (output-format (output-format-from-filename output-file))
         ;; Staircase π(x) sampled at integers from 2 to 200
         (xmax 200)
         (integers (iota (- xmax 1) 2))
         (xs-pi (map exact->inexact integers))
         (ys-pi (map (lambda (n) (exact->inexact (pi-count n))) integers))

         ;; Li(x) as smooth curve
         (n 300)
         (step (/ (- xmax 2.0) n))
         (xs-li (iota n 2.0 step))
         (ys-li (map li xs-li))

         ;; x/ln(x) as comparison
         (ys-xlogx (map (lambda (x) (/ x (log x))) xs-li)))
    (with-output-to-file output-file
      (lambda ()
        (graph (merge xs-pi xs-li xs-li)
               (merge ys-pi ys-li ys-xlogx)
               #:output-format output-format
               #:bitmap-size "800x600"
               #:top-label "Prime Counting: \\pi(x), Li(x), x/ln(x)"
               #:x-label "x"
               #:y-label "Count of primes"
               #:x-limits '(2.0 200.0)
               #:y-limits '(0.0 55.0)
               #:toggle-use-color #t
               #:grid-style 3
               #:line-width 0.003
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
