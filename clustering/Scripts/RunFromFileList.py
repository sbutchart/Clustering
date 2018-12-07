#!/usr/bin/env python
import os,sys
import glob, os
import ntpath

RunOver=10000
TreeName="SNSimTree"
TreeDir=["snanatrigprim2000"]

OutputDirectory="/dune/data2/users/plasorak/PhilHitPrim/"
Config=" -c 0 "
f = open("/dune/app/users/rodriges/run-snana-on-grid/snana-output-files-defaultnoise.list", "r")

Executable="/dune/app/users/plasorak/Clustering/build/clustering/RunDAQClustering"
NCompletedFile=0
Offset=0
for x in f:
    FileName=x.rstrip()
    for CurrentTreeDir in TreeDir:
        CurrentTree = CurrentTreeDir+"/"+TreeName
        FileNameOutput = "Clustered_"+CurrentTreeDir+"_"+ntpath.basename(FileName)
        #my_file = Path(OutputDirectory+FileNameOutput)
        print(Executable+" -i "+FileName+" -t "+CurrentTree+" -o "+OutputDirectory+FileNameOutput+Config+" -f "+str(Offset))
        os.system(Executable+" -i "+FileName+" -t "+CurrentTree+" -o "+OutputDirectory+FileNameOutput+Config+" -f "+str(Offset))
        
    NCompletedFile = NCompletedFile+1
    Offset=Offset+1000

    if NCompletedFile>=RunOver:
        break
    

