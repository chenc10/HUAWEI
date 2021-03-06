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

u_s NodeNum = 2749;
u_s ValidNodeNum = 2749;
bool IsChanged = 0;
u_s RepeatTimes = -1;
int RandSeed = 0;
float SDNRatio = -0.1;
int LoopTimes = 0;
int PathInMemo = 0;
int Threshold = 1500;
int BatchNum = 500;

SDN * MySDN;
vector<node *> AllNodeVector;
mMap *Map;

void myprint_path();
u_s converge_process(bool sign);
void inject_up(u_s );
void create_adj();

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
char Buffer[70000];
char IBuffer[70000];
class mMap{
	public:
		vector<vector<path> *> MOldPathDB;
		vector<vector<path> *> MNewPathDB;
		vector<u_s> LeftTimesVector;
		list<u_s> OInMemoList;
		list<u_s> NInMemoList;
		mMap():MOldPathDB(NodeNum,NULL),MNewPathDB(NodeNum,NULL),LeftTimesVector(NodeNum, 0){
		}
		vector<path> * get(u_s ID){
			if(MOldPathDB[ID])
				return MOldPathDB[ID];
			sprintf(name, "Old/%d", ID);
			strcat(name,".pathdata");
			//fprintf(stderr,"			read %s from disk\n", name);
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
				fgets(Buffer,70000,fp);
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
			//fprintf(stderr,"			finish reading %s from disk\n", name);
			PathInMemo ++;
			OInMemoList.push_back(ID);
			if(PathInMemo >= Threshold)
				reduce_memo();
			return MOldPathDB[ID];
		}
		void reduce_memo(){
			while(!NInMemoList.empty() && PathInMemo > Threshold - BatchNum){ 
				write_to_disk(NInMemoList.front(), 1);
				NInMemoList.pop_front();
			}
			while(!OInMemoList.empty() && PathInMemo > Threshold - BatchNum){
				write_to_disk(OInMemoList.front(), 0);
				OInMemoList.pop_front();
			}
		}
		void write_to_disk(u_s ID, bool sign){
			vector<path> * PathVector;
			if(sign){
				sprintf(name, "New/%d", ID);
				PathVector = MNewPathDB[ID];
				MNewPathDB[ID] = NULL;
			}else{
				sprintf(name, "Old/%d", ID);
				PathVector = MOldPathDB[ID];
				MOldPathDB[ID] = NULL;
			}
			strcat(name,".pathdata");
			//fprintf(stderr,"				write %s to disk\n", name);
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
			PathInMemo --;
			//fprintf(stderr,"				 finish write %s to disk\n", name);
		}
		void check(u_s ID){
			LeftTimesVector[ID] --;
			if(!LeftTimesVector[ID]){
				delete MOldPathDB[ID];
				MOldPathDB[ID] = NULL;
				OInMemoList.remove(ID);
				PathInMemo --;
				fprintf(stderr,"			delete %d from memo\n", ID);
			}
		}
		void record(u_s ID, vector<path>* & PathVector){
			fprintf(stderr,"  			currently %d pathINmemo \n", PathInMemo);
			NInMemoList.push_back(ID);
			MNewPathDB[ID] = PathVector;
			PathVector = NULL;
			if(PathInMemo >= Threshold)
				reduce_memo();	
		}
		void roll_all(){
			fprintf(stderr,"enter roll\n");
			system("rm -rf Old");
			system("mv New Old");
			system("mkdir New");
			for(u_s ID = 0; ID < ValidNodeNum; ID ++){
				LeftTimesVector[ID] = AllNodeVector[ID]->AdjList.size();
				LeftTimesVector[ID] ++;
				MOldPathDB[ID] = MNewPathDB[ID];
				MNewPathDB[ID] = NULL;
			}
			//Here we assume that all the OldPathDB has been deleted (a result when everything goes well)
			if(!OInMemoList.empty())
				fprintf(stderr,"\n\nerror! OInMemoList not empty before roll\n\n\n");
			OInMemoList = NInMemoList;
			NInMemoList.clear();
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
	u_s MarkedID;
	OldPathVector = Map->get(ID); 
	NewPathVector = new vector<path>(*OldPathVector);
	PathInMemo ++;
	vector<path> * TmpPathVector;
	vector<path> * MarkedPathVector;
	fprintf(stderr,"			 enter update: %d\n", ID);
	for( u_s i = 0; i < ValidNodeNum; i ++){
	//	fprintf(stderr,"			   within update: <%d -> %d>\n", ID, i);
		ChangedInThisLoop = 0;
		if(find(AdjList.begin(), AdjList.end(), i) != AdjList.end())	
				continue;
		OldPathVector = Map->get(ID); 
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
						MarkedID = *Adj;
						ChangedInThisLoop = 1;
				}
		}
		if(ChangedInThisLoop){
				MarkedPathVector = Map->get(MarkedID);
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
	system("mkdir -p New");
	system("mkdir -p Old");
	SDNRatio = atoi(argv[2])/1000.0;
	RandSeed = atoi(argv[3]);
	RepeatTimes = atoi(argv[4]);

	NodeNum = ValidNodeNum + RepeatTimes;

	Map = new mMap();
	//ValidNodeNum = NodeNum - RepeatTimes;
	srand(RandSeed);
	AllNodeVector.assign(NodeNum, NULL);
#ifdef DEBUG
#endif
	create_adj();
	Map->roll_all();
	fprintf(stderr,"\n\n first roll over\n\n");
	fprintf(stderr,"initial converge time: %d\n",converge_process(0));
	exit(-1);
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
		if(1){
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
		fprintf(stderr," LoopTimes: %d\n", LoopTimes);
		IsChanged = 0;
		for(u_s i=0; i < ValidNodeNum; i++){
			AllNodeVector[i]->bgp_update();
			fprintf(stderr,"	-LP:%d	bgpupdate: %d over\n",LoopTimes, i);
		}
		Map->roll_all();
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

void create_adj(){
	FILE * fp;
	fp = fopen("small.data", "r");
	if(fp == NULL){
		fprintf(stderr,"Error open small.data\n");
		exit(-1);
	}
	char * tok = NULL;
	for( u_s i = 0, j = 0; i < ValidNodeNum; i ++){
		j = 0;
		fprintf(stderr,"create_adj: %d\n", i);
		fgets(IBuffer, 70000, fp);
		tok = strtok(IBuffer, "\t");
		AllNodeVector[i] = new node(i);
		while(tok = strtok(NULL,"\t")){
			if(tok[0] == 10)
				continue;
			AllNodeVector[i]->add_adj(atoi(tok));
			j ++;
		}
		if(j > Threshold)
			fprintf(stderr,"warning: Max Neighbor Num exceeds Buffer! Slow! Suggested: %d\n",j);
		Map->record(i,AllNodeVector[i]->NewPathVector);
	}
	fclose(fp);
}
