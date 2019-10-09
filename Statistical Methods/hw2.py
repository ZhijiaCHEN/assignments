import matplotlib.pyplot as plt
import matplotlib
from math import log
from statistics import mean
import random
from datetime import datetime
random.seed(datetime.now())



alpha = 2
theta = 3

def random_asteriod_size(num):
    def cdf_reverse(y):
        return theta/((1-y)**(1/alpha))
    Y = []
    while len(Y) < num:
        y = random.random()
        if y == 1: continue
        Y.append(y)
    
    return [cdf_reverse(y) for y in Y]

def alpha_MLE(X):
    return 1/(sum([log(x)/len(X) for x in X])-log(theta))

def alpha_MOM(X):
    return mean(X)/(mean(X)-3)

def q_1_5():
    sampleSize = range(10, 5000, 50)
    randomSizes = [random_asteriod_size(n) for n in sampleSize]
    mleEst = [alpha_MLE(X) for X in randomSizes]
    mleErr = [abs(a-alpha)/alpha*100 for a in mleEst]
    momEst = [alpha_MOM(X) for X in randomSizes]
    momErr = [abs(a-alpha)/alpha*100 for a in momEst]


    fig = plt.figure()
    plt.plot(sampleSize, mleErr, color='red', label='MLE')
    plt.plot(sampleSize, momErr, color='blue', label='MOM')
    plt.xlabel('sample size')
    plt.ylabel('error(%)')
    #plt.yscale('log')
    plt.legend()
    plt.show()
    
q_1_5()