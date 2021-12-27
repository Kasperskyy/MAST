#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <malloc.h>
#define CHARTOINT 48

typedef struct Node {
	Node* parent;
	Node* brother;
	Node* son;
	int label,childCount;	
}Node;

typedef struct DepthNode {
	DepthNode* nextNode;
	Node* givenNode;
}DepthNode;

typedef struct DepthList {
	DepthList* prevDepth;
	DepthList* nextDepth;
	DepthNode* firstNode;
}DepthList;

typedef struct Tree {
	Node* root;
	Node** map;
	int* rMap;
	int vertCount, nodeCount;
}Tree;

typedef enum nodeType{Parent,Brother,Son}nodeType;
void addNode(Node* givenNode,int type);
void createRoot(Tree* givenTree);
void addDepth(DepthList* &givenDepth);
void addDepthNode(DepthList* givenDepth, Node* givenNode);
void addMapVertices(Tree* givenTree, DepthList* depths, int depthCount,int maxInt);
void makeRMap(Tree* givenTree);
int mast(Tree* A, Tree* B);
int childSearchDeep(Node* givenNode, int searching);
int findTableMax(int* A, int* B, int** table, int countB, int countA, Node** childrenA, Node** childrenB);
void recursMax(int* B, int* A, Node** permutations, Node** otherChildren, int** table, int maxSize, int permuLength, int* max, int currPos, bool switchNeeded);
void swap(Node** a, Node** b);
void freeTrees(Tree* trees, int treeCount);

int main()
{	
	Tree* trees;
	Node* currentNode;
	int treeCount;
	std::string input;
	char specificInput=' ';
	std::cin >> treeCount;
	trees = (Tree*)malloc(sizeof(Tree) * treeCount);
	if (trees != NULL)
	{
		for (int i = 0; i < treeCount; i++)
		{	
			DepthList* depths = NULL;
			int counter = 0,maxDepth = 0,currDepth = 0,maxInt = 1;
			trees[i].nodeCount = 0;
			trees[i].vertCount = 0;
			std::cin >> input;
			createRoot(trees + i);
			trees[i].map = NULL;
			specificInput = input[counter];
			currentNode = trees[i].root;			
			while (specificInput != ';')
			{							
				switch (specificInput)
				{
				case '(':		
					trees[i].vertCount++;
					currDepth++;
					if (currDepth > maxDepth)
					{
						maxDepth = currDepth;
						addDepth(depths);						
					}
					if(maxDepth!=1)
						depths = depths->nextDepth;
					addDepthNode(depths,currentNode);
					addNode(currentNode, Son);
					currentNode->childCount++;
					currentNode = currentNode->son;
					break;
				case ')':
					currDepth--;
					if (currDepth != 0)
						depths = depths->prevDepth;					
					currentNode = currentNode->parent;
					break;
				case ',':					
					addNode(currentNode, Brother);
					currentNode = currentNode->brother;
					currentNode->parent->childCount++;
					break;
				case ';':
					break;
				default:
					int value = 0;
					trees[i].nodeCount++;
					int numberLength = 1;
					while ((counter+numberLength)<input.size() && input[counter + numberLength] >= 0+CHARTOINT && input[counter + numberLength] <= 9+CHARTOINT)
					{
						numberLength++;
					}
					for (int j = 0; j < numberLength; j++)
					{						
						value += (input[counter + numberLength - j-1]-CHARTOINT) * pow(10, j);
					}
					counter += (numberLength-1);
					//reading a number and assigning the value.					
					currentNode->label = value;
					if (value > maxInt)
						maxInt = value;					
					break;
				}
				counter++;	
				specificInput = input[counter];
			}
			addMapVertices(trees + i,depths,maxDepth,maxInt );
			makeRMap(trees + i);		
		}
		for (int i = 0; i < treeCount - 1; i++)
		{
			for (int j = i+1; j < treeCount; j++)
			{			
				std::cout << trees[i].nodeCount-mast(trees + i, trees + j)<<std::endl;				
			}
		}
		freeTrees(trees, treeCount);
	}
}

void freeTrees(Tree* trees,int treeCount)
{
	for (int i = 0; i < treeCount; i++)
	{
		free(trees[i].rMap);
		for (int j = 0; j < (trees[i].vertCount + trees[i].nodeCount); j++)
		{
			free(trees[i].map[j]);
		}
		free(trees[i].map);
	}
	free(trees);
}

int mast(Tree* A, Tree* B)
{
	int rowCount = B->nodeCount + B->vertCount;
	int colCount = A->nodeCount + A->vertCount;
	int** table = (int**)malloc(rowCount* sizeof(int*));	
	for (int i = 0; i < rowCount; i++)
	{
		table[i] = (int*)malloc(colCount * sizeof(int));
	}
	for (int i = 0; i < B->nodeCount; i++)
	{
		for (int j = 0; j < A->nodeCount; j++)
		{
			if (B->map[i]->label == A->map[j]->label)
				table[i][j] = 1;
			else
				table[i][j] = 0;
		}
	}
	for (int i = 0; i < B->nodeCount; i++)
	{
		for (int j = A->nodeCount; j < colCount; j++)
		{
			table[i][j] = childSearchDeep(A->map[j], B->map[i]->label);
		}
	}
	for (int i = B->nodeCount; i < rowCount; i++)
	{
		for (int j = 0; j < A->nodeCount; j++)
		{
			table[i][j] = childSearchDeep(B->map[i], A->map[j]->label);
		}
	}
	for (int i = B->nodeCount; i < rowCount; i++)
	{
		for (int j = A->nodeCount; j < colCount; j++)
		{
			int max = 0;
			Node* temp;
			temp = B->map[i]->son;
			for (int k = 0; k < B->map[i]->childCount; k++)
			{
				if (table[B->rMap[temp->label-1]][j] > max)
					max = table[B->rMap[temp->label-1]][j];
				temp = temp->brother;
			}
			temp = A->map[j]->son;
			for (int k = 0; k < A->map[j]->childCount; k++)
			{
				if (table[i][A->rMap[temp->label - 1]] > max)
					max = table[i][A->rMap[temp->label - 1]];
				temp = temp->brother;
			}			
			Node** childrenA, ** childrenB;
			temp = A->map[j]->son;			
			childrenA = (Node**)malloc(sizeof(Node*) * A->map[j]->childCount);
			childrenB = (Node**)malloc(sizeof(Node*) * B->map[i]->childCount);		
			for (int k = 0; k < A->map[j]->childCount; k++)
			{
				childrenA[k] = temp;
				temp = temp->brother;
			}
			temp = B->map[i]->son;
			for (int k = 0; k < B->map[i]->childCount; k++)
			{
				childrenB[k] = temp;
				temp = temp->brother;
			}			
			int newMax=findTableMax(A->rMap,B->rMap,table,B->map[i]->childCount,A->map[j]->childCount,childrenA,childrenB);
			if (newMax > max)
				max = newMax;
			table[i][j] = max;
			free(childrenA);
			free(childrenB);
		}
	}
	int answer = table[rowCount - 1][colCount - 1];
	for (int i = 0; i < rowCount; i++)
		free(table[i]);
	free(table);
	return answer;
}

int findTableMax(int* A, int* B, int** table, int countB, int countA,Node** childrenA, Node**childrenB)
{		
	int max = 0;		
	if (countA > countB)
		recursMax(B, A, childrenA, childrenB, table, countA, countB, &max, 0, true);
	else
		recursMax(B, A, childrenB, childrenA, table, countB, countA, &max, 0, false);	
	return max;
}

void recursMax(int* B, int* A,Node** permutations, Node** otherChildren, int** table,int maxSize,int permuLength,int * max, int currPos,bool switchNeeded)
{
	if (currPos == permuLength)
	{
		int sum = 0;
		for (int i = 0; i < permuLength; i++)
		{
			if (switchNeeded)
				sum += table[B[otherChildren[i]->label - 1]][A[permutations[i]->label - 1]];				
			else
				sum += table[B[permutations[i]->label - 1]][A[otherChildren[i]->label - 1]];
		}		
		if (sum > * max)
			*max = sum;
	}
	else
	{
		for (int i = currPos; i < maxSize; i++)
		{
			swap(&permutations[currPos], &permutations[i]);
			recursMax(B,A,permutations, otherChildren, table, maxSize, permuLength,max, currPos + 1,switchNeeded);
			swap(&permutations[currPos], &permutations[i]);
		}
	}
}

void swap(Node** a, Node** b)
{
	Node *temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

int childSearchDeep(Node* givenNode, int searching)
{
	if (givenNode->parent == NULL)
		return true;//root is a parent of all nodes
	if (givenNode->label == searching)
		return true;
	if (givenNode->son == NULL)
		return false;
	else
	{			
		Node* temp = givenNode->son;
		for (int i = 0; i < givenNode->childCount; i++)
		{		
			int found;
			found = childSearchDeep(temp, searching);
			if (found)
				return true;
			temp = temp->brother;
		}
		return false;
	}		
}

void addDepth(DepthList*& givenDepth)
{
	DepthList* newDepth;
	newDepth = (DepthList*)malloc(sizeof(DepthList));
	if (newDepth != NULL)
	{
		if (givenDepth == NULL)
		{
			givenDepth = newDepth;
			givenDepth->firstNode = NULL;
			givenDepth->nextDepth = NULL;
			givenDepth->prevDepth = NULL;
		}
		else
		{
			newDepth->firstNode = NULL;
			newDepth->prevDepth = givenDepth;
			givenDepth->nextDepth = newDepth;
		}
	}
}

void addDepthNode(DepthList* givenDepth,Node * givenNode)
{
	DepthNode* newNode, *temp;
	newNode = (DepthNode*)malloc(sizeof(DepthNode));
	if (newNode != NULL)
	{
		temp = givenDepth->firstNode;
		if (temp == NULL)
		{
			givenDepth->firstNode = newNode;
			givenDepth->firstNode->givenNode = givenNode;
			givenDepth->firstNode->nextNode = NULL;
		}
		else
		{
			while (temp->nextNode != NULL)
				temp = temp->nextNode;
			temp->nextNode = newNode;
			newNode->givenNode = givenNode;
			newNode->nextNode = NULL;
		}
	}
}

void addMapVertices(Tree* givenTree, DepthList* depths, int depthCount,int maxInt)
{	
	givenTree->map=(Node**)malloc(sizeof(Node*) * (givenTree->vertCount+givenTree->nodeCount));
	DepthList* temp = depths,*tempPrev;
	Node* tempNode,* childAdd;
	DepthNode* tempDepthNode;
	int counter = 0,nodeCounter=0;
	for (int i = 0; i < depthCount-1; i++)
	{
		temp = temp->nextDepth;
	}
	for (int i = 0; i < depthCount; i++)
	{
		while (temp->firstNode != NULL)
		{	
			counter++;
			maxInt++;
			tempNode = temp->firstNode->givenNode;
			tempNode->label = maxInt;
			givenTree->map[givenTree->nodeCount + counter - 1] = tempNode;
			if (tempNode->son != NULL)
			{
				childAdd = tempNode->son;
				while (true)
				{
					if (childAdd->son == NULL)
					{
						givenTree->map[nodeCounter] = childAdd;
						nodeCounter++;
					}
					if (childAdd->brother != NULL)
						childAdd = childAdd->brother;
					else
						break;
					
				}
			}
			tempDepthNode = temp->firstNode;
			temp->firstNode = temp->firstNode->nextNode;
			free(tempDepthNode);
		}
		tempPrev = temp;
		temp = temp->prevDepth;
		free(tempPrev);
	}
}

void makeRMap(Tree* givenTree)
{
	int* temp = (int*)malloc(sizeof(int) * (givenTree->vertCount + givenTree->nodeCount));
	if (temp != NULL)
	{
		for (int i = 0; i < givenTree->vertCount + givenTree->nodeCount; i++)
		{
			temp[givenTree->map[i]->label-1] = i;
			givenTree->rMap = temp;
		}
	}
}

void createRoot(Tree* givenTree)
{
	Node* temp;	
	temp = (Node*)malloc(sizeof(Node));
	if (temp != NULL)
	{
		temp->parent = NULL;
		temp->label = NULL;
		temp->childCount = NULL;
		temp->brother = NULL;
		givenTree->root = temp;		
	}	
}

void addNode(Node* givenNode, int type)
{
	Node* temp;
	temp = (Node*)malloc(sizeof(Node));
	if (temp != NULL)
	{
		temp->brother = NULL;
		temp->son = NULL;
		temp->childCount = 0;
		switch (type)
		{
		case Son:			
			givenNode->son = temp;
			temp->parent = givenNode;
			break;
		case Brother:
			if(givenNode->son==nullptr)
				givenNode->son = NULL;
			givenNode->brother = temp;
			temp->parent = givenNode->parent;
			break;
		}
	}
}	
