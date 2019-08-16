#!/bin/bash

echo "Process Number" $PROCESS

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup ifdhc
setup root v6_12_06a  -q e17:prof
setup tbb v2018_2a  -q e17:prof

#set -e
let "PROCESS_FROM1 = ${PROCESS}+1"
echo "PROCESS_FROM1 " $PROCESS_FROM1
printf -v PROCESS_2D "%02d" $PROCESS_FROM1

TARBALL="/pnfs/dune/persistent/users/plasorak/clustering_multi_config.tar"; echo "TARBALL " $TARBALL
LISTINPUTFILE="/pnfs/dune/persistent/users/plasorak/lists/mcc11_daq_snana_radio_x5.list"$PROCESS_2D; echo "LISTINPUTFILE " $LISTINPUTFILE
OUTPUTDIR="/pnfs/dune/persistent/users/plasorak/ClusteredWireMultipleBackground"; echo "OUTPUTDIR " $OUTPUTDIR
EXECUTABLE="clustering/RunDAQClustering_WireOnly"; echo "EXECUTABLE " $EXECUTABLE
let "OFFSET = 500000 * ${PROCESS}" || true
echo "INITIAL OFFSET " $OFFSET

ifdh cp -D $TARBALL ./
ifdh cp -D $LISTINPUTFILE ./

LISTINPUTFILELOCAL=$(basename -- "$LISTINPUTFILE")
TARBALLFILENAME=$(basename -- "$TARBALL")

tar -xvf $TARBALLFILENAME
LIBRARYNAME="${TARBALLFILENAME%.*}"
LD_LIBRARY_PATH=`pwd`/clustering:$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH

TREE="snanatrigprim2000"

for FILE in $(cat $LISTINPUTFILELOCAL)
do
    ifdh cp -D $FILE .
    INPUTFILE=$(basename -- "$FILE")
    echo "INPUT FILE " $INPUTFILE
    echo "STARTING TO PROCESS TREE " $TREE
    OUTPUTFILE=${INPUTFILE/.root/_${TREE}_Clustered.root}
    echo "OUTPUTFILE " $OUTPUTFILE
    COMMAND="$EXECUTABLE -i $INPUTFILE -t $TREE/SNSimTree -o $OUTPUTFILE -f $OFFSET"
    echo "COMMAND " $COMMAND
    $COMMAND
    let "OFFSET = ${OFFSET} + 400"
    ifdh cp -D --force $OUTPUTFILE $OUTPUTDIR
    rm $INPUTFILE
done

