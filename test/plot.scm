(use-modules (plotutils plot)
             (srfi srfi-1)
             (srfi srfi-64))

(test-begin "plot.scm")
(test-display "path" (getenv "PATH"))
(test-display "guile:version" (version))
(test-display "cwd" (getcwd))
(test-end "plot.scm")
