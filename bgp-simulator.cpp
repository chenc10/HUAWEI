#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<vector>
#include<list>
#include<string>
#include<algorithm>

using namespace std;
typedef unsigned short u_s;

//#define DEBUG

class node;
class SDN;

u_s NodeNum = 0;
u_s ValidNodeNum = 0;
bool IsChanged = 0;
u_s RepeatTimes = -1;
int RandSeed = 0;
float SDNRatio = -0.1;


SDN * MySDN;
vector<node *> AllNodeVector;
bool ** AdjMatrix;

void myprint_path();
u_s converge_process(bool sign);
void SFENG(bool ** AdjMatrix, bool MySeed[5][5]);
void inject_up(u_s );

class path{
	public:
		bool Validity;
		list<u_s> Path;
		path(u_s From){
			Validity = 0;
			Path.push_back(From);
		}
};	
class node{
	public:
		u_s ID;
		list<u_s> AdjList;
		vector<path> OldPathVector;
		vector<path> NewPathVector;
		bool IsInSDN;
		node(u_s myID): OldPathVector(NodeNum, path(myID)), NewPathVector(NodeNum, path(myID)){
			this->ID = myID;
			this->IsInSDN = 0;
		}
		void add_adj(u_s myID){
			AdjList.push_back(myID);
			if(!NewPathVector[myID].Validity){
				NewPathVector[myID].Validity = 1;
				NewPathVector[myID].Path.push_back(myID);
			}
			else{
				NewPathVector[myID].Path.clear();
				NewPathVector[myID].Path.push_back(ID);
				NewPathVector[myID].Path.push_back(myID);
			}
		}
		void bgp_update(){
			bool ChangedInThisLoop;
			u_s TmpPathLength;
			u_s MarkedID;
			for( u_s i = 0; i < ValidNodeNum; i ++){
				ChangedInThisLoop = 0;
				if(find(AdjList.begin(), AdjList.end(), i) != AdjList.end())	
						continue;
				if(!OldPathVector[i].Validity)
						TmpPathLength = NodeNum + NodeNum;
				else
						TmpPathLength = OldPathVector[i].Path.size();
				for(list<u_s>::iterator Adj=AdjList.begin(); Adj!=AdjList.end(); Adj++){
						if(!AllNodeVector[*Adj]->OldPathVector[i].Validity)
								continue;
						if(find(AllNodeVector[*Adj]->OldPathVector[i].Path.begin(), AllNodeVector[*Adj]->OldPathVector[i].Path.end(),ID) != AllNodeVector[*Adj]->OldPathVector[i].Path.end())
								continue;
						if( AllNodeVector[*Adj]->OldPathVector[i].Path.size() + 1 < TmpPathLength){
								TmpPathLength = AllNodeVector[*Adj]->OldPathVector[i].Path.size() + 1;
								MarkedID = *Adj;
								ChangedInThisLoop = 1;
						}
				}
				if(ChangedInThisLoop){
						NewPathVector[i].Validity = 1;
						NewPathVector[i].Path = AllNodeVector[MarkedID]->OldPathVector[i].Path;
						NewPathVector[i].Path.push_front(ID);

						IsChanged = 1;
				}
			}	
		}
};

class SDN{
	public:
		u_s Size;
		vector<u_s> AsVector;
		SDN(u_s SdnSize){
			if(!SdnSize)
				return;
			if(SdnSize > ValidNodeNum)
				return;
			Size = 0;
			u_s CurrentNodeID;
			AsVector.assign(SdnSize,-1); 
			list<u_s> Queue;
			u_s StartNodeID = (u_s)(ValidNodeNum * (rand()%100/100.0));
			Queue.push_back(StartNodeID);
			AsVector[Size] = StartNodeID;
			Size ++;
			AllNodeVector[StartNodeID]->IsInSDN = 1;
			while(Size < SdnSize){	
				CurrentNodeID = Queue.front();
				Queue.pop_front();
				for(list<u_s>::iterator itor = AllNodeVector[CurrentNodeID]->AdjList.begin(); itor != AllNodeVector[CurrentNodeID]->AdjList.end(); itor ++){
					if(AllNodeVector[*itor]->IsInSDN)
						continue;
					AsVector[Size] = *itor;
					AllNodeVector[*itor]->IsInSDN = 1;
					Size ++;	
					Queue.push_back(*itor);
					if(Size == SdnSize)
						break;
				}
			}
#ifdef DEBUG
			fprintf(stderr,"=SDN List:");
			for( u_s i = 0; i < Size; i ++){
				fprintf(stderr," %d-", AsVector[i]);
			}
			fprintf(stderr,"\n");
#endif
		}
		void local_converge(){
			bool TmpIsChanged = IsChanged;
			while(1){
				IsChanged = 0;
				for(u_s i = 0; i < Size; i ++)
					AllNodeVector[AsVector[i]]->OldPathVector = AllNodeVector[AsVector[i]]->NewPathVector;
				for(u_s i = 0; i < Size; i ++){
					AllNodeVector[AsVector[i]]->bgp_update();
				}
				if(!IsChanged)
					break;
			}
			IsChanged = TmpIsChanged;
		}
};


int main(int argc, char * argv[]){
	if(argc < 5){
		fprintf(stderr,"error! not enough args\n");
		exit(-1);
	}
	NodeNum = atoi(argv[1]);
	SDNRatio = atoi(argv[2])/1000.0;
	RandSeed = atoi(argv[3]);
	RepeatTimes = atoi(argv[4]);

	ValidNodeNum = NodeNum - RepeatTimes;
	srand(RandSeed);
	AllNodeVector.assign(NodeNum, NULL);
	bool MySeed[5][5] = {{0,1,0,0,1},
						{1,0,0,1,0},
						{0,0,0,1,0},
						{0,1,1,0,0},
						{1,0,0,0,0}	};
	AdjMatrix = new bool * [NodeNum];
	for( int i = 0; i < NodeNum; i ++){
		AdjMatrix[i] = new bool [NodeNum];
		memset(AdjMatrix[i],0,NodeNum*sizeof(bool));
	}
	SFENG(AdjMatrix,MySeed);
#ifdef DEBUG
	for(u_s i = 0; i < NodeNum; i ++){
		for( u_s j = 0; j < NodeNum; j++)
			fprintf(stderr,"%d ", AdjMatrix[i][j]);
		fprintf(stderr,"\n");
	}
#endif
	for( u_s i = 0; i < ValidNodeNum; i ++){
		AllNodeVector[i] = new node(i);
		for( u_s j = 0; j < ValidNodeNum; j ++){
			if(AdjMatrix[i][j])
				AllNodeVector[i]->add_adj(j);
		}
	}
	fprintf(stderr,"initial converge time: %d\n",converge_process(0));
	MySDN = new SDN((u_s)(ValidNodeNum * SDNRatio));
	for( u_s i = 0; i < RepeatTimes; i ++){
		inject_up(ValidNodeNum);
		fprintf(stderr,"update converge time: %d\n",converge_process(1));
	}
#ifdef DEBUG
	myprint_path();
#endif
	return 0;
}
void inject_up(u_s p){
	AllNodeVector[p] = new node(p);
	for( u_s i = 0; i < p; i ++){
		if(AdjMatrix[i][p]){
			AllNodeVector[p]->add_adj(i);
			AllNodeVector[i]->add_adj(p);
			fprintf(stderr, "#	R# link up info injected: %d - %d\n",p,i); 
			break;
		}
	}
	ValidNodeNum ++;
}

u_s converge_process(bool sign){
	int LoopTimes = 0;
	while(1){
		IsChanged = 0;
		for(u_s i=0; i < ValidNodeNum; i++){
			AllNodeVector[i]->OldPathVector = AllNodeVector[i]->NewPathVector;
		}
		for(u_s i=0; i < ValidNodeNum; i++)
			AllNodeVector[i]->bgp_update();
		if(sign)
			MySDN->local_converge();	
		LoopTimes = LoopTimes + 1;
		if(!IsChanged)
			break;
	}
	return LoopTimes;
}

void myprint_path(){
	for( u_s i = 0; i < NodeNum; i ++){
		fprintf(stderr, "%d ######################- \n", i);
		for( u_s j = 0; j < NodeNum; j ++){
			fprintf(stderr,"(%d): ", j);
			if(! AllNodeVector[i]->NewPathVector[j].Validity)
				fprintf(stderr,"Invalidate\n");	
			else{
				for(list<u_s>::iterator itor = AllNodeVector[i]->NewPathVector[j].Path.begin(); itor != AllNodeVector[i]->NewPathVector[j].Path.end(); itor++)
					fprintf(stderr,"%3d ", *itor);
				fprintf(stderr,"\n");
			}
		}
	}
}

void SFENG(bool ** AdjMatrix, bool MySeed[5][5]){
	int sumlinks = 0;
	for(int i = 0; i < 5; i ++)
		for(int j = 0; j < 5; j++){
			AdjMatrix[i][j] = MySeed[i][j];
			sumlinks += MySeed[i][j];
		}
	int pos = 5;
	int rnode = 0, deg = 0;
	int linkage;
	float rvalue, rlink; 
	int mlinks = 1;
	while(pos < NodeNum){
		linkage = 0;
		rvalue = rand()%100/100.0;
		if(rvalue < 0.8)
			mlinks = 1;
		else if(rvalue < 0.95)
			mlinks = 2;
		else 
			mlinks = 3;
		while(linkage<mlinks){
			rnode = (int)(rand()%100/100.0*pos);
			deg = 0;
			for( int i = 0; i < pos; i ++){
			   	deg += AdjMatrix[rnode][i];
			}
			deg *= 2;
			rlink = rand()%100/100.0;
			if(rlink < float(deg)/sumlinks && AdjMatrix[rnode][pos]!=1){
				AdjMatrix[pos][rnode] = 1;
				AdjMatrix[rnode][pos] = 1;
				linkage = linkage + 1;
				sumlinks = sumlinks + 2;
			}
		}
		pos ++;
	}
}	
