#script (python)
import clingo
T = clingo.Tuple
N = clingo.Number
true = clingo.TruthValue._True
false = clingo.TruthValue._False

def test(x):
    print("got argument {} whose arguments are {}".format(x.string, x.arguments))
    return x

def push(x, y):
    return T([x]+y.arguments)

def lenth(x):
    if x.arguments is None:
        return 0
    else:
        return len(x.arguments)

def not_in(x, y):
    for i in y.arguments:
        if x.number == i.number:
            return 0
    return 1
#end.


peer(1,2;2,1;2,3;3,2;3,4;4,3;1,3;3,1).
route(SRC, DST, (SRC, DST)):-peer(SRC, DST).
%notIn(X,Y):-@not_in(Y, PATH) > 1.
{advertisement(X, Y, D, PATH)}=1:-peer(X, Y), route(X, D, PATH), @not_in(Y, PATH) > 0.
route(SRC, DST, @push(SRC, PATH)):-advertisement(TRNST, SRC, DST, PATH).
:-advertisement(X, Y, D, PATH1), advertisement(X, Y, D, PATH2), @lenth(PATH1) > @lenth(PATH2).

#show route/3.
#show advertisement/4.
%#show notIn/2.
