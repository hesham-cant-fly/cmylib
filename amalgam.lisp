#!/usr/bin/env -S sbcl --script

(defparameter *source-dir* #P"source/")
(defparameter *header-dir* #P"headers/")

(defun file-content (path)
  (with-open-file (stream path)
    (let ((content (make-string (file-length stream))))
      (read-sequence content stream)
      content)))

(defun split-doc-comment (content)
  (let ((start (position-if-not
                (lambda (c)
                  (member c '(#\Space #\Tab #\Newline #\Return)))
                content)))
    (if (and start
             (>= (- (length content) start) 3)
             (string= "/**" content :start1 0 :end1 3
                                    :start2 start :end2 (+ start 3)))
        (let ((end (search "*/" content :start2 (+ start 3))))
          (if end
              (values (subseq content start (+ end 2))
                      (subseq content (+ end 2)))
              (values "" content)))
        (values "" content))))

(defun write-guarded-file (out name header &optional impl)
  (let ((guard (format nil "~:@(~A_H~)" name))
        (impl-macro (format nil "~:@(~A_IMPL~)" name)))

    (multiple-value-bind (doc rest)
        (split-doc-comment header)

      ;; Documentation comment
      (when (> (length doc) 0)
        (format out "~A~%~%" doc))

      ;; Header guard
      (format out "#ifndef ~A~%#define ~A~%~%"
              guard guard)

      (when impl
        (format out "/* #define ~A */~%~%" impl-macro))
      (format out "~A" rest)

      ;; Implementation
      (when impl
        (format out
                "~%~%#ifdef ~A~%~A~%#endif /* ~A_IMPL */"
                impl-macro
                impl
                impl-macro))

      ;; End guard
      (format out "~%~%#endif /* ~A */~%" guard))))

(defun amalgamate (name)
  (let* ((h-path (merge-pathnames (format nil "~A.h" name) *source-dir*))
         (c-path (merge-pathnames (format nil "~A.c" name) *source-dir*))
         (out-path (merge-pathnames (format nil "~A.h" name) *header-dir*)))

    (with-open-file (out out-path
                         :direction :output
                         :if-exists :supersede
                         :if-does-not-exist :create)
      (write-guarded-file out
                          name
                          (file-content h-path)
                          (file-content c-path)))))

(defun guard-file (from to name)
  (with-open-file (out to
                       :direction :output
                       :if-exists :supersede
                       :if-does-not-exist :create)
    (write-guarded-file out
                        name
                        (file-content from))))

(defun main ()
  (ensure-directories-exist *header-dir*)

  (dolist (h (directory (merge-pathnames "*.h" *source-dir*)))
    (let* ((name (pathname-name h))
           (c (merge-pathnames (format nil "~A.c" name) *source-dir*))
           (out (merge-pathnames (format nil "~A.h" name) *header-dir*)))

      (if (probe-file c)
          (progn
            (amalgamate name)
            (format t "Amalgamated: ~A~%" name))

          (progn
            (guard-file h out name)
            (format t "Guarded: ~A~%" name))))))

(main)
