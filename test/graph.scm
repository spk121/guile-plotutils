(use-modules (plotutils graph)
             (srfi srfi-1)
             (srfi srfi-64))

(test-begin "graph.scm")

(define xvalues (iota 101 0 0.1))
(define yvalues (map (lambda (x) (* x x)) xvalues))
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

(test-end "graph.scm")
