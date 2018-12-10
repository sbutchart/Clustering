#!/usr/bin/env python
import os,sys
import glob, os
import ntpath

RunOver=10000
TreeName="SNSimTree"
TreeDir=["snanaDEF45cm0100Hz5snrNonRefl",
         "snanaEFF15cm0100Hz5snrNonRefl",
         "snanaEFF30cm0100Hz5snrNonRefl",
         "snanaEFF60cm0100Hz5snrNonRefl",
         "snanaNSE45cm0010Hz5snrNonRefl",
         "snanaNSE45cm0300Hz5snrNonRefl",
         "snanaNSE45cm1000Hz5snrNonRefl",
         "snanaREF45cm0100Hz5snrOptRefl",
         "snanaREF45cm0100Hz5snrPesRefl",
         "snanaSNR45cm0100Hz3snrNonRefl",
         "snanaSNR45cm0100Hz4snrNonRefl",
         "snanaSNR45cm0100Hz7snrNonRefl"]

InputDirectory="/pnfs/dune/scratch/dunepro/mcc11/snb_nov2018/hist/mcc11_pd_tdr/"
OutputDirectory="/dune/data2/users/plasorak/PhotonProdCluster/"
FilePatern="mcc11_pd_tdr_dra_hist-*.root"
Config=" -c 0 "

Executable="/dune/app/users/plasorak/Clustering/build/clustering/RunDAQClustering"
NCompletedFile=0
AllTheFile=glob.glob(InputDirectory+FilePatern)
for FileName in AllTheFile:
    print(str(NCompletedFile)+" / "+str(len(AllTheFile)))
    for CurrentTreeDir in TreeDir:
        CurrentTree = CurrentTreeDir+"/"+TreeName
        FileNameOutput = "Clustered_"+CurrentTreeDir+"_"+ntpath.basename(FileName)
        #my_file = Path(OutputDirectory+FileNameOutput)
        if not os.path.isfile(OutputDirectory+FileNameOutput):
            print(Executable+" -i "+FileName+" -t "+CurrentTree+" -o "+OutputDirectory+FileNameOutput+Config)
            os.system(Executable+" -i "+FileName+" -t "+CurrentTree+" -o "+OutputDirectory+FileNameOutput+Config)

    NCompletedFile = NCompletedFile+1

    if NCompletedFile>=RunOver:
        break
    

