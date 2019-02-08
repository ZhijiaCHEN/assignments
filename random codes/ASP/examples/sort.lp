set(1..4).
next(X,Z) :- set(X), #false : X < Y, set(Y), Y < Z; set(Z), X < Z.

# The variables inside the condition body are instantiated after all the variables outside the condition are instantiated. All the instantiation of the condition body should make the condition evaluate to true.
