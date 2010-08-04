(use-modules (ice-9 rdelim)
	     (ice-9 format)
	     (srfi srfi-1)
	     (plotutils graph))

(define x (iota 101 0 0.1))
(define y (map (lambda (x) (* (sin x) (exp (- x)))) x))
(define miny (apply min y))
(define y2 (map (lambda (a) (- a miny)) y))

;;

(format #t "No options")
(graph x y)
(read-line)

(format #t "toggle-axis-end \"x\"")
(graph x y #:toggle-axis-end "x")
(read-line)

(format #t "toggle-axis-end \"y\"")
(graph x y #:toggle-axis-end "y")
(read-line)

(format #t "toggle-axis-end \"xy\"")
(graph x y #:toggle-axis-end "xy")
(read-line)

(format #t "font-size 0.1")
(graph x y #:font-size 0.1)
(read-line)

(format #t "font-name \"Times-Italic\"")
(graph x y #:font-name "Times-Italic")
(read-line)

(let loop ((i 0))
  (if (<= i 4)
      (begin
	(format #t "grid-style ~S" i)
	(graph x y #:grid-style i)
	(read-line)
	(loop (+ i 1)))))

(format #t "height of plot 0.4")
(graph x y #:height-of-plot 0.4)
(read-line)

(format #t "toggle-frame-on-top #t")
(graph x y #:toggle-frame-on-top #t)
(read-line)

(format #t "tick-size 0.1")
(graph x y #:tick-size 0.1)
(read-line)

(format #t "tick-size -0.1")
(graph x y #:tick-size -0.1)
(read-line)

(format #t "toggle-log-axis \"x\"")
(graph x y2 #:toggle-log-axis "x")
(read-line)

(format #t "toggle-log-axis \"y\"")
(graph x y2 #:toggle-log-axis "y")
(read-line)

(format #t "toggle-log-axis \"xy\"")
(graph x y2 #:toggle-log-axis "xy")
(read-line)

(format #t "top-label \"Title\"")
(graph x y #:top-label "Title")
(read-line)

(format #t "toggle-no-ticks \"x\"")
(graph x y #:toggle-no-ticks "x")
(read-line)

(format #t "toggle-no-ticks \"y\"")
(graph x y #:toggle-no-ticks "y")
(read-line)

(format #t "toggle-no-ticks \"xy\"")
(graph x y #:toggle-no-ticks "xy")
(read-line)

(format #t "y-label \"units\"")
(graph x y #:y-label "units")
(read-line)

(format #t "y-label \"units\", toggle-rotate-y-label #t")
(graph x y #:y-label "units" #:toggle-rotate-y-label #t)
(read-line)

(format #t "right-shift 0.0")
(graph x y #:right-shift 0.0)
(read-line)

(format #t "x-limits (list -0.4 11.1 0.1)")
(graph x y #f #f #:x-limits (list -0.4 11.1 0.1))
(read-line)

(format #t "x-limits (list -0.4 11.1 0.1); toggle-round-to-next-tick \"x\"")
(graph x y #f #f #:x-limits (list -0.4 11.1 0.1) #:toggle-round-to-next-tick "x")
(read-line)

(define tddx '(1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8))
(define tddy '(1.0 3.0 4.0 3.0 1.0 0.0 1.0 3.0))
(format #t "x-limits (list -0.3 3.1)")
(graph tddx tddy #f #f #:x-limits (list -0.3 3.1))
(read-line)
