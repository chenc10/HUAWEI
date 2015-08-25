from optparse import OptionParser 
from random import * 
from Queue import Queue
from copy import deepcopy
#import numpy as np

IsDebug = True 
GraphEnabled = True 

AllNodeList = []
Adj_Matrix = [[]]
SDNList = []
NodeNum = 10
IsChanged = False
class path:
    def __init__(self, From):
        self.Validity = False
        self.Path = [From] 

class node:
    def __init__(self, ID):
        AllNodeList.append(self)
        self.ID = ID
        self.AdjList = []
        self.OldPathVector = []
        self.NewPathVector = []
        self.SDN = []
        for i in range(NodeNum):
            self.NewPathVector.append(path(ID))
        
    def add_adj(self, ID):
        self.AdjList.append(ID) 
        if self.NewPathVector[ID].Validity == False:
            self.NewPathVector[ID].Validity = True
            self.NewPathVector[ID].Path.append(ID)
        else:
            self.NewPathVector[ID].Path = [self.ID,ID]
        #myprint_path(0) 

    def bgp_update(self):
        global IsChanged
        for i in range(NodeNum):
            ChangedInThisLoop = False
            if i in self.AdjList:#avoid handling direct neighbor, waste time
                continue
            if self.OldPathVector[i].Validity == False:
                TmpPathLength = float('inf')
            else:
                TmpPathLength = len(self.OldPathVector[i].Path)
            for Adj in self.AdjList:
                if AllNodeList[Adj].OldPathVector[i].Validity == False:
                    continue
                if self.ID in AllNodeList[Adj].OldPathVector[i].Path:#avoid loop 
                    continue 
                if len(AllNodeList[Adj].OldPathVector[i].Path) + 1 < TmpPathLength:
                    TmpPathLength = len(AllNodeList[Adj].OldPathVector[i].Path)+1
                    MarkedID = Adj
                    ChangedInThisLoop = True 
            if ChangedInThisLoop == True: 
                    self.NewPathVector[i].Validity = True
                    self.NewPathVector[i].Path = deepcopy(AllNodeList[MarkedID].OldPathVector[i].Path)
                    self.NewPathVector[i].Path.insert(0,self.ID)
                    IsChanged = True

class SDN:
    def __init__(self, SdnID, SdnSize):
        if SdnSize == 0:
            return
        self.ID = SdnID
        self.Size = 0
        self.AsList = []
        SDNList.append(self)
        self.create(SdnSize)
    def create(self, SdnSize): #keep adding new ASes
        MyQueue = Queue(-1)
        StartNodeID = int(random()*NodeNum)
        print "# R# StartID: ", StartNodeID
        self.add_AS(StartNodeID)
        MyQueue.put(StartNodeID)
        while self.Size <> SdnSize: 
            CurrentNodeID = MyQueue.get() 
            for AdjNodeID in AllNodeList[CurrentNodeID].AdjList:
                if self.ID in AllNodeList[AdjNodeID].SDN:
                    continue
                self.add_AS(AdjNodeID)
                MyQueue.put(AdjNodeID)
                if self.Size == SdnSize:
                    break
    def add_AS(self, AsID):
        self.Size = self.Size + 1
        self.AsList.append(AsID)
        AllNodeList[AsID].SDN.append(self.ID)
    def local_converge(self):
        global IsChanged
        TmpIsChanged = IsChanged
        while True:
            #myprint_path()
            IsChanged = False
            for i in self.AsList:
                AllNodeList[i].OldPathVector = deepcopy(AllNodeList[i].NewPathVector)
            for i in self.AsList:
                AllNodeList[i].bgp_update()
            if IsChanged == False:
                break
        IsChanged = TmpIsChanged #recover info about whether modified in normal update (non-SDN), SDN converge cannot change IsChanged
        #myprint_path()
        
def myprint_matrix(M):
    if not IsDebug:
        return
    for i in range(len(M)):
        print '(%d) ' %i,
        for j in range(len(M[i])):
            print M[i][j],
        print ''
        
def myprint_path(ID=-1):
    if not IsDebug:
        return
    if not GraphEnabled:
        return 
    print ''
    print 'begin to print path:'
    if ID == -1:
        print '============================'
        for ID in range(NodeNum):
            print '%d ------' %ID
            for i in range(len(AllNodeList[ID].NewPathVector)):
                print '(%d): ' %i,
                if AllNodeList[ID].NewPathVector[i].Validity == False:
                    print "Invalidate"
                else:
                    for j in range(len(AllNodeList[ID].NewPathVector[i].Path)):
                        print '%3d ' %AllNodeList[ID].NewPathVector[i].Path[j],
                    print ''
    else:
        print "below print the path in node %d :" % ID
        for i in range(len(AllNodeList[ID].NewPathVector)):
            print '(%d): ' %i,
            if AllNodeList[ID].NewPathVector[i].Validity == False:
                print "Invalidate"
                for j in range(len(AllNodeList[ID].NewPathVector[i].Path)):
                    print '%3d ' %AllNodeList[ID].NewPathVector[i].Path[j],
                print ''

def myprint_graph(adjacency_matrix, name):
    if not IsDebug:
        return
    if not GraphEnabled:
        return 
    import networkx as nx
    import matplotlib.pyplot as plt

    adjacency_matrix = np.array(adjacency_matrix)
    rows, cols = np.where(adjacency_matrix == 1)
    edges = zip(rows.tolist(), cols.tolist())
    gr = nx.Graph()
    gr.add_edges_from(edges)
    # nx.draw(gr) # edited to include labels
    nx.draw_networkx(gr)
    # now if you decide you don't want labels because your graph
    # is too busy just do: nx.draw_networkx(G,with_labels=False)
    #plt.show()
    plt.savefig('topo_'+name+'.pdf')

def myprint_graphfile(adjacency_matrix, name):
    if not IsDebug:
        return
    if not GraphEnabled:
        return
      

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
        if mlinks == -1:
            tmp = random()
            if tmp < 0.6:#override mlinks.
                mlinks = 1
            elif tmp < 0.9:
                mlinks = 2
            else:
                mlinks = 3
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

def converge_process():
    global IsChanged
    CurrentNodeNum = len(AllNodeList)
    LoopTimes = 0
    while True:
        #myprint_path()
        IsChanged = False
        for i in range(CurrentNodeNum):
            AllNodeList[i].OldPathVector = deepcopy(AllNodeList[i].NewPathVector)
        for i in range(CurrentNodeNum):
            AllNodeList[i].bgp_update()
        for sdn in SDNList:
            sdn.local_converge()
        LoopTimes = LoopTimes + 1
        if IsChanged == False:
            break
    myprint_path()
    return LoopTimes

def inject_up(i):
    # currently havenot consider the locality of the up link, ID1 and ID2 may not be close to each other
    CurrentNode = node(i)
    for j in range(i):
        if Adj_Matrix[i][j]:
            CurrentNode.add_adj(j)
            AllNodeList[j].add_adj(i)
            print  "#   R# link up info injected", i, j 
            break


if __name__ == "__main__":
    Parser = OptionParser()
    Parser.add_option("-n","--nodenum",dest="NodeNum",default=20)
    Parser.add_option("-r","--sdnratio",dest="SdnRatio",default=20)#The unit of SdnRatio is percent
    Parser.add_option("-s","--seed",dest="Seed",default=0)#The unit of SdnRatio is percent
    (options, args) = Parser.parse_args()
    NodeNum = int(options.NodeNum)
    SdnRatio = float(options.SdnRatio)/100.0

    seed(int(options.Seed))
    
    myseed =[[0,1,0,0,1],[1,0,0,1,0],[0,0,0,1,0],[0,1,1,0,0],[1,0,0,0,0]] 
    Adj_Matrix = SFENG(NodeNum, -1, myseed)
    myprint_matrix(Adj_Matrix)
    myprint_graph(Adj_Matrix,'0')
    
    for i in range(NodeNum-10):
        CurrentNode = node(i)
        for j in range(NodeNum-10):
            if Adj_Matrix[i][j]:
                CurrentNode.add_adj(j)
    #print AllNodeList
    print "initial converge time: # %d" % converge_process()
    SDN(0,int(NodeNum*SdnRatio))
    for i in range(10):
        inject_up(NodeNum - 10 + i)
        print "update converge time: # %d" %converge_process()
        print "# Update #%d over" %i


