from matplotlib import pyplot as plt
import uproot
import numpy

file =uproot.open('LMC.root')
tree = file['Throws']
back = tree.array('BackgroundTestStatistics3')
sign = tree.array('SignalTestStatistics')
bins = numpy.linspace(numpy.minimum(back)-10, 0, 100)
plt.hist(numpy.clip(back, bins[0], bins[-1]), bins,histtype='step',density=True, fill=False, color='blue', label='background')
plt.hist(numpy.clip(sign, bins[0], bins[-1]), bins,histtype='step',density=True, fill=False, color='red',  label='signal+background')
plt.yscale('log')
plt.show()

