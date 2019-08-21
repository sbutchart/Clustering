from matplotlib import pyplot as plt
from root_pandas import read_root
import numpy

data = read_root('LikelihoodTrigger.root', 'mapping')
data_config = data.query('Config==0')
plt.hist2d(data_config.ENu*1000., data_config.SumADC,bins=(100,100),cmap=plt.cm.jet)
plt.colorbar()
plt.margins(0)
plt.xlabel('ENu [MeV]')
plt.ylabel('Sum ADC')
plt.show()

