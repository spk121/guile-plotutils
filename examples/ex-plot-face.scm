(use-modules (plotutils plot))

(define fp (open-output-file "face.png" #:binary #t))
(define param (newplparams))
(setplparam! param "BITMAPSIZE" "400x400")
(define plotter (newpl "png" fp (current-error-port) param))

(openpl! plotter)
(space! plotter 0.0 0.0 1000.0 1000.0)
(linewidth! plotter 0.25)
(pencolorname! plotter "red")
(erase! plotter)
(move! plotter 600.0 300.0)

;; eyes
(circle! plotter 300 500 10)
(circle! plotter 700 500 10)

;; nose
(bezier2! plotter 450 400 500 500 550 400)

;; mouth
(bezier2! plotter 300 100 500 50 700 100)

(move! plotter 100.0 100.0)
(fontsize! plotter 50.0)
(alabel! plotter 'c 'c "Face!")
(closepl! plotter)
(close fp)

