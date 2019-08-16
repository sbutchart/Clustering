#!/bin/bash


for i in {0..48}
do
    echo "Doing Config "$i
    ../../build/clustering/RunSmartTrigger -l ../../build/list --feature fasdf --config ${i} -o LikelihoodTrigger_Config${i}.root
    echo "Done with config "$i
done
