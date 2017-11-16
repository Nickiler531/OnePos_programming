#!/usr/bin/env python
#<examples/doc_model1.py>
import numpy as npy
from lmfit import  Model
from lmfit import minimize, Parameters
import matplotlib.pyplot as plt

x = [4.54, 3.62, 0, 2.25]
y = [1.38, 0, 0, 2.85]
# d = [2.955532994924,2.32192893401,2.583147208122]
d = [2.95,2.26,2.56,2.95]

# TEST POINTS
# x = [-1.91373, -0.935453, 0.630964, 0.310857,0.0431084]
# y = [-0.799904, -0.493735, -0.653075, -0.018258, 1.25321]
# d = [2.04001, 0.959304, 0.728477, 0.301885, 1.19012]

# x = [0, 0, 1, 1]
# y = [0, 1, 1, 0] 
# d = [0.5, 0.5, 0.5, 0.5]
#d=[190,181,357,190]


eps = 1/npy.power(d,2)
print eps


def residuals(params,x,y,data,eps_data):
	x0 = params['x0']
	y0 = params['y0']

	model = npy.linalg.norm([x-x0, y-y0])
	return (data-model)/eps_data

params = Parameters()
params.add('x0',value=0)
params.add('y0',value=0)

out = minimize(residuals,params,'leastsq',args=(x,y,d,eps))
print out.params['x0'].value
print out.params['y0'].value
print out.success




def norm_funct(x, y, x0, y0):
	#return npy.linalg.norm([x-x0,y - y0])
	return npy.sqrt(npy.power(x-x0,2)+npy.power(y-y0,2))
gmodel = Model(norm_funct, independent_vars =['x', 'y'])
result = gmodel.fit(d,x=x,y=y,x0=0,y0=0)

print(result.fit_report())

print result.params


fig, ax = plt.subplots(1,1)

for xi,yi,di in zip(x,y,d):
	node_point = plt.Circle((xi, yi), 0.05, color='r')
	node_distance = node1 = plt.Circle((xi, yi), di, edgecolor='r',alpha=0.1)
	ax.add_artist(node_point)
	ax.add_artist(node_distance)


point = plt.Circle((out.params['x0'].value, out.params['y0'].value), 0.05, color='b')
ax.add_artist(point)

point2 = plt.Circle((result.params['x0'].value, result.params['y0'].value), 0.05, color='g')
ax.add_artist(point2)

real = plt.Circle((2.12,1.15), 0.05, color='r')
ax.add_artist(real)


ax.plot([-2, 5], [-2, 5])

plt.show()

# x = data[:, 0]
# y = data[:, 1]

# def gaussian(x, amp, cen, wid):
#     "1-d gaussian: gaussian(x, amp, cen, wid)"
#     return (amp/(sqrt(2*pi)*wid)) * exp(-(x-cen)**2 /(2*wid**2))

# gmodel = Model(gaussian)
# result = gmodel.fit(y, x=x, amp=5, cen=5, wid=1)

# print(result.fit_report())

# plt.plot(x, y,         'bo')
# plt.plot(x, result.init_fit, 'k--')
# plt.plot(x, result.best_fit, 'r-')
# plt.show()
# #<end examples/doc_model1.py>