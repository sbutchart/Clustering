#!/bin/bash
ResultFile="ResultPhilMCC11.txt"
echo "" > $ResultFile

for dir in snanatrigprim1000 snanatrigprim1200 snanatrigprim1400 snanatrigprim1600 snanatrigprim1800 snanatrigprim2000 snanatrigprim2400 snanatrigprim800
#for dir in snanatrigprim800
do
    if [ ! -e ${dir} ]
    then
       mkdir $dir
    fi
    offset=0
    for file in `cat /dune/app/users/rodriges/run-snana-on-grid/snana-output-files-defaultnoise.list`
    do
        filebase=`basename $file`
        echo $offset
        #echo $filebase $dir
        ./build/RunDAQClustering -i ${file} -t ${dir}/SNSimTree -o ${dir}/clustered_${filebase} -s $offset
        offset=$(( $offset+10000 ))
    done
    list=`ls -1 ${dir}/clustered_*.root`
    #echo $list
    if [ -e ./${dir}/clustered.root ]
    then
        rm -rf ./${dir}/clustered.root
    fi
    hadd ${dir}/clustered.root ${list}
    ./build/GetEffBackRate -i ${dir}/clustered.root -o ${dir}/trigprim.txt
    echo ${dir} >> $ResultFile
    cat ${dir}/trigprim.txt >> $ResultFile
done





