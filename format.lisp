
(declaim (ftype (function (string) t) parse-fmt-string))
(defun parse-fmt-string (str)
  (let ((strs (uiop:split-string str :separator "{}"))
        (i 1))
    `(concatenate 'string
                  ,@(mapcar (lambda (item)
                              (prog1
                                  (if (evenp i)
                                      `(princ-to-string ,(read-from-string item))
                                      item)
                                (incf i)))
                            strs))))

(defmacro fmt (str)
  (parse-fmt-string str))

(defun dispatch-fmt (stream char x)
  (declare (ignore char x))
  (let ((dih (read stream)))
    (unless (stringp dih)
      (error "#f dispatch macro expected a string"))
    (parse-fmt-string dih)))

(set-dispatch-macro-character #\# #\f #'dispatch-fmt)
