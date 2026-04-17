#!/usr/bin/guile \
-e main -s
!#
;;; ex-plot-sierpinski-triangle.scm — Sierpiński Triangle
;;; The Sierpiński triangle (Sierpiński gasket) is a fractal attractor
;;; described by Wacław Sierpiński in 1915.  It is formed by recursively
;;; subdividing an equilateral triangle into four smaller equilateral
;;; triangles and removing the central one.  It has Hausdorff dimension
;;; log(3)/log(2) ≈ 1.585.  It also arises in Pascal's triangle
;;; (cells with odd binomial coefficients) and in the Chaos Game.

(use-modules (plotutils plot))

(define *plotter* #f)

(define (draw-filled-triangle! x1 y1 x2 y2 x3 y3 color)
  "Draw a filled triangle with the given vertices."
  (pencolorname! *plotter* color)
  (fillcolorname! *plotter* color)
  (filltype! *plotter* 1)
  (move! *plotter* x1 y1)
  (cont! *plotter* x2 y2)
  (cont! *plotter* x3 y3)
  (cont! *plotter* x1 y1)
  (endpath! *plotter*)
  (filltype! *plotter* 0))

(define (sierpinski depth x1 y1 x2 y2 x3 y3)
  "Recursively draw the Sierpiński triangle."
  (if (= depth 0)
      (draw-filled-triangle! x1 y1 x2 y2 x3 y3 "midnight blue")
      (let ((mx12 (/ (+ x1 x2) 2.0)) (my12 (/ (+ y1 y2) 2.0))
            (mx23 (/ (+ x2 x3) 2.0)) (my23 (/ (+ y2 y3) 2.0))
            (mx13 (/ (+ x1 x3) 2.0)) (my13 (/ (+ y1 y3) 2.0)))
        ;; Bottom-left triangle
        (sierpinski (- depth 1) x1 y1 mx12 my12 mx13 my13)
        ;; Bottom-right triangle
        (sierpinski (- depth 1) mx12 my12 x2 y2 mx23 my23)
        ;; Top triangle
        (sierpinski (- depth 1) mx13 my13 mx23 my23 x3 y3))))

(define (main args)
  (let* ((fp (open-output-file "plot-sierpinski-triangle.svg" #:binary #t))
         (param (newplparams)))
    (setplparam! param "BITMAPSIZE" "800x800")
    (let ((plotter (newpl "svg" fp (current-error-port) param)))
      (set! *plotter* plotter)
      (openpl! plotter)
      (space! plotter -50.0 -50.0 850.0 850.0)
      (linewidth! plotter 0.2)
      (erase! plotter)

      ;; Equilateral triangle vertices
      (let* ((side 700.0)
             (height (* side (/ (sqrt 3.0) 2.0)))
             (x1 50.0) (y1 50.0)
             (x2 (+ x1 side)) (y2 50.0)
             (x3 (+ x1 (/ side 2.0))) (y3 (+ 50.0 height)))
        (sierpinski 8 x1 y1 x2 y2 x3 y3))

      (closepl! plotter)
      (close fp))))
