#!/bin/bash




for i in {1..30}
do
    echo "Starting the nevent = " $i
    ./clustering/RunSmartTriggerStatisticalTests --ntoy 1000000 --config 13 --event ${i} --output LikelihoodTriggerOutput_n${i}
done
