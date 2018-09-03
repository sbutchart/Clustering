#!/bin/bash
ResultFile="ResultPhilMCC11.txt"
echo "" > $ResultFile
OutputDir="/dune/data/users/plasorak/PhilHitPrimitives"
for threshold in 800 1000 1200 1400 1600 1800 2000 2400
do
    dir="snanatrigprim"$threshold
    CompleteDir=${OutputDir}/${dir}
    if [ ! -e ${CompleteDir} ]
    then
       mkdir ${CompleteDir}
    fi
    offset=0
    
    for file in `cat /dune/app/users/rodriges/run-snana-on-grid/snana-output-files-defaultnoise-pdreco.list`
    do
        filebase=`basename $file`
        echo $offset
        ./build/RunDAQClustering -i ${file} -t ${dir}/SNSimTree -o ${CompleteDir}/clustered_${filebase} -s $offset
        offset=$(( $offset+10000 ))
    done
    list=`ls -1 ${CompleteDir}/clustered_*.root`
    if [ -e ${CompleteDir}/clustered.root ]
    then
        rm -rf ${CompleteDir}/clustered.root
    fi
    hadd ${CompleteDir}/clustered.root ${list}
    ./build/GetEffBackRate -i ${CompleteDir}/clustered.root -o ${CompleteDir}/trigprim.txt -n $threshold
    echo ${CompleteDir} >> $ResultFile
    cat ${CompleteDir}/trigprim.txt >> $ResultFile
done





