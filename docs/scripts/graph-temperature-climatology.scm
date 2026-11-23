;;; graph-temperature-climatology.scm — Daily temperature climatology
;;; The mean temperature for each day of the year in a temperate European
;;; city, with error bars running from the day's mean minimum to its mean
;;; maximum temperature.  The data are synthetic: a seasonal cycle with a
;;; day-night range that widens in summer, plus small ripples, standing
;;; in for a 30-year average of real observations.
;;; Shows #:y-error with bounds->errors, and #f for points without a bar.

(use-modules (srfi srfi-1)
             (plotutils graph))

(define pi (* 4.0 (atan 1.0)))

;; +1 on the warmest day of the year (day 200, mid-July), -1 half a year
;; earlier or later
(define (season day)
  (cos (* 2.0 pi (/ (- day 200) 365.0))))

;; Small irregularities, as a 30-year average still has
(define (ripple day)
  (+ (* 0.40 (sin (* 2.0 pi (/ (* 5 day) 365.0))))
     (* 0.25 (sin (+ 1.3 (* 2.0 pi (/ (* 11 day) 365.0)))))
     (* 0.15 (sin (+ 0.4 (* 2.0 pi (/ (* 23 day) 365.0)))))))

;; Mean daily temperature: about 2.5 C in mid-January, 19.5 C in mid-July
(define (mean-temperature day)
  (+ 11.0 (* 8.5 (season day)) (ripple day)))

;; Difference between the day's maximum and minimum: about 4.5 C in
;; winter and 9.5 C in summer
(define (daily-range day)
  (+ 7.0 (* 2.5 (season day))))

;; The mean of hourly readings sits a little nearer the night-time low
;; than the afternoon high, so the bars are asymmetric.
(define (min-temperature day)
  (- (mean-temperature day) (* 0.45 (daily-range day))))

(define (max-temperature day)
  (+ (mean-temperature day) (* 0.55 (daily-range day))))

(define (output-format-from-filename path)
  (let loop ((i (- (string-length path) 1)))
    (cond
     ((< i 0) "svg")
     ((char=? (string-ref path i) #\.)
      (let ((ext (string-downcase (substring path (+ i 1) (string-length path)))))
        (if (string=? ext "eps") "ps" ext)))
     (else (loop (- i 1))))))

(define (main args)
  (let* ((output-file (if (> (length args) 1)
                          (cadr args)
                          "graph-temperature-climatology.svg"))
         (output-format (output-format-from-filename output-file))
         (days (iota 365 1))
         (means (map mean-temperature days))
         ;; (lower . upper) amounts from the absolute minima and maxima
         (bars (bounds->errors means
                               (map min-temperature days)
                               (map max-temperature days)))
         ;; A bar on every day would be too dense, so draw one a week;
         ;; #f means no bar on that day.
         (weekly-bars (map (lambda (day bar)
                             (and (= 4 (modulo day 7)) bar))
                           days bars)))
    (with-output-to-file output-file
      (lambda ()
        (graph days means
               #:y-error weekly-bars
               #:output-format output-format
               #:bitmap-size "1000x1000"
               #:top-label "Daily Temperature (synthetic data)"
               #:x-label "Day of year"
               #:y-label "Temperature (\\deC)"
               #:x-limits '(1 365)
               #:y-limits '(0 26 5)
               #:grid-style 3
               ;; no marker; the symbol size sets the width of the bar caps
               #:symbol '(0 0.015)
               #:line-width 0.003
               #:font-name "HersheySerif"))
      #:binary #t)))

(main (command-line))
