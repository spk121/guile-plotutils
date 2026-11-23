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

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1) (cadr args) "plot-golden-spiral.svg"))
         (output-format (output-format-from-filename output-file))
         (fp (open-output-file output-file #:binary #t))
         (param (newplparams)))
    (setplparam! param "BITMAPSIZE" "800x800")
    (let ((plotter (newpl output-format fp (current-error-port) param)))
      (openpl! plotter)
      ;; The eleven squares tile an 89 x 144 rectangle spanning x from
      ;; -24 to 65 and y from -39 to 105.  Use a square user space around
      ;; it, so that circles stay round and text isn't stretched.
      (space! plotter -60.0 -46.0 102.0 116.0)
      (erase! plotter)

      ;; Start with a 1x1 square.  Each following square, whose side is
      ;; the next Fibonacci number, goes on the right, top, left, or
      ;; bottom of the rectangle tiled so far, in turn; its side equals
      ;; that edge of the rectangle.  A quarter-circle arc in each square,
      ;; centred on the corner given below, joins the previous square's
      ;; arc, so together they trace the spiral.
      (let loop ((sides (cdr (fibonacci-list 11)))
                 (side 0)                    ; 0=right 1=top 2=left 3=bottom
                 (square '(0.0 0.0 1.0 1.0))  ; x0 y0 x1 y1
                 (rect '(0.0 0.0 1.0 1.0))    ; the rectangle tiled so far
                 (centre 'upper-right))       ; arc centre in this square
        (let ((x0 (list-ref square 0)) (y0 (list-ref square 1))
              (x1 (list-ref square 2)) (y1 (list-ref square 3)))
          ;; the square
          (pencolorname! plotter "tan")
          (linewidth! plotter 0.15)
          (box! plotter x0 y0 x1 y1)
          ;; its quarter-circle, counterclockwise from one neighbouring
          ;; corner of the centre to the other
          (pencolorname! plotter "dark red")
          (linewidth! plotter 0.6)
          (case centre
            ((upper-left)  (arc! plotter x0 y1 x0 y0 x1 y1))
            ((lower-left)  (arc! plotter x0 y0 x1 y0 x0 y1))
            ((lower-right) (arc! plotter x1 y0 x1 y1 x0 y0))
            ((upper-right) (arc! plotter x1 y1 x0 y1 x1 y0))))
        (when (pair? sides)
          (let* ((s (exact->inexact (car sides)))
                 (rx0 (list-ref rect 0)) (ry0 (list-ref rect 1))
                 (rx1 (list-ref rect 2)) (ry1 (list-ref rect 3)))
            (case side
              ((0) (loop (cdr sides) 1 (list rx1 ry0 (+ rx1 s) (+ ry0 s))
                         (list rx0 ry0 (+ rx1 s) ry1) 'upper-left))
              ((1) (loop (cdr sides) 2 (list rx0 ry1 (+ rx0 s) (+ ry1 s))
                         (list rx0 ry0 rx1 (+ ry1 s)) 'lower-left))
              ((2) (loop (cdr sides) 3 (list (- rx0 s) (- ry1 s) rx0 ry1)
                         (list (- rx0 s) ry0 rx1 ry1) 'lower-right))
              ((3) (loop (cdr sides) 0 (list (- rx1 s) (- ry0 s) rx1 ry0)
                         (list rx0 (- ry0 s) rx1 ry1) 'upper-right))))))

      ;; Title
      (pencolorname! plotter "black")
      (move! plotter 20.5 110.0)
      (fontname! plotter "HersheySerif")
      (fontsize! plotter 7.0)
      ;; ASCII only: libplot's fonts are Latin-1, and the conversion of
      ;; other characters depends on the locale the script runs in
      (alabel! plotter 'c 'c "Golden Spiral -- Fibonacci Rectangles")

      (closepl! plotter)
      ;; Delete the plotter before closing the port: Postscript (.eps)
      ;; output is written only when the plotter is deleted.
      (deletepl! plotter)
      (close fp))))

(main (command-line))
