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

InputDirectory="/dune/data2/users/plasorak/PhotonProdCluster/"
OutputDirectory="/dune/data2/users/plasorak/PhotonClusterHadd/"
Executable="hadd -f "

for TreeName in TreeDir:
    #print(Executable+OutputDirectory+TreeName+".root `ls "+InputDirectory+"Clustered_"+TreeName+"*.root`")
    os.system(Executable+OutputDirectory+TreeName+".root `ls "+InputDirectory+"Clustered_"+TreeName+"*.root`")
    #os.system("ls "+InputDirectory+"Clustered_"+TreeName+"*.root")
