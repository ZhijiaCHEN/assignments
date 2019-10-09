import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from numpy import log, power, mean, var
from numpy.random import poisson, binomial, gamma
from math import e
from matplotlib.colors import BoundaryNorm
from matplotlib.ticker import MaxNLocator
from scipy.special import factorial
from scipy.stats import bernoulli
from math import log as mlog

lambdaPrint = '\u03BB'

def word_occuren(v, Y):
    return [poisson(v*y/1000) for y in Y]

def log_likelihood(X, Lambda):
    return log(Lambda) * sum(X) - len(X) * Lambda - sum([log(factorial(x)) for x in X])

def log_likelihood2(X, Y, V):
    return [sum([xi*mlog(v*yi/1000)-v*yi/1000-mlog(factorial(xi)) for xi, yi in zip(X, Y)]) for v in V]

lambdaStart = 0.01
lambdaStop = 30
lambdaResolution = 0.01

xStart = 1
xStop = 30
xResolution = 1




def q_1_9():
    X = [12, 4, 5, 3, 7, 5, 6]
    Lambda = np.arange(lambdaStart, lambdaStop, lambdaResolution)
    P = log_likelihood(X, Lambda)

    fig = plt.figure()

    plt.plot(Lambda, P)
    plt.xlabel(lambdaPrint)
    plt.ylabel('l({})'.format(lambdaPrint))
    plt.show()
    
def q_1_10():
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

def q_2_10():
    Y = [1730, 947, 1830, 1210, 1100]
    v = 10
    while True:
        X = word_occuren(v, Y)
        V = np.arange(lambdaStart, lambdaStop, lambdaResolution)
        P = log_likelihood2(X, Y, V)
        vEst = V[P.index(max(P))]
        print('v estimate: {}'.format(vEst))
        if abs(vEst -v)<1:
            break
    plt.plot(V, P)
    plt.xlabel('\u03BD')
    plt.ylabel('l(\u03BD)')
    plt.show()

def q_2_11():
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

def q_3_3():
    n, pi = 1, .3
    lambdaPolitics = 30
    thetaOther = 0.02
    Z = binomial(n, pi, 1000)
    
    XAll = [poisson(lambdaPolitics) if z else binomial(1000, thetaOther) for z in Z]
    X = [[],[]]
    for x, z in zip(XAll, Z):
        if z:
            X[0].append(x)
        else:
            X[1].append(x)
    n_bins = max([len(set(s)) for s in X])
    colors = ['red', 'blue']
    plt.plot()
    axis = plt.gca()
    axis.hist(X, n_bins, histtype='bar', color=colors, label=['Politics', 'Other'])
    axis.legend(prop={'size': 10})
    plt.xlabel('X')
    plt.ylabel('frequency')
    plt.show()

def q_4_3():
    alpha = 10
    theta = 1
    X1 = [poisson(lambdai) for lambdai in gamma(alpha, theta, 1000)]
    meanX1 = mean(X1)
    varX1 = var(X1)
    X2 = poisson(10, 1000)
    meanX2 = mean(X2)
    varX2 = var(X2)
    return X1
#q_1_9()
#q_1_10()
#q_2_10()
#q_3_3()
q_4_3()