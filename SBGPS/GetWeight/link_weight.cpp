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

char IBuffer[70000];
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
void visit(u_s c){
	for( u_s i = 0, f = 0; i < AllNodeVector[c]->Father.size(); i ++){
		f = AllNodeVector[c]->Father[i];
		LW[c][f] = LW[c][f] + 1;
		LW[f][c] = LW[f][c] + 1;
		visit(f);
	}
}

void handle(u_s S){
	deque<u_s> Q;
	for( u_s i = 0; i < NodeNum; i ++){
		AllNodeVector[i]->Type = 0;
		AllNodeVector[i]->Level = -1;
		AllNodeVector[i]->Father.clear(); 
	}
	AllNodeVector[S]->Level = 0;
	Q.push_back(S);
	FinalNodes.clear();
	int OldLevel = -1;
	u_s c;
	while(!Q.empty()){
		c = Q.front();	
		Q.pop_front();
		if(AllNodeVector[c]->Level != OldLevel){
			FinalNodes.clear();
			FinalNodes.push_back(c);
			OldLevel = AllNodeVector[c]->Level;
		}else
			FinalNodes.push_back(c);	
		AllNodeVector[c]->Type = 2;
		for( u_s i = 0, n = 0; i < AllNodeVector[c]->AdjList.size(); i ++){
			n = AllNodeVector[c]->AdjList[i];
			if(AllNodeVector[n]->Type == 0){
				Q.push_back(n);
				AllNodeVector[n]->Type = 1;
				AllNodeVector[n]->Level = AllNodeVector[c]->Level + 1;
				AllNodeVector[n]->Father.push_back(c);
//				for( u_s j = 0; j < AllNodeVector[n]->AdjList.size(); j ++){
//					if(AllNodeVector[n]->AdjList[j] == c){
//						AllNodeVector[n]->Father.push_back(j);
//						break;
//					}
//				}
			}else if(AllNodeVector[n]->Type == 1){
				if(AllNodeVector[n]->Level == AllNodeVector[c]->Level + 1)
					AllNodeVector[n]->Father.push_back(c);
			}
		}
	}
	for(u_s i = 0; i < FinalNodes.size(); i ++){
		visit(FinalNodes[i]);
	}
}

int main(int argc, char * argv[]){
	LW = new int * [NodeNum];
	for( u_s i = 0; i < NodeNum; i ++){
		LW[i] = new int [NodeNum];
	}
	for( u_s i = 0; i < NodeNum; i ++){
		for( u_s j = 0; j < NodeNum; j ++){
			LW[i][j] = 0;
		}
		printf("%d\n", i);
	}
	AllNodeVector.assign(NodeNum, NULL);

	printf("here\n");
	create_adj();
	for(u_s i = 0; i < NodeNum; i ++){
		handle(i);
		printf("loop: %d over\n", i);
	}
	printf("over\n");
	int tmp = 0;
	for( u_s i = 0; i < NodeNum; i ++){
		tmp = 0;
		for( u_s j = 0; j < NodeNum; j ++)
			tmp += LW[i][j];
		printf("	%d: %d\n", i, tmp);
	}

	return 0;
}

void create_adj( ){
	//After the executiong of this function, the paths should be valid 
	FILE * fp;
	fp = fopen("init.data", "r");
	if(fp == NULL){
		fprintf(stderr,"Error! No data file\n");
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
//	for( u_s i = 0; i < NodeNum; i ++){
//		for( u_s j = 0, n = 0; j < AllNodeVector[i]->AdjList.size(); j ++){
//			n = AllNodeVector[i]->AdjList[j];
//				
}
