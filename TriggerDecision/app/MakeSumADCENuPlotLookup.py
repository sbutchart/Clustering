from matplotlib import pyplot as plt
from root_pandas import read_root
import numpy
energybinning=numpy.linspace(0, 50,  50)
timebinning  =numpy.linspace(0, 10, 100)
sumadcbinning=numpy.linspace(0,200,  50)



plt.figure(num=None, figsize=(8, 6), dpi=80, facecolor='w', edgecolor='k')
plt.subplot(2, 1, 1)
data = read_root('LikelihoodTrigger.root', 'mapping')
data_config = data.query('Config==13')

enu_clipped    = numpy.clip(data_config.ENu*1000 , energybinning[0], energybinning[-1])
elep_clipped   = numpy.clip(data_config.ELep*1000, energybinning[0], energybinning[-1])
sumadc_clipped = numpy.clip(data_config.SumADC  , sumadcbinning[0], sumadcbinning[-1])

histo, xbins, ybins, _ = plt.hist2d(enu_clipped, sumadc_clipped,bins=(energybinning, sumadcbinning),cmap=plt.cm.jet)
plt.colorbar()
plt.xlabel('ENu [MeV]')
plt.ylabel('Sum ADC')

plt.subplot(2, 1, 2)
histo, xbins, ybins, _ = plt.hist2d(elep_clipped, sumadc_clipped,bins=(energybinning, sumadcbinning),cmap=plt.cm.jet)
# plt.hist2d(time_clipped, enu_clipped,bins=(timebinning, energybinning),cmap=plt.cm.jet)
plt.colorbar()
plt.xlabel('ELep [MeV]')
plt.ylabel('Sum ADC')
# plt.xlabel('Time [s]')
# plt.ylabel('ENu [MeV]')

# plt.subplot(2, 2, 3)
# plt.hist(time_clipped, bins=timebinning,density=True)
# plt.xlabel('Time [s]')
# plt.ylabel('PDF')

# plt.subplot(2, 2, 4)
# plt.hist(enu_clipped, bins=energybinning, density=True)
# plt.xlabel('ENu [MeV]')
# plt.ylabel('PDF')
plt.show()

