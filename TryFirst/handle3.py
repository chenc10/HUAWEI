#from optparse import OptionParser
import os
#Parser = OptionParser()
#Parser.add_option("-r",dest="filename");
#(options,args)=Parser.parse_args()
#filename=options.filename
res=[]

num=0

f=open("Log/1.log",'r')
d=f.readlines()
print "a=[",
inum = 0
for i in range(len(d)):
    if d[i][0] <> 'c':
        continue
    inum = inum + 1
    res.append(0.0)
    tmp=d[i].split(' ')
    if inum == 4:
        continue
    print tmp[-1][:-1],
    print ',',
f.close()

print
for i in range(len(d)):
    if d[i][0] <> 'c':
        continue
    inum = inum + 1
    res.append(0.0)
    tmp=d[i].split(' ')
    print tmp[-2],
    print ',',
exit()
files=os.listdir("Log")
for filename in files:
    filename="Log/"+filename
    f=open(filename,'r')
    d=f.readlines()
    seq = 0
    for i in range(len(d)):
        if d[i][0] <> 'c':
            continue
        tmp = d[i].split(' ')
#       print tmp[-2], tmp[-1]
        res[seq] = res[seq] + float(tmp[-2])
        seq = seq + 1
    num = num+1
#   print "currently: ", num
    f.close()

for i in range(inum):
    res[i] = res[i]/num
for i in range(inum):
    if i == 3:
        continue
    print res[i],",",
