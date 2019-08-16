#!/usr/bin/env python
import os
import subprocess
import glob
import ntpath
import signal
import sys

RunOver=10000
TreeName="SNSimTree"
TreeDir=["snanatrigprim2000"]

OutputDirectory="/dune/data/users/plasorak/nhit"
Config=" -c 0 "
Config=""
filename="list.txt"
f = open(filename, "r")

def file_len(fname):
    with open(fname) as f:
        for i, l in enumerate(f):
            pass
    return i + 1

divideBy =1
startFrom=0

if len(sys.argv)>1:
    if len(sys.argv)==3:
        divideBy =int(sys.argv[2])
        startFrom=int(sys.argv[1])
    else:
        print "You mus specify the how to divide the inut file and where you start"
        print "EXAMPLE: ./RunFromFileList.py 2 10"
        print "EXAMPLE: will divide the list in 10 and run the third portion of it"



Executable="/dune/app/users/plasorak/Clustering/build/clustering/RunDAQClustering"
NCompletedFile=0
Offset=0

oneStage = file_len(filename) / divideBy
startFile = oneStage*startFrom
endFile = oneStage*(1+startFrom)
print "Dividing the file in "+str(oneStage)
print "Starting from "+str(startFile)+" and ending in "+str(endFile)

exit

globalIT=0
for x in f:
    FileName=x.rstrip()
    for CurrentTreeDir in TreeDir:
        CurrentTree = CurrentTreeDir+"/"+TreeName
        FileNameOutput = "Clustered_"+CurrentTreeDir+"_"+ntpath.basename(FileName)
        Command = Executable+" -i "+FileName+" -t "+CurrentTree+" -o "+OutputDirectory+"/"+FileNameOutput+Config+" -f "+str(Offset)
        if globalIT>=startFile and globalIT<endFile:
            print(Command)
            subprocess.check_call(Command,shell=True)
            print("\n\n\n\n\n")
        
    NCompletedFile = NCompletedFile+1
    Offset=Offset+1000
    if NCompletedFile>=RunOver:
        break
    globalIT=globalIT+1


