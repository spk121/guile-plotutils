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

(test-end "graph.scm")
