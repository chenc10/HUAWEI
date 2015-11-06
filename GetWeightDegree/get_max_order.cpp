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
		node(u_s myID){
			ID = myID;
			Type = 0;
			Level = -1;
		}
		void add_adj(u_s myID){
			AdjList.push_back(myID);
		}
};

int main(int argc, char * argv[]){
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
	for( u_s i = 0; i < NodeNum; i ++){
		SUM[i] = 0;
		for( u_s j = 0; j < NodeNum; j ++){
			SUM[i] += LW[i][j];
		}
	//	printf("SUM[%d]: %d\n",i, SUM[i]);
	}
	for( u_s i = 0; i < NodeNum; i ++){
		printf("%d %d\n", i, SUM[i]);
	}
	return 0;

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
		Valid[max_i] = 0;
		printf("%d\n", max_i);
		for( u_s i = 0; i < NodeNum; i ++){
			if( !Valid[i])
				continue;
			SUM[i] -= LW[i][max_i];
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
		fgets(IBuffer, 200000, fp);
		tok = strtok(IBuffer, "+");
		LW[i][j] = atoi(tok);//printf("%s\n", tok);
		j ++;
		while(tok = strtok(NULL,"+")){
			if(tok[0] == 10)
				continue;
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
