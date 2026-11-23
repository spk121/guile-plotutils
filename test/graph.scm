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

(test-end "graph.scm")
