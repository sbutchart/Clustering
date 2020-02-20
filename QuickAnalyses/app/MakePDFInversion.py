from matplotlib import pyplot as plt
import uproot
from math import sqrt
import numpy
from ROOT import TProfile, TH1D, TCanvas, TFile
import progressbar
data     = uproot.open('~/Desktop/LikelihoodTrigger.root')['mapping']
spectrum = TFile('ComptonElectron.root').Get('elect_maximum_energy')

sumadc = data.array('SumADC')
elep   = data.array('ELep')

spectrum_2 = TH1D(spectrum.Clone())

spectrum_2.Reset()
print(type(spectrum_2))
for e,_ in enumerate(elep):
    print (elep[e])
    spectrum_2.Fill(elep[e])

spectrum_2.Divide(spectrum)

sum_adc_spectrum = TH1D("",";SumADC;Count", 20, 0, 200)

for e,_ in enumerate(elep):
    w = spectrum_2.GetBinContent(spectrum_2.FindBin(elep[e]))
    sum_adc_spectrum.Fill(sumadc, w)

c = TCanvas
sum_adc_spectrum.Draw()
c.Print('new_sum_adc.pdf')


