import matplotlib.pyplot as plt
import sys

if (len(sys.argv) == 4):
    	gs_file = sys.argv[1]
    	tour_file = sys.argv[2]
    	percp_file = sys.argv[3]
else:
	print "wrong parameter number!"
	sys.exit()

def read_data(file):
	ps = []
	with open(file, "r") as f:
		line = f.readline()
		line = line.strip().split(' ')
		for i in range(0, len(line)):
			ps.append(float(line[i]))
		print file, len(line)
	return ps

gs = read_data(gs_file)
tour = read_data(tour_file)
percp = read_data(percp_file)
ls = list(range(0, len(gs)))
plt.figure()
plt.plot(ls, gs, 'r-', label='G-Share')
plt.plot(ls, tour, 'g-', label='Tournament')
plt.plot(ls, percp, 'b-', label='Perceptron')
plt.xscale('log')
plt.title('Training Curve of Various Predictors on Trace fp_1')
plt.xlabel('Round (*1000)')
plt.ylabel('Misprediction Rate')
plt.legend()
plt.show()
