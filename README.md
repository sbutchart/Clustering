# Clustering and SN trigger "SNCT"
Supernova Clustering and triggering code at DUNE.

## Installation
To compile it, you need to make sure you have a ROOT setup (`source <wherever root is>/bin/thisroot.sh`), have `cmake` and `boost`.
I usually setup a developer version of `dunetpc` and that gives me everything I need on the fnal machines.
```
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
```
./build/Clustering/app/DAQClustering
```

Command line parameters:
 - `-h/--help` to get an idea of the command line parameters.
 - *Required* input file from the larsoft `SNAna_Module`: `-i/--input` (this is a file)
 - *Required* which tree to analyse in the input file: `-t/--tree`
 - *Required* output file where all the clusters get saved: `-o/--output` (this is also a root file)
 - *Optional* the configuration, what parameters of the clustering algorithm to run with: `-c/--config` this is a number between 0 and whatever the size of the vectors in [here](https://github.com/plasorak/Clustering/blob/reorganisation/Clustering/Clustering.hh#L89). Nominally you can just go with 0, but if you don't specify this, it will run over _all_ the configurations, which could really make this stage slow.
 - *Optional* number of event: `-e/--event`


## Analyse the result
### Visual output
```sh
./build/Analyser/app/AnalyseDAQClustering
```
That will produce a big `pdf` document which contains a lot of information about the clustering.

Command line parameters:
 - `-h/--help` to get an idea of the command line parameters.
 - *Required* input file from the clustering stage: `-i/--input` (this is a file)
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
 - *Required* input file from the clustering stage: `-i/--input` (this is a file)
 - *Required* the configuration, what parameters of the cluster algorithm you want to plot with: `-c/--config` this is a number between 0 and whatever the size of the vectors in [here](https://github.com/plasorak/Clustering/blob/reorganisation/Clustering/Clustering.hh#L89). Nominally you can just go with 0.
 - *Optional* number of event: `-e/--event` (not sure thisis working correctly, use with caution)
 - *Optional* you can also specify a number of hits cut: `--hitcut` (default is 0, which is whatever the clustering algorithm is in the previous stage).
 - *Optional* (but VERY IMPORTANT!) detector scaling ; this is how you get to a full 10kT module from whatever you generated in larsoft. By default this is 0.12, i.e. the workspace that was used to generate the radiological background was 12% of the total volume of the 10kT.
