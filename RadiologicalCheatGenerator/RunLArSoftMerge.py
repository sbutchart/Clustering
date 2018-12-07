import glob, os, ntpath

InputDirectory="/dune/app/users/plasorak/Clustering/build/Argon_39/"
FilePatern="*.fcl"
AllTheFile=glob.glob(InputDirectory+FilePatern)
OutputDirectory="/dune/data2/users/plasorak/RadioEvents/"

for FileName in AllTheFile:
    FileNameOutput = ntpath.basename(FileName)
    os.system("lar -c "+FileName+" -o"+OutputDirectory+"/"+FileNameOutput+" -n 1")
