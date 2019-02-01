r0(1). r0(2). r1(1).
r1(X):-r0(X), X < 0.
r2(X):-r0(X), r0(Y), not r1(Y), X > Y.
r3(X):-r0(X), r0(Y), not r1(Y), X > Y.

