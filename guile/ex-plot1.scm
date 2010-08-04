(use-modules (plotutils plot))

(define (draw-c-curve plotter dx dy order)
  (if (>= order 14)
      (contrel! plotter dx dy)
      ;; else
      (begin
	(draw-c-curve plotter (* 0.5 (- dx dy)) (* 0.5 (+ dx dy)) (+ order 1))
	(draw-c-curve plotter (* 0.5 (+ dx dy)) (* 0.5 (- dy dx)) (+ order 1)))))

(define fp (open-output-file "tmp.png"))
(define param (newplparams))
(setplparam! param "BITMAPSIZE" "400x400")
(define plotter (newpl "png" fp (current-error-port) param))

(openpl! plotter)
(space! plotter 0.0 0.0 1000.0 1000.0)
(linewidth! plotter 0.25)
(pencolorname! plotter "red")
(erase! plotter)
(move! plotter 600.0 300.0)

(draw-c-curve plotter 0.0 400.0 0)
(closepl! plotter)
(close fp)

