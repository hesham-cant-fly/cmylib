#!/usr/bin/env -S sbcl --script

(require 'asdf)

(defparameter *cc* "clang")
(defparameter *test-dir* #P"test/")
(defparameter *include-dir* #P"headers/")
(defparameter *verbose-output* nil)

(defparameter *build-dir* #P".build/")

(defvar *compilation-failure* 0)
(defvar *failure-count* 0)

(defun string-join (str &optional (sep ""))
  (reduce (lambda (a b) (concatenate 'string a sep b)) str))

(defun compile-command (path)
  (list *cc*
        (namestring path)
        "-o"
        (concatenate 'string (namestring (merge-pathnames *build-dir* (pathname-name path))) ".out")
        "-I"
        (namestring *include-dir*)
        (if *verbose-output* "-DTEST_VERBOSE" "")))

(defun compile-it (path)
  (let ((cmd (compile-command path)))
    (format t "Compiling: ~A~%" path)
    (multiple-value-bind (output error-output exit-code)
        (uiop:run-program
         (string-join cmd " ")
         :output *standard-output*
         :error-output *standard-output*
         :ignore-error-status t)
      (declare (ignore output error-output))
      (unless (= exit-code 0)
        (incf *compilation-failure*)))))

(defun test-it (path)
  (format t "Testing: ~A~%" path)
  (multiple-value-bind (output error-output exit-code)
      (uiop:run-program
       (namestring path)
       :output *standard-output*
       :error-output *standard-output*
       :ignore-error-status t)
    (declare (ignore output error-output))
    (unless (= exit-code 0)
      (incf *failure-count*))))

(defun main ()
  (ensure-directories-exist *build-dir*)
  (setf *compilation-failure* 0)
  (setf *failure-count* 0)
  
  (dolist (path (directory (merge-pathnames "*.c" *test-dir*)))
    (compile-it path))

  (unless (= *compilation-failure* 0)
    (format t "~A Test Failed to Compile!~%" *compilation-failure*))

  (dolist (path (directory (merge-pathnames "*.out" *build-dir*)))
    (test-it path))

  (unless (= *failure-count* 0)
    (format t "~A Test Failed!~%" *failure-count*)))

(main)
