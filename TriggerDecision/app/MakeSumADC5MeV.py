from matplotlib import pyplot as plt
import matplotlib
import pandas as pd
import uproot
import numpy
from root_pandas import read_root

data = read_root('~/Desktop/Generate_Electron_gen_g4_detsim_addnoise_trigprim_gaushit_fasthit_snana.root', 'arbitraryanatrigprim2000/SNSimTree')

list_data = []

for index, row in data.iterrows():
    sadc=0.
    for adc in row.Hit_SADC:
        sadc+=adc
    loc_data = [sadc, row.True_E[0]]
    list_data.append(loc_data)


energybinning=numpy.linspace(0, 10, 20)
sadcbinning  =numpy.linspace(0, 40, 10)

df = pd.DataFrame(list_data,columns=['SumADC', 'True_E'])

plt.hist2d(y=df.SumADC/100, x=df.True_E*1000.,bins=(energybinning, sadcbinning),cmap=plt.cm.jet)
plt.colorbar()
plt.xlabel('E electron [MeV]')
plt.ylabel('SumADC')
plt.show()

