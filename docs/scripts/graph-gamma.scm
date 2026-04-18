;;; ex-graph-gamma.scm — The Gamma function Γ(x) via Lanczos approximation
;;; Γ extends the factorial to real numbers: Γ(n) = (n−1)! for positive
;;; integers.  Has poles at non-positive integers.  Plotted in two
;;; segments avoiding the pole at x=0.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

(define (lanczos-gamma x)
  "Compute Γ(x) using the Lanczos approximation (g=7)."
  (let ((p (list 0.99999999999980993
                 676.5203681218851
                 -1259.1392167224028
                 771.32342877765313
                 -176.61502916214059
                 12.507343278686905
                 -0.13857109526572012
                 9.9843695780195716e-6
                 1.5056327351493116e-7)))
    (if (< x 0.5)
        ;; Reflection formula: Γ(x) = π / (sin(πx) · Γ(1−x))
        (/ pi (* (sin (* pi x)) (lanczos-gamma (- 1.0 x))))
        (let* ((x (- x 1.0))
               (g 7)
               (a (car p))
               (ag (let loop ((i 1) (acc a))
                     (if (>= i (length p))
                         acc
                         (loop (+ i 1)
                               (+ acc (/ (list-ref p i) (+ x i)))))))
               (t (+ x g 0.5)))
          (* (sqrt (* 2.0 pi))
             (expt t (+ x 0.5))
             (exp (- t))
             ag)))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-gamma.svg"))
         (output-format (output-format-from-filename output-file))
         (n 300)
         ;; Segment from 0.1 to 5.0 (avoiding pole at 0)
         (xs1-min 0.08)
         (xs1-max 5.0)
         (step1 (/ (- xs1-max xs1-min) n))
         (xs1 (iota n xs1-min step1))
         (ys1 (map lanczos-gamma xs1))
         ;; Segment from -3.95 to -0.05 (between poles)
         (xs2-parts
          ;; Three inter-pole segments: (-3.95,-3.05), (-2.95,-2.05), (-1.95,-0.05)
          (append
           (let* ((a -3.95) (b -3.05) (s (/ (- b a) 80)))
             (map (lambda (i) (+ a (* i s))) (iota 80)))
           (list #f)
           (let* ((a -2.95) (b -2.05) (s (/ (- b a) 80)))
             (map (lambda (i) (+ a (* i s))) (iota 80)))
           (list #f)
           (let* ((a -1.95) (b -1.05) (s (/ (- b a) 80)))
             (map (lambda (i) (+ a (* i s))) (iota 80)))
           (list #f)
           (let* ((a -0.95) (b -0.05) (s (/ (- b a) 80)))
             (map (lambda (i) (+ a (* i s))) (iota 80)))))
         (ys2-parts
          (map (lambda (v) (if v (lanczos-gamma v) #f)) xs2-parts))
         ;; Clamp values for display
         (clamp (lambda (v) (if v (max -8.0 (min 8.0 v)) #f)))
         (ys2-clamped (map clamp ys2-parts)))
    (with-output-to-file output-file
      (lambda ()
        (graph (merge xs2-parts xs1)
               (merge ys2-clamped ys1)
               #:output-format output-format
               #:bitmap-size "800x600"
               #:top-label "Gamma Function \\Gamma(x)"
               #:x-label "x"
               #:y-label "\\Gamma(x)"
               #:x-limits '(-4.0 5.0)
               #:y-limits '(-8.0 8.0)
               #:toggle-use-color #t
               #:grid-style 3
               #:line-width 0.004
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
