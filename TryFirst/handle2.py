#from optparse import OptionParser
import os
#Parser = OptionParser()
#Parser.add_option("-r",dest="filename");
#(options,args)=Parser.parse_args()
#filename=options.filename
res=[]

num=0
chosen = 36 

for i in range(9):
    res.append(0)

files=os.listdir("Log")
for filename in files:
#    tmp = filename[:-4]
#    if int(tmp) > 2:
#        continue
    filename="Log/"+filename
    f=open(filename,'r')
    d=f.readlines()
    seq = 0
    for i in range(len(d)):
        tmp = d[i].split(' ')
        if tmp[0][0] <> '\t':
            continue
        if int(tmp[0][1:]) <> chosen:
            continue
#       print tmp[-2], tmp[-1]
        seq = seq + 1
        res[seq] = res[seq] + float(tmp[2])
    seq = seq + 1
    while seq < 9:
        res[seq] = res[seq] + 100.0
        seq = seq + 1
    num = num+1
#    print "currently: ", num
    f.close()

#f=open("Log/1.log",'r')
#d=f.readlines()
#print "a=[",
#for i in range(len(d)):
#    if i == 0:
#        continue
#    tmp=d[i].split(' ')
#    print tmp[-1][:-1],
#    if i < 12:
#        print ',',
#f.close()
#
#print "]" 

for i in range(9):
    res[i] = res[i]/num
for i in range(9):
    print res[i],",",

