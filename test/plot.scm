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

;; Number of open file descriptors, or #f where that can't be counted.
(define (open-fd-count)
  (and (file-exists? "/proc/self/fd")
       (length (scandir "/proc/self/fd"))))


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
    (let* ((fds-before (open-fd-count))
           (p (newpl "ps" port (current-error-port) (newplparams))))
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
        (test-equal "its file descriptors are closed"
          fds-before (open-fd-count)))
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

(test-end "plot.scm")
