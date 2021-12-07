(use-modules (ice-9 rdelim)
	     (ice-9 format)
	     (srfi srfi-1)
	     (plotutils graph))

(define x (iota 101 0 0.1))
(define y (map (lambda (x) (* (sin x) (exp (- x)))) x))
(define miny (apply min y))
(define y2 (map (lambda (a) (- a miny)) y))
(define y3 (map (lambda (x) (* (cos x) (exp (- x)))) x))

;; If 1.8.7 is being used, there is a critical bug in optargs
;; that makes using (graph) much less neat.
;; see http://savannah.gnu.org/bugs/?30611
;; Until then, I'm just going to have to insert #f for the 3rd
;; and 4th optional argument


(format #t "Default style, y data only")
(graph y #f #f #f
       #:top-label "Default style, y data only")
(newline)
(display "Press <enter> to continue")
(read-line)

(format #t "Default style, x y data")
(graph x y #f #f
       #:top-label "Default style, x y data")
(newline)
(display "Press <enter> to continue")
(read-line)

(format #t "Two data sets")
(graph (merge x x) (merge y y3) #f #f
       #:toggle-use-color #t
       #:top-label "Two data sets")
(read-line)


(format #t "toggle-axis-end \"x\"")
(graph x y #f #f
       #:toggle-axis-end "x"
       #:top-label "toggle-axis-end \"x\"")
(read-line)

(format #t "toggle-axis-end \"y\"")
(graph x y #f #f
       #:toggle-axis-end "y"
       #:top-label "toggle-axis-end \"y\"")
(read-line)

(format #t "toggle-axis-end \"xy\"")
(graph x y #f #f
       #:toggle-axis-end "xy"
       #:top-label "toggle-axis-end \"xy\"")
(read-line)

(format #t "font-size 0.1")
(graph x y #f #f
       #:font-size 0.1
       #:top-label "font-size 0.1")
(read-line)

(format #t "font-name \"Times-Italic\"")
(graph x y #f #f
       #:font-name "Times-Italic"
       #:top-label "font-name \"Times-Italic\"")
(read-line)

(let loop ((i 0))
  (if (<= i 4)
      (begin
	(format #t "grid-style ~S" i)
	(graph x y #f #f
	       #:grid-style i
	       #:top-label (format #f "grid-style ~S" i))
	(read-line)
	(loop (+ i 1)))))

(format #t "height of plot 0.4")
(graph x y #f #f
       #:height-of-plot 0.4
       #:top-label "height of plot 0.4")
(read-line)

(format #t "toggle-frame-on-top #t")
(graph x y #f #f
       #:toggle-frame-on-top #t
       #:top-label "toggle-frame-on-top #t")
(read-line)

(format #t "tick-size 0.1")
(graph x y #f #f
       #:tick-size 0.1
       #:top-label "tick-size 0.1")
(read-line)

(format #t "tick-size -0.1")
(graph x y #f #f
       #:tick-size -0.1
       #:top-label "tick-size -0.1")
(read-line)

(format #t "toggle-log-axis \"x\"")
(graph x y2 #f #f
       #:toggle-log-axis "x"
       #:top-label "toggle-log-axis \"x\"")
(read-line)

(format #t "toggle-log-axis \"y\"")
(graph x y2 #f #f
       #:toggle-log-axis "y"
       #:top-label "toggle-log-axis \"y\"")
(read-line)

(format #t "toggle-log-axis \"xy\"")
(graph x y2 #f #f
       #:toggle-log-axis "xy"
       #:top-label "toggle-log-axis \"xy\"")
(read-line)

(format #t "toggle-no-ticks \"x\"")
(graph x y #f #f
       #:toggle-no-ticks "x"
       #:top-label "toggle-no-ticks \"x\"")
(read-line)

(format #t "toggle-no-ticks \"y\"")
(graph x y #f #f
       #:toggle-no-ticks "y"
       #:top-label "toggle-no-ticks \"y\"")
(read-line)

(format #t "toggle-no-ticks \"xy\"")
(graph x y #f #f
       #:toggle-no-ticks "xy"
       #:top-label "toggle-no-ticks \"xy\"")
(read-line)

(format #t "y-label \"units\"")
(graph x y #f #f
       #:y-label "units"
       #:top-label "y-label \"units\"")
(read-line)

(format #t "y-label \"units\", toggle-rotate-y-label #t")
(graph x y #f #f
       #:y-label "units"
       #:toggle-rotate-y-label #t
       #:top-label "y-label \"units\"\ntoggle-rotate-y-label #t")
(read-line)

(format #t "right-shift 0.0")
(graph x y #f #f
       #:right-shift 0.0
       #:top-label "right-shift 0.0")
(read-line)

(format #t "x-limits (list 0 11.1 2.0); toggle-round-to-next-tick \"x\"")
(graph x y #f #f
       #:x-limits (list 0 11.1 2.0)
       #:toggle-round-to-next-tick "x"
       #:top-label "toggle-round-to-next-tick \"x\"")
(read-line)

(format #t "y-limits (list 0 0.5 0.2); toggle-round-to-next-tick \"y\"")
(graph x y #f #f
       #:y-limits (list 0 0.5 0.2)
       #:toggle-round-to-next-tick "y"
       #:top-label "toggle-round-to-next-tick \"y\"")
(read-line)

(format #t "y-limits (list 0 0.5 0.2); toggle-round-to-next-tick \"xy\"")
(graph x y #f #f
       #:x-limits (list 0 11.1 2.0)
       #:y-limits (list 0 0.5 0.2)
       #:toggle-round-to-next-tick "xy"
       #:top-label "toggle-round-to-next-tick \"xy\"")
(read-line)

(format #t "toggle-transpose-axes #t")
(graph x y #f #f
       #:toggle-transpose-axes #t
       #:top-label "toggle-transpose-axes #t")
(read-line)

(format #t "upward-shift 0.0")
(graph x y #f #f
       #:upward-shift 0.0
       #:top-label "upward-shift 0.0")
(read-line)

(format #t "width of plot 0.4")
(graph x y #f #f
       #:width-of-plot 0.4
       #:top-label "width of plot 0.4")
(read-line)

(format #t "x-limits #t")
(graph x y #f #f
       #:x-limits #t
       #:top-label "x-limits #t")
(read-line)

(format #t "x-limits 1")
(graph x y #f #f
       #:x-limits 1
       #:top-label "x-limits 1")
(read-line)

(format #t "x-limits (list 1)")
(graph x y #f #f
       #:x-limits '(1)
       #:top-label "x-limits '(1)")
(read-line)

(format #t "x-limits (list 1 2)")
(graph x y #f #f
       #:x-limits '(1 2)
       #:top-label "x-limits '(1 2)")
(read-line)

(format #t "x-limits (list 1 2 0.1)")
(graph x y #f #f
       #:x-limits '(1 2 0.1)
       #:top-label "x-limits '(1 2 0.1)")
(read-line)

(format #t "x-label \"units\"")
(graph x y #f #f
       #:x-label "units"
       #:top-label "x-label \"units\"")
(read-line)

(format #t "y-limits #t")
(graph x y #f #f
       #:y-limits #t
       #:top-label "y-limits #t")
(read-line)

(format #t "y-limits 1")
(graph x y #f #f
       #:y-limits -1
       #:top-label "y-limits -1")
(read-line)

(format #t "y-limits (list -1)")
(graph x y #f #f
       #:y-limits '(-1)
       #:top-label "y-limits '(-1)")
(read-line)

(format #t "y-limits (list -1 2)")
(graph x y #f #f
       #:y-limits '(-1 2)
       #:top-label "y-limits '(-1 2)")
(read-line)

(format #t "y-limits (list -1 2 0.1)")
(graph x y #f #f
       #:y-limits '(-1 2 0.1)
       #:top-label "y-limits '(-1 2 0.1)")
(read-line)

(format #t "y-label \"units\"")
(graph x y #f #f
       #:y-label "units"
       #:top-label "y-label \"units\"")
(read-line)

(format #t "bg-color \"green\"")
(graph x y #f #f
       #:bg-color "green"
       #:top-label "bg-color \"green\"")
(read-line)

(format #t "bitmap-size \"200x200\"")
(graph x y #f #f
       #:bitmap-size "200x200"
       #:top-label "bitmap-size \"200x200\"")
(read-line)

(format #t "bg-color \"green\"; emulate-color \"yes\"")
(graph x y #f #f
       #:bg-color "green"
       #:emulate-color "yes"
       #:top-label "bg-color \"green\"; emulate-color \"yes\"")
(read-line)

(format #t "frame-color \"green\"")
(graph x y #f #f
       #:frame-color "green"
       #:top-label "frame-color \"green\"")
(read-line)

(format #t "frame-line-width 0.01")
(graph x y #f #f
       #:frame-line-width 0.01
       #:top-label "frame-line-width 0.01")
(read-line)



