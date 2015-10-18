//realize the transfering from memory space to disk space
#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<vector>
#include<list>
#include<stack>
#include<string>
#include<algorithm>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<io.h>

using namespace std;
typedef unsigned short u_s;

//#define DEBUG

class node;
class SDN;
class mMap;

//u_s NodeNum = 43548;
//u_s NodeNum = 2749;
u_s NodeNum = 301;
//u_s ValidNodeNum = 43548;
//u_s ValidNodeNum = 300;
bool IsChanged = 0;
u_s RepeatTimes = -1;
int RandSeed = 0;
float SDNRatio = -0.1;
int LoopTimes = 0;
int PathInMemo = 0;
int Threshold = 100;
int BatchNum = 20;


SDN * MySDN;
vector<node *> AllNodeVector;
vector<bool> ValidVector;
vector<u_s> CNVector;
vector<u_s> TmpPLVector;
stack<u_s> Stack;
mMap *Map;

void myprint_path();
u_s converge_process(bool sign);
void inject_up(u_s );
void create_adj( u_s );

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
		char VisitType;
		bool IsInSDN;

		node(u_s myID){
			OldPathVector = NULL;
			NewPathVector = NULL;
		//	NewPathVector = new vector<path>(NodeNum, path(myID));
		//	PathInMemo ++;
			ID = myID;
			VisitType = 0;
			IsInSDN = 0;
		}
		void add_adj(u_s myID){
			AdjList.push_back(myID);
			return;
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
			FILE * fp;
			fp = fopen(name, "r");
			if(fp == NULL){
				if(LoopTimes){
					fprintf(stderr,"Error open name\n");
					exit(-1);
				}
				sprintf(name, "Old_zero/%d", ID);
				strcat(name, ".pathdata");
				fp = fopen(name, "r");
				if(fp == NULL){
					if(ID == NodeNum - 1){
						MOldPathDB[ID] = new vector<path>(NodeNum, path(ID));
						PathInMemo ++;
						OInMemoList.push_back(ID);
						return MOldPathDB[ID];
					}
					fprintf(stderr,"Error open name\n");
					exit(-1);
				}
			}
			u_s PathLen = 0;
			char * tok = NULL;
			MOldPathDB[ID] = new vector<path>(NodeNum, path(ID));
			for( u_s j = 0; j < NodeNum; j ++){
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
			fprintf(stderr,"  			currently %d pathINmemo, begin to reduce memo \n", PathInMemo);
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
				strcat(name,".pathdata");
				PathVector = MNewPathDB[ID];
				MNewPathDB[ID] = NULL;
			}else{
				sprintf(name, "Old/%d", ID);
				strcat(name,".pathdata");
				PathVector = MOldPathDB[ID];
				MOldPathDB[ID] = NULL;
				if(access(name,F_OK) == 0){
					fprintf(stderr,"write already exist\n");
					delete PathVector;
					PathVector = NULL;
					PathInMemo --;
					return;
				}
			}
			fprintf(stderr,"				write %s to disk\n", name);
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
			//fprintf(stderr,"  			currently %d pathINmemo \n", PathInMemo);
			NInMemoList.push_back(ID);
			MNewPathDB[ID] = PathVector;
			PathVector = NULL;
			if(PathInMemo >= Threshold)
				reduce_memo();	
		}
		void roll_zero(){
			for(u_s ID = 0; ID < NodeNum; ID ++){
				AllNodeVector[ID] -> VisitType = 0;
				LeftTimesVector[ID] = AllNodeVector[ID]->AdjList.size();
				LeftTimesVector[ID] ++;
			}
			//Here we assume that all the OldPathDB has been deleted (a result when everything goes well)
			if(!OInMemoList.empty())
				fprintf(stderr,"\n\nerror! OInMemoList not empty before roll\n\n\n");
		}
		void roll_all(){
			fprintf(stderr,"	enter roll\n");
			sprintf(name,"Old%d",LoopTimes);
			rename("Old", name);		
			rename("New","Old");
			mkdir("New",0700);
			for(u_s ID = 0; ID < NodeNum; ID ++){
				AllNodeVector[ID] -> VisitType = 0;
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
		void roll_record(){
			while(!OInMemoList.empty()){
				write_to_disk(OInMemoList.front(), 0);
				OInMemoList.pop_front();
			}
		}
		void roll(u_s ID){
			LeftTimesVector[ID] = AllNodeVector[ID]->AdjList.size();
			LeftTimesVector[ID] ++;
			MOldPathDB[ID] = MNewPathDB[ID];
			MNewPathDB[ID] = NULL;
		}	
};

void node::bgp_update(bool u_sign = 0){
	OldPathVector = Map->get(ID); 
	NewPathVector = new vector<path>(*OldPathVector);
	PathInMemo ++;
	vector<path> * TmpPathVector;

	ValidVector.assign(NodeNum, 0);
	for(vector<u_s>::iterator Adj=AdjList.begin(); Adj!=AdjList.end(); Adj++)
		ValidVector[*Adj] = 1;
	CNVector.assign(NodeNum, 0);
	TmpPLVector.assign(NodeNum, NodeNum + 2);
	for(u_s i = 0; i < NodeNum; i ++){
		if((*NewPathVector)[i].Validity)
			TmpPLVector[i] = (*NewPathVector)[i].Path.size();
	}
	for(vector<u_s>::iterator Adj=AdjList.begin(); Adj!=AdjList.end(); Adj++){
		if(u_sign){
			if(!AllNodeVector[*Adj]->IsInSDN)
				continue;
		}
		TmpPathVector = Map->get(*Adj);
		for( u_s i = 0; i < NodeNum; i ++){
			if(ValidVector[i])
				continue;
			if(!(*TmpPathVector)[i].Validity)
				continue;
			if(find((*TmpPathVector)[i].Path.begin(), (*TmpPathVector)[i].Path.end(),ID) != (*TmpPathVector)[i].Path.end())
				continue;
			if(!(*NewPathVector)[i].Validity){
				(*NewPathVector)[i].Validity = 1;
				CNVector[i] = *Adj + 1;
				TmpPLVector[i] = (*TmpPathVector)[i].Path.size() + 1;
				IsChanged = 1;
			}else if((*TmpPathVector)[i].Path.size() + 1 < TmpPLVector[i]){
				CNVector[i] = *Adj + 1;
				TmpPLVector[i] = (*TmpPathVector)[i].Path.size() + 1;
				IsChanged = 1;
			}
		}
	}
	for(u_s i = 0; i < NodeNum; i ++){
		if(CNVector[i]){
			(*NewPathVector)[i].Path = (*(Map->get(CNVector[i]-1)))[i].Path;
			(*NewPathVector)[i].Path.push_back(ID);
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
			if(SdnSize > NodeNum)
				return;
			u_s CurrentNodeID;
			AsVector.assign(SdnSize,-1); 
			list<u_s> Queue;
			u_s StartNodeID = (u_s)(NodeNum * (rand()%100/100.0));
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
	rename("Old","Old_before");
	rename("New","New_before");
//	system("/bin/rm -rf New");
//	system("/bin/rm -rf Old");
	mkdir("Old",0700);
	mkdir("New",0700);
//	system("mkdir -p New");
//	system("mkdir -p Old");
	SDNRatio = atoi(argv[2])/1000.0;
	RandSeed = atoi(argv[3]);
	RepeatTimes = atoi(argv[4]);

	//NodeNum = ValidNodeNum + RepeatTimes;

	Map = new mMap();
	//ValidNodeNum = NodeNum - RepeatTimes;
	srand(RandSeed);
	AllNodeVector.assign(NodeNum, NULL);
#ifdef DEBUG
#endif
	create_adj(u_s((rand()%100/100.0)*(NodeNum-1)));
	Map->roll_zero();
	fprintf(stderr,"\n\n first roll over\n\n");
	fprintf(stderr,"initial converge time: %d\n",converge_process(0));
	exit(-1);
	MySDN = new SDN((u_s)(NodeNum * SDNRatio));
	for( u_s i = 0; i < RepeatTimes; i ++){
		inject_up(NodeNum);
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
	NodeNum ++;
}

u_s converge_process(bool sign){
	LoopTimes = 0;
	u_s CurrentNode = 0;
	u_s CurrentNb = 0;
	while(1){
		fprintf(stderr," LoopTimes: %d\n", LoopTimes);
		IsChanged = 0;
		for( u_s k = 0; k < NodeNum; k ++){
			if(!AllNodeVector[k]->VisitType){
				AllNodeVector[k]->VisitType = 1;
				Stack.push(k);
				while(!Stack.empty()){
					CurrentNode = Stack.top();
					Stack.pop();
					AllNodeVector[CurrentNode]->bgp_update();
					fprintf(stderr,"	   -LP:%d	bgpupdate: %d over\n",LoopTimes, CurrentNode);
					for(u_s i = 0; i < AllNodeVector[CurrentNode]->AdjList.size(); i ++){
						CurrentNb = AllNodeVector[CurrentNode]->AdjList[i];
						if(AllNodeVector[CurrentNb]->VisitType)
							continue;
						AllNodeVector[CurrentNb]->VisitType = 1;
						Stack.push(CurrentNb);
					}
				}	
			}
		}

		Map->roll_all();
		if(sign)
			MySDN->local_converge();	
		LoopTimes = LoopTimes + 1;
		if(!IsChanged){
			Map->roll_record();	
			break;
		}
	}
	return LoopTimes;
}

void myprint_path(){
	for( u_s i = 0; i < NodeNum; i ++){
		fprintf(stderr, "%d ######################- \n", i);
		for( u_s j = 0; j < NodeNum; j ++){
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

void create_adj( u_s p){
	//After the executiong of this function, the paths should be valid 
	FILE * fp;
	fp = fopen("exp.data", "r");
	if(fp == NULL){
		fprintf(stderr,"Error open small.data\n");
		exit(-1);
	}
	char * tok = NULL;
	for( u_s i = 0; i < NodeNum-1; i ++){
		fprintf(stderr,"create_adj: %d\n", i);
		fgets(IBuffer, 70000, fp);
		tok = strtok(IBuffer, "\t");
		AllNodeVector[i] = new node(i);
		while(tok = strtok(NULL,"\t")){
			if(tok[0] == 10)
				continue;
			AllNodeVector[i]->add_adj(atoi(tok));
		}
//		Map->record(i,AllNodeVector[i]->NewPathVector);
	}
	fclose(fp);

	AllNodeVector[p]->add_adj(NodeNum-1);
	AllNodeVector[NodeNum-1] = new node(NodeNum-1);
	AllNodeVector[NodeNum-1]->add_adj(p);

	vector<path> * TmpPathVector1;
	vector<path> * TmpPathVector2;
	TmpPathVector1 = Map->get(p);
	if((*TmpPathVector1)[NodeNum-1].Validity){
		Map->roll_record();
		return;
	}
	TmpPathVector2 = Map->get(NodeNum-1);
	(*TmpPathVector1)[NodeNum-1].Validity = 1;
	(*TmpPathVector1)[NodeNum-1].Path.push_back(NodeNum-1);
	(*TmpPathVector2)[p].Validity = 1;
	(*TmpPathVector2)[p].Path.push_back(p);
	Map->roll_record();
//	exit(-1);
}
