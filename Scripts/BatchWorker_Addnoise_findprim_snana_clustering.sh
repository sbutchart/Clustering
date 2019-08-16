#!/bin/bash

echo "Process Number" $PROCESS

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup ifdhc
setup dunetpc v08_11_00  -q e17:prof

set -e
let "LINE = ${PROCESS}+1"
echo "LINE " $LINE
TARBALL="/pnfs/dune/persistent/users/plasorak/clustering_trigprim_config.tar"; echo "TARBALL " $TARBALL
LARTARBALL="/pnfs/dune/persistent/users/plasorak/dunetpc.tar"; echo "LARTARBALL " $LARTARBALL

LISTINPUTFILE="/pnfs/dune/persistent/users/plasorak/background.list"; echo "LISTINPUTFILE " $LISTINPUTFILE
OUTPUTDIR="/pnfs/dune/persistent/users/plasorak/ClusteredTrigPrimMultiple"; echo "OUTPUTDIR " $OUTPUTDIR
OUTPUTDIRLAR="/pnfs/dune/persistent/users/plasorak/SNAna_MCC11"; echo "OUTPUTDIRLAR " $OUTPUTDIRLAR
EXECUTABLE="clustering/RunDAQClustering_WireOnly"; echo "EXECUTABLE " $EXECUTABLE
FCL="/pnfs/dune/persistent/users/plasorak/addnoise_findprim_snanas2.fcl"; echo "FCL " $FCL
let "OFFSET = 1000 * ${PROCESS}" || true

echo "OFFSET " $OFFSET

ifdh cp -D $TARBALL ./
ifdh cp -D $LISTINPUTFILE ./
ifdh cp -D $FCL ./

LISTINPUTFILELOCAL=$(basename -- "$LISTINPUTFILE")
FCLFILE=$(basename -- "$FCL")

FILE=`sed "${LINE}q;d" $LISTINPUTFILELOCAL`
echo "FILE " $FILE
ifdh cp -D  $FILE ./

INPUTFILE=$(basename -- "$FILE")

TARBALLFILENAME=$(basename -- "$TARBALL")
tar -xvf $TARBALLFILENAME
LIBRARYNAME="clustering"
LD_LIBRARY_PATH=`pwd`/$LIBRARYNAME:$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH

OUTPUTFILE1=${INPUTFILE/.root/_addnoise_trigprim_snana.root}
echo "OUTPUTFILE1 " $OUTPUTFILE1
COMMAND="lar -c ${FCLFILE} -s ${INPUTFILE} -T ${OUTPUTFILE1} -n 2"
echo $COMMAND
$COMMAND
ifdh cp -D $OUTPUTFILE1 $OUTPUTDIRLAR

TREES[0]="snanatrigprim1800"
TREES[1]="snanatrigprim2000"
TREES[2]="snanatrigprim2200"

for TREE in ${TREES[@]}
do
    echo "STARTING TO PROCESS TREE " $TREE
    OUTPUTFILE2=${OUTPUTFILE1/.root/_${TREE}_Clustered.root}
    COMMAND="$EXECUTABLE -i $OUTPUTFILE1 -t $TREE/SNSimTree -o $OUTPUTFILE2 -f $OFFSET"
    echo "COMMAND " $COMMAND
    $COMMAND
    ifdh cp -D $OUTPUTFILE2 $OUTPUTDIR
done
