vtx(a;b;c;d).
edge(a,b).
edge(b,c).
edge(c,d).
edge(d,a).
edge(b,a).
edge(c,b).
edge(d,c).
edge(a,d).
{in(X,Y)}:-edge(X,Y).
:-in(V1,V2),in(V3,V2),V1!=V3.
:-in(V1,V2),in(V1,V3),V2!=V3.
rchble(X,Y):-in(X,Y).
rchble(V1,V3):-in(V1,V2),rchble(V2,V3).
:-vtx(V1),vtx(V2),not rchble(V1,V2).



#show in/2.
