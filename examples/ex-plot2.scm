(use-modules (plotutils plot))

(define SIZE 100.0)
(define EXPAND 2.2)

(define (draw-boxed-string plotter s size angle)
  (textangle! plotter angle)
  (let* ((true-size (fontsize! plotter size))
	 (width (labelwidth! plotter s)))
    (ellipserel! plotter 0.0
		 0.0
		 (* EXPAND 0.5 width)
		 (* EXPAND 0.5 true-size)
		 angle)
    (alabel! plotter 'c 'c s)))

(define plotter-params (newplparams))
(setplparam! plotter-params "PAGESIZE" "letter")
(define fp (open-output-file "tmp.png" #:binary #t))
(define plotter (newpl "png"
		       fp
		       (current-error-port)
		       plotter-params))
(openpl! plotter)
(space! plotter (- SIZE) (- SIZE) SIZE SIZE)
(pencolorname! plotter "blue")
(fillcolorname! plotter "white")
(filltype! plotter 1)
(fontname! plotter "NewCenturySchlbk-Roman")

(let loop ((i 80))
  (let* ((theta (* i 0.5))
	 (radius (/ SIZE (expt theta 0.35))))
    (move! plotter (* radius (cos theta))  (* radius (sin theta)))
    (draw-boxed-string plotter "GNU libplot!" (* 0.04 radius)
		       (- (* 180.0 theta (/ 1.0 3.1415)) 90.0))
    (if (> i 1)
	(loop (- i 1)))))

(closepl! plotter)
