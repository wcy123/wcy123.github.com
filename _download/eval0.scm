(define true #t)
(define false #f)
(define (my-eval exp env)
  (cond ((self-evaluating? exp) exp)
        (else (error "Unknown expression type -- EVAL" exp))))

(define (self-evaluating? exp)
  (cond ((number? exp) true)
        ((string? exp) true)
        (else false)))
