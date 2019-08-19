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