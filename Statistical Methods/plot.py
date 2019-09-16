import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from numpy import log, power
from math import e
from matplotlib.colors import BoundaryNorm
from matplotlib.ticker import MaxNLocator
from scipy.special import factorial

lambdaPrint = '\u03BB'

def log_likelihood(X, Lambda):
    return log(Lambda) * sum(X) - len(X) * Lambda - sum([log(factorial(x)) for x in X])

X = [12, 4, 5, 3, 7, 5, 6]
lambdaStart = 0.01
lambdaStop = 30
lambdaResolution = 0.01


Lambda = np.arange(lambdaStart, lambdaStop, lambdaResolution)
P = log_likelihood(X, Lambda)

fig = plt.figure()

plt.plot(Lambda, P)
plt.xlabel(lambdaPrint)
plt.ylabel('l({})'.format(lambdaPrint))
plt.show()


xStart = 1
xStop = 30
xResolution = 1


Lambda, X = np.mgrid[slice(lambdaStart, lambdaStop + lambdaResolution, lambdaResolution),
                slice(xStart, xStop + xResolution, xResolution)]

z = Lambda**X*e**(-Lambda)/(factorial(X))
levels = MaxNLocator(nbins=15).tick_values(z.min(), z.max())


# pick the desired colormap, sensible levels, and define a normalization
# instance which takes data values and translates those into levels.
cmap = plt.get_cmap('PiYG')
norm = BoundaryNorm(levels, ncolors=cmap.N, clip=True)

fig, (ax0) = plt.subplots(nrows=1)

im = ax0.pcolormesh(X, Lambda, z, cmap=cmap, norm=norm)
coloarbar = fig.colorbar(im, ax=ax0)
coloarbar.ax.set_title('possibility')
#ax0.set_title('possibility mess function')
plt.xlabel('X(random quantities)')
plt.ylabel('{}(unknown constants)'.format(lambdaPrint))
"""
cf = ax0.contourf(X + xResolution/2.,
                  Lambda + lambdaResolution/2., z, levels=levels,
                  cmap=cmap)
fig.colorbar(cf, ax=ax0)
ax0.set_title('p')
"""
plt.show()