import matplotlib.pyplot as plt
import sys

if (len(sys.argv) == 2):
    file = sys.argv[1]
else:
	print "wrong parameter number!"
	sys.exit()

with open(file, "r") as f:
	line = f.readline()
	line = line.strip().split(' ')
	for elem in line:
		elem = float(elem)
	print len(line)
	print line

ls = list(range(0, len(line)))

plt.plot(ls, line)
plt.show()