# Clustering and SN trigger "SNCT"
Supernova Clustering and triggering code at DUNE.

## Installation
To compile it, you need to make sure you have ROOT setup, have `cmake` and `boost`.
On FNAL/CERN machines (change versions as they become updated):
```sh
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_65_03d00 -q e20:prof
setup cmake v3_24_1
```

Then, one can clone and build the repository:
```sh
git clone <whatever the address of this is>
cd <wherever you want to build>
mkdir build
cd build
cmake <this directory>
make -j`nproc` # or "make" if you don't want to use all the processors of your machines to build (I can't see a reason why you wouldn't).
```

## Preliminary steps
The clustering algorithm runs on `larsoft` events. To do this, one need to sequentially run the following `fhicl`:
```
prodmarley_nue_spectrum_radiological_timedep_hudepohl_11.2M_dune10kt_1x2x6.fcl 
supernova_g4_dune10kt_1x2x6.fcl
nonoise_nozs_detsim_supernova_dune10kt_1x2x6.fcl
addnoise_findprim_snanas.fcl
```
### Generation
`prodmarley_nue_spectrum_radiological_timedep_hudepohl_11.2M_dune10kt_1x2x6.fcl` runs the "generation" step. This means the output of this stage will be a file containing __primary_ particles that comes from:
 - 1 Supernova interaction in the detector
 - all the radiogenic background in the detector, to name a few:
   - Argon 39 beta decays
   - Argon 42 beta decays
   - Neutron coming from rock and concrete
   - Radon coming from the photon detection system

### Geant4
`supernova_g4_dune10kt_1x2x6.fcl` runs the "geant4" steps. This steps propagates all the particles from the previous stage in and around the detector. The `supernova` prefix indicate that this stage is appropriate for low energy simulations. Difference with the `standard` prefix includes:
 - Saving all the EM shower particles in the output
 - Having more reliable neutron simulations

The Geant4 stage handles the propagation of the particle, the creation of ionisation electrons and scintillation photons in the LAr, and the propagation of these ionisation electrons and scintillation photons, although these 2 steps are not actually done with the Geant4 software for computation reasons (there is wayyyyy to many ionisation electrons to follow in Geant4).
The output file therefore has (amongst other things):
 - A list of all the particles that were created by and after the primary particles
 - A list of all the electrons that arrive on each wire and at which time (called tick)
 - A list of all the photons that arrive on each photon detector
 
### Detsim
`nonoise_nozs_detsim_supernova_dune10kt_1x2x6.fcl` runs the "detsim" step, i.e. everything that is related to the readout. This means all the shaping from number of electrons to actual ADC readout values. An interesting point is that there isn't any noise generated on the wire at this stage (this was done for storage reason), and similarly, no zero suppression is applied (i.e. we store the _full_ waveform for 2.2ms at 2MHz for all the channels of all the APA). At the end of this stage, one gets wire adc vectors with only the shaped signal on them.

### Add noise, find prim, analyser
`addnoise_findprim_snanas.fcl` run 3 steps in 1:
 - It adds the noise on the wire.
 - Run the trigger primitive finder algorithm (i.e. finds the hits on the wire). This is the same algorithm that is run online on ProtoDUNE, so we know it's possible to actually trigger with this.
 - Dumps all the necessary information in a ROOT `TTree` for Supernova clustering and triggering.

## Running clustering
This is ran using:
```sh
./build/Clustering/app/DAQClustering
```

Command line parameters:
 - `-h/--help` to get an idea of the command line parameters.
 - *Required* input file from the larsoft `SNAna_Module`: `-i/--input` (this is a file)
 - *Required* which tree to analyse in the input file: `-t/--tree`
 - *Required* output file where all the clusters get saved: `-o/--output` (this is also a root file)
 - *Optional* the configuration, what parameters of the clustering algorithm to run with: `-c/--config` this is a number between 0 and whatever the size of the vectors in [here](https://github.com/plasorak/Clustering/blob/reorganisation/Clustering/Clustering.hh#L89). Nominally you can just go with 0, but if you don't specify this, it will run over _all_ the configurations, which could really make this stage slow.
 - *Optional* number of event: `-e/--event`

Note an interesting file for TESTING can be found here:
```sh
/dune/data/users/plasorak/trigprim2000ManyConfig/Clustered_snanatrigprim2000_prodmarley_nue_spectrum_radiological_timedep_hudepohl_11.2M_3perevent_dune10kt_1x2x6_g4_detsim_defaultnoise_pdreco_snana.root
```
This file has 3 SN events per larsoft event and therefore shouldn't be used calculate efficiencies...
## Analyse the result
### Visual output
```sh
./build/Analyser/app/AnalyseDAQClustering
```
That will produce a big `pdf` document which contains a lot of information about the clustering.

Command line parameters:
 - `-h/--help` to get an idea of the command line parameters.
 - *Required* input file from the clustering stage: `-i/--input` (this is a cluster file)
 - *Required* output file where all the clusters get saved: `-o/--output` (this is a pdf file)
 - *Required* the configuration, what parameters of the cluster algorithm you want to plot with: `-c/--config` this is a number between 0 and whatever the size of the vectors in [here](https://github.com/plasorak/Clustering/blob/reorganisation/Clustering/Clustering.hh#L89). Nominally you can just go with 0.
 - *Optional* number of event: `-e/--event` (not sure thisis working correctly, use with caution)
 - *Optional* you can also specify a number of hits cut: `--hitcut` (default is 0, which is whatever the clustering algorithm is in the previous stage).

### Background and efficiency
But sometimes, we just want to know how the is performing _overall_ (in the SN triggering counting approach, for example), in this case, we need 2 numbers, the efficiency and the background rate. This is just what this is doing.
```sh
./build/Analyser/app/GetEffBackRate
```

Command line parameters:
 - `-h/--help` to get an idea of the command line parameters.
 - *Required* input file from the clustering stage: `-i/--input` (this is a cluster file)
 - *Optional* (but important) the output file: `-o/--output` a text file where the background and efficiency will be saved, with their respective uncertainties.
 - *Required* the configuration, what parameters of the cluster algorithm you want to plot with: `-c/--config` this is a number between 0 and whatever the size of the vectors in [here](https://github.com/plasorak/Clustering/blob/reorganisation/Clustering/Clustering.hh#L89). Nominally you can just go with 0.
 - *Optional* number of event: `-e/--event` (not sure thisis working correctly, use with caution)
 - *Optional* you can also specify a number of hits cut: `--hitcut` (default is 0, which is whatever the clustering algorithm is in the previous stage).
 - *Optional* (but VERY IMPORTANT!) detector scaling ; this is how you get to a full 10kT module from whatever you generated in larsoft. By default this is 0.12, i.e. the workspace that was used to generate the radiological background was 12% of the total volume of the 10kT.

## Supernova sensitivities
### Traditional counting approach
To get plot with the traditionnal approach one can use:

``` sh
./build/BurstTrigger/SNBurst --output-pdf tradi.pdf --output-root tradi.root -t ../BurstTrigger/data/SNTheoryDistributions.root --input-text BackEff.txt
```
This will create `tradi.pdf` which contain histograms related to the efficiency of the trigger.
It will also create afile called `tradi.root`, which can be fed into:

``` sh
./build/BurstTrigger/PlotSNBurst -i tradi.root -o nice_pdf.pdf
```

### Likelihood approach
To get the sensitivity with the likelihood trigger on has to run 2 steps:
 - The first step to get the background and signal PDF.
 - Second step is to actually run the trigger simulations.
Note in the previous case (counting approach), getting the PDF is just the same as getting the number of events (i.e. background rate and efficiency).

To realise the first step, it's basically something quite similar to what is realised by `AnalyseDAQClustering`, but not quite, since you need to be careful about normalisations etc..

``` sh
./TriggerDecision/app/DumpForStatisticalTrigger --list ../FileLists/prodmarley_nue_spectrum_radiological_timedep_hudepohl_1.2M_3perevent_dune10kt_1x2x6_snanatrigprim2000_clustered_manyconfig.list --nconfig 48
```

Runs over a list of files which has 48 configurations in them (this will take a while to run). It creates an output file `LikelihoodTrigger.root` (you can modify this name in the command line) whichcontains many PDF for all the configurations and files. An example of a file is here:

``` sh
/dune/app/users/plasorak/thiago_clustering/Clustering/build/LikelihoodTrigger.root
```

Then, you can run:
``` sh
./TriggerDecision/app/SmartTriggerStatisticalTests --ntoy 10000 --nevent 10 --config 2 --output out.root --method Likelihood --inputsignal LikelihoodTrigger.root  --inputbackground LikelihoodTrigger.root
```
This will throw 10k toys for supernova of 10 events using the configuration number 2 of the file before. Note, you can have different files for the PDF of the signal and background (in case, for example, where you used a file which had only radiological samples in them).

Once you have run that, you are done, because `out.root` now contains 10000 likelihoods in the case where you have background and signal+background. You can just plot them and get the sensitivity by using the python script:

``` sh
python TriggerDecision/MakeLikelihoodPlots.py
```
_Note this is not in the build directory._
You will need to have `uproot` (which I have no clue how to install on the gpvm, but otherwise I think you can do `pip install uproot` on your laptop), and change the first line `file = uproot.open('~/Desktop/real_llh_trig_lmc.root')` to be whatever/wherever the file `out.root` is.
