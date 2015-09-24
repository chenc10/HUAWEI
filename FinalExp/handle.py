#from optparse import OptionParser
import os
#Parser = OptionParser()
#Parser.add_option("-r",dest="filename");
#(options,args)=Parser.parse_args()
#filename=options.filename
res=[]
for i in range(12):
    res.append(0.0)

num=0

files=os.listdir("Log")
for filename in files:
    filename="Log/"+filename
    f=open(filename,'r')
    d=f.readlines()
    for i in range(len(d)):
        if i==0:
            continue
        tmp = d[i].split(' ')
#       print tmp[-2], tmp[-1]
        res[i-1] = res[i-1] + float(tmp[-2])
    num = num+1
    print "currently: ", num
    
    f.close()

f=open("Log/1.log",'r')
d=f.readlines()
print "a=[",
for i in range(len(d)):
    if i == 0:
        continue
    tmp=d[i].split(' ')
    print tmp[-1][:-1],
    if i < 12:
        print ',',
f.close()

print "]" 

for i in range(12):
    res[i] = res[i]/num
for i in range(12):
    print res[i],",",


