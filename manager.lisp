#!/usr/bin/env -S sbcl --script

(require 'asdf)
(load "./script/amalgam.lisp")
(load "./script/test.lisp")

(defun get-help ()
  (format nil
          "Usage: amalgam <command>~%~
           ~%~
           Commands:~%~
             amalgam  -- Runs the amalgamation script~%~
             test     -- Runs tests~%"))

(defun process-args (args)
  (dolist (arg args)
    (cond
     ((string= arg "amalgam")
      (run-amalgamation))

     ((string= arg "test")
      ;; TODO: Run amalgamations as neccecary
      (run-tests))

     (t
      (format t "Unknown command: ~A~%~%" arg)
      (format t "~A" (get-help))))))

(defun main ()
  (let ((args (uiop:command-line-arguments)))
    (cond
     ((null args)
      (format t "~A" (get-help)))

     (t
      (process-args args)))))

(main)
