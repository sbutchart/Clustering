#!/usr/bin/env python
import os
import subprocess
import glob
import ntpath
import signal

RunOver=10000
TreeName="SNSimTree"
TreeDir=["snanatrigprim2000"]

OutputDirectory="/dune/data/users/plasorak/"
Config=" -c 0 "
f = open("list.txt", "r")

Executable="/dune/app/users/plasorak/Clustering/build/clustering/RunDAQClustering"
NCompletedFile=0
Offset=0

for x in f:
    FileName=x.rstrip()
    for CurrentTreeDir in TreeDir:
        CurrentTree = CurrentTreeDir+"/"+TreeName
        FileNameOutput = "Clustered_"+CurrentTreeDir+"_"+ntpath.basename(FileName)
        #my_file = Path(OutputDirectory+FileNameOutput)
        Command = Executable+" -i "+FileName+" -t "+CurrentTree+" -o "+OutputDirectory+"/"+FileNameOutput+Config+" -f "+str(Offset)
        print(Command)
        subprocess.check_call(Command,shell=True)
        print("\n\n\n\n\n")
        
    NCompletedFile = NCompletedFile+1
    Offset=Offset+1000

    if NCompletedFile>=RunOver:
        break
    

