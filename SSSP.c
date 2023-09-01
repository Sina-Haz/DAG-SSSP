#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

typedef struct vertex{ //Vertex for a weighted directed graph
    char* Vname;
    int edgeWeight;
    struct vertex* next;

}vertex;

long numVertices;
vertex* visited;
void initializePtr(vertex** ptr);
void printSSSP(vertex** list,int* dist,char*** topSortedArr);

void createVertex(vertex** list,char* name){ //Add a vertex with some name to end of the list
    int count = 0;
    while((*list)[count].Vname != NULL && count < numVertices){
        count++;
    }
    (*list)[count].Vname = malloc(sizeof(char)*85);
    strcpy((*list)[count].Vname,name); //should be using strncpy to avoid buffer overflows
    (*list)[count].edgeWeight = 0;
    (*list)[count].next=NULL;
}

//Create an edge by adding in a vertex to it's corresponding "head" on the adjacency list
void createEdge(vertex** list,char* head, char* addOn,int weight){
    vertex* New = malloc(sizeof(vertex));
    New -> Vname = malloc(sizeof(char)*85);
    strcpy(New->Vname,addOn); //again should be using strncpy
    New->edgeWeight = 0;
    New -> next = NULL;

    int count = 0;
    while(strcmp(head,(*list)[count].Vname) != 0 && count < numVertices){ //Look for the vertex which is getting the edge
        count++;
    }

    vertex* ptr = &(*list)[count];
    // Here I determine where to put in the vertex in adjacency list and account for edge cases
    if (ptr -> next == NULL || strcmp(New -> Vname,ptr -> next -> Vname) < 0){
        New -> next = ptr -> next;
        New -> edgeWeight = weight;
        ptr -> next = New;
    }
    else{
        while(ptr -> next != NULL && (strcmp(New->Vname,ptr -> next -> Vname) > 0)){
            ptr = ptr -> next;
        }
        New -> next = ptr -> next;
        New -> edgeWeight = weight;
        ptr -> next = New;
    }
}


void InitializeGraph(vertex** list){
    for(int i = 0;i < numVertices;i++){
        (*list)[i].Vname = NULL;
        (*list)[i].edgeWeight = 0;
        (*list)[i].next = NULL;
    }
}

//Create initial adjacency list from file (no edges yet)
void nameGraph(vertex** list,FILE* fp){
    char* name = malloc(sizeof(char)*85);
    for(int i = 0;i < numVertices;i++){
        fscanf(fp,"%s\n",name);
        createVertex(list,name);
    }
    free(name);
}

// Construct edges from file data
void makeEdges(vertex** list,FILE* fp){
    char* name1 = malloc(sizeof(char)*85);
    char* name2 = malloc(sizeof(char)*85);
    int weight;
    while(fscanf(fp,"%s %s %d\n",name1,name2,&weight) != EOF){
        createEdge(list,name1,name2,weight);
    }
    free(name1);
    free(name2);
}

// Checks to see if a name corresponds to a new node or not. If its new, return 0, else 1
int isNewNode(char* name){
    int num = 0;
    vertex* ptr = visited;
    if(ptr -> Vname == NULL){
        return num;
    }
    else{
        while(ptr != NULL){
            if(strcmp(name,ptr->Vname) == 0){
                return 1;
            }
            ptr = ptr -> next;
        }
    }
    return num;
}


void Push(vertex** stackHead,char* name,int edgeWeight){
    vertex* tempStack = malloc(sizeof(vertex));
    tempStack -> Vname = malloc(sizeof(char)*85);
    strcpy(tempStack->Vname,name);
    tempStack->edgeWeight=edgeWeight;
    tempStack->next = NULL;

    if((*stackHead)->Vname == NULL){
        vertex* temp = (*stackHead);
        *stackHead = tempStack;
        free(temp);
    }
    else{
        tempStack->next = *stackHead;
        *stackHead = tempStack;
    }

    if(isNewNode(name) == 0){
        vertex* newVisit = malloc(sizeof(vertex));
        newVisit -> Vname = malloc(sizeof(char)*85);
        strcpy(newVisit->Vname,name);
        newVisit->edgeWeight = edgeWeight;
        newVisit -> next = NULL;

        if(visited->Vname == NULL){
            vertex* temp = visited;
            visited = newVisit;
            free(temp);
        }
        else{
            vertex* ptr = visited;
            while(ptr -> next != NULL){
                ptr = ptr->next;
            }
            ptr->next = newVisit;
        }
    }

}

void Pop(vertex** stackHead){
    vertex* temp;
    temp = *stackHead;
    if((*stackHead) -> Vname != NULL){
        *stackHead = (*stackHead) -> next;
        free(temp->Vname);
        free(temp);
    }
}

int compareFunction(const void* Vname1,const void* Vname2){
    return strcmp(*(const char**)Vname1,*(const char**)Vname2);
}

void sortGraph(vertex** list){ //Use prewritten algorithms and string comparison to sort adjacency list in alphabetical order
    qsort(*list,numVertices,sizeof(vertex),compareFunction);
}

int inStack(vertex** stackHead,char* name){
    vertex* ptr = *stackHead;
    if(ptr->Vname == NULL){
        return 1;
    }
    else{
        while(ptr != NULL){
            if(strcmp(name,ptr->Vname) == 0){
                return 0;
            }
            ptr = ptr -> next;
        }
    }
    return 1;
}

/*Everytime exploring a Node in DFS, mark it as currently being explored (i.e in stack)
after return from that node set it as unexplored (i.e pop from stack),
If during DFS you come across Node being already explored have a cycle*/
int isCycle(vertex** list,char* start,int weight,vertex** stackHead){
    int num = 1;
    Push(stackHead,start,weight);

    int count = 0;
    while(strcmp((*list)[count].Vname,start) != 0 && count < numVertices){
        count++;
    }

    vertex* ptr = &(*list)[count];
    while(ptr -> next != NULL){
        ptr = ptr -> next;
        if(inStack(stackHead,ptr->Vname) == 0){
            return 0;
        }
        if(isNewNode(ptr->Vname) == 0){
            num = isCycle(list,ptr->Vname,ptr->edgeWeight,stackHead);
        }
    }
    Pop(stackHead);
    return num;
}

void topSort(vertex** list,char* start,int weight,vertex** stackHead,vertex** topStack){
    Push(stackHead,start,weight);

    int count = 0;
    while(strcmp((*list)[count].Vname,start) != 0 && count < numVertices){
        count++;
    }

    vertex* ptr = &(*list)[count];
    while(ptr -> next != NULL){
        ptr = ptr -> next;
        if(isNewNode(ptr->Vname) == 0){
            topSort(list,ptr->Vname,ptr->edgeWeight,stackHead,topStack);
        }
    }
    Push(topStack,(*stackHead)->Vname,(*stackHead)->edgeWeight);
    Pop(stackHead);
}

vertex* topSortAll(vertex** list){
    vertex* topStack;
    initializePtr(&topStack);
    for(int i = 0;i<numVertices;i++){
        vertex* ptr = &(*list)[i];
        if(isNewNode(ptr -> Vname) == 0){
            vertex* DFSstack;
            initializePtr(&DFSstack);
            topSort(list,ptr->Vname,ptr->edgeWeight,&DFSstack,&topStack);
        }
    }
    return topStack;
}

void stackToArr(vertex** stackHead,char*** arr){
    for(int i = 0;i < numVertices;i++){
        strcpy((*arr)[i],(*stackHead)->Vname);
        Pop(stackHead);
    }
}

void printList(vertex** list){
    vertex* ptr = *list;
    while(ptr != NULL){
        printf("%s ",ptr->Vname);
        ptr = ptr->next;
    }
    printf("\n");
}

void freeVisits(){
    vertex* temp;
    while(visited != NULL){
        temp = visited;
        visited = visited -> next;
        free(temp->Vname);
        free(temp);
    }
}

void initializePtr(vertex** ptr){
    *ptr = malloc(sizeof(vertex));
    (*ptr)->Vname = NULL;
    (*ptr)->edgeWeight = 0;
    (*ptr)->next = NULL;
}

int getListInd(vertex** list,char* name){
    int count = 0;
    while(strcmp(name,(*list)[count].Vname) != 0 && count < numVertices){
        count++;
    }
    return count;

}

int getArrInd(char*** arr,char* name){
    int ind = -1;
    for(int i = 0;i<numVertices;i++){
        if(strcmp((*arr)[i],name) == 0){
            ind = i;
        }
    }
    return ind;
}

void SSSP(vertex** list,char* src,char*** topSortedArr,int** dist){
    for(int i = 0;i < numVertices;i++){
        //int x = getListInd(list,(*topSortedArr)[i]);
        if(strcmp((*topSortedArr)[i],src) != 0){
            (*dist)[i] = INT_MAX;
        }
        else{
            (*dist)[i] = 0;
        }
    }

    for(int i = 0;i < numVertices;i++){
        if((*dist)[i] != INT_MAX){
            vertex* ptr = &(*list)[getListInd(list,(*topSortedArr)[i])];
            while(ptr->next != NULL){
                ptr = ptr->next;
                int index = getArrInd(topSortedArr,ptr->Vname);
                if((*dist)[index] == INT_MAX){
                    (*dist)[index] = (*dist)[i] + ptr->edgeWeight;
                }
                else{
                    if((*dist)[index] > (*dist)[i] + ptr->edgeWeight){
                        (*dist)[index] = (*dist)[i] + ptr->edgeWeight;
                    }
                }
            }
        }
    }
    printSSSP(list,*dist,topSortedArr);
}

void printSSSP(vertex** list,int* dist,char*** topSortedArr){
    printf("\n");
    for(int i = 0;i < numVertices;i++){
        vertex* ptr = &(*list)[i];
        int num = dist[getArrInd(topSortedArr,ptr->Vname)];
        if(num == INT_MAX){
            printf("%s %s\n",ptr->Vname,"INF");
        }
        else{
            printf("%s %d\n",ptr->Vname,num);
        }
    }
}

void deleteAll(vertex** list){
    vertex* temp;
    for(int i = 0;i < numVertices;i++){
        vertex* ptr = &(*list)[i];
        free(ptr->Vname);
        ptr = ptr -> next;
        while(ptr != NULL){
            temp = ptr;
            ptr = ptr->next;
            free(temp -> Vname);
            free(temp);
        }
    }
    free(*list);
}

int main(int argc,char* argv[]){

    if(argc > 1){
        FILE* graphFile = fopen(argv[1],"r");
        fscanf(graphFile,"%ld\n",&numVertices);
        vertex* adjList = malloc(sizeof(vertex)*numVertices);

        InitializeGraph(&adjList);
        nameGraph(&adjList,graphFile);
        sortGraph(&adjList);
        makeEdges(&adjList,graphFile);

        vertex* stackHead;
        initializePtr(&stackHead);

        initializePtr(&visited);

        if(isCycle(&adjList,adjList[0].Vname,adjList[0].edgeWeight,&stackHead) == 0){
            printf("CYCLE\n");
            return EXIT_SUCCESS;
        }
        else{
            freeVisits();
            initializePtr(&visited);

            vertex* topStack = topSortAll(&adjList);
            char** topSortArr = malloc(sizeof(char*)*numVertices);
            for(int i = 0;i < numVertices;i++){
                topSortArr[i] = malloc(sizeof(char)*85);
            }
            stackToArr(&topStack,&topSortArr);
            int* distance = malloc(sizeof(int)*numVertices);
            if(argc > 2){
                FILE* queryFile = fopen(argv[2],"r");
                char* newName = malloc(sizeof(char)*85);
                while(fscanf(queryFile,"%s\n",newName) != EOF){
                    SSSP(&adjList,newName,&topSortArr,&distance);
                }
                free(newName);
            }
            freeVisits();
            for(int i = 0;i < numVertices;i++){
                free(topSortArr[i]);
            }
            free(topSortArr);
            deleteAll(&adjList);
        }


    }

    return EXIT_SUCCESS;
}