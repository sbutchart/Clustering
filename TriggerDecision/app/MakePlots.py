from matplotlib import pyplot as plt
import uproot

file =uproot.open('LMC.root')
tree = file['Throws']
back = tree.array('BackgroundStatistic')
_ = plt.hist(back, bins=100)


