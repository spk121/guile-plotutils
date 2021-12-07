(define-module (plotutils graph)
  #:use-module (srfi srfi-1)
  #:use-module (ice-9 optargs)
  #:export (merge
	        graph))

;; The C library provides the following, which are not currently exported

;;          (graph?
;; 	    new-graph
;; 	    save-screen!
;; 	    toggle-transpose-axes!

;; 	    toggle-auto-bump!
;; 	    toggle-use-color!
;; 	    toggle-frame-on-top!
;; 	    portable-output!
;; 	    emulate-color!
;; 	    graph-version
;; 	    toggle-rotate-y-label!

;; 	    font-size!
;; 	    grid-style!
;; 	    height-of-plot!
;; 	    clip-mode!
;; 	    toggle-log-axis!
;; 	    toggle-no-ticks!
;; 	    line-mode!
;; 	    fill-fraction!
;; 	    right-shift!
;; 	    upward-shift!
;; 	    width-of-plot!
;; 	    output-format!
;; 	    font-name!
;; 	    rotation-angle!
;; 	    title-font-name!
;; 	    symbol-font-name!
;; 	    toggle-round-to-next-tick!

;; 	    top-label!
;; 	    tick-size!
;; 	    line-width!
;; 	    x-label!
;; 	    y-label!
;; 	    toggle-axis-end!
;; 	    blankout!
;; 	    bitmap-size!
;; 	    title-font-size!
;; 	    frame-line-width!
;; 	    max-line-length!
;; 	    page-size!
;; 	    pen-colors
;; 	    bg-color!
;; 	    frame-color!
;; 	    auto-abscissa!
;; 	    x-limits!
;; 	    y-limits!
;; 	    symbol!
;; 	    reposition!
;; 	    xy-data!
;; 	    generate
;; 	    ))

(define (merge . list-of-lists)
  "Given one or more lists, append them into a single list, separating
the input lists with #f.  For example,
(merge '(1 2) '(3 4)) => '(1 2 #f 3 4)"

  (cond
   ((null-list? list-of-lists)
    '())
   ((= 1 (length list-of-lists))
    (car list-of-lists))
   (else
    (let loop ((merged (car list-of-lists))
		       (rest (cdr list-of-lists)))
	  (if (= 1 (length rest))
	      (append merged (list #f) (car rest))
	      ;; else
	      (loop (append merged (list #f) (car rest))
		        (cdr rest)))))))

(defmacro keys-list ()
  `output-format toggle-axis-end font-size font-name
  grid-style height-of-plot toggle-frame-on-top
  tick-size clip-mode toggle-log-axis top-label
  toggle-no-ticks toggle-rotate-y-label right-shift
  toggle-round-to-next-tick save-screen toggle-transpose-axes
  upward-shift width-of-plot x-limits x-label y-limits
  y-label bg-color bitmap-size emulate-color frame-color
  frame-line-width max-line-length page-size pen-colors
  rotation title-font-name title-font-size auto-abscissa
  toggle-auto-bump line-mode symbol line-width
  fill-fraction toggle-use-color symbol-font-name
  blankout portable-output)

;; reposition

(defmacro unpack-keys ()
  `(begin
     (if output-format
	     (output-format! gr output-format))
     (if toggle-axis-end
	     (toggle-axis-end! gr toggle-axis-end))
     (if font-size
	     (font-size! gr font-size))
     (if font-name
	     (font-name! gr font-name))
     (if grid-style
	     (grid-style! gr grid-style))
     (if height-of-plot
	     (height-of-plot! gr height-of-plot))
     (if toggle-frame-on-top
	     (toggle-frame-on-top! gr))
     (if tick-size
	     (tick-size! gr tick-size))
     (if clip-mode
	     (clip-mode! gr clip-mode))
     (if toggle-log-axis
	     (toggle-log-axis! gr toggle-log-axis))
     (if top-label
	     (top-label! gr top-label))
     (if toggle-no-ticks
	     (toggle-no-ticks! gr toggle-no-ticks))
     (if toggle-rotate-y-label
	     (toggle-rotate-y-label! gr))
     (if right-shift
	     (right-shift! gr right-shift))
     (if toggle-round-to-next-tick
	     (toggle-round-to-next-tick! gr toggle-round-to-next-tick))
     (if save-screen
	     (save-screen! gr))
     (if toggle-transpose-axes
	     (toggle-transpose-axes! gr))
     (if upward-shift
	     (upward-shift! gr upward-shift))
     (if width-of-plot
	     (width-of-plot! gr width-of-plot))
     (if x-limits
	     (cond
	      ((and (not (list? x-limits)) (eq? x-limits #t)
		        (x-limits! gr)))
	      ((and (not (list? x-limits)) (real? x-limits))
	       (x-limits! gr x-limits))
	      ((and (list? x-limits) (= 1 (length x-limits)))
	       (x-limits! gr (first x-limits)))
	      ((and (list? x-limits) (= 2 (length x-limits)))
	       (x-limits! gr (first x-limits) (second x-limits)))
	      ((and (list? x-limits) (= 3 (length x-limits)))
	       (x-limits! gr (first x-limits) (second x-limits) (third x-limits)))
	      (else
	       (x-limits! gr))))
     (if x-label
	     (x-label! gr x-label))
     (if y-limits
	     (cond
	      ((and (not (list? y-limits)) (eq? y-limits #t)
		        (y-limits! gr)))
	      ((and (not (list? y-limits)) (real? y-limits))
	       (y-limits! gr y-limits))
	      ((and (list? y-limits) (= 1 (length y-limits)))
	       (y-limits! gr (first y-limits)))
	      ((and (list? y-limits) (= 2 (length y-limits)))
	       (y-limits! gr (first y-limits) (second y-limits)))
	      ((and (list? y-limits) (= 3 (length y-limits)))
	       (y-limits! gr (first y-limits) (second y-limits) (third y-limits)))
	      (else
	       (y-limits! gr))))
     (if y-label
	     (y-label! gr y-label))
     (if bg-color
	     (bg-color! gr bg-color))
     (if bitmap-size
	     (bitmap-size! gr bitmap-size))
     (if emulate-color
	     (emulate-color! gr emulate-color))
     (if frame-color
	     (frame-color! gr frame-color))
     (if frame-line-width
	     (frame-line-width! gr frame-line-width))
     (if max-line-length
	     (max-line-length! gr max-line-length))
     (if page-size
	     (page-size! gr page-size))
     (if pen-colors
	     (pen-colors! gr pen-colors))
     (if title-font-name
	     (title-font-name! gr title-font-name))
     (if auto-abscissa
	     (cond
	      ((and (not (list? auto-abscissa)) (eq? auto-abscissa #t))
	       (auto-abscissa! gr))
	      ((and (not (list? auto-abscissa)) (real? auto-abscissa))
	       (auto-abscissa! gr auto-abscissa))
	      ((and (list? auto-abscissa) (= 1 (length auto-abscissa)))
	       (auto-abscissa! gr (first auto-abscissa)))
	      ((and (list? auto-abscissa) (= 2 (length auto-abscissa)))
	       (auto-abscissa! gr (first auto-abscissa) (second auto-abscissa)))
	      (else
	       (auto-abscissa! gr))))
     (if toggle-auto-bump
	     (toggle-auto-bump! gr))
     (if line-mode
	     (line-mode! gr line-mode))
     (if symbol
	     (cond
	      ((and (not (list? symbol)) (eq? symbol #t))
	       (symbol! gr))
	      ((and (not (real? symbol)) (real? symbol))
	       (symbol! gr symbol))
	      ((and (list? symbol) (= 1 (length symbol)))
	       (symbol! gr (first symbol)))
	      ((and (list? symbol) (= 2 (length symbol)))
	       (symbol! gr (first symbol) (second symbol)))
	      (else
	       (symbol! gr))))
     (if line-width
	     (line-width! gr line-width))
     (if fill-fraction
	     (fill-fraction! gr fill-fraction))
     (if toggle-use-color
	     (toggle-use-color! gr))
     (if symbol-font-name
	     (symbol-font-name! gr symbol-font-name))
     (if portable-output
	     (portable-output! gr))))


(define* (graph arg1 #:optional (arg2 #f) (arg3 #f) (arg4 #f) #:key
		        output-format toggle-axis-end font-size font-name
		        grid-style height-of-plot toggle-frame-on-top
		        tick-size clip-mode toggle-log-axis top-label
		        toggle-no-ticks toggle-rotate-y-label right-shift
		        toggle-round-to-next-tick save-screen toggle-transpose-axes
		        upward-shift width-of-plot x-limits x-label y-limits
		        y-label bg-color bitmap-size emulate-color frame-color
		        frame-line-width max-line-length page-size pen-colors
		        rotation title-font-name title-font-size auto-abscissa
		        toggle-auto-bump line-mode symbol line-width
		        fill-fraction toggle-use-color symbol-font-name
		        ;; reposition
                blankout portable-output
                (outp (current-output-port))
                (errp (current-error-port))
                (test #f))
  (let ((gr (new-graph)))
    (unpack-keys)

    (cond

     ;; arg1 is the ordinate.  There is no abscissa.
     ((and (list? arg1) (not arg2) (not arg3) (not arg4))
      (if (not auto-abscissa)
	      (auto-abscissa! gr))
      (xy-data! gr #f arg1))

     ;; arg1 is the abscissa.  arg2 is the ordinate.
     ((and (list? arg1) (list? arg2) (not arg3) (not arg4))
      (xy-data! gr arg1 arg2))

     ;; arg1 is a function. arg2 is x-min arg3 is x-max
     ((and (procedure? arg1) (real? arg2) (real? arg3) (not arg4))
      (let* ((n 400)
	         (xmin arg2)
	         (xmax arg3)
	         (step (/ (- xmax xmin) n))
	         (xdata (iota n xmin step))
	         (ydata (map arg1 xdata)))
	    (xy-data! gr xdata ydata)))

     ;; arg1 is a function. arg2 is x-min arg3 is x-max. arg4 is step
     ((and (procedure? arg1) (real? arg2) (real? arg3) (real? arg4))
      (let* ((xmin arg2)
	         (xmax arg3)
	         (step arg4)
	         (n (inexact->exact (floor (/ (- xmax xmin) step))))
	         (xdata (iota n xmin step))
	         (ydata (map arg1 xdata)))
	    (xy-data! gr xdata ydata)))

     (else
      (error "blammo!")))

    (if test
        (%graph-test gr test)
        ;; else
        (generate gr outp errp))))


(load-extension "libguile-plotutils" "gupl_graph_init")
