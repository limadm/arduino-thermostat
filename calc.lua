c = 2e-6
r = 100
v = 110

z = 1/(math.pi*2*60*c) + r
i = v/z
p = i*15
io.write(("supply: %.2fµF\t%dV\t%.3fA\t%.2fW\n"):format(c*1e6, v, i, p))

r1 = 10e3
r0 = 9.5e3
t0 = 273.15 + 25
b = 3.95e3
v = 2.945
v = math.floor(v / 5 * 1023)
r = r1 * v / (1023 - v)
t = 1/(1/t0 + math.log(r/r0)/b) - 273.15
io.write(("sensor: %d\t%.1fkOhm\t%.1f°C\n"):format(v,r/1e3,t))
