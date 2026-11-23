(use-modules (plotutils plot)
             (ice-9 ftw)
             (ice-9 textual-ports)
             (srfi srfi-1)
             (srfi srfi-11)
             (srfi srfi-64))

;; Return a fresh temporary output port and its file name.
(define (temp-output-port)
  (let ((port (mkstemp! (string-copy "/tmp/gupl-test-XXXXXX"))))
    (values port (port-filename port))))

(define (file->string name)
  (call-with-input-file name get-string-all))

;; Number of this process's file descriptors open on the file NAME, or #f
;; where that can't be determined.  Counting only descriptors on one file
;; keeps the result independent of unrelated ones, such as those of other
;; tests' plotters, which the garbage collector may close at any time.
(define (fds-open-on name)
  (and (file-exists? "/proc/self/fd")
       (let ((target (canonicalize-path name)))
         (length
          (filter (lambda (fd)
                    (equal? target
                            (false-if-exception
                             (readlink (string-append "/proc/self/fd/" fd)))))
                  (scandir "/proc/self/fd"))))))


(test-begin "plot.scm")
(test-assert "newplparams returns a <plparams>"
  (plparams? (newplparams)))
(test-assert "<plparams> equal themselves"
  (let ((p (newplparams)))
    (eq? p p)))

(test-assert "newpl returns a <plotter>"
  (plotter? (newpl "png" (current-output-port) (current-error-port) (newplparams))))
(test-assert "<plotter> equals itself"
  (let ((p (newpl "png" (current-output-port) (current-error-port) (newplparams))))
    (eq? p p)))

(test-assert "linemod! is exported"
  (procedure? linemod!))

(test-equal "closepath! returns 0 on an open plotter"
  0
  (let ((p (newpl "svg" (open-output-file "/dev/null")
                  (current-error-port) (newplparams))))
    (openpl! p)
    (move! p 0 0)
    (cont! p 1 0)
    (cont! p 1 1)
    (let ((ret (closepath! p)))
      (closepl! p)
      ret)))

;; deletepl! releases a Plotter immediately.  Postscript Plotters write
;; nothing until they are deleted, so this also checks that deletepl!
;; is what produces the output.
(test-group "deletepl! deletes a Plotter immediately"
  (let-values (((port name) (temp-output-port)))
    (let* ((fds-before (fds-open-on name))
           (p (newpl "ps" port (current-error-port) (newplparams)))
           (fds-with-plotter (fds-open-on name)))
      (openpl! p)
      (space! p 0 0 10 10)
      (line! p 0 0 10 10)
      (closepl! p)
      (test-equal "Postscript output is held until deletion"
        0 (stat:size (stat name)))
      (test-equal "deletepl! returns 0" 0 (deletepl! p))
      (test-assert "Postscript output is written by deletepl!"
        (string-contains (file->string name) "%%EOF"))
      (test-assert "a deleted plotter is no longer a plotter?"
        (not (plotter? p)))
      (test-equal "deleting twice is harmless" 0 (deletepl! p))
      (test-equal "using a deleted plotter is an error"
        'wrong-type-arg
        (catch #t (lambda () (openpl! p) 'no-error) (lambda (key . args) key)))
      (when fds-before
        (test-equal "the plotter held a descriptor on the output file"
          (+ fds-before 1) fds-with-plotter)
        (test-equal "its file descriptors are closed"
          fds-before (fds-open-on name)))
      (close-port port)
      (delete-file name))))

;; Deliberately do NOT call deletepl! here: this Plotter is left for the
;; garbage collector to delete.  Single-page formats such as SVG write
;; their output at closepl!, so the file is complete without deletepl!.
(test-group "a Plotter not deleted with deletepl!"
  (let-values (((port name) (temp-output-port)))
    (let ((p (newpl "svg" port (current-error-port) (newplparams))))
      (openpl! p)
      (space! p 0 0 10 10)
      (line! p 0 0 10 10)
      (closepl! p)
      (test-assert "SVG output is complete after closepl!"
        (string-contains (file->string name) "</svg>"))
      (test-assert "the plotter is still a plotter?"
        (plotter? p))
      (close-port port)
      (delete-file name))))

;; Plotters write to a duplicate of the port's file descriptor, bypassing
;; the port's buffer, so pending Scheme output must be flushed first or it
;; would land after the plot.
(test-group "Scheme output and plotter output stay in order"
  (let-values (((port name) (temp-output-port)))
    (display "BEFORE-NEWPL\n" port)
    (let ((p (newpl "svg" port (current-error-port) (newplparams))))
      (openpl! p)
      (display "BEFORE-CLOSEPL\n" port)
      (line! p 0 0 1 1)
      (closepl! p)
      (display "AFTER-CLOSEPL\n" port)
      (deletepl! p)
      (close-port port)
      (let* ((text (file->string name))
             (before-newpl (string-contains text "BEFORE-NEWPL"))
             (before-closepl (string-contains text "BEFORE-CLOSEPL"))
             (svg (string-contains text "<svg"))
             (after-closepl (string-contains text "AFTER-CLOSEPL")))
        (test-equal "text written before newpl comes first" 0 before-newpl)
        (test-assert "text written before closepl! precedes the SVG"
          (< before-closepl svg))
        (test-assert "text written after closepl! follows the SVG"
          (< svg after-closepl)))
      (delete-file name))))

(test-group "deletepl! after the port is closed"
  (let-values (((port name) (temp-output-port)))
    (let ((p (newpl "ps" port (current-error-port) (newplparams))))
      (openpl! p)
      (line! p 0 0 1 1)
      (closepl! p)
      (close-port port)
      (test-equal "deletepl! still succeeds" 0 (deletepl! p))
      (test-assert "and still writes the Postscript"
        (string-contains (file->string name) "%%EOF"))
      (delete-file name))))

(test-group "linedash! takes a list of dash lengths"
  (let-values (((port name) (temp-output-port)))
    (let ((p (newpl "svg" port (current-error-port) (newplparams))))
      (openpl! p)
      (space! p 0 0 100 100)
      (test-equal "integer dashes" 0 (linedash! p '(10 5)))
      (line! p 0 0 100 100)
      (test-equal "real dashes with an offset" 0 (linedash! p '(10.5 2.5) 3.25))
      (line! p 0 100 100 0)
      (test-equal "an empty list means solid" 0 (linedash! p '()))
      (test-equal "a negative dash is rejected" -1 (linedash! p '(4 -1)))
      (test-equal "a non-list is an error"
        'wrong-type-arg
        (catch #t (lambda () (linedash! p 5) 'no-error) (lambda (key . args) key)))
      (test-equal "a non-number element is an error"
        'wrong-type-arg
        (catch #t (lambda () (linedash! p '(1 x)) 'no-error)
          (lambda (key . args) key)))
      (closepl! p)
      (deletepl! p)
      (close-port port)
      (let ((text (file->string name)))
        (test-assert "integer dash pattern reaches the output"
          (string-contains text "stroke-dasharray=\"10, 5\""))
        (test-assert "real dash pattern and offset reach the output"
          (and (string-contains text "stroke-dasharray=\"10.5, 2.5\"")
               (string-contains text "stroke-dashoffset=\"3.25\""))))
      (delete-file name))))

(test-group "integer arguments"
  (let ((p (newpl "svg" (open-output-file "/dev/null")
                  (current-error-port) (newplparams))))
    (openpl! p)
    (test-equal "an integer-valued real is accepted" 0 (filltype! p 1.0))
    (test-equal "as are integer-valued colors" 0 (pencolor! p 0.0 65535.0 0))
    (test-equal "and marker types" 0 (marker! p 1 1 3.0 5))
    (test-equal "a non-integer is still an error"
      'wrong-type-arg
      (catch #t (lambda () (filltype! p 1.5) 'no-error)
        (lambda (key . args) key)))
    (test-equal "coordinates too big for a C int use the float functions"
      0 (line! p 0 0 10000000000 0))
    (closepl! p)
    (deletepl! p)))

(test-group "list-fonts"
  (let ((png (list-fonts "png"))
        (svg (list-fonts "svg")))
    (test-assert "returns a list of strings"
      (and (pair? png) (every string? png)))
    (test-equal "starts with the Hershey fonts, as GNU graph does"
      "HersheySerif" (car png))
    (test-assert "bitmap formats have only the Hershey fonts"
      (and (not (member "Times-Roman" png))
           (every (lambda (name) (string-prefix? "Hershey" name)) png)))
    (test-assert "SVG also has the Postscript fonts"
      (member "Times-Roman" svg))
    (test-assert "PCL has the PCL fonts"
      (member "Univers" (list-fonts "pcl")))
    (test-equal "a plotter gives the same list as its output format"
      svg
      (let* ((p (newpl "svg" (open-output-file "/dev/null")
                       (current-error-port) (newplparams)))
             (fonts (list-fonts p)))
        (deletepl! p)
        fonts))
    (test-equal "an unknown output format is an error"
      'misc-error
      (catch #t (lambda () (list-fonts "no-such-format") 'no-error)
        (lambda (key . args) key)))
    (test-equal "a deleted plotter is an error"
      'wrong-type-arg
      (let ((p (newpl "svg" (open-output-file "/dev/null")
                      (current-error-port) (newplparams))))
        (deletepl! p)
        (catch #t (lambda () (list-fonts p) 'no-error)
          (lambda (key . args) key))))
    (test-assert "listing fonts for Postscript writes nothing"
      ;; a Postscript Plotter writes its document when deleted; the
      ;; temporary one used by list-fonts has no output stream
      (let-values (((port name) (temp-output-port)))
        (with-output-to-port port (lambda () (list-fonts "ps")))
        (close-port port)
        (let ((size (stat:size (stat name))))
          (delete-file name)
          (zero? size))))))

(test-end "plot.scm")
