from random import *
from Queue import Queue
AllNodeList = []
Adj_Matrix = [[]]
NodeNum = 300
class node:
    def __init__(self, ID):
        AllNodeList.append(self)
        self.ID = ID
        self.Type = 0
        self.AdjList = []
        self.Father = []
        self.Level = -1

    def add_adj(self, ID):
        self.AdjList.append(ID)

def SFENG(Nodes, mlinks, seed):
    Net = [[0 for col in range(Nodes)] for row in range(Nodes)]
    pos = len(seed)
    for i in range(pos):
        for j in range(pos):
            Net[i][j] = seed[i][j]
    sumlinks = 0
    for i in range(pos):
        sumlinks = sumlinks + sum(Net[i])
    while pos < Nodes:
        linkage = 0
        tmp = random()
        if tmp < 0.8:#override mlinks.
            mlinks = 1
        elif tmp < 0.95:
            mlinks = 2
        else:
            mlinks = 3
        #mlinks = 1
        while linkage <> mlinks:
            rnode = int(random()*pos)
            deg = sum(Net[rnode]) * 2
            rlink = random() 
            if rlink < float(deg) / sumlinks and Net[pos][rnode] <> 1 and Net[rnode][pos] <> 1:
                Net[pos][rnode] = 1
                Net[rnode][pos] = 1
                linkage = linkage + 1
                sumlinks = sumlinks + 2
        pos = pos + 1
    return Net

def visit(c):
    for n in AllNodeList[c].Father:
        LW[c][n] = LW[c][n] + 1
        LW[n][c] = LW[n][c] + 1
        visit(n)
def handle(S):
    Q = Queue(-1)
    for i in range(len(AllNodeList)):
        AllNodeList[i].Type = 0
        AllNodeList[i].Level = -1
        AllNodeList[i].Father = []
    AllNodeList[S].Level = 0
    Q.put(S)
    FinalNodes = []
    OldLevel = -1
    while not Q.empty():
        c = Q.get()
        if AllNodeList[c].Level <> OldLevel:
            FinalNodes = [c]
            OldLevel = AllNodeList[c].Level
        else:
            FinalNodes.append(c)
        AllNodeList[c].Type = 2
        for n in AllNodeList[c].AdjList:
            if AllNodeList[n].Type == 0:
                Q.put(n)
                AllNodeList[n].Type = 1
                AllNodeList[n].Level = AllNodeList[c].Level + 1
                AllNodeList[n].Father.append(c)
            elif AllNodeList[n].Type == 1:
                if AllNodeList[n].Level == AllNodeList[c].Level + 1:
                    AllNodeList[n].Father.append(c)

    for n in FinalNodes:
        visit(n)
            
if __name__ == "__main__":
    seed(200)
    myseed =[[0,1,0,0,1],[1,0,0,1,0],[0,0,0,1,0],[0,1,1,0,0],[1,0,0,0,0]] 
    #myseed =[[0,1,0,0,1],[1,0,0,1,0],[0,0,0,1,0],[0,1,1,0,0],[1,0,0,0,0]] 
    Adj_Matrix = SFENG(NodeNum, -1, myseed)
#    for i in range(NodeNum):
#        for j in range(NodeNum):
#            print Adj_Matrix[i][j],
#        print 
    LW = [[0 for i in range(NodeNum)] for j in range(NodeNum)]
    #myprint_matrix(Adj_Matrix)
    
    for i in range(NodeNum):
        CurrentNode = node(i)
        for j in range(NodeNum):
            if Adj_Matrix[i][j]:
                CurrentNode.add_adj(j)
    print len(AllNodeList)
    for i in range(NodeNum):
        handle(i)
    #handle(0)
#    for i in range(NodeNum):
#        for j in range(NodeNum):
#            print LW[i][j],
#        print 
    for i in range(NodeNum):
        print i,sum(LW[i]), len(AllNodeList[i].AdjList)

