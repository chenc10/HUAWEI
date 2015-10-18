//realize the transfering from memory space to disk space
#include<iostream>
#include<fstream>
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
class mMap;

u_s NodeNum = 0;
u_s ValidNodeNum = 0;
bool IsChanged = 0;
u_s RepeatTimes = -1;
int RandSeed = 0;
float SDNRatio = -0.1;
int LoopTimes = 0;
int PathInMemo = 0;

SDN * MySDN;
vector<node *> AllNodeVector;
mMap *Map;
bool ** AdjMatrix;

void myprint_path();
u_s converge_process(bool sign);
void SFENG(bool ** AdjMatrix, bool MySeed[5][5]);
void inject_up(u_s );

class path{
	public:
		bool Validity;
		vector<u_s> Path;
		path(u_s From){
			Validity = 0;
			Path.push_back(From);
		}
};	
class node{
	public:
		u_s ID;
		vector<u_s> AdjList;
		vector<path> *OldPathVector;
		vector<path> *NewPathVector;
		bool IsInSDN;
		node(u_s myID){
			OldPathVector = NULL;
			NewPathVector = new vector<path>(NodeNum, path(myID));
			PathInMemo ++;
			ID = myID;
			IsInSDN = 0;
		}
		void add_adj(u_s myID){
			AdjList.push_back(myID);
			if(!(*NewPathVector)[myID].Validity){
				(*NewPathVector)[myID].Validity = 1;
				(*NewPathVector)[myID].Path.push_back(myID);
			}
			else{
				(*NewPathVector)[myID].Path.clear();
				(*NewPathVector)[myID].Path.push_back(ID);
				(*NewPathVector)[myID].Path.push_back(myID);
			}
		}
		void bgp_update(bool);
};
char name[20];
char Buffer[1000];
class mMap{
	public:
		vector<vector<path> *> MOldPathDB;
		vector<vector<path> *> MNewPathDB;
		vector<int> LeftTimesVector;
		mMap():MOldPathDB(NodeNum,NULL),MNewPathDB(NodeNum,NULL),LeftTimesVector(NodeNum, -1){
		}
		vector<path> * get(u_s ID){
			if(MOldPathDB[ID])
				return MOldPathDB[ID];
			sprintf(name, "%d", ID);
			strcat(name,".pathdata");
			FILE * fp;
			fp = fopen(name, "r");
			if(fp == NULL){
				fprintf(stderr,"Error open name\n");
				exit(-1);
			}
			u_s PathLen = 0;
			char * tok = NULL;
			MOldPathDB[ID] = new vector<path>(NodeNum, path(ID));
			for( u_s j = 0; j < ValidNodeNum; j ++){
				PathLen = 0;
				fgets(Buffer,1000,fp);
				if(Buffer[0] == 'I')
					continue;
				else{
					for(u_s k = 0; k < strlen(Buffer); k ++)
						if(Buffer[k] == '-')
							PathLen ++;
					u_s * TmpInt = new u_s [PathLen];
					tok = strtok(Buffer, "-");
					TmpInt[0] = atoi(tok);
					for(u_s k = 1; k < PathLen; k ++){
						tok = strtok(NULL, "-");
						TmpInt[k] = atoi(tok);
					}	
					(*(MOldPathDB[ID]))[j].Path = vector<u_s>(TmpInt, TmpInt+PathLen);	
					(*(MOldPathDB[ID]))[j].Validity = 1;	
					delete[] TmpInt;
				}
			}
			fclose(fp);
			PathInMemo ++;
			return MOldPathDB[ID];
		}
		void check(u_s ID){
			LeftTimesVector[ID] --;
			if(!LeftTimesVector[ID]){
				delete MOldPathDB[ID];
				MOldPathDB[ID] = NULL;
				PathInMemo --;
			}
		}
		void record(u_s ID, vector<path>* & PathVector){
			if(PathInMemo < 10){
				MNewPathDB[ID] = PathVector;
				PathVector = NULL;
				return;
			}
			sprintf(name, "%d", ID);
			strcat(name,".pathdata");
			FILE * fp;
			fp = fopen(name, "w");
			if(fp == NULL){
				fprintf(stderr,"Error open name\n");
				exit(-1);
			}

			for( u_s j = 0; j < NodeNum; j ++){
				if(! (*(PathVector))[j].Validity)
					fprintf(fp,"I\n");	
				else{
					for(vector<u_s>::iterator itor = (*(PathVector))[j].Path.begin(); itor != (*(PathVector))[j].Path.end(); itor++)
						fprintf(fp,"%d-", *itor);
					fprintf(fp,"\n");
				}
			}
			fclose(fp);
			delete PathVector;
			PathVector = NULL;
			MNewPathDB[ID] = NULL;
			PathInMemo --;
		}
		void roll(u_s ID){
			LeftTimesVector[ID] = AllNodeVector[ID]->AdjList.size();
			LeftTimesVector[ID] ++;
			MOldPathDB[ID] = MNewPathDB[ID];
			MNewPathDB[ID] = NULL;
		}	
};

void node::bgp_update(bool u_sign = 0){
	bool ChangedInThisLoop;
	u_s TmpPathLength;
	OldPathVector = Map->get(ID); 
	NewPathVector = new vector<path>(*OldPathVector);
	PathInMemo ++;
	vector<path> * TmpPathVector;
	vector<path> * MarkedPathVector;
	for( u_s i = 0; i < ValidNodeNum; i ++){
		ChangedInThisLoop = 0;
		if(find(AdjList.begin(), AdjList.end(), i) != AdjList.end())	
				continue;
		if(!(*OldPathVector)[i].Validity)
				TmpPathLength = NodeNum + NodeNum;
		else
				TmpPathLength = (*OldPathVector)[i].Path.size();
		for(vector<u_s>::iterator Adj=AdjList.begin(); Adj!=AdjList.end(); Adj++){
				if(u_sign){
					if(!AllNodeVector[*Adj]->IsInSDN)
						continue;
				}
				TmpPathVector = Map->get(*Adj);
				if(!(*TmpPathVector)[i].Validity)
						continue;
				if(find((*TmpPathVector)[i].Path.begin(), (*TmpPathVector)[i].Path.end(),ID) != (*TmpPathVector)[i].Path.end())
						continue;
				if((*TmpPathVector)[i].Path.size() + 1 < TmpPathLength){
						TmpPathLength = (*TmpPathVector)[i].Path.size() + 1;
						MarkedPathVector = TmpPathVector;
						ChangedInThisLoop = 1;
				}
		}
		if(ChangedInThisLoop){
				(*NewPathVector)[i].Validity = 1;
				(*NewPathVector)[i].Path = (*MarkedPathVector)[i].Path;
				(*NewPathVector)[i].Path.push_back(ID);
				IsChanged = 1;
		}
	}	
	for( u_s i = 0; i < AdjList.size(); i ++){
		if(u_sign)
			if(!AllNodeVector[AdjList[i]]->IsInSDN)
				continue;
		Map->check(AdjList[i]);
	}	
	Map->record(ID,NewPathVector);
	Map->check(ID);
}
class SDN{
	public:
		u_s Size;
		vector<u_s> AsVector;
		SDN(u_s SdnSize){
			Size = 0;
			if(!SdnSize)
				return;
			if(SdnSize > ValidNodeNum)
				return;
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
				for(vector<u_s>::iterator itor = AllNodeVector[CurrentNodeID]->AdjList.begin(); itor != AllNodeVector[CurrentNodeID]->AdjList.end(); itor ++){
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
			//Note: SDN converge not save memo bestly
			bool TmpIsChanged = IsChanged;
			while(1){
				IsChanged = 0;
				for(u_s i = 0; i < Size; i ++)
					AllNodeVector[AsVector[i]]->bgp_update(1);
				for(u_s i = 0; i < Size; i ++)
					Map->roll(AsVector[i]);
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

	Map = new mMap();
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
		Map->record(i,AllNodeVector[i]->NewPathVector);
	}
	for(u_s i = 0; i < ValidNodeNum; i ++)
		Map->roll(i);
	fprintf(stderr,"initial converge time: %d\n",converge_process(0));
	MySDN = new SDN((u_s)(ValidNodeNum * SDNRatio));
	for( u_s i = 0; i < RepeatTimes; i ++){
		inject_up(ValidNodeNum);
		fprintf(stderr,"update converge time: %d\n",converge_process(1));
	}
#ifdef DEBUG
	myprint_path();
	fprintf(stderr,"finish\n");
#endif
	return 0;
}
void inject_up(u_s p){
	AllNodeVector[p] = new node(p);
	for( u_s i = 0; i < p; i ++){
		if(AdjMatrix[i][p]){
			AllNodeVector[i]->NewPathVector = Map->get(i);	
			AllNodeVector[p]->add_adj(i);
			AllNodeVector[i]->add_adj(p);
			Map->record(i,AllNodeVector[i]->NewPathVector);
			Map->record(p,AllNodeVector[p]->NewPathVector);
			Map->roll(i);
			Map->roll(p);
			fprintf(stderr, "#	R# link up info injected: %d - %d\n",p,i); 
			break;
		}
	}
	ValidNodeNum ++;
}

u_s converge_process(bool sign){
	LoopTimes = 0;
	while(1){
		IsChanged = 0;
		for(u_s i=0; i < ValidNodeNum; i++)
			AllNodeVector[i]->bgp_update();
		for(u_s i=0; i < ValidNodeNum; i++)
			Map->roll(i);
		if(sign)
			MySDN->local_converge();	
		LoopTimes = LoopTimes + 1;
		if(!IsChanged)
			break;
	}
	return LoopTimes;
}

void myprint_path(){
	for( u_s i = 0; i < ValidNodeNum; i ++){
		fprintf(stderr, "%d ######################- \n", i);
		for( u_s j = 0; j < ValidNodeNum; j ++){
			fprintf(stderr,"(%d): ", j);
			AllNodeVector[i]->NewPathVector = Map->get(i);
			if(! (*(AllNodeVector[i]->NewPathVector))[j].Validity)
				fprintf(stderr,"Invalidate\n");	
			else{
				for(vector<u_s>::iterator itor = (*(AllNodeVector[i]->NewPathVector))[j].Path.begin(); itor != (*(AllNodeVector[i]->NewPathVector))[j].Path.end(); itor++)
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
