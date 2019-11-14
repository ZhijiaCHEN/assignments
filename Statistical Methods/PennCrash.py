import os
import pandas as pd
import matplotlib.pyplot as plt

crash = pd.read_csv("PennCrash.csv")
crash["hasCrash04"] = [1 if x > 0 else 0 for x in crash["adt04"]]
#crash.hist(bins=50, column=['adt04', 'adt08', 'adt12', 'length'])
crash.plot(kind="scatter", x="hasCrash04", y="adt04", alpha=0.3)
plt.show()