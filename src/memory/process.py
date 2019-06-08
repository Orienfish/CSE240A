from __future__ import division
import matplotlib.pyplot as plt
import sys

def read_data(file):
	total_brch = 0
	total_mis = 0
	with open(file, "r") as f:
		line = f.readline()
		elem = line.strip().split(':')[1]
		branch = int(elem.strip())
		line = f.readline()
		elem = line.strip().split(':')[1]
		mis = int(elem.strip())
		f.readline()
		print branch, mis
		total_brch += branch
		total_mis += mis
	misp_rate = total_mis / total_brch
	print misp_rate
	return misp_rate

predictor = ['gshare', 'tour', 'perp']
space = [2, 4, 8, 16, 32, 64]
res = dict()

for p in predictor:
	res[p] = []
	for i in range(0, len(space)):
		print p, space[i]
		res[p].append(read_data(p + "_" + str(space[i]) + ".txt"))

print res

plt.figure()
plt.plot(space, res['gshare'], 'r-*', label='G-Share')
plt.plot(space, res['tour'], 'g-v', label='Tournament')
plt.plot(space, res['perp'], 'b-o', label='Perceptron')
plt.title('Misprediction Rate Under Various Memory Contraints')
plt.xlabel('Memory (kbit)')
plt.ylabel('Misprediction Rate')
#plt.xscale('log')
#plt.yscale('log')
plt.legend()
plt.show()