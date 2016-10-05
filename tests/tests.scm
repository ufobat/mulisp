()
#t
#f
1
-5/7
12.667
-.78
"Hello world"
#\t
'(a b c)
(define a "hello world")
a
(set! a 1)
a
(set! a (- a 1))
a
(= a 0)
(= a 1)

((lambda (x) (+ x x)) 4)
((lambda x x) 3 4 5 6)
((lambda (x y . z) z) 3 4 5 6)
(if (> 3 2) 'yes 'no)
(if (> 2 3) 'yes 'no)
(if (> 3 2) (- 3 2) (+ 3 2))

(define make-counter
  (lambda (initial-value)
    ((lambda (counter)
       (lambda (difference)
         (set! counter (+ counter difference))
         (counter)))
     initial-value)))

(define counter-0 (make-counter 10))
(define counter-1 (make-counter -5))

(counter-0 10)
(counter-0 -5)
(counter-1 10)
