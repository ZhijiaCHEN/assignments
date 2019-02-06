% for node 1 
r(10).
b(10) :- r(10), not r(120).
b(120) :- r(120).
r(120) :- b(20).

% for node 2
r(20).
b(20) :- r(20), not r(210).
b(210) :- r(210).
r(210) :- b(10).

#show r/1. 
#show b/1.