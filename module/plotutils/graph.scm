(define-module (plotutils graph)
  #:use-module (srfi srfi-1)
  #:use-module (ice-9 optargs)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-11)
  ;; `merge' deliberately replaces Guile's core `merge' (which merges
  ;; sorted lists) in modules that import this one; #:replace says so,
  ;; instead of warning about it on every import.
  #:replace (merge)
  #:export (bounds->errors
	        dataset
	        dataset?
	        graph
	        subgraph
	        multigraph))

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
;; 	    pen-colors!
;; 	    bg-color!
;; 	    frame-color!
;; 	    auto-abscissa!
;; 	    x-limits!
;; 	    y-limits!
;; 	    symbol!
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
	     (unless (pen-colors! gr pen-colors)
	       (error "graph: invalid #:pen-colors string" pen-colors)))
     (if title-font-name
	     (title-font-name! gr title-font-name))
     (if blankout
	     (blankout! gr blankout))
     (if title-font-size
	     (title-font-size! gr title-font-size))
     (if rotation
	     (rotation-angle! gr (if (number? rotation)
				     (number->string rotation)
				     rotation)))
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
	      ((and (not (list? symbol)) (real? symbol))
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


;;; Error bars

(define (nonnegative-real? x)
  (and (real? x) (>= x 0)))

;; An error bar for one point: #f (no bar), a nonnegative real E (the bar
;; spans value-E to value+E), or a pair (LOWER . UPPER) of nonnegative
;; reals (the bar spans value-LOWER to value+UPPER).
(define (check-error-bar keyword e)
  (unless (or (not e)
              (nonnegative-real? e)
              (and (pair? e)
                   (nonnegative-real? (car e))
                   (nonnegative-real? (cdr e))))
    (error (format #f "graph: each ~a must be #f, a nonnegative number, \
or a pair (lower . upper) of nonnegative numbers, but got" keyword)
           e))
  e)

;; The x value of each point when the abscissa is generated automatically:
;; START, START+DELTA, ..., restarting at START after each #f separator,
;; as the C reader does.
(define (auto-abscissa-values ydata start delta)
  (let loop ((ys ydata) (x start) (acc '()))
    (cond ((null? ys) (reverse acc))
          ((not (car ys)) (loop (cdr ys) start (cons #f acc)))
          (else (loop (cdr ys) (+ x delta) (cons x acc))))))

;; Expand an #:x-error or #:y-error specification SPEC into a list with
;; one error bar (or #f) per element of YDATA, for xy-data!.  XDATA gives
;; the x value of each element, for a procedure SPEC.
(define (expand-error-spec keyword spec xdata ydata)
  (cond
   ((not spec) #f)
   ;; One bar per point.  (Test list? before pair?: a list is a pair too.)
   ((list? spec)
    (unless (= (length spec) (length ydata))
      (error (format #f "graph: ~a has ~a elements, but there are ~a data \
points (use #f for points without a bar)" keyword (length spec) (length ydata))
             spec))
    (for-each (lambda (e y)
                (check-error-bar keyword e)
                (when (and (not y) e)
                  (error (format #f "graph: ~a has a bar where the data have \
a #f separator; build both with merge so they line up" keyword)
                         spec)))
              spec ydata)
    spec)
   ;; A procedure of x and y returning the bar for each point.
   ((procedure? spec)
    (map (lambda (x y) (and y (check-error-bar keyword (spec x y))))
         xdata ydata))
   ;; The same bar for every point.
   ((or (real? spec) (pair? spec))
    (check-error-bar keyword spec)
    (map (lambda (y) (and y spec)) ydata))
   (else
    (error (format #f "graph: ~a must be a number, a pair, a list, or a \
procedure, but got" keyword)
           spec))))

(define (bounds->errors data lows highs)
  "Given a list of DATA values and the LOWS and HIGHS of their error bars
(absolute limits, e.g. a confidence interval), return a list of
(lower . upper) error amounts suitable for #:x-error or #:y-error.  A
point whose value, low, or high is #f gets no bar (#f)."
  (unless (= (length data) (length lows) (length highs))
    (error "bounds->errors: the lists must have the same length"
           (list (length data) (length lows) (length highs))))
  (map (lambda (v lo hi)
         (cond ((not (and v lo hi)) #f)
               ((<= lo v hi) (cons (- v lo) (- hi v)))
               (else (error "bounds->errors: expected low <= value <= high, \
but got" (list lo v hi)))))
       data lows highs))

;;; Data and datasets

;; The x and y data for graph's (or dataset's) positional arguments, as
;; two values: xdata (#f for an automatic abscissa) and ydata.  Returns
;; #f and #f if the arguments are none of the accepted forms.
(define (data-from-arguments arg1 arg2 arg3 arg4)
  (cond
   ;; arg1 is the ordinate.  There is no abscissa.
   ((and (list? arg1) (not arg2) (not arg3) (not arg4))
    (values #f arg1))

   ;; arg1 is the abscissa.  arg2 is the ordinate.
   ((and (list? arg1) (list? arg2) (not arg3) (not arg4))
    (values arg1 arg2))

   ;; arg1 is a function. arg2 is x-min arg3 is x-max
   ((and (procedure? arg1) (real? arg2) (real? arg3) (not arg4))
    ;; 400 intervals, i.e. 401 points including both endpoints
    (let* ((n 400)
           (xmin arg2)
           (xmax arg3)
           (step (/ (- xmax xmin) n))
           (xdata (iota (+ n 1) xmin step)))
      (values xdata (map arg1 xdata))))

   ;; arg1 is a function. arg2 is x-min arg3 is x-max. arg4 is step
   ((and (procedure? arg1) (real? arg2) (real? arg3) (real? arg4))
    (let* ((xmin arg2)
           (xmax arg3)
           (step arg4)
           ;; number of points from xmin up to and including xmax; the
           ;; small fuzz keeps e.g. (0 0.3 0.1) from losing 0.3 to
           ;; floating-point roundoff
           (n (+ 1 (inexact->exact (floor (+ (/ (- xmax xmin) step) 1e-9)))))
           (xdata (iota n xmin step)))
      (values xdata (map arg1 xdata))))

   (else (values #f #f))))

(define data-forms
  "(graph ylist), (graph xlist ylist), (graph proc xmin xmax), (graph proc xmin xmax step), or (graph (list dataset ...))")

;; The step and start of an automatic abscissa, from #:auto-abscissa.
(define (auto-abscissa-parameters auto-abscissa)
  (cond ((real? auto-abscissa) (values auto-abscissa 0))
        ((and (list? auto-abscissa) (= 1 (length auto-abscissa)))
         (values (first auto-abscissa) 0))
        ((and (list? auto-abscissa) (= 2 (length auto-abscissa)))
         (values (first auto-abscissa) (second auto-abscissa)))
        (else (values 1 0))))

;; A #:symbol value as (index . size), or #f if not given; a missing size
;; is DEFAULT-SIZE.
(define (symbol-spec spec default-size)
  (cond ((not spec) #f)
        ((eq? spec #t) (cons 1 default-size))
        ((real? spec) (cons spec default-size))
        ((and (list? spec) (= 1 (length spec))) (cons (first spec) default-size))
        ((and (list? spec) (= 2 (length spec))) (cons (first spec) (second spec)))
        (else (error "graph: #:symbol must be #t, a number, or a list (number size), but got" spec))))

;; The linemode N auto-bumps later, as the C reader bumps it at the start
;; of each new dataset: up from a positive linemode, down otherwise.
(define (bump-linemode linemode n)
  (if (> linemode 0) (+ linemode n) (- linemode n)))

;; Marks a dataset option that wasn't given, where #f is a valid value.
(define unset (list 'unset))

(define-record-type <dataset>
  (make-dataset xdata ydata line-mode symbol line-width fill-fraction
                use-color symbol-font-name x-error y-error)
  dataset?
  (xdata dataset-xdata)
  (ydata dataset-ydata)
  (line-mode dataset-line-mode)
  (symbol dataset-symbol)
  (line-width dataset-line-width)
  (fill-fraction dataset-fill-fraction)
  (use-color dataset-use-color)
  (symbol-font-name dataset-symbol-font-name)
  (x-error dataset-x-error)
  (y-error dataset-y-error))

(define* (dataset arg1 #:optional (arg2 #f) (arg3 #f) (arg4 #f) #:key
                  line-mode symbol line-width fill-fraction (use-color unset)
                  symbol-font-name x-error y-error)
  "One set of data for graph, with its own style.  The data take the forms
graph's do: a y list, x and y lists, or a procedure with a range.  Options
not given take graph's values; an unset #:line-mode keeps auto-bumping."
  (let-values (((xdata ydata) (data-from-arguments arg1 arg2 arg3 arg4)))
    (unless ydata
      (error "dataset: expected (dataset ylist), (dataset xlist ylist), (dataset proc xmin xmax), or (dataset proc xmin xmax step), but got"
             (filter identity (list arg1 arg2 arg3 arg4))))
    (when (and line-mode (not (integer? line-mode)))
      (error "dataset: #:line-mode must be an integer, but got" line-mode))
    (when symbol
      (let ((spec (symbol-spec symbol 0.03)))
        (unless (and (integer? (car spec)) (<= 0 (car spec) 255))
          (error "dataset: the #:symbol number must be from 0 to 255, but got" (car spec)))
        (unless (and (real? (cdr spec)) (> (cdr spec) 0))
          (error "dataset: the #:symbol size must be positive, but got" (cdr spec)))))
    (when (and line-width (not (and (real? line-width) (>= line-width 0))))
      (error "dataset: #:line-width must be a nonnegative number, but got" line-width))
    (when (and fill-fraction (not (and (real? fill-fraction) (<= fill-fraction 1))))
      (error "dataset: #:fill-fraction must be a number no more than 1, but got" fill-fraction))
    (when (and symbol-font-name (not (string? symbol-font-name)))
      (error "dataset: #:symbol-font-name must be a string, but got" symbol-font-name))
    (make-dataset xdata ydata line-mode symbol line-width fill-fraction
                  use-color symbol-font-name x-error y-error)))

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
                blankout portable-output
                x-error y-error reposition
                (outp (current-output-port))
                (errp (current-error-port))
                (test #f)
                ;; internal: return the configured graph, for multigraph
                (%subgraph #f))
  (let ((gr (new-graph)))
    (unpack-keys)
    (when reposition
      (unless (and (list? reposition) (= 3 (length reposition))
                   (every real? reposition))
        (error "graph: #:reposition must be a list (x y scale), but got"
               reposition))
      (apply reposition! gr reposition))

    (cond
     ;; A list of datasets, each with its own style.
     ((and (pair? arg1) (every dataset? arg1))
      (when (or arg2 arg3 arg4)
        (error (string-append "graph: expected " data-forms ", but got")
               (filter identity (list arg1 arg2 arg3 arg4))))
      (let-values (((delta start) (auto-abscissa-parameters auto-abscissa)))
        (let ((default-symbol (or (symbol-spec symbol 0.03) (cons 0 0.03)))
              (default-font (or symbol-font-name "ZapfDingbats"))
              (default-line-width (or line-width -0.001)) ; <0: libplot default
              (default-fill (or fill-fraction -1.0))      ; <0: no filling
              (default-color (and toggle-use-color #t))
              (first-linemode (or line-mode 1))
              (bumping? (not toggle-auto-bump))
              ;; datasets so far, counting each one merged into a list
              (count 0))
          (for-each
           (lambda (ds)
             (let ((sym (or (symbol-spec (dataset-symbol ds) (cdr default-symbol))
                            default-symbol))
                   (xdata (dataset-xdata ds))
                   (ydata (dataset-ydata ds)))
               ;; This dataset's style, or graph's where it gives none.
               (%dataset-style! gr (car sym) (cdr sym)
                                (or (dataset-symbol-font-name ds) default-font)
                                (or (dataset-line-width ds) default-line-width)
                                (or (dataset-fill-fraction ds) default-fill)
                                (if (eq? (dataset-use-color ds) unset)
                                    default-color
                                    (and (dataset-use-color ds) #t)))
               ;; Its own linemode, or the one auto-bumping would give it had
               ;; no dataset set one, so an override affects only its own
               ;; dataset.
               (line-mode! gr (or (dataset-line-mode ds)
                                  (if bumping?
                                      (bump-linemode first-linemode count)
                                      first-linemode)))
               (set! count (+ count 1 (length (filter not ydata))))
               ;; Generate x values only for a dataset given just y values.
               (if xdata
                   (auto-abscissa! gr 0) ; a step of 0 turns it off
                   (auto-abscissa! gr delta start))
               (let ((xvalues (or xdata (auto-abscissa-values ydata start delta))))
                 (xy-data! gr xdata ydata
                           (expand-error-spec "#:x-error"
                                              (or (dataset-x-error ds) x-error)
                                              xvalues ydata)
                           (expand-error-spec "#:y-error"
                                              (or (dataset-y-error ds) y-error)
                                              xvalues ydata)))))
           arg1))))

     ;; A single set of data, styled by graph's options.
     (else
      (let-values (((xdata ydata) (data-from-arguments arg1 arg2 arg3 arg4)))
        (unless ydata
          (error (string-append "graph: expected " data-forms ", but got")
                 (filter identity (list arg1 arg2 arg3 arg4))))
        (when (and (not xdata) (not auto-abscissa))
          (auto-abscissa! gr))
        ;; x values for procedure-valued error bars, matching what the C
        ;; reader will generate for an automatic abscissa
        (let ((xvalues (or xdata
                           (let-values (((delta start)
                                         (auto-abscissa-parameters auto-abscissa)))
                             (auto-abscissa-values ydata start delta)))))
          (xy-data! gr xdata ydata
                    (expand-error-spec "#:x-error" x-error xvalues ydata)
                    (expand-error-spec "#:y-error" y-error xvalues ydata))))))

    (cond (%subgraph gr)
          (test (%graph-test gr test))
          (else (generate gr outp errp)))))

(define (subgraph . arguments)
  "Configure a graph, taking the arguments graph takes, without drawing
it; pass a list of these to multigraph to draw them on one page."
  (apply graph (append arguments (list #:%subgraph #t))))

(define* (multigraph graphs #:key output-format bg-color bitmap-size
                     emulate-color max-line-length page-size rotation
                     portable-output save-screen
                     (outp (current-output-port))
                     (errp (current-error-port)))
  "Draw GRAPHS, a list of graphs made by subgraph, on one page.  Each is
drawn where its #:reposition puts it; each after the first is drawn over
a blanked-out background.  The page options given here (output format,
sizes, and so on) apply to the whole page; any not given are taken from
the first graph."
  (unless (and (pair? graphs) (every graph? graphs))
    (error "multigraph: expected a list of graphs made by subgraph, but got"
           graphs))
  ;; The page is set up from the first graph's page settings.
  (let ((first-graph (car graphs)))
    (when output-format (output-format! first-graph output-format))
    (when bg-color (bg-color! first-graph bg-color))
    (when bitmap-size (bitmap-size! first-graph bitmap-size))
    (when emulate-color (emulate-color! first-graph emulate-color))
    (when max-line-length (max-line-length! first-graph max-line-length))
    (when page-size (page-size! first-graph page-size))
    (when rotation
      (rotation-angle! first-graph (if (number? rotation)
                                       (number->string rotation)
                                       rotation)))
    (when portable-output (portable-output! first-graph))
    (when save-screen (save-screen! first-graph)))
  (generate-multigraph graphs outp errp))


(load-extension "libguile-plotutils" "gupl_graph_init")
