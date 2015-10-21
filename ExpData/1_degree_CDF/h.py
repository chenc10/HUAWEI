m=dict()
f=open("data",'r')
d=f.readlines()
for i in range(len(d)):
    tmp=d[i].split('+')
    a=int(tmp[1])
    if not m.has_key(a):
        m[a]=1
    else:
        m[a] = m[a] + 1

r = sorted(m.items())
s = 0
k = []
for i in range(len(r)):
    s = s + r[i][1]
    k.append([r[i][0],s])
#    r[i][1] = s

for i in range(len(r)):
    k[i][1] = float(k[i][1])/k[-1][1]
x = 0
y = 0.0

resultx=[]
resulty=[]
for i in range(len(r)):
    if k[i][0] > x * 5 or k[i][1] > y + 0.1:
        x = k[i][0]
        y = k[i][1]
        resultx.append(k[i][0])
        resulty.append(k[i][1])
    
print resultx
print resulty
exit
res = [ v for v in sorted(m.values())] 
s = 0
print len(res)
print 
