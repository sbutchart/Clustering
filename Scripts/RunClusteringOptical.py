#!/usr/bin/env python
import os
import glob
import subprocess
import ntpath
import sys
RunOver=10000
TreeName="SNSimTree"
TreeDir=["snanaDEF35QENonRefl1PE",
         "snanaEFF15QENonRefl1PE",
         "snanaEFF25QENonRefl1PE",
         "snanaEFF45QENonRefl1PE",
         "snanaREF35QEOptRefl1PE",
         "snanaREF35QEPesRefl1PE",
         "snanaTHR35QENonRefl2PE",
         "snanaTHR35QENonRefl3PE"]
# TreeDir=["snanaDEF45cm0100Hz5snrNonRefl",
#          "snanaEFF15cm0100Hz5snrNonRefl",
#          "snanaEFF30cm0100Hz5snrNonRefl",
#          "snanaEFF60cm0100Hz5snrNonRefl",
#          "snanaNSE45cm0010Hz5snrNonRefl",
#          "snanaNSE45cm0300Hz5snrNonRefl",
#          "snanaNSE45cm1000Hz5snrNonRefl",
#          "snanaREF45cm0100Hz5snrOptRefl",
#          "snanaREF45cm0100Hz5snrPesRefl",
#          "snanaSNR45cm0100Hz3snrNonRefl",
#          "snanaSNR45cm0100Hz4snrNonRefl",
#          "snanaSNR45cm0100Hz7snrNonRefl"]
# TreeDir=["snanaDEF45cm0100Hz5snrNonRefl",
#          "snanaEFF15cm0100Hz5snrNonRefl",
#          "snanaEFF30cm0100Hz5snrNonRefl",
#          "snanaEFF60cm0100Hz5snrNonRefl",
#          "snanaREF45cm0100Hz5snrOptRefl",
#          "snanaREF45cm0100Hz5snrPesRefl"]

Config=" -c 0"
Config=""
filename="optical.list3"
f = open(filename, "r")
OutputDirectory="/dune/data2/users/plasorak/PhotonProdClusterMultipleConfig_March19/"

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

Executable="/dune/app/users/plasorak/Clustering/build/clustering/RunDAQClustering_PhotonOnly"
NCompletedFile=0
Offset=0

total = file_len(filename)
oneStage = file_len(filename) / divideBy
startFile = oneStage*startFrom
endFile = oneStage*(1+startFrom)
print "Dividing the file in "+str(oneStage)
print "Starting from "+str(startFile)+" and ending in "+str(endFile)

globalIT=0
for x in f:
    FileName=x.rstrip()
    print(str(NCompletedFile)+" / "+str(total))
    for CurrentTreeDir in TreeDir:
        CurrentTree = CurrentTreeDir+"/"+TreeName
        FileNameOutput = "Clustered_"+CurrentTreeDir+"_"+ntpath.basename(FileName)
        #my_file = Path(OutputDirectory+FileNameOutput)
        Command=Executable+" -i "+FileName+" -t "+CurrentTree+" -o "+OutputDirectory+"/"+FileNameOutput+Config+" -f "+str(Offset)
        if globalIT>=startFile and globalIT<endFile:
            print(Command)
            subprocess.check_call(Command, shell=True)
    print("\n\n\n\n\n")
    NCompletedFile = NCompletedFile+1
    Offset=Offset+1000
    if NCompletedFile>=RunOver:
        break
    globalIT=globalIT+1
