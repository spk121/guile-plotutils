#!/usr/bin/guile \
-e main -s
!#
;;; ex-plot-golden-spiral.scm — Golden Spiral constructed from Fibonacci arcs
;;; The golden spiral is a logarithmic spiral whose growth factor is ϕ,
;;; the golden ratio (1+√5)/2 ≈ 1.618.  Here it is approximated by
;;; quarter-circle arcs inscribed in squares whose side lengths follow
;;; the Fibonacci sequence: 1, 1, 2, 3, 5, 8, 13, 21, ...
;;; Also draws the Fibonacci rectangle tiling underneath.
;;; This construction connects number theory (Fibonacci/Lucas numbers),
;;; geometry (logarithmic spirals), and phyllotaxis in nature.

(use-modules (plotutils plot))

(define (fibonacci-list n)
  "Return the first N Fibonacci numbers starting from 1, 1."
  (let loop ((i 2) (a 1) (b 1) (acc (list 1 1)))
    (if (>= i n) (reverse acc)
        (let ((c (+ a b)))
          (loop (+ i 1) b c (cons c acc))))))

(define (main args)
  (let* ((fp (open-output-file "plot-golden-spiral.svg" #:binary #t))
         (param (newplparams)))
    (setplparam! param "BITMAPSIZE" "800x800")
    (let ((plotter (newpl "svg" fp (current-error-port) param)))
      (openpl! plotter)
      (space! plotter -10.0 -10.0 60.0 60.0)
      (erase! plotter)

      (let* ((fibs (fibonacci-list 11))
             (scale 1.0))

        ;; We'll track the corner of each square and direction of growth.
        ;; Directions cycle: right, up, left, down (quadrant rotation)
        ;; Start at origin, first two 1×1 squares.
        (let loop ((remaining fibs)
                   (cx 0.0) (cy 0.0)  ; current square corner (lower-left)
                   (dir 0))            ; 0=right, 1=up, 2=left, 3=down
          (when (pair? remaining)
            (let* ((s (* (car remaining) scale)) ; side length
                   ;; Compute the four corners of this square based on direction
                   ;; The arc sweeps a quarter circle from one edge to the next
                   )
              ;; Draw the square
              (pencolorname! plotter "tan")
              (linewidth! plotter 0.1)
              (box! plotter cx cy (+ cx s) (+ cy s))

              ;; Draw the quarter-circle arc inscribed in this square
              (pencolorname! plotter "dark red")
              (linewidth! plotter 0.3)
              (let* ((qdir (modulo dir 4))
                     ;; The arc center depends on which corner of the square
                     ;; dir=0: arc centered at upper-right, from upper-left to lower-right
                     ;; dir=1: arc centered at upper-left, from lower-left to upper-right
                     ;; dir=2: arc centered at lower-left, from lower-right to upper-left
                     ;; dir=3: arc centered at lower-right, from upper-right to lower-left
                     (acx (cond ((= qdir 0) (+ cx s)) ((= qdir 1) cx)
                                ((= qdir 2) cx)       ((= qdir 3) (+ cx s))))
                     (acy (cond ((= qdir 0) (+ cy s)) ((= qdir 1) (+ cy s))
                                ((= qdir 2) cy)       ((= qdir 3) cy)))
                     (ax0 (cond ((= qdir 0) cx)       ((= qdir 1) cx)
                                ((= qdir 2) (+ cx s)) ((= qdir 3) (+ cx s))))
                     (ay0 (cond ((= qdir 0) (+ cy s)) ((= qdir 1) cy)
                                ((= qdir 2) cy)       ((= qdir 3) (+ cy s))))
                     (ax1 (cond ((= qdir 0) (+ cx s)) ((= qdir 1) (+ cx s))
                                ((= qdir 2) cx)       ((= qdir 3) cx)))
                     (ay1 (cond ((= qdir 0) cy)       ((= qdir 1) (+ cy s))
                                ((= qdir 2) (+ cy s)) ((= qdir 3) cy))))
                (arc! plotter acx acy ax0 ay0 ax1 ay1))

              ;; Compute next square's lower-left corner
              (let* ((qdir (modulo dir 4))
                     (ncx (cond ((= qdir 0) (+ cx s)) ((= qdir 1) cx)
                                ((= qdir 2) (- cx (car (if (pair? (cdr remaining))
                                                            (cdr remaining) (list s)))))
                                ((= qdir 3) cx)))
                     (ncy (cond ((= qdir 0) cy)       ((= qdir 1) (+ cy s))
                                ((= qdir 2) cy)
                                ((= qdir 3) (- cy (car (if (pair? (cdr remaining))
                                                            (cdr remaining) (list s))))))))
                (loop (cdr remaining) ncx ncy (+ dir 1)))))))

      ;; Title
      (pencolorname! plotter "black")
      (move! plotter 2.0 56.0)
      (fontname! plotter "HersheySerif")
      (fontsize! plotter 2.0)
      (alabel! plotter 'l 'c "Golden Spiral — Fibonacci Rectangles")

      (closepl! plotter)
      (close fp))))
