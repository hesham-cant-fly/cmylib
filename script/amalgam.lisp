(defparameter *source-dir* #P"source/")
(defparameter *header-dir* #P"headers/")
(defparameter *amalgamated-name* "cmylib")

;;;; -------------------------------------------------------------------------
;;;; File utilities
;;;; -------------------------------------------------------------------------

(defun file-content (path)
  (with-open-file (stream path)
    (let ((content (make-string (file-length stream))))
      (read-sequence content stream)
      content)))


(defun copy-file (from to)
  (with-open-file (in from
                     :direction :input
                     :element-type '(unsigned-byte 8))
    (with-open-file (out to
                        :direction :output
                        :if-exists :supersede
                        :if-does-not-exist :create
                        :element-type '(unsigned-byte 8))
      (let ((buffer (make-array 4096
                                :element-type '(unsigned-byte 8))))
        (loop
          for count = (read-sequence buffer in)
          while (> count 0)
          do (write-sequence buffer out :end count))))))


;;;; -------------------------------------------------------------------------
;;;; Documentation comment
;;;; -------------------------------------------------------------------------

(defun split-doc-comment (content)
  (let ((start
          (position-if-not
           (lambda (c)
             (member c '(#\Space #\Tab #\Newline #\Return)))
           content)))

    (if (and start
             (>= (- (length content) start) 3)
             (string= "/**"
                      content
                      :start1 0
                      :end1 3
                      :start2 start
                      :end2 (+ start 3)))

        (let ((end (search "*/"
                           content
                           :start2 (+ start 3))))

          (if end
              (values
               (subseq content start (+ end 2))
               (subseq content (+ end 2)))

              (values "" content)))

        (values "" content))))


;;;; -------------------------------------------------------------------------
;;;; Include parsing
;;;; -------------------------------------------------------------------------

(defun local-include-name (line)
  "Return the filename from #include \"foo.h\"."
  (let ((start (search "#include" line)))

    (when start
      (let ((quote1
              (position #\"
                        line
                        :start (+ start 8))))

        (when quote1
          (let ((quote2
                  (position #\"
                            line
                            :start (1+ quote1))))

            (when quote2
              (subseq line
                      (1+ quote1)
                      quote2))))))))


(defun header-dependencies (name all-names)
  "Return headers directly included by NAME."
  (let ((path
          (merge-pathnames
           (format nil "~A.h" name)
           *source-dir*))
        (deps '()))

    (with-open-file (in path)
      (loop
        for line = (read-line in nil nil)
        while line
        do
           (let ((include (local-include-name line)))
             (when (and include
                        (member include
                                all-names
                                :test #'string=)
                        (not (string= include name)))
               (pushnew include
                        deps
                        :test #'string=)))))

    (nreverse deps)))


;;;; -------------------------------------------------------------------------
;;;; Dependency sorting
;;;; -------------------------------------------------------------------------

(defun topological-sort (names)
  "Return headers in dependency-first order."
  (let ((temporary '())
        (permanent '())
        (result '()))

    (labels
        ((visit (name)
           ;; Circular dependency.
           (when (member name temporary
                         :test #'string=)
             (error
              "Circular header dependency involving ~A"
              name))

           ;; Already processed.
           (unless (member name permanent
                           :test #'string=)

             (push name temporary)

             ;; Dependencies first.
             (dolist (dependency
                       (header-dependencies name names))
               (visit dependency))

             (setf temporary
                   (delete name
                           temporary
                           :test #'string=))

             (push name permanent)
             (push name result))))

      (dolist (name names)
        (visit name)))

    (nreverse result)))


;;;; -------------------------------------------------------------------------
;;;; Guard generation
;;;; -------------------------------------------------------------------------

(defun header-guard (name)
  (format nil "~:@(~A_H~)" name))


(defun implementation-macro (name)
  (format nil "~:@(~A_IMPL~)" name))

(defun definition-macro (name)
  (format nil "~:@(~A_DEF~)" name))

(defun write-guarded-file (out name header &optional implementation)
  (let ((guard (header-guard name))
        (impl-macro (implementation-macro name)))

    (multiple-value-bind (doc rest)
        (split-doc-comment header)

      ;; Documentation comment comes before the guard.
      (when (> (length doc) 0)
        (format out "~A~%~%" doc))

      ;; Guard.
      (format out
              "#ifndef ~A~%#define ~A~%~%"
              guard
              guard)

      ;; Implementation switch.
      (when implementation
        (format out
                "/* #define ~A */~%~%"
                impl-macro))

      ;; Header.
      (format out "~A" rest)

      ;; Implementation.
      (when implementation
        (format out
                "~%~%#ifdef ~A~%"
                impl-macro)

        (format out "~A" implementation)

        (format out
                "~%#endif /* ~A */"
                impl-macro))

      ;; Guard end.
      (format out
              "~%~%#endif /* ~A */~%"
              guard))))


;;;; -------------------------------------------------------------------------
;;;; Individual headers
;;;; -------------------------------------------------------------------------

(defun amalgamate (name)
  (let* ((h-path
           (merge-pathnames
            (format nil "~A.h" name)
            *source-dir*))

         (c-path
           (merge-pathnames
            (format nil "~A.c" name)
            *source-dir*))

         (out-path
           (merge-pathnames
            (format nil "~A.h" name)
            *header-dir*)))

    (with-open-file (out out-path
                         :direction :output
                         :if-exists :supersede
                         :if-does-not-exist :create)

      (write-guarded-file
       out
       name
       (file-content h-path)
       (file-content c-path)))))


(defun guard-file (from to name)
  (with-open-file (out to
                       :direction :output
                       :if-exists :supersede
                       :if-does-not-exist :create)

    (write-guarded-file
     out
     name
     (file-content from))))


;;;; -------------------------------------------------------------------------
;;;; Streaming header writing
;;;; -------------------------------------------------------------------------

(defun write-header-body (out path names)
  "Write PATH to OUT while removing internal #include directives."
  (with-open-file (in path)
    (loop
      for line = (read-line in nil nil)
      while line
      do
         (let ((include (local-include-name line)))

           (unless (and include
                        (member include
                                names
                                :test #'string=))

             (write-line line out))))))


(defun write-implementation (out path)
  (with-open-file (in path)
    (loop
      for line = (read-line in nil nil)
      while line
      do
         (write-line line out))))


;;;; -------------------------------------------------------------------------
;;;; Amalgamated header
;;;; -------------------------------------------------------------------------

(defun write-amalgamated-header (out name names)
  (let* ((h-path
           (merge-pathnames
            (format nil "~A.h" name)
            *source-dir*))

         (c-path
           (merge-pathnames
            (format nil "~A.c" name)
            *source-dir*))

         (guard
           (header-guard name))

         (impl-macro
           (implementation-macro name)))

    ;; Read only enough of the header to extract its documentation
    ;; comment. The remainder is then streamed.
    (let ((header (file-content h-path)))

      (multiple-value-bind (doc rest)
          (split-doc-comment header)

        ;; Documentation.
        (when (> (length doc) 0)
          (format out "~A~%~%" doc))

        ;; Guard.
        (format out
                "#ifndef ~A~%#define ~A~%~%"
                guard
                guard)

        ;; Implementation switch.
        (when (probe-file c-path)
          (format out
                  "/* #define ~A */~%~%"
                  impl-macro))

        ;; Write the remainder.
        ;;
        ;; NOTE:
        ;; REST is normally small enough after the documentation
        ;; extraction, but the actual header body is still written
        ;; through the stream below.
        ;;
        ;; To avoid the large REST string, locate its starting
        ;; position and stream the original file.
        (let ((start
                (if (> (length doc) 0)
                    (+ (search "*/" header) 2)
                    0)))

          (with-open-file (in h-path)
            ;; Skip the documentation comment.
            (when (> start 0)
              (file-position in start))

            (loop
              for line = (read-line in nil nil)
              while line
              do
                 (let ((include (local-include-name line)))
                   (unless (and include
                                (member include
                                        names
                                        :test #'string=))
                     (write-line line out))))))

        ;; Implementation.
        (when (probe-file c-path)
          (format out
                  "~%~%#ifdef ~A~%"
                  impl-macro)

          (write-implementation out c-path)

          (format out
                  "#endif /* ~A */"
                  impl-macro))

        ;; Guard.
        (format out
                "~%~%#endif /* ~A */~%"
                guard)))))

(defun create-amalgamated-header (ordered-names)
  (let* ((out-path
          (format nil "~A.h" *amalgamated-name*))
         (guard
           (header-guard *amalgamated-name*))
         (def-macro
          (definition-macro *amalgamated-name*))
         (impl-macro
          (implementation-macro *amalgamated-name*)))

    (with-open-file (out out-path
                         :direction :output
                         :if-exists :supersede
                         :if-does-not-exist :create)

      ;; Header documentation.
      (format out
              "/*~%")
      (format out
              " * @file ~A.h~%"
              *amalgamated-name*)
      (format out
              " * Amalgamated header.~%")
      (format out
              " * Generated automatically.~%")
      (format out
              " */~%~%")

      ;; Header guard.
      (format out
              "#ifndef ~A~%#define ~A~%~%"
              guard
              guard)

      ;; Implementation switch.
      (format out
              "/* #define ~A */~%~%"
              impl-macro)

      ;; Definition switch
      (format out "#ifndef ~A~%" def-macro)
      (format out "#  define ~A~%" def-macro)
      (format out "#endif /* !~A */~%~%" def-macro)
      (dolist (name ordered-names)
        (format out
                "#define ~A ~A~%"
                (definition-macro name)
                def-macro))
      (format out "~%")

      ;; Headers.
      (dolist (name ordered-names)
        (format t "Merging: ~A~%" name)

        (let ((h-path
               (merge-pathnames
                (format nil "~A.h" name)
                *source-dir*)))

          (write-header-body
           out
           h-path
           ordered-names))

        (format out "~%"))

      ;; Implementations.
      (format out
              "~%#ifdef ~A~%"
              impl-macro)

      (dolist (name ordered-names)
        (let ((c-path
                (merge-pathnames
                 (format nil "~A.c" name)
                 *source-dir*)))

          (when (probe-file c-path)
            (format t "Merging implementation: ~A~%" name)

            (write-implementation
             out
             c-path)

            (format out "~%"))))

      (format out
              "#endif /* ~A */~%"
              impl-macro)

      ;; End guard.
      (format out
              "~%#endif /* ~A */~%"
              guard)

      (format t "~%Created: ~A~%"
              out-path))))

;;;; -------------------------------------------------------------------------
;;;; Main
;;;; -------------------------------------------------------------------------

(defun run-amalgamation ()
  (ensure-directories-exist *header-dir*)

  (let* ((headers
           (directory
            (merge-pathnames
             "*.h"
             *source-dir*)))

         (names
           (mapcar #'pathname-name headers))

         (ordered-names
           (topological-sort names)))

    ;; Generate individual headers.
    (dolist (h headers)
      (let* ((name (pathname-name h))

             (c
               (merge-pathnames
                (format nil "~A.c" name)
                *source-dir*))

             (out
               (merge-pathnames
                (format nil "~A.h" name)
                *header-dir*)))

        (if (probe-file c)

            (progn
              (amalgamate name)
              (format t
                      "Amalgamated: ~A~%"
                      name))

            (progn
              (guard-file h out name)
              (format t
                      "Guarded: ~A~%"
                      name)))))

    ;; Show dependency order.
    (format t "~%Dependency order:~%")
    (dolist (name ordered-names)
      (format t "  ~A~%" name))

    ;; Create cmylib.h.
    (create-amalgamated-header ordered-names)))

