#!/bin/bash

# CONFIGS[0]=0
# CONFIGS[1]=9
# CONFIGS[2]=17
# CONFIGS[2]=25
# CONFIGS[2]=33

for CONFIG in {0..48}
do
    ../../build/clustering/RunSmartTriggerStatisticalTests --ntoy 5000000 --config $CONFIG --output LikelihoodTrigger_Config$CONFIG --input ../../build/LikelihoodTrigger.root 
done
