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

#define NodeNum 51895 //300

char IBuffer[200000];
int ** LW;

vector<node *> AllNodeVector;
vector<u_s> FinalNodes;

void create_adj();

class node{
	public:
		u_s ID;
		vector<u_s> AdjList;
		u_s Type;
		vector<u_s> Father;
		short Level;
		bool IsInSDN;
		node(u_s myID){
			ID = myID;
			Type = 0;
			Level = -1;
			IsInSDN = 0;
		}
		void add_adj(u_s myID){
			AdjList.push_back(myID);
		}
};

int main(int argc, char * argv[]){
	AllNodeVector.assign(NodeNum, NULL);
	LW = new int * [NodeNum];
	for( u_s i = 0; i < NodeNum; i ++){
		LW[i] = new int [NodeNum];
	}
	for( u_s i = 0; i < NodeNum; i ++){
		for( u_s j = 0; j < NodeNum; j ++){
			LW[i][j] = 0;
		}
	//	printf("%d\n", i);
	}
	//printf("here\n");
	create_adj();
	int  * SUM = new int [NodeNum];
	int * TSUM = new int [NodeNum];
	for( u_s i = 0; i < NodeNum; i ++){
		SUM[i] = 0;
		TSUM[i] = 0;
		for( u_s j = 0; j < NodeNum; j ++){
			SUM[i] += LW[i][j];
			TSUM[i] += LW[i][j];
		}
	//	printf("SUM[%d]: %d\n",i, SUM[i]);
	}
	char  * Valid = new char [NodeNum];
	for( u_s i = 0; i < NodeNum; i ++){
		Valid[i] = 1;
	}
	int max_sum = 0;
	int max_i = -1;
	for( u_s s = 0; s < NodeNum; s ++){
		max_sum = 0;
		max_i = -1;
		for( u_s i = 0; i < NodeNum; i ++){
			if( !Valid[i])
				continue;
			if(max_sum < SUM[i]){
				max_sum = SUM[i];
				max_i = i;
			}
		}
		if(max_i == -1){
			return 0;
		}
		AllNodeVector[max_i]->IsInSDN = 1;
		for( u_s j = 0; j < AllNodeVector[max_i]->AdjList.size(); j++){
			AllNodeVector[AllNodeVector[max_i]->AdjList[j]]->IsInSDN = 1;
		}
		Valid[max_i] = 0;
		printf("%d %d\n", max_i,max_sum);
//			if(Valid[7])
//				printf("SUM[7]:%d\n", SUM[7]);
		for( u_s i = 0, m=0; i < NodeNum; i ++){
			if( !Valid[i])
				continue;
			SUM[i] = TSUM[i];
//			if(max_i == 20338 && i == 7) { 
//				m = 1;
//				printf("20338-7: SUM[7]: %d\n", SUM[7]);
//			}	
			if(AllNodeVector[i]->IsInSDN)
				for( u_s j = 0; j < AllNodeVector[i]->AdjList.size(); j++){
//					if(m) printf("1&&& %d\n",j);
					if(AllNodeVector[AllNodeVector[i]->AdjList[j]]->IsInSDN){
						SUM[i] -= LW[i][AllNodeVector[i]->AdjList[j]];
//						if(m) printf("2&&&%d - achieved!: SUM[7]:%d\n",j,SUM[7]);
					}
				}
//			m=0;
		}
		//break;
	}
	return 0;
}

void create_adj( ){
	//After the executiong of this function, the paths should be valid 
	FILE * fp;
	fp = fopen("link_weight.data", "r");
	if(fp == NULL){
		fprintf(stderr,"Error! No data file\n");
		exit(-1);
	}
	char * tok = NULL;
	for( u_s i = 0, j = 0; i < NodeNum; i ++){
		j = 0;
		AllNodeVector[i] = new node(i);
		fgets(IBuffer, 200000, fp);
		tok = strtok(IBuffer, "+");
		LW[i][j] = atoi(tok);
		if(atoi(tok))
			AllNodeVector[i]->add_adj(j);
		j ++;
		while(tok = strtok(NULL,"+")){
			if(tok[0] == 10)
				continue;
			if(atoi(tok))
				AllNodeVector[i]->add_adj(j);
			LW[i][j] = atoi(tok);
			j ++;
		//	AllNodeVector[i]->add_adj(atoi(tok));
		}
		//printf("create %d over!\n", i);
	}
	fclose(fp);
//	for( u_s i = 0; i < NodeNum; i ++){
//		for( u_s j = 0, n = 0; j < AllNodeVector[i]->AdjList.size(); j ++){
//			n = AllNodeVector[i]->AdjList[j];
//				
}
