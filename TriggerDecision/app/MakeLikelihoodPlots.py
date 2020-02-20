from matplotlib import pyplot as plt
import matplotlib
import pandas as pd
import uproot
import numpy

file = uproot.open('~/Desktop/real_llh_trig_lmc.root')
tree = file['Throws']
back = tree.array('BackgroundTestStatistics')
sign = tree.array('SignalTestStatistics')
bins = numpy.linspace(back.min()-10, 0, 100)

binwidth = bins[1] - bins[0]
data = pd.DataFrame(columns = ['efficiency','fake_rate'])
integration_time = 10
month_time = 3600*24*30.5

LikelihoodCut = None

plt.figure(num=None, figsize=(8, 6), dpi=80, facecolor='w', edgecolor='k')
plt.subplot(2, 1, 1)
back_llh, _, _ = plt.hist(numpy.clip(back, bins[0], bins[-1]), bins,histtype='step', stacked=True, density=True, fill=False, color='blue', label='background')
sign_llh, _, _ = plt.hist(numpy.clip(sign, bins[0], bins[-1]), bins,histtype='step', stacked=True, density=True, fill=False, color='red',  label='signal+background')
plt.legend(loc='upper center')
plt.xlabel('Likelihood')
plt.ylabel('n Toys')

for i in range(len(back_llh)):
    efficiency = 0
    fake_rate = 0
    for ii in numpy.linspace(0,i,i):
        ii = int(ii)
        efficiency += sign_llh[ii] * binwidth
        fake_rate  += back_llh[ii] * binwidth * month_time / integration_time
        if (fake_rate>1 and LikelihoodCut is None):
            LikelihoodCut = 0.5*(bins[i-1]+bins[i])
            print ("LikelihoodCut", LikelihoodCut)
            
    data.loc[i] = [efficiency, fake_rate]

axes = plt.gca()
plt.plot([LikelihoodCut, LikelihoodCut], axes.get_ylim(), 'k-')
plt.yscale('log')

plt.subplot(2, 1, 2)
plt.plot(data['fake_rate'], data['efficiency'])
plt.xscale('log')
plt.xlabel('Fake rate [per month]')
plt.ylabel('Triggering efficiency')
plt.show()

