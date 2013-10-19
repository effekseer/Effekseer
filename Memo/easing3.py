# Easing
# 始点 ( 0.0, 0.0 )
# 終点 ( 1.0, 1.0 )

# 傾き 45度が角度0
# 始点から ( 1.0, 0.0 ) への角度をマイナス45度
# 終点から ( 0.0, 1.0 ) への角度をマイナス45度
# とする。

# 始点の角度をa1、終点の角度をa2とする。

# 始点の傾き
# g1 = tan(a1+45)

# 終点の傾き
# g2 = tan(a2+45)

# y = a * t^3 + b * t^2 + c * t^1 + d

# 始点終点の位置より
# 0 = d
# 1 = a + b + c + d

# 傾きより
# y' = a * 3 * t^2 + b * 2 * t + c
# g1 = c
# g2 = a * 3 + b * 2 + c

# d = 0
# c = g1
# a = g2 - g1 - ( 1 - c ) * 2
# b = (g2 - g1 - (a * 3)) / 2

import math

a1 = 0.0
a2 = 0.0

g1 = math.tan( (a1 + 45.0) / 180.0 * 3.1415 )
g2 = math.tan( (a2 + 45.0) / 180.0 * 3.1415 )

c = g1
a = g2 - g1 - ( 1.0 - c ) * 2.0
b = (g2 - g1  - ( a * 3.0 )) / 2.0

def get_value(t):
	return a * t * t * t + b * t * t + c * t


f = open('easing.csv', 'w')
f.write(str(g1) + ',' + str(g2) + ',' + str(a) + ',' + str(b)+ ',' + str(c) + '\n')
for x in xrange( int(0), int(11) ):
	value0 = float(x) / 10.0
	value1 = get_value(value0)
	f.write(str(value0) + ',' + str(value1) + '\n')

f.close()