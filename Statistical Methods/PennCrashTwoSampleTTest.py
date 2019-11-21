import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
from scipy import stats
from math import floor
width_category = {'<= 20':0, '> 20 and < 24':1, '>= 24':2}
shoulder_category = {'<= 3': 0, '> 3 and <= 6': 1, '> 6': 2}
driveways_category = {'None':0, '> 0 and <= 10':1, '> 10': 2}
curves_category = {'None':0, '>= 1':1}
intersection_category = {'None':0, '>= 1':1}
speed_category = {'Low':0, 'High':1}
crash = pd.read_csv("PennCrash.csv")
crash["hasCrash04"] = [1 if x > 0 else 0 for x in crash["adt04"]]
crash["totDiff"] = [y - x for x, y in zip(crash["tot04"], crash["tot12"])]

lengthVSCrash = {}
lengthVSLoad = {}
loadVSCrash = {}
curvatureVSCrash = {}
curveVSCrash = {}
intersectionVSCrash = {}
drivewaysVSCrash = {}
speedVSCrash = {}

curvatureDistribution = {}
for x, y, treat in zip(crash["tot04"], crash["curvature"], crash["Treat"]):
    #if x == 0 or not treat:continue
    y = floor(y)
    curvatureVSCrash[y] = curvatureVSCrash.get(y, 0) + x
    curvatureDistribution[y] = curvatureDistribution.get(y, 0) + 1

curvatureDistributionX = list(curvatureDistribution.keys())
curvatureDistributionX.sort()
curvatureVSMeanCrashY = [curvatureDistribution[k] for k in curvatureDistributionX]
curvatureVSMeanCrashY = [y / sum(curvatureVSMeanCrashY) for y in curvatureVSMeanCrashY]
plt.bar(curvatureDistributionX, curvatureVSMeanCrashY)
plt.xlabel('curve degree')
plt.ylabel('probability')
plt.xlim(0, 50)
#plt.yscale('log')
plt.show()

curvatureVSMeanCrashX = list(curvatureDistribution.keys())
curvatureVSMeanCrashX.sort()
curvatureVSMeanCrashY = [curvatureVSCrash[k]/curvatureDistribution[k] for k in curvatureVSMeanCrashX]
plt.bar(curvatureVSMeanCrashX, curvatureVSMeanCrashY)
plt.xlabel('curve degree')
plt.ylabel('# crashes')
plt.xlim(0, 50)
#plt.yscale('log')
plt.show()


crashes = {}
for c in crash["tot04"]:
    if c > 0:
        crashes[c] = crashes.get(c, 0) + 1
crashDistributionX = list(crashes.keys())
crashDistributionX.sort()
crashDistributionY = [crashes[x] for x in crashDistributionX]
crashDistributionY = [y/sum(crashDistributionY) for y in crashDistributionY]
plt.bar(crashDistributionX, crashDistributionY)
plt.xlabel('# crashes')
plt.ylabel('possibility')
plt.show()

# two sample t-test on crash differences
#treatDiff = [x for x, y in zip(crash["totDiff"], crash["Treat"]) if y]
#untreatDiff = [x for x, y in zip(crash["totDiff"], crash["Treat"]) if not y]
#plt.hist(crash["curvature"], 50)
#plt.show()
treatDiff = [x for x, y, curvature in zip(crash["totDiff"], crash["Treat"], crash["curvature"]) if y and curvature > 10]
untreatDiff = [x for x, y, curvature in zip(crash["totDiff"], crash["Treat"], crash["curvature"]) if not y and curvature > 10]
#treatDiff = [x for x, y, width in zip(crash["totDiff"], crash["Treat"], crash["width"]) if y and width_category[width] == 2]
#untreatDiff = [x for x, y, width in zip(crash["totDiff"], crash["Treat"], crash["width"]) if not y and width_category[width] == 2]
#treatDiff = [x for x, y, speed in zip(crash["totDiff"], crash["Treat"], crash["speed"]) if y and speed_category[speed] == 1]
#untreatDiff = [x for x, y, speed in zip(crash["totDiff"], crash["Treat"], crash["speed"]) if not y and speed_category[speed] == 1]
treatDiff = [x for x, y, tot04, tot08 in zip(crash["totDiff"], crash["Treat"], crash["tot04"], crash["tot08"]) if y and tot04 >= 1]
untreatDiff = [x for x, y, tot04, tot08 in zip(crash["totDiff"], crash["Treat"], crash["tot04"], crash["tot08"]) if not y and tot04 >= 1]
t, p = stats.ttest_ind(treatDiff, untreatDiff)
print('treat population size: {}, untreat population size: {}'.format(len(treatDiff), len(untreatDiff)))
print('treat mean: {}, untreat mean: {}'.format(sum(treatDiff)/len(treatDiff), sum(untreatDiff)/len(untreatDiff)))
print('t value:{}, p value: {}'.format(t, p/2))

treatDiffDistribution = {}
untreatDiffDistribution = {}
for diff in treatDiff:
    treatDiffDistribution[diff] = treatDiffDistribution.get(diff, 0) + 1
for diff in untreatDiff:
    untreatDiffDistribution[diff] = untreatDiffDistribution.get(diff, 0) + 1
        
treatDiffX = [k for k in treatDiffDistribution.keys()]
treatDiffX.sort()
treatDiffY = [treatDiffDistribution[x] for x in treatDiffX]
treatDiffY = [y/sum(treatDiffY) for y in treatDiffY]

untreatDiffX = [k for k in untreatDiffDistribution.keys()]
untreatDiffX.sort()
untreatDiffY = [untreatDiffDistribution[x] for x in untreatDiffX]
untreatDiffY = [y/sum(untreatDiffY) for y in untreatDiffY]

plt.subplots_adjust(hspace=0.5)
ax1=plt.subplot(2, 1, 1)
ax1.set_title('without rumble, population size: {}'.format(len(untreatDiff)))
ax1.set_xlabel('tot12 - tot04')
ax1.set_ylabel('possibility')
ax1.set_xlim([-7, 7])
plt.bar(untreatDiffX, untreatDiffY)

ax2=plt.subplot(2, 1, 2)
ax2.set_title('with rumble, population size: {}'.format(len(treatDiff)))
ax2.set_xlabel('tot12 - tot04')
ax2.set_ylabel('possibility')
ax2.set_xlim([-7, 7])
plt.bar(treatDiffX, treatDiffY)
plt.show()

crash04 = {}
crash12 = {}
for treat, tot04, tot12 in zip(crash["Treat"], crash["tot04"], crash["tot12"]):
    if treat == 0: continue
    crash04[tot04] = crash04.get(tot04, 0) + 1
    crash12[tot12] = crash12.get(tot12, 0) + 1
crash04X = [k for k in crash04.keys()]
crash04X.sort()
crash04Y = [crash04[x] for x in crash04X]

crash12X = [k for k in crash12.keys()]
crash12X.sort()
crash12Y = [crash12[x] for x in crash12X]

plt.subplots_adjust(hspace=0.5)
ax1=plt.subplot(2, 1, 1)
ax1.set_title('crash # distribution before treat')
ax1.set_xlabel('# crash')
ax1.set_ylabel('frequency')
#ax1.set_xlim([-7, 7])
plt.bar(crash04X, crash04Y)

ax2=plt.subplot(2, 1, 2)
ax2.set_title('crash # distribution after treat')
ax2.set_xlabel('# crash')
ax2.set_ylabel('frequency')
#ax2.set_xlim([-7, 7])
plt.bar(crash12X, crash12Y)
plt.show()



catCnt = {}
for x, y in zip(crash["tot04"], crash["speed"]):
    if x == 0: continue
    #lengthVSCrash += [round(y, 2)]*x
    speedVSCrash[speed_category[y]] = speedVSCrash.get(speed_category[y], 0) + x
    catCnt[speed_category[y]] = catCnt.get(speed_category[y], 0)+1
X = list(speedVSCrash.keys())
X.sort()
Y = [speedVSCrash[x]/catCnt[x] for x in X]
plt.bar(X, Y)
plt.xlabel('driveways category')
plt.ylabel('# crashes')
plt.show()
"""
for x, y in zip(crash["tot04"], crash["intersections"]):
    #lengthVSCrash += [round(y, 2)]*x
    intersectionVSCrash += [intersection_category[y]]*x
n, bins, patches = plt.hist(intersectionVSCrash, 3, facecolor='blue', alpha=0.5)
plt.hist(intersectionVSCrash, 3, facecolor='blue', alpha=0.5)
plt.xlabel('has intersection')
plt.ylabel('# crashes')
plt.show()
"""

ax3=plt.subplot(2, 1, 1)
ax3.set_title('adt04 vs # Crashes')
ax3.set_xlabel('AADT')
ax3.set_ylabel('# crashes')
for x, y in zip(crash["tot04"], crash["adt04"]):
    loadVSCrash += [round(y)]*x
n, bins, patches = plt.hist(loadVSCrash, 100, facecolor='blue', alpha=0.5)

loadVSCrash = []
ax3=plt.subplot(2, 1, 2)
ax3.set_title('adt12 vs # Crashes')
ax3.set_xlabel('AADT')
ax3.set_ylabel('# crashes')
for x, y in zip(crash["tot12"], crash["adt12"]):
    loadVSCrash += [round(y)]*x
n, bins, patches = plt.hist(loadVSCrash, 100, facecolor='blue', alpha=0.5)

"""
plt.subplots_adjust(hspace=0.5)
ax1=plt.subplot(3, 1, 1)
ax1.set_title('Horizontal Curve Existence vs # Crashes')
ax1.set_xlabel('has curves')
ax1.set_ylabel('# crashes')
for x, y in zip(crash["tot04"], crash["curves"]):
    #lengthVSCrash += [round(y, 2)]*x
    lengthVSCrash += [curves_category[y]]*x
n, bins, patches = plt.hist(lengthVSCrash, 3, facecolor='blue', alpha=0.5)

ax2=plt.subplot(3, 1, 2)
ax2.set_title('Curvature vs # Crashes')
ax2.set_xlabel('curve degree')
ax2.set_ylabel('# crashes')
for x, y in zip(crash["tot04"], crash["curvature"]):
    curvatureVSCrash += [y]*x
n, bins, patches = plt.hist(curvatureVSCrash, 100, facecolor='blue', alpha=0.5)
"""



"""
plt.figure(1)
plt.subplots_adjust(hspace=0.5)
ax1=plt.subplot(3, 1, 1)
ax1.set_title('Roadway Segment Length vs # Crashes')
ax1.set_xlabel('length (mile)')
ax1.set_ylabel('# crashes')
for x, y in zip(crash["tot04"], crash["length"]):
    #lengthVSCrash += [round(y, 2)]*x
    lengthVSCrash += [y]*x
n, bins, patches = plt.hist(lengthVSCrash, 50, facecolor='blue', alpha=0.5)

ax2=plt.subplot(3, 1, 2)
ax2.set_title('Curvature vs # Crashes')
ax2.set_xlabel('curve degree')
ax2.set_ylabel('# crashes')
for x, y in zip(crash["tot04"], crash["curvature"]):
    curvatureVSCrash += [y]*x
n, bins, patches = plt.hist(curvatureVSCrash, 100, facecolor='blue', alpha=0.5)

ax3=plt.subplot(3, 1, 3)
ax3.set_title('Annual Average Daily Traffic vs # Crashes')
ax3.set_xlabel('AADT')
ax3.set_ylabel('# crashes')
for x, y in zip(crash["tot04"], crash["adt04"]):
    loadVSCrash += [round(y)]*x
n, bins, patches = plt.hist(loadVSCrash, 50, facecolor='blue', alpha=0.5)
#crash.hist(bins=50, column=['adt04', 'adt08', 'adt12', 'length'])
#
"""

