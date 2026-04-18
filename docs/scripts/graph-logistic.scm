;;; ex-graph-logistic.scm — The Logistic Map orbit diagram
;;; x_{n+1} = r·x_n·(1 − x_n)
;;; For different values of the parameter r, the long-term behavior
;;; transitions from a fixed point to period-doubling cascades to chaos.
;;; This is the canonical example of how simple deterministic systems
;;; produce complex aperiodic dynamics (Feigenbaum universality).

(use-modules (srfi srfi-1)
             (plotutils graph))

(define (logistic-orbit r x0 transient keep)
  "Iterate the logistic map x -> r*x*(1-x) from x0.
Discard TRANSIENT iterates then collect KEEP iterates.
Returns (r-list . x-list) for plotting."
  (let iterate ((i 0) (x x0))
    (if (< i transient)
        (iterate (+ i 1) (* r x (- 1.0 x)))
        ;; Now collect
        (let collect ((j 0) (x x) (rs '()) (xs '()))
          (if (>= j keep)
              (cons (reverse rs) (reverse xs))
              (let ((xnew (* r x (- 1.0 x))))
                (collect (+ j 1) xnew
                         (cons r rs) (cons xnew xs))))))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-logistic.svg"))
         (output-format (output-format-from-filename output-file))
         (r-min 2.5)
         (r-max 4.0)
         (n-r 600)
         (r-step (/ (- r-max r-min) n-r))
         (transient 200)
         (keep 80)
         (result
          (let loop ((i 0) (all-r '()) (all-x '()))
            (if (>= i n-r)
                (cons (reverse all-r) (reverse all-x))
                (let* ((r (+ r-min (* i r-step)))
                       (orb (logistic-orbit r 0.4 transient keep)))
                  (loop (+ i 1)
                        (append (car orb) all-r)
                        (append (cdr orb) all-x))))))
         (xs (car result))
         (ys (cdr result)))
    (with-output-to-file output-file
      (lambda ()
        (graph xs ys
               #:output-format output-format
               #:bitmap-size "1000x1000"
               #:top-label "Logistic Map Bifurcation Diagram"
               #:x-label "r"
               #:y-label "x*"
               #:x-limits '(2.5 4.0)
               #:y-limits '(0.0 1.0)
               #:line-mode 0
               #:symbol '(1 0.004)
               #:grid-style 2
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
