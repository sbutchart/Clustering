#!/bin/bash

echo "Process Number" $PROCESS

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup ifdhc
setup root v6_12_06a  -q e17:prof
setup tbb v2018_2a  -q e17:prof

set -e
let "LINE = ${PROCESS}+1"
echo "LINE " $LINE
TARBALL="/pnfs/dune/persistent/users/plasorak/clustering.tar"; echo "TARBALL " $TARBALL
LISTINPUTFILE="/pnfs/dune/persistent/users/plasorak/optical.list"; echo "LISTINPUTFILE " $LISTINPUTFILE
OUTPUTDIR="/pnfs/dune/persistent/users/plasorak/ClusteredPhotonMultiple"; echo "OUTPUTDIR " $OUTPUTDIR
EXECUTABLE="clustering/RunDAQClustering_PhotonOnly"; echo "EXECUTABLE " $EXECUTABLE
let "OFFSET = 1000 * ${PROCESS}" || true

echo "OFFSET " $OFFSET

ifdh cp -D $TARBALL ./
ifdh cp -D $LISTINPUTFILE ./
LISTINPUTFILELOCAL=$(basename -- "$LISTINPUTFILE")
FILE=`sed "${LINE}q;d" $LISTINPUTFILELOCAL`
echo "FILE " $FILE
ifdh cp -D  $FILE ./
INPUTFILE=$(basename -- "$FILE")

TARBALLFILENAME=$(basename -- "$TARBALL")
tar -xvf $TARBALLFILENAME
LIBRARYNAME="${TARBALLFILENAME%.*}"
LD_LIBRARY_PATH=`pwd`/$LIBRARYNAME:$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH

TREES[0]="snanaDEF45cm0100Hz5snrNonRefl"
TREES[1]="snanaEFF15cm0100Hz5snrNonRefl"
TREES[2]="snanaEFF30cm0100Hz5snrNonRefl"
TREES[3]="snanaEFF60cm0100Hz5snrNonRefl"
TREES[4]="snanaNSE45cm0010Hz5snrNonRefl"
TREES[5]="snanaNSE45cm0300Hz5snrNonRefl"
TREES[6]="snanaNSE45cm1000Hz5snrNonRefl"
TREES[7]="snanaREF45cm0100Hz5snrOptRefl"
TREES[8]="snanaREF45cm0100Hz5snrPesRefl"
TREES[9]="snanaSNR45cm0100Hz3snrNonRefl"
TREES[10]="snanaSNR45cm0100Hz4snrNonRefl"
TREES[11]="snanaSNR45cm0100Hz7snrNonRefl"

for TREE in ${TREES[@]}
do
    echo "STARTING TO PROCESS TREE " $TREE
    OUTPUTFILE=${INPUTFILE/.root/_${TREE}_Clustered.root}
    echo "OUTPUTFILE " $OUTPUTFILE
    COMMAND="$EXECUTABLE -i $INPUTFILE -t $TREE/SNSimTree -o $OUTPUTFILE -f $OFFSET"
    echo "COMMAND " $COMMAND
    $COMMAND
    ifdh cp -D $OUTPUTFILE $OUTPUTDIR
done

