#include "stdio.h"
#include "stdlib.h"

/**
    This is for regex-graph's node, containing current state char and nextState (array)
    @attribute data : char data for this state
    @attribute size : size of array for next state
    @attribute capacity : capacity of array for next state
    @attribute nextState : dynamic array for next state (pointer)
*/
typedef struct TRDArray
{
    char data;
    int size;
    int capacity;
    struct TRDArray **nextState;
    int visited;
} TRDArray;

/**
    function to get size of TRDArray-next-state-array
    @param array : current state node
    @return size of TRDArray-next-state-array size
*/
int trDArrayGetSize(TRDArray *array)
{
    return array->size;
}

/**
    function to get size of TRDArray-next-state-capacity
    @param array : current state node
    @return size of TRDArray-next-state-array capacity
*/
int trDArrayGetCapacity(TRDArray *array)
{
    return array->capacity;
}

/**
    function to get data hold by node (state)
    @param array : current state node
    @return char data of the current state
*/
char trDArrayGetData(TRDArray *array)
{
    return array->data;
}

/**
    function to initialize TRDArray with state data
    @param data : data of current state
    @return the initialized of TRDArray
*/
TRDArray *trDArrayInit(char data)
{
    TRDArray *trDArray = (TRDArray*)malloc(sizeof(TRDArray));
    trDArray->data = data;
    trDArray->size = 0;
    trDArray->capacity = 2;
    trDArray->nextState = (TRDArray**)malloc(sizeof(TRDArray*) * trDArrayGetCapacity(trDArray));
    trDArray->visited = 0;
    return trDArray;
}

/**
    function to get element of next-state-array of TRDArray at specific index
    @param trDarray : current state node holding the next-state-array
    @param index : index of next-state-array that will be retrieved
    @return TRDarray from next-state-array of trDarray at index
*/
TRDArray* trDArrayGetElement(TRDArray *trDarray, int index)
{
    if (index >= trDArrayGetSize(trDarray))
    {
        return NULL;
    }
    return trDarray->nextState[index];
}

/**
    function to push new next state to current state next-state-array
    @param array : the current state that will added a new next state
    @param newData : new next state that will be pushed
*/
void trDArrayPush(TRDArray *array, TRDArray* newData)
{

    // if array is full, then create the copy with bigger capacity (3/2 times bigger than before)
    if (array->size >= array->capacity)
    {
        array->capacity = trDArrayGetCapacity(array) * 3 / 2;
        TRDArray **newArrayNextState = (TRDArray**)malloc(sizeof(TRDArray*) * trDArrayGetCapacity(array));
        for (int i = 0; i < trDArrayGetSize(array); i++)
        {
            newArrayNextState[i] = trDArrayGetElement(array, i);
        }
        free(array->nextState);
        array->nextState = newArrayNextState;
    }

    array->nextState[trDArrayGetSize(array)] = newData;
    array->size += 1;
}

/**
    Free the current graph node
    @param garbage : node that will be freed
*/
void trDArrayDelete(TRDArray *garbage)
{
    free(garbage->nextState);
    garbage->nextState = NULL;
    free(garbage);
}

/**
    Recursive function to do depth first traversal to add all node to garbage collector
    @param garbageCollector : array used to save all pointer of graph node
    @param nextState : current state that visited
*/
void trDArrayDeleteAllFuncRec(TRDArray *garbageCollector, TRDArray *nextState)
{
    for (int i = 0; i < trDArrayGetSize(nextState); i++)
    {
        if (trDArrayGetElement(nextState, i)->visited == 0)
        {
            trDArrayPush(garbageCollector, trDArrayGetElement(nextState, i));
            trDArrayGetElement(nextState, i)->visited = 1;
            trDArrayDeleteAllFuncRec(garbageCollector, trDArrayGetElement(nextState, i));
        }
    }
}

/**
    Function to delete all graph node by recursion
    @param root : root of graph
*/
void trDArrayDeleteAll(TRDArray **root)
{
    TRDArray *garbageCollector = trDArrayInit((char)0);
    trDArrayPush(garbageCollector, *root);
    (*root)->visited = 1;
    trDArrayDeleteAllFuncRec(garbageCollector, *root);

    for (int i = 0; i < trDArrayGetSize(garbageCollector); i++)
    {
        if (trDArrayGetElement(garbageCollector, i) != NULL)
        {
            trDArrayDelete(trDArrayGetElement(garbageCollector, i));
        }
    }
    free(garbageCollector);
    *root = NULL;
}

int main()
{
    TRDArray *trDArray = trDArrayInit('a');
    for (int i = 33; i < 61; i++)
    {
        trDArrayPush(trDArray, trDArrayInit(i));
    }
    for (int i = 0; i < trDArrayGetSize(trDArray); i++)
    {
        printf("%c ", trDArrayGetElement(trDArray,i)->data);
    }



//    trDArrayDeleteAll(&trDArray);
//    printf("%s\n", trDArray == NULL ? "NULL" : "Tidak Otomatis");

//    int *p = (int*)malloc(sizeof(int) * 8);
//    printf("%s\n", p == NULL ? "NULL" : "Tidak Otomatis");
    return 0;
}
