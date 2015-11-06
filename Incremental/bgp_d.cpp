#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<vector>
#include<list>
#include<deque>
#include<string>
#include<algorithm>

using namespace std;
typedef unsigned short u_s;

//#define DEBUG

class node;
class SDN;

u_s IP;

#define NodeNum 51895 //300

//u_s NodeNum = 300;
bool IsChanged = 0;
u_s RepeatTimes = -1;
int RandSeed = 0;
float SDNRatio = -0.1;
u_s * SortVector;
int  CT;
u_s * FinalLen;

char IBuffer[70000];

SDN * MySDN;
vector<node *> AllNodeVector;

void myprint_path();
u_s converge_process();
void create_adj();
void inject_up(u_s );
int compare( const void * ,const  void * );

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
		vector<u_s> AdjList;
		path OldPathVector;
		path NewPathVector;
		u_s IsInSDN;
		u_s VisitType;
		node(u_s myID): OldPathVector(myID), NewPathVector(myID){
			ID = myID;
			IsInSDN = 0;
			VisitType = 0;
		}
		void add_adj(u_s myID){
			AdjList.push_back(myID);
		}
		void bgp_update(bool u_sign=0){
			bool ChangedInThisLoop;
			u_s TmpPathLength;
			u_s MarkedID;
			ChangedInThisLoop = 0;
			if(!OldPathVector.Validity)
				TmpPathLength = NodeNum + 10;
			else
				TmpPathLength = OldPathVector.Path.size();
			for(vector<u_s>::iterator Adj=AdjList.begin(); Adj!=AdjList.end(); Adj++){
				if(u_sign){
					if(AllNodeVector[*Adj]->IsInSDN != IsInSDN)
						continue;
				}
				if(!AllNodeVector[*Adj]->OldPathVector.Validity)
					continue;
				if(find(AllNodeVector[*Adj]->OldPathVector.Path.begin(), AllNodeVector[*Adj]->OldPathVector.Path.end(),ID) != AllNodeVector[*Adj]->OldPathVector.Path.end())
					continue;
				if( AllNodeVector[*Adj]->OldPathVector.Path.size() + 1 < TmpPathLength){
					TmpPathLength = AllNodeVector[*Adj]->OldPathVector.Path.size() + 1;
					MarkedID = *Adj;
					ChangedInThisLoop = 1;
				}
			}
			if(ChangedInThisLoop){
				NewPathVector.Validity = 1;
				NewPathVector.Path = AllNodeVector[MarkedID]->OldPathVector.Path;
				NewPathVector.Path.push_front(ID);
				IsChanged = 1;
			}
		}
};

class SDN{
	public:
	//	bool Extensible;
		vector<u_s> AsVector;
		void add(u_s i, u_s level){
			AsVector.push_back(i);
			AllNodeVector[i]->IsInSDN = 1;
		}
		
		u_s set(u_s level){
			u_s CurrentID;
			for(u_s i = 0; i < level; i ++){
				CurrentID = SortVector[i];
				if(!AllNodeVector[CurrentID]->IsInSDN)
					add(CurrentID, i+1);
				//AllNodeVector[CurrentID]->IsInSDN = i+1;
				for( u_s j = 0; j < AllNodeVector[CurrentID]->AdjList.size(); j ++){
					if(AllNodeVector[AllNodeVector[CurrentID]->AdjList[j]]->IsInSDN)
						continue;
					add(AllNodeVector[CurrentID]->AdjList[j], AllNodeVector[CurrentID]->IsInSDN);
				}
			}
#ifdef DEBUG
			fprintf(stderr,"=SDN List:");
			for( u_s i = 0; i < AsVector.size(); i ++){
				fprintf(stderr," %d-", AsVector[i]);
			}
			fprintf(stderr,"\n");
#endif
			return AsVector.size();
		}
		void local_converge(){
			bool TmpIsChanged = IsChanged;
			while(1){
				IsChanged = 0;
				for(u_s i = 0; i < AsVector.size(); i ++)
					AllNodeVector[AsVector[i]]->OldPathVector = AllNodeVector[AsVector[i]]->NewPathVector;
				for(u_s i = 0; i < AsVector.size(); i ++){
					AllNodeVector[AsVector[i]]->bgp_update(1);
				}
				if(!IsChanged)
					break;
			}
			IsChanged = TmpIsChanged;
		}
};


int main(int argc, char * argv[]){
	if(argc < 2){
		fprintf(stderr,"error! not enough args\n");
		exit(-1);
	}
	RandSeed = atoi(argv[1]);

	srand(RandSeed);
	AllNodeVector.assign(NodeNum, NULL);

	create_adj();

//	IP = u_s(NodeNum*(rand()%100/100.0));
	IP = rand()%NodeNum;
	fprintf(stderr,"#chosen node: %d\n", IP);
	AllNodeVector[IP]->NewPathVector.Validity = 1;

	MySDN = new SDN();
	u_s SDN_Size = 0;
	int last = -1;
	u_s oldsize;
	FinalLen = new u_s [NodeNum];
	for( u_s i = 0; i < NodeNum; i ++){
		FinalLen[i] = 0;
	}
	CT = -1;
	converge_process();
	for( u_s j = 0; j < NodeNum; j ++){
		if(!AllNodeVector[j]->NewPathVector.Validity){
			fprintf(stderr,"Error, error\n");
			exit(-1);
		}
		FinalLen[j] = AllNodeVector[j]->NewPathVector.Path.size();
	}
	for( u_s j=0; j < NodeNum; j ++){
		AllNodeVector[j]->NewPathVector = path(j);
	}	
	AllNodeVector[IP]->NewPathVector.Validity = 1;
	CT = 0;
	for(u_s i=0; SDN_Size < 0.99 * NodeNum ; i ++ ){
		oldsize = SDN_Size;
		SDN_Size = MySDN->set(i);
	//	fprintf(stderr," SDNSize: %d, last: %d\n", SDN_Size, last);
		if((int)((float)SDN_Size/NodeNum*100)/2 == last)
			continue;
		else
			last=(int)((float)SDN_Size/NodeNum*100)/2;
		fprintf(stderr,"converge time: %d %d %d\n\n",converge_process(), int(100*((float)SDN_Size)/NodeNum),i);
//		if(SDN_Size == 0){
//			for( u_s j = 0; j < NodeNum; j ++){
//				if(!AllNodeVector[j]->NewPathVector.Validity){
//					fprintf(stderr,"Error, error\n");
//					exit(-1);
//				}
//				FinalLen[j] = AllNodeVector[j]->NewPathVector.Path.size();
//			}
//		}
		for( u_s j=0; j < NodeNum; j ++){
			AllNodeVector[j]->NewPathVector = path(j);
		}	
		AllNodeVector[IP]->NewPathVector.Validity = 1;
		CT ++;
	}
	return 0;
}

u_s converge_process(){
	int LoopTimes = 0;
	u_s CN = 0;
	while(1){
		IsChanged = 0;
		for(u_s i=0; i < NodeNum; i++){
			AllNodeVector[i]->OldPathVector = AllNodeVector[i]->NewPathVector;
		}
		for(u_s i=0; i < NodeNum; i++)
			AllNodeVector[i]->bgp_update();
//			fprintf(stderr,"+ Loop(Before SDN): %d \n", LoopTimes);	
//			CN = 0;
//			for(u_s i = 0; i < NodeNum; i ++){
//				if(AllNodeVector[i]->NewPathVector.Validity && AllNodeVector[i]->NewPathVector.Path.size() == FinalLen[i]){
//					//fprintf(stderr," %d - (%d) + ", i, FinalLen[i]);
//					CN ++;
//				}
//			}
//			fprintf(stderr,"	+ %d nodes(Before SDN) converged\n", CN);	
		MySDN->local_converge();	
		CN = 0;
		for(u_s i = 0; i < NodeNum; i ++){
			if(AllNodeVector[i]->NewPathVector.Validity && AllNodeVector[i]->NewPathVector.Path.size() == FinalLen[i])
				CN ++;
		}
		fprintf(stderr,"	%d %d %d (para serial no.; LP no.; *) nodes converged\n", CT, LoopTimes, (u_s)(100*((float)CN)/NodeNum));	
		LoopTimes = LoopTimes + 1;
		if(!IsChanged)
			break;
	}
	return LoopTimes;
}

void myprint_path(){
	for( u_s i = 0; i < NodeNum; i ++){
		fprintf(stderr,"(%d): ", i);
		if(! AllNodeVector[i]->NewPathVector.Validity)
			fprintf(stderr,"Invalidate\n");	
		else{
			for(list<u_s>::iterator itor = AllNodeVector[i]->NewPathVector.Path.begin(); itor != AllNodeVector[i]->NewPathVector.Path.end(); itor++)
				fprintf(stderr,"%3d ", *itor);
			fprintf(stderr,"\n");
		}
	}
}

void create_adj( ){
	//After the executiong of this function, the paths should be valid 
	FILE * fp;
	fp = fopen("init.data", "r");
	if(fp == NULL){
		fprintf(stderr,"Error open small.data\n");
		exit(-1);
	}
	char * tok = NULL;
	for( u_s i = 0; i < NodeNum; i ++){
		fgets(IBuffer, 70000, fp);
		tok = strtok(IBuffer, "\t");
		AllNodeVector[i] = new node(i);
		while(tok = strtok(NULL,"\t")){
			if(tok[0] == 10)
				continue;
			AllNodeVector[i]->add_adj(atoi(tok));
		}
	}
	fclose(fp);
	SortVector = new u_s [NodeNum];
	fp = fopen("incremental_degree.list", "r");
	if(fp == NULL){
		fprintf(stderr,"Error open small.data\n");
		exit(-1);
	}
	while(fgets(IBuffer, 70000, fp)){
		SortVector[i] = atoi(IBuffer);
	}
	fclose(fp);
//	for(u_s i = 0; i < NodeNum; i ++)
//		fprintf(stderr,"%d ", SortVector[i]);
//	fprintf(stderr,"\n");
//	for(u_s i = 0; i < NodeNum; i ++)
//		fprintf(stderr,"%d ", AllNodeVector[SortVector[i]]->AdjList.size());
}
int compare(const void * a,const void * b){
	return int(AllNodeVector[*(u_s *)b]->AdjList.size()) - AllNodeVector[*(u_s *)a]->AdjList.size();
}
