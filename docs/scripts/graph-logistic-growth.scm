;;; ex-graph-logistic-growth.scm — Logistic growth and predator-prey dynamics
;;; 1) Logistic growth: dP/dt = r·P·(1 − P/K), solution P(t) = K/(1 + ((K−P₀)/P₀)e^{−rt})
;;; Models population growth with carrying capacity (Verhulst, 1838).
;;; 2) Lotka-Volterra predator-prey: solved via simple Euler integration,
;;; showing the characteristic oscillatory coexistence cycle.
;;; dx/dt = αx − βxy,  dy/dt = δxy − γy

(use-modules (srfi srfi-1)
             (plotutils graph))

;;; --- Lotka-Volterra via Euler method ---
(define (lotka-volterra alpha beta delta gamma x0 y0 dt n)
  "Return (ts xs ys) for the Lotka-Volterra system."
  (let loop ((i 0) (x x0) (y y0) (ts '()) (xs '()) (ys '()))
    (if (>= i n)
        (list (reverse ts) (reverse xs) (reverse ys))
        (let* ((t (* i dt))
               (dx (* dt (- (* alpha x) (* beta x y))))
               (dy (* dt (- (* delta x y) (* gamma y)))))
          (loop (+ i 1)
                (+ x dx) (+ y dy)
                (cons t ts) (cons x xs) (cons y ys))))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "graph-lotka-volterra.svg"))
         (output-format (output-format-from-filename output-file))
         ;; Lotka-Volterra
         (lv (lotka-volterra 1.1 0.4 0.1 0.4 10.0 10.0 0.005 6000))
         (lv-ts (car lv))
         (lv-xs (cadr lv))
         (lv-ys (caddr lv)))
    ;; Lotka-Volterra time series
    (with-output-to-file output-file
      (lambda ()
        (graph (merge lv-ts lv-ts)
               (merge lv-xs lv-ys)
               #:output-format output-format
               #:top-label "Lotka-Volterra Predator-Prey Dynamics"
               #:x-label "Time"
               #:y-label "Population"
               #:toggle-use-color #t
               #:grid-style 3
               #:line-width 0.004
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
