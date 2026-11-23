(use-modules (plotutils graph)
             (ice-9 rdelim)
             (srfi srfi-1)
             (srfi srfi-64))

(test-begin "graph.scm")

(define xvalues (iota 101 0 0.1))
(define yvalues (map (lambda (x) (* x x)) xvalues))
(test-equal "output-format defaults to meta"
  "meta"
  (graph xvalues yvalues
         #:test "output_format"))
(test-equal "output-format"
  "png"
  (graph xvalues yvalues
         #:output-format "png"
         #:test "output_format"))
(test-equal "toggle-axis-end"
  0
  (graph xvalues yvalues
         #:test "switch_axis_end"))
(test-equal "toggle-axis-end x"
  1
  (graph xvalues yvalues
         #:toggle-axis-end "x"
         #:test "switch_axis_end"))
(test-equal "toggle-axis-end y"
  2
  (graph xvalues yvalues
         #:toggle-axis-end "y"
         #:test "switch_axis_end"))
(test-equal "toggle-axis-end xy"
  3
  (graph xvalues yvalues
         #:toggle-axis-end "xy"
         #:test "switch_axis_end"))
(test-equal "font-size 0.1"
  0.1
  (graph xvalues yvalues
         #:font-size 0.1
         #:test "font_size"))
(test-equal "font-name Serif"
  "Serif"
  (graph xvalues yvalues
         #:font-name "Serif"
         #:test "font_name"))
(test-equal "symbol_index 2"
  2
  (graph xvalues yvalues
         #:font-name "Serif"
         #:symbol '(2 0.25)
         #:test "symbol_index"))
(test-equal "symbol_size 0.25"
  0.25
  (graph xvalues yvalues
         #:font-name "Serif"
         #:symbol '(2 0.25)
         #:test "symbol_size"))

(test-assert "pen-colors accepted"
  (graph xvalues yvalues
         #:pen-colors "1=red:2=green"
         #:test "output_format"))
(test-equal "pen-colors rejects a bad string"
  'misc-error
  (catch #t
    (lambda ()
      (graph xvalues yvalues
             #:pen-colors "9=red"
             #:test "output_format")
      'no-error)
    (lambda (key . args) key)))

(test-equal "symbol given as a bare number"
  5
  (graph xvalues yvalues
         #:symbol 5
         #:test "symbol_index"))

(test-equal "function plot includes both endpoints (default step)"
  401
  (graph (lambda (x) (* x x)) 0 1
         #:test "no_of_points"))
(test-equal "function plot includes both endpoints (explicit step)"
  5
  (graph (lambda (x) (* x x)) 0 1 1/4
         #:test "no_of_points"))
(test-equal "function plot endpoint survives float roundoff"
  4
  (graph (lambda (x) (* x x)) 0 0.3 0.1
         #:test "no_of_points"))

(test-equal "rotation given as a number"
  "90"
  (graph xvalues yvalues
         #:rotation 90
         #:test "rotation_angle"))
(test-equal "rotation given as a string"
  "180"
  (graph xvalues yvalues
         #:rotation "180"
         #:test "rotation_angle"))

(test-equal "title-font-size"
  0.12
  (graph xvalues yvalues
         #:title-font-size 0.12
         #:test "title_font_size"))

(test-equal "blankout"
  1.0
  (graph xvalues yvalues
         #:blankout 1.0
         #:test "blankout_fraction"))

(test-equal "x-limits with only an upper limit"
  '(#f #t 10.0)
  (map (lambda (key)
         (graph xvalues yvalues
                #:x-limits '(#f 10)
                #:test key))
       '("spec_min_x" "spec_max_x" "max_x")))
(test-equal "y-limits with only a spacing"
  '(#f #f #t 2.0)
  (map (lambda (key)
         (graph xvalues yvalues
                #:y-limits '(#f #f 2)
                #:test key))
       '("spec_min_y" "spec_max_y" "spec_spacing_y" "spacing_y")))
(test-equal "x-limits with all three"
  '(#t 1.0 #t 9.0 #t 0.5)
  (map (lambda (key)
         (graph xvalues yvalues
                #:x-limits '(1 9 0.5)
                #:test key))
       '("spec_min_x" "min_x" "spec_max_x" "max_x" "spec_spacing_x" "spacing_x")))

(test-assert "bad arguments give a descriptive error"
  (catch 'misc-error
    (lambda () (graph "not a list") #f)
    (lambda (key subr msg args . rest)
      ;; (error msg . irritants) gives msg as the first format argument
      (string-contains (car args) "expected (graph ylist)"))))

(test-assert "text written before graph precedes the graph"
  (let* ((port (mkstemp! (string-copy "/tmp/gupl-test-XXXXXX")))
         (name (port-filename port)))
    (display "HEADER\n" port)
    (graph xvalues yvalues #:output-format "svg" #:outp port)
    (close-port port)
    (let ((text (call-with-input-file name
                  (lambda (in) (read-line in)))))
      (delete-file name)
      (equal? text "HEADER"))))

(test-equal "a large dataset is read completely"
  20000
  (graph (iota 20000) (iota 20000) #:test "no_of_points"))
(test-equal "xy-data! checks that the data are lists"
  'wrong-type-arg
  (catch #t
    (lambda ()
      ((@@ (plotutils graph) xy-data!)
       ((@@ (plotutils graph) new-graph))
       (iota 3) (list->vector (iota 3)))
      'no-error)
    (lambda (key . args) key)))

;;; Error bars.  "points" gives each point as (x y xmin xmax ymin ymax).

(define (error-bars . args)
  (apply graph '(1 2 3) '(10 20 30) (append args '(#:test "points"))))
(define (error-key thunk)
  (catch #t (lambda () (thunk) 'no-error) (lambda (key . args) key)))

(test-equal "y-error: one number for every point"
  '((1.0 10.0 #f #f 8.0 12.0) (2.0 20.0 #f #f 18.0 22.0)
    (3.0 30.0 #f #f 28.0 32.0))
  (error-bars #:y-error 2))
(test-equal "y-error: one (lower . upper) pair for every point"
  '((1.0 10.0 #f #f 9.0 13.0) (2.0 20.0 #f #f 19.0 23.0)
    (3.0 30.0 #f #f 29.0 33.0))
  (error-bars #:y-error '(1 . 3)))
(test-equal "y-error: a list, mixing numbers, pairs, and #f"
  '((1.0 10.0 #f #f 9.0 11.0) (2.0 20.0 #f #f 18.0 24.0)
    (3.0 30.0 #f #f #f #f))
  (error-bars #:y-error '(1 (2 . 4) #f)))
(test-equal "y-error: a procedure of x and y"
  '((1.0 10.0 #f #f 9.0 11.0) (2.0 20.0 #f #f 18.0 22.0)
    (3.0 30.0 #f #f 27.0 33.0))
  (error-bars #:y-error (lambda (x y) (/ y 10))))
(test-equal "x-error takes the same forms"
  '((1.0 10.0 0.75 1.25 #f #f) (2.0 20.0 1.5 3.0 #f #f)
    (3.0 30.0 #f #f #f #f))
  (error-bars #:x-error '(0.25 (0.5 . 1) #f)))
(test-equal "error lists built with merge line up with the data"
  '((1.0 10.0 #f #f 9.0 11.0) (3.0 30.0 #f #f 27.0 33.0))
  (graph (merge '(1) '(3)) (merge '(10) '(30))
         #:y-error (merge '(1) '(3))
         #:test "points"))
(test-equal "bounds->errors turns absolute limits into amounts"
  '((2 . 1) (5 . 6) #f)
  (bounds->errors '(10 20 30) '(8 15 #f) '(11 26 31)))
(test-equal "y-error from bounds->errors"
  '((1.0 10.0 #f #f 8.0 11.0) (2.0 20.0 #f #f 15.0 26.0)
    (3.0 30.0 #f #f #f #f))
  (error-bars #:y-error (bounds->errors '(10 20 30) '(8 15 #f) '(11 26 31))))
(test-equal "an error list of the wrong length is an error"
  'misc-error
  (error-key (lambda () (error-bars #:y-error '(1 2)))))
(test-equal "a negative error amount is an error"
  'misc-error
  (error-key (lambda () (error-bars #:y-error -1))))
(test-equal "a bar at a #f separator is an error"
  'misc-error
  (error-key (lambda ()
               (graph (merge '(1) '(2)) (merge '(1) '(2))
                      #:y-error '(1 1 1) #:test "points"))))
(test-equal "bounds->errors rejects a value outside its bounds"
  'misc-error
  (error-key (lambda () (bounds->errors '(5) '(6) '(7)))))
(test-assert "on a log axis a bar reaching zero keeps its point"
  (let ((points (graph '(1 2) '(1 10)
                       #:toggle-log-axis "y"
                       #:y-error 5
                       #:test "points")))
    (and (= 2 (length points))
         (equal? -inf.0 (list-ref (first points) 4)))))
(test-assert "a graph with error bars can be generated"
  (let* ((port (mkstemp! (string-copy "/tmp/gupl-test-XXXXXX")))
         (name (port-filename port)))
    (graph '(1 2 3) '(10 20 30)
           #:y-error '(1 . 2) #:x-error 0.1
           #:output-format "svg" #:outp port)
    (close-port port)
    (let ((size (stat:size (stat name))))
      (delete-file name)
      (> size 0))))

(test-assert "log-axis tick spacings that are too fine or not positive are ignored"
  (let ((null (open-output-file "/dev/null")))
    (for-each (lambda (limits)
                (graph '(1 10 100) '(1 10 100)
                       #:output-format "svg" #:toggle-log-axis "y"
                       #:y-limits limits #:outp null #:errp null))
              '((1 1e12 1) (1 1000 0) (1 1000 -5) (1 1000 100)))
    #t))

(test-equal "integer-valued reals are accepted as integers"
  '(3 AXES_AND_BOX_AND_GRID)
  (list (graph xvalues yvalues #:symbol 3.0 #:test "symbol_index")
        (graph xvalues yvalues #:grid-style 3.0 #:test "grid_spec")))

(test-assert "portable-output writes text metafile output"
  (let* ((port (mkstemp! (string-copy "/tmp/gupl-test-XXXXXX")))
         (name (port-filename port)))
    (graph xvalues yvalues #:portable-output #t #:outp port)
    (close-port port)
    (let ((header (call-with-input-file name read-line)))
      (delete-file name)
      ;; binary metafiles start with "#PLOT 1", portable ones "#PLOT 2"
      (equal? header "#PLOT 2"))))

;; Invalid axis settings raise Scheme errors.  (A zero tick spacing used
;; to call exit(), ending the whole Guile process.)
(define (generate-error-message . options)
  (let ((null (open-output-file "/dev/null")))
    (catch 'misc-error
      (lambda ()
        (apply graph '(1 2 3) '(1 4 9) #:output-format "svg" #:outp null
               options)
        #f)
      (lambda (key subr message args . rest)
        (apply format #f message args)))))

(test-assert "a zero tick spacing is an error, not an exit"
  (string-contains (generate-error-message #:x-limits '(0 10 0))
                   "spacing between ticks"))
(test-assert "an infinite axis limit is an error"
  (string-contains (generate-error-message #:x-limits (list 0 +inf.0))
                   "not finite"))
(test-assert "a NaN axis limit is an error"
  (string-contains (generate-error-message #:y-limits (list +nan.0 1))
                   "not finite"))
(test-assert "infinite data are an error"
  (let ((null (open-output-file "/dev/null")))
    (catch 'misc-error
      (lambda ()
        (graph '(1 2 3) (list 1 +inf.0 3) #:output-format "svg" #:outp null)
        #f)
      (lambda (key subr message args . rest)
        (string-contains (apply format #f message args) "not finite")))))
(test-assert "a nonpositive limit on a log axis is an error"
  (string-contains (generate-error-message #:y-limits '(-1 10)
                                           #:toggle-log-axis "y")
                   "logarithmic axis"))
(test-equal "valid limits still work"
  #f
  (generate-error-message #:x-limits '(0 10 2)))

(define (errp-output . options)
  "Generate a graph of points including a negative y value, returning
what was written to its error port."
  (let* ((null (open-output-file "/dev/null"))
         (port (mkstemp! (string-copy "/tmp/gupl-test-XXXXXX")))
         (name (port-filename port)))
    (apply graph '(1 2 3) '(-1 10 100) #:output-format "svg"
           #:outp null #:errp port options)
    (close-port port)
    (let ((text (call-with-input-file name
                  (lambda (in) (read-delimited "" in)))))
      (delete-file name)
      (if (eof-object? text) "" text))))

(test-assert "a point dropped from a log plot is reported on #:errp"
  (string-contains (errp-output #:toggle-log-axis "y")
                   "inappropriate point (1,-1)"))
(test-assert "plotter warnings are written to #:errp"
  (string-contains (errp-output #:x-limits '(1 3 1e-9))
                   "gives too many ticks"))
(test-assert "the too-few-ticks warning is given for each graph"
  (and (string-contains (errp-output #:toggle-log-axis "y") "too few")
       (string-contains (errp-output #:toggle-log-axis "y") "too few")))
(test-assert "reading warnings wait for generate"
  (string-contains (graph '(1 2 3) '(-1 10 100)
                          #:toggle-log-axis "y" #:test "warnings")
                   "inappropriate point"))

(define (first-line-color . options)
  "The stroke color of the first polyline in an SVG graph."
  (let* ((port (mkstemp! (string-copy "/tmp/gupl-test-XXXXXX")))
         (name (port-filename port)))
    (apply graph '(1 2 3) '(1 4 9) #:output-format "svg"
           #:toggle-use-color #t #:outp port options)
    (close-port port)
    (let* ((svg (call-with-input-file name (lambda (in) (read-delimited "" in))))
           (line (string-contains svg "<polyline"))
           (start (+ 8 (string-contains svg "stroke=\"" line))))
      (delete-file name)
      (substring svg start (string-index svg #\" start)))))

(test-equal "pen-colors sets the line color" "blue"
  (first-line-color #:pen-colors "1=blue"))
(test-equal "pen-colors doesn't carry over to the next graph" "red"
  (first-line-color))
(test-equal "a rejected pen-colors string changes nothing" "red"
  (begin
    (catch 'misc-error
      (lambda () (first-line-color #:pen-colors "1=green:9=x"))
      (lambda args #f))
    (first-line-color)))

;; Changing the symbol font between datasets: points already read keep
;; their font (its name used to be freed under them), and the new font
;; applies to later data (it used to be ignored).
(test-assert "each dataset keeps its own symbol font"
  (let* ((internal (lambda (name) (module-ref (resolve-module '(plotutils graph)) name)))
         (g ((internal 'new-graph)))
         (port (mkstemp! (string-copy "/tmp/gupl-test-XXXXXX")))
         (name (port-filename port)))
    ((internal 'output-format!) g "svg")
    ((internal 'symbol!) g 65 0.05)
    ((internal 'symbol-font-name!) g "Times-Roman")
    ((internal 'xy-data!) g '(1 2 3) '(1 2 3))
    ((internal 'symbol-font-name!) g "Courier")
    ((internal 'xy-data!) g '(1 2 3) '(3 2 1))
    ((internal 'generate) g port port)
    (close-port port)
    (let ((svg (call-with-input-file name (lambda (in) (read-delimited "" in)))))
      (delete-file name)
      (and (string-contains svg "font-family=\"Times-Roman")
           (string-contains svg "font-family=\"Courier")))))

;;; Datasets.  "point-styles" gives each point's style as (linemode symbol
;;; symbol-size line-width fill-fraction use-color symbol-font-name).

(define (first-style-of-each-dataset styles points-per-dataset)
  "The style of the first point of each dataset of POINTS-PER-DATASET points."
  (let loop ((styles styles) (acc '()))
    (if (null? styles)
        (reverse acc)
        (loop (list-tail styles points-per-dataset) (cons (car styles) acc)))))

(define (dataset-styles . args)
  (first-style-of-each-dataset
   (apply graph (append args '(#:test "point-styles")))
   3))

(test-assert "dataset makes a dataset"
  (dataset? (dataset '(1 2 3) '(4 5 6))))
(test-equal "dataset checks its data"
  'misc-error
  (catch #t (lambda () (dataset "no data") 'no-error)
    (lambda (key . args) key)))
(test-equal "dataset checks its options"
  'misc-error
  (catch #t (lambda () (dataset '(1 2) #:symbol 300) 'no-error)
    (lambda (key . args) key)))

(test-equal "each dataset gets its own style; unset options take graph's"
  '((1 0 0.03 -0.001 -1.0 #f "ZapfDingbats")
    (0 3 0.05 -0.001 -1.0 #f "ZapfDingbats")
    (3 0 0.03 0.01 0.5 #t "Courier"))
  (dataset-styles (list (dataset '(1 2 3) '(1 2 3))
                        (dataset '(1 2 3) '(2 3 4) #:line-mode 0 #:symbol '(3 0.05))
                        (dataset '(1 2 3) '(3 4 5) #:line-width 0.01
                                 #:fill-fraction 0.5 #:use-color #t
                                 #:symbol-font-name "Courier"))))

(test-equal "graph-level options are the defaults for every dataset"
  '((2 5 0.02 0.004 -1.0 #t "ZapfDingbats")
    (3 5 0.02 0.004 -1.0 #f "ZapfDingbats"))
  (dataset-styles (list (dataset '(1 2 3) '(1 2 3))
                        (dataset '(1 2 3) '(2 3 4) #:use-color #f))
                  #:line-mode 2 #:symbol '(5 0.02) #:line-width 0.004
                  #:toggle-use-color #t))

(test-equal "a dataset's linemode doesn't change the auto-bumping of later ones"
  '(1 0 3)
  (map car
       (dataset-styles (list (dataset '(1 2 3) '(1 2 3))
                             (dataset '(1 2 3) '(1 2 3) #:line-mode 0)
                             (dataset '(1 2 3) '(1 2 3))))))

(test-equal "datasets merged into one dataset's lists count for auto-bumping"
  '(1 2 3)
  (map car
       (first-style-of-each-dataset
        (graph (list (dataset (merge '(1 2 3) '(1 2 3)) (merge '(1 2 3) '(1 2 3)))
                     (dataset '(1 2 3) '(1 2 3)))
               #:test "point-styles")
        3)))

(test-equal "a dataset of y values alone gets an automatic abscissa"
  '((0.0 5.0) (1.0 6.0) (10.0 1.0) (20.0 2.0))
  (map (lambda (p) (list (first p) (second p)))
       (graph (list (dataset '(5 6)) (dataset '(10 20) '(1 2)))
              #:test "points")))

(test-equal "error bars per dataset, or graph's for datasets without"
  '((9.0 11.0) (19.0 21.0) (29.5 30.5) (39.5 40.5))
  (map (lambda (p) (list (fifth p) (sixth p)))
       (graph (list (dataset '(1 2) '(10 20) #:y-error 1)
                    (dataset '(3 4) '(30 40)))
              #:y-error 0.5 #:test "points")))

(test-assert "a graph of datasets can be generated"
  (let* ((port (mkstemp! (string-copy "/tmp/gupl-test-XXXXXX")))
         (name (port-filename port)))
    (graph (list (dataset '(1 2 3) '(1 4 9))
                 (dataset (lambda (x) (* 2 x)) 0 3 #:symbol 3))
           #:output-format "svg" #:outp port)
    (close-port port)
    (let ((size (stat:size (stat name))))
      (delete-file name)
      (> size 0))))

(test-end "graph.scm")
