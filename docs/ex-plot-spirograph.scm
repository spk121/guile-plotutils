#!/usr/bin/guile \
-e main -s
!#
;;; ex-plot-spirograph.scm — Spirograph (Hypotrochoid and Epitrochoid) curves
;;; A hypotrochoid is traced by a point attached to a circle of radius r
;;; rolling inside a fixed circle of radius R:
;;;   x(t) = (R−r)cos(t) + d·cos((R−r)t/r)
;;;   y(t) = (R−r)sin(t) − d·sin((R−r)t/r)
;;; An epitrochoid rolls outside:
;;;   x(t) = (R+r)cos(t) − d·cos((R+r)t/r)
;;;   y(t) = (R+r)sin(t) − d·sin((R+r)t/r)
;;; These curves belong to the family of roulettes studied since
;;; Dürer (1525) and were popularized by the Spirograph toy (1965).

(use-modules (plotutils plot))

(define pi (* 4.0 (atan 1.0)))

(define (draw-hypotrochoid! plotter R r d n-steps color)
  "Draw a hypotrochoid on plotter with given parameters."
  (pencolorname! plotter color)
  (let* ((dt (/ (* 2.0 pi) n-steps))
         (x0 (+ (- R r) d))
         (y0 0.0))
    (move! plotter x0 y0)
    (let loop ((i 1))
      (if (<= i (* n-steps 10))  ; go around enough times for closure
          (let* ((t (* i dt))
                 (x (+ (* (- R r) (cos t))
                        (* d (cos (* (/ (- R r) r) t)))))
                 (y (- (* (- R r) (sin t))
                        (* d (sin (* (/ (- R r) r) t))))))
            ;; Stop if we've returned very close to start after at least one loop
            (if (and (> i n-steps)
                     (< (+ (* (- x x0) (- x x0)) (* (- y y0) (- y y0))) 0.01))
                (cont! plotter x0 y0)
                (begin
                  (cont! plotter x y)
                  (loop (+ i 1)))))))))

(define (draw-epitrochoid! plotter R r d n-steps color)
  "Draw an epitrochoid on plotter with given parameters."
  (pencolorname! plotter color)
  (let* ((dt (/ (* 2.0 pi) n-steps))
         (x0 (- (+ R r) d))
         (y0 0.0))
    (move! plotter x0 y0)
    (let loop ((i 1))
      (if (<= i (* n-steps 12))
          (let* ((t (* i dt))
                 (x (- (* (+ R r) (cos t))
                        (* d (cos (* (/ (+ R r) r) t)))))
                 (y (- (* (+ R r) (sin t))
                        (* d (sin (* (/ (+ R r) r) t))))))
            (if (and (> i n-steps)
                     (< (+ (* (- x x0) (- x x0)) (* (- y y0) (- y y0))) 0.01))
                (cont! plotter x0 y0)
                (begin
                  (cont! plotter x y)
                  (loop (+ i 1)))))))))

(define (main args)
  (let* ((fp (open-output-file "plot-spirograph.svg" #:binary #t))
         (param (newplparams)))
    (setplparam! param "BITMAPSIZE" "800x800")
    (let ((plotter (newpl "svg" fp (current-error-port) param)))
      (openpl! plotter)
      (space! plotter -220.0 -220.0 220.0 220.0)
      (linewidth! plotter 0.4)
      (erase! plotter)

      ;; Hypotrochoid #1: R=105, r=35, d=30  (a 3-lobed rose-like)
      (draw-hypotrochoid! plotter 105.0 35.0 30.0 2000 "crimson")

      ;; Hypotrochoid #2: R=144, r=45, d=40  (a rich multi-petal)
      (draw-hypotrochoid! plotter 144.0 45.0 40.0 2000 "dark green")

      ;; Epitrochoid: R=60, r=45, d=30
      (draw-epitrochoid! plotter 60.0 45.0 30.0 2000 "royal blue")

      (closepl! plotter)
      (close fp))))
