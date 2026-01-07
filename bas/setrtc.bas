LET n$ = "-q sntp pool.ntp.org"
DIM t$(100)
OPEN #2,"V>t$"
.$ newt n$
CLOSE #2
PRINT t$
