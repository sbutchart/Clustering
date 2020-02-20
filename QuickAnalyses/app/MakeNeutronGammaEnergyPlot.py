from matplotlib import pyplot as plt
import uproot
from math import sqrt
import numpy
from ROOT import TProfile, TH1D, TCanvas, TFile
import progressbar
data = uproot.open('~/Desktop/Neutron_hist.root')['neutron/NeutronTree']

pdg      = data.array('True_PDG')
end_proc = data.array('True_EndProcess')
end_med  = data.array('True_EndMedium')
proc     = data.array('True_Process')
mother   = data.array('True_Mother')
tr_id    = data.array('True_ID')
energy   = data.array('True_Energy')

gamma_energy = []
electron_energy = []

nevent=len(pdg)
with progressbar.ProgressBar(max_value=nevent) as bar:
    
    for iEvent, _ in enumerate(pdg):
        bar.update(iEvent)
        if (iEvent >= nevent): break
        
        neutron_id_list = []
        for iParticle, _ in enumerate(pdg[iEvent]):
            end_proc_str = end_proc[iEvent][iParticle].decode("utf-8")
            end_med_str  = end_med [iEvent][iParticle].decode("utf-8")
            if pdg[iEvent][iParticle] == 2112 and end_proc_str.find('apture') != -1 and end_med_str.find('LAr') != -1:
                neutron_id_list.append(tr_id[iEvent][iParticle])

        gamma_id_list = []

        for neutron_id in neutron_id_list:
            gamma = []
            for iParticle, _ in enumerate(pdg[iEvent]):
                if pdg[iEvent][iParticle] == 22 and mother[iEvent][iParticle] == neutron_id:
                    gamma.append(energy[iEvent][iParticle])
                    gamma_id_list.append(tr_id[iEvent][iParticle])
            gamma_energy.append(gamma)

        for gamma_id in gamma_id_list:
            electron = []
            for iParticle, _ in enumerate(pdg[iEvent]):
                proc_str = proc[iEvent][iParticle].decode("utf-8")
                if abs(pdg[iEvent][iParticle]) == 11 and mother[iEvent][iParticle] == gamma_id and proc_str.find('compt') != -1:
                    electron.append(energy[iEvent][iParticle])
            electron_energy.append(gamma)


gamm_individual_energy = TH1D("gamma_individual_energy", "Gamma individual energy;Energy [MeV];Count", 100, 0, 20)
gamm_summed_energy     = TH1D("gamma_summed_energy",     "Summed gamma energy;Energy [MeV];Count", 100, 0, 20)
gamm_maximum_energy    = TH1D("gamma_maximum_energy",    "Highest energy gamma;Energy [MeV];Count", 100, 0, 20)
gamm_multiplicity      = TH1D("gamma_multiplicity",      "Gamma multiplicity;n gamma;Count", 21, -0.5, 20.5)
elec_individual_energy = TH1D("elect_individual_energy", "Electron individual energy;Energy [MeV];Count", 100, 0, 20)
elec_maximum_energy    = TH1D("elect_maximum_energy",    "Highest energy electron;Energy [MeV];Count", 100, 0, 20)
elec_multiplicity      = TH1D("elect_multiplicity",      "Compton electron multiplicity;n electron;Count", 21, -0.5, 20.5)

gamm_individual_energy.SetStats(0)
gamm_summed_energy    .SetStats(0)
gamm_maximum_energy   .SetStats(0)
gamm_multiplicity     .SetStats(0)
elec_individual_energy.SetStats(0)
elec_maximum_energy   .SetStats(0)
elec_multiplicity     .SetStats(0)

for event in gamma_energy:
    e=0
    max_e = 0
    gamm_multiplicity.Fill(len(event))
    for energy in event:
        gamm_individual_energy.Fill(energy*1000)
        e = e + energy*1000
        if (energy>max_e):
            max_e = energy
    gamm_summed_energy.Fill(e)
    gamm_maximum_energy.Fill(max_e * 1000)

for event in electron_energy:
    e=0
    max_e = 0
    elec_multiplicity.Fill(len(event))
    for energy in event:
        elec_individual_energy.Fill(energy*1000)
        e = e + energy*1000
        if (energy>max_e):
            max_e = energy
    elec_maximum_energy.Fill(max_e * 1000)

bin5MeV = gamm_maximum_energy.FindBin(5)
binMax  = gamm_maximum_energy.GetXaxis().GetNbins()
nevent5MeV = gamm_maximum_energy.Integral(bin5MeV,binMax)
nevent = gamm_maximum_energy.Integral()
percent = 100. * nevent5MeV / nevent
error = percent * (1/sqrt(nevent5MeV)+1/sqrt(nevent))
print(percent,"+/-" ,error, "% of the capturing neutrons produce photons of energy > 5 MeV")

nevent5MeV = elec_maximum_energy.Integral(bin5MeV,binMax)
nevent = elec_maximum_energy.Integral()
percent = 100. * nevent5MeV / nevent
error = percent * (1/sqrt(nevent5MeV)+1/sqrt(nevent))
print(percent,"+/-" ,error, "% of the capturing neutrons produce electrons of energy > 5 MeV")

c = TCanvas()

c.Print("NeutronEnergyPlots.pdf[")
gamm_individual_energy .Draw() 
c.Print("NeutronEnergyPlots.pdf")
gamm_summed_energy     .Draw()
c.Print("NeutronEnergyPlots.pdf")
gamm_maximum_energy    .Draw()
c.Print("NeutronEnergyPlots.pdf")
gamm_multiplicity      .Draw()
c.Print("NeutronEnergyPlots.pdf")


elec_individual_energy .Draw() 
c.Print("NeutronEnergyPlots.pdf")
elec_maximum_energy    .Draw()
c.Print("NeutronEnergyPlots.pdf")
elec_multiplicity      .Draw()
c.Print("NeutronEnergyPlots.pdf")

f = TFile("ComptonElectron.root", "RECREATE")
elec_maximum_energy    .Write()
f.Close()


c.Print("NeutronEnergyPlots.pdf]")
