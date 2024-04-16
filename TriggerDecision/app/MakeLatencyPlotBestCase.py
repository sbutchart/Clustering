import numpy
from root_pandas import read_root
from pandas import DataFrame
from matplotlib import pyplot as plt

data = read_root('LikelihoodTrigger.root', 'mapping')
data_config = data.query('Config==0')

data_latency_2 = []
data_latency_4 = []
data_latency_10 = []

binning = numpy.linspace(0,10,100)
time_pdf, _ = numpy.histogram(data_config.Time, bins=binning)
time_pdf = time_pdf / numpy.sum(time_pdf)
print(len(data_config.Time))
# print (len(time_pdf))
# print (len(binning))
binning = numpy.linspace(0,10,99)
print (numpy.sum(time_pdf))# plt.bar(x=binning,height=time_pdf, width=0.1)
# plt.show()
for i in range(100000):
    nevent = numpy.random.poisson(2,1)
    times = []
    times.clear()
    for ievent in range(nevent[0]):
        times.append(numpy.random.choice(time_pdf,1)[0])

    if len(times)>=2:
        times.sort()
        data_latency_2.append(times[1])

    if len(times)<2:
        data_latency_2.append(-0.1)

    nevent = numpy.random.poisson(4,1)
    times.clear()
    for ievent in range(nevent[0]):
        times.append(numpy.random.choice(time_pdf,1)[0])

    if len(times)>=2:
        times.sort()
        data_latency_4.append(times[1])

    if len(times)<2:
        data_latency_4.append(-0.1)
        
    nevent = numpy.random.poisson(10,1)
    times.clear()
    for ievent in range(nevent[0]):
        times.append(numpy.random.choice(time_pdf,1)[0])

    if len(times)>=2:
        times.sort()
        data_latency_10.append(times[1])

    if len(times)<2:
        data_latency_10.append(-0.1)

plt.hist(data_latency_2,  histtype='step', fill=False, color='red', bins=100, density=True, label='2 events')
plt.hist(data_latency_4,  histtype='step', fill=False, color='blue', bins=100, density=True, label='4 events')
plt.hist(data_latency_10, histtype='step', fill=False, color='green', bins=100, density=True, label='10 events')
plt.xlabel('Latency [s]')
plt.show()
