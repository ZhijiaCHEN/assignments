import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import numpy as np

def average_tree_growth():
    treeGrowth = pd.read_csv("TreeGrowth.txt")
    annualprecDict = {}
    summerpdsIDict = {}
    wintertempDict = {}
    for yr, annualprec, summerpdsi, wintertemp in zip(treeGrowth['yr'],treeGrowth['annualprec'],treeGrowth['summerpdsi'],treeGrowth['wintertemp']):
        if yr in annualprecDict:
            assert annualprecDict[yr] == annualprec
        else:
            annualprecDict[yr] = annualprec
        if yr in summerpdsIDict:
            assert summerpdsIDict[yr] == summerpdsi
        else:
            summerpdsIDict[yr] = summerpdsi
        if yr in wintertempDict:
            assert wintertempDict[yr] == wintertemp
        else:
            wintertempDict[yr] = wintertemp
    yr = [y for y in annualprecDict]
    yr.sort()
    annualprec = [annualprecDict[y] for y in yr]
    summerpdsi = [summerpdsIDict[y] for y in yr]
    wintertemp = [wintertempDict[y] for y in yr]

    IDo = None
    yro = None
    cmo = None
    preo = None
    pdsio = None
    tempo = None

    treeGrowthAverage = pd.DataFrame({'id':[], 'yr':[], 'cm':[], 'dcm':[], 'annualprec':[], 'summerpdsi':[], 'wintertemp':[]})
    for IDn, yrn, cmn, pren, pdsin, tempn in zip(treeGrowth['id'], treeGrowth['yr'], treeGrowth['cm'], treeGrowth['annualprec'], treeGrowth['summerpdsi'], treeGrowth['wintertemp']):
        if IDo == IDn:
            assert yrn > yro
            dyr = yrn-yro
            dcm = (cmn-cmo)/dyr
            cmAverage = (cmn+cmo)/2
            yrnIdx = yr.index(yrn)
            yroIdx = yr.index(yro)
            if yrnIdx - yroIdx > 1:
                preAverage = sum(annualprec[yroIdx:yrnIdx+1])/(yrnIdx-yroIdx+1)
                pdsiAverage = sum(summerpdsi[yroIdx:yrnIdx+1])/(yrnIdx-yroIdx+1)
                tempAverage = sum(wintertemp[yroIdx:yrnIdx+1])/(yrnIdx-yroIdx+1)
                pass
            else:
                preAverage = (pren+preo)/2
                pdsiAverage = (pdsin+pdsio)/2
                tempAverage = (tempn+tempo)/2
            new_row = {'id':IDo, 'yr':yro, 'cm':cmAverage, 'dcm':dcm, 'annualprec':preAverage, 'summerpdsi':pdsiAverage, 'wintertemp':tempAverage}
            treeGrowthAverage = treeGrowthAverage.append(new_row, ignore_index=True)

        IDo = IDn
        yro = yrn
        cmo = cmn
        preo = pren
        pdsio = pdsin
        tempo = tempn

    treeGrowthAverage.to_csv(index=False, path_or_buf='TreeGrowthAverage.csv')

def eda():
    treeGrowth = pd.read_csv("TreeGrowth.txt")

    annualprecDict = {}
    summerpdsIDict = {}
    wintertempDict = {}
    for yr, annualprec, summerpdsi, wintertemp in zip(treeGrowth['yr'],treeGrowth['annualprec'],treeGrowth['summerpdsi'],treeGrowth['wintertemp']):
        if yr in annualprecDict:
            assert annualprecDict[yr] == annualprec
        else:
            annualprecDict[yr] = annualprec
        if yr in summerpdsIDict:
            assert summerpdsIDict[yr] == summerpdsi
        else:
            summerpdsIDict[yr] = summerpdsi
        if yr in wintertempDict:
            assert wintertempDict[yr] == wintertemp
        else:
            wintertempDict[yr] = wintertemp
    yr = [y for y in annualprecDict]
    yr.sort()
    annualprec = [annualprecDict[y] for y in yr]
    summerpdsi = [summerpdsIDict[y] for y in yr]
    wintertemp = [wintertempDict[y] for y in yr]
    plt.subplots_adjust(wspace=0.3)
    plt.subplot(1,3,1)
    
    plt.plot(yr, summerpdsi, marker='o')
    plt.xlim(1997,2012)
    plt.xlabel('year')
    plt.ylabel('summer PDSI')
    plt.subplot(1,3,2)
    plt.plot(yr, annualprec, marker='o')
    plt.xlim(1997,2012)
    plt.xlabel('year')
    plt.ylabel('annual precipitation')
    plt.subplot(1,3,3)
    plt.plot(yr, wintertemp, marker='o')
    plt.xlim(1997,2012)
    plt.xlabel('year')
    plt.ylabel('winter temperature')
    plt.show()
    annualprecNorm  = [(x-(max(annualprec)+min(annualprec))/2)/(max(annualprec)-min(annualprec))*2 for x in annualprec]
    summerpdsiNorm  = [(x-(max(summerpdsi)+min(summerpdsi))/2)/(max(summerpdsi)-min(summerpdsi))*2 for x in summerpdsi]
    wintertempNorm  = [(x-(max(wintertemp)+min(wintertemp))/2)/(max(wintertemp)-min(wintertemp))*2 for x in wintertemp]
    print('correlation between pdsi and precipitation = {}'.format(np.corrcoef(annualprec, summerpdsi)))
    print('correlation between pdsi and winter temperature = {}'.format(np.corrcoef(wintertemp, summerpdsi)))
    plt.plot(yr, annualprecNorm , marker='o', label = 'annual precipitation')
    plt.plot(yr, summerpdsiNorm , marker='v', linestyle='dashed', label = 'summer PDSI')
    plt.plot(yr, wintertempNorm , marker='s', label = 'winter temperature')
    plt.legend()
    plt.show()

    YR = {}
    CM = {}
    PDSI = {}
    WINTERTEMP = {}
    PRECIPITATION = {}
        


    rmID = []
    for id, yr, cm, annualprec, summerpdsi, wintertemp in zip(treeGrowth['id'], treeGrowth['yr'], treeGrowth['cm'], treeGrowth['annualprec'], treeGrowth['summerpdsi'], treeGrowth['wintertemp']):
        if id in YR:
            if CM[id][-1] > cm+1:
                rmID.append(id)
            YR[id].append(yr)
            #CM[id].append(cm-CM[id][0])
            CM[id].append(cm)
            PRECIPITATION[id].append(annualprec)
            PDSI[id].append(summerpdsi)
            WINTERTEMP[id].append(wintertemp)
        else:
            YR[id] = [yr]
            CM[id] = [cm]
            PRECIPITATION[id] = [annualprec]
            PDSI[id] = [summerpdsi]
            WINTERTEMP[id] = [wintertemp]
    
    plt.subplots_adjust(hspace=0.5)
    for id in YR:
        plt.subplot(2, 2, 1)
        plt.plot(YR[id], CM[id], color='k', marker = 'o')
        plt.subplot(2, 2, 2)
        plt.plot(YR[id], PDSI[id], color='k', marker = 'o')
        plt.subplot(2, 2, 3)
        plt.plot(YR[id], WINTERTEMP[id], color='k', marker = 'o')
        plt.subplot(2, 2, 4)
        plt.plot(YR[id], PRECIPITATION[id], color='k', marker = 'o')
    plt.subplot(2, 2, 1)
    plt.xlabel('year')
    plt.ylabel('diameter')
    plt.subplot(2, 2, 2)
    plt.xlabel('year')
    plt.ylabel('summer PDSI')
    plt.subplot(2, 2, 3)
    plt.xlabel('year')
    plt.ylabel('winter temperature')
    plt.subplot(2, 2, 4)
    plt.xlabel('year')
    plt.ylabel('annual precipitation')
    plt.show()
        
    cnt = 0
    for id in YR:
        if id in rmID: continue
        #if YR[id][0] > 2004:continue
        if YR[id][-1] != 2011 or len(YR[id]) < 5:
            cnt += 1
            continue
        if len(YR[id]) < 5:continue
        for i in range(1, len(CM[id])):
            CM[id][i] -= CM[id][0]
        CM[id][0] = 0
        color = 'k'
        marker = 'o'

        if CM[id][-1] <= 4:
            color = 'red'
            marker = '.'
        elif CM[id][-1] <= 7:
            color = 'green'
            marker = 'd'
        else:
            color = 'blue'
            marker = '^'

        plt.plot([y - YR[id][0] for y in YR[id]], CM[id], color=color, marker=marker)
    plt.xlabel('# of observed year')
    plt.ylabel('diameter difference with initial')
    plt.show()

    plt.hist([x for id, x in zip(treeGrowth['id'], treeGrowth['cm']) if id not in rmID ], bins=100)
    plt.xlabel('diameter')
    plt.ylabel('frequency')
    plt.show()

    newPlant = []
    growthDitribution = {}
    ido = None
    yro = None
    cmo = None
    preo = None
    pdsio = None
    tempo = None
    for idn, yrn, cmn in zip(treeGrowth['id'], treeGrowth['yr'], treeGrowth['cm']):
        if ido == idn and yrn != 2007:
            dcm = (cmn-cmo)/(yrn-yro)
            growthDitribution[yrn] = growthDitribution.get(yrn, [])+[(cmn, dcm)]
        if ido != idn and yrn > 1998:
            newPlant.append(yrn)
        ido = idn
        yro = yrn
        cmo = cmn
    for i, yr in enumerate(sorted(growthDitribution.keys())):
        cm = [x[0] for x in growthDitribution[yr]]
        dcm = [x[1] for x in growthDitribution[yr]]
        plt.scatter(cm, dcm, marker='o', alpha=0.3)
    plt.xlabel('diameter')
    plt.ylabel('annual diameter change')
    plt.show()

    print('new plants added in year: {}'.format(set(newPlant)))
    markerDict = {1998:'.', 2002:'v', 2004:'s', 2006:'d', 2007:'+', 2008:'*', 2010:'1', 2011:'p'}
    plt.subplots_adjust(hspace=0.5)
    plt.subplots_adjust(wspace=0.5)
    for i, yr in enumerate(sorted(growthDitribution.keys())):
        axes = plt.subplot(2, 3, i+1)
        cm = [x[0] for x in growthDitribution[yr]]
        dcm = [x[1] for x in growthDitribution[yr]]
        plt.scatter(cm, dcm, marker='o', alpha=0.5)
        plt.xlabel('diameter in year {}'.format(yr))
        plt.ylabel('growth')
        plt.ylim(-1.2, 1.2)
        #axes.set_ylabel('annual diameter change')
    plt.show()
    plt.hist(treeGrowth['cm'], bins=50)
    plt.xlabel('average annual growth (cm)')
    plt.ylabel('frequency')
    plt.show()


def group():
    YR = {}
    CM = {}
    def pdsiGroup(x):
        if x > -1:
            return 1
        elif x > -2:
            return 2
        else:
            return 3
            
    growthGroup = {} 
    treeGrowth = pd.read_csv("TreeGrowth.txt")
    averageTreeGrowth = pd.read_csv("TreeGrowthAverage.csv")
    groupTreeGrowth = pd.DataFrame({'id':[], 'yr':[], 'cm':[], 'dcm':[], 'annualprec':[], 'summerpdsi':[], 'wintertemp':[], 'growthGroup':[], 'pdsiGroup':[]})
    rmID = []
    for id, yr, cm in zip(treeGrowth['id'], treeGrowth['yr'], treeGrowth['cm']):
        if id in YR:
            if CM[id][-1] > cm+1:
                rmID.append(id)
            YR[id].append(yr)
            #CM[ID].append(cm-CM[ID][0])
            CM[id].append(cm)
        else:
            YR[id] = [yr]
            CM[id] = [cm]
    for id in CM:
        if YR[id][0] != 1998:
            rmID.append(id)
        if CM[id][-1] <= 4:
            growthGroup[id] = 1
        elif CM[id][-1] <= 7:
            growthGroup[id] = 2
        else:
            growthGroup[id] = 3
    pcnt = 0
    ncnt = 0
    dcmo = None
    for id, yr, cm, dcm, annualprec, summerpdsi, wintertemp in zip(averageTreeGrowth['id'], averageTreeGrowth['yr'], averageTreeGrowth['cm'], averageTreeGrowth['dcm'], averageTreeGrowth['annualprec'], averageTreeGrowth['summerpdsi'], averageTreeGrowth['wintertemp']):
        if yr == 2006:
            if dcm < dcmo:
                ncnt += 1
            else:
                pcnt += 1
        dcmo = dcm
        if id in rmID: continue
        new_row = {'id':str(id), 'yr':str(yr), 'cm':cm, 'dcm':dcm, 'annualprec':annualprec, 'summerpdsi':summerpdsi, 'wintertemp':wintertemp, 'growthGroup':str(growthGroup[id]), 'pdsiGroup':str(pdsiGroup(summerpdsi))}
        groupTreeGrowth = groupTreeGrowth.append(new_row, ignore_index=True)
    groupTreeGrowth.to_csv(index=False, path_or_buf='groupTreeGrowth.csv')
#average_tree_growth()
group()
eda()