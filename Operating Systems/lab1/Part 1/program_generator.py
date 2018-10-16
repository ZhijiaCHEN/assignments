import os, sys
from itertools import permutations
pyPath = os.path.dirname(os.path.abspath(__file__))
perm = list(permutations(['i', 'j', 'k']))

hFiles = []
cFiles = []
for i in range(len(perm)):
    hFile = open(os.path.join(pyPath, "matrix{}.h".format(i)), "wt")
    hFiles.append(hFile)
    cFile = open(os.path.join(pyPath, "matrix{}.c".format(i)), "wt")
    cFiles.append(cFile)

templateHfile = open(os.path.join(pyPath, "matrixTemplate.h"), "rt")
for line in templateHfile:
    for i in range(len(perm)):
        hFiles[i].write(line.replace('__CNT__', str(i)))
templateHfile.close()

templateCfile = open(os.path.join(pyPath, "matrixTemplate.c"), "rt")
for line in templateCfile:
    for i in range(len(perm)):
        cFiles[i].write(line.replace('IDX1', perm[i][0]).replace('IDX2', perm[i][1]).replace('IDX3', perm[i][2]).replace('__CNT__', str(i)).replace('matrixTemplate.h', 'matrix{}.h'.format(i)))
templateCfile.close()

for i in range(len(perm)):
    hFiles[i].close()
    cFiles[i].close()

matrixHeader = open(os.path.join(pyPath, 'matrix.h'), 'wt')
for i in range(len(perm)):
    matrixHeader.write('# include "matrix{}.h"\n'.format(str(i)))
matrixHeader.close()