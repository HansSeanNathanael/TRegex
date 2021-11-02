#include "stdio.h"
#include "stdlib.h"
#include "string.h"

enum {NORMAL, START, END, EMPTY, NUMBERS, LETTERS, ANYTHING, SYMBOL};

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
    int type;
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

int trDArrayGetType(TRDArray *array)
{
    return array->type;
}

/**
    function to initialize TRDArray with state data
    @param data : data of current state
    @return the initialized of TRDArray
*/
TRDArray *trDArrayInit(char data, int type)
{
    TRDArray *trDArray = (TRDArray*)malloc(sizeof(TRDArray));
    trDArray->data = data;
    trDArray->size = 0;
    trDArray->capacity = 2;
    trDArray->nextState = (TRDArray**)malloc(sizeof(TRDArray*) * trDArrayGetCapacity(trDArray));
    trDArray->visited = 0;
    trDArray->type = type;
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
    Function to pop last element from next-state-array
    @param trDArray : array that will be popped
*/
void trDArrayPop(TRDArray *trDArray)
{
    if (trDArrayGetSize(trDArray) > 0)
    {
        trDArray->size -= 1;
    }
}

/**
    Function to empty next-state-array
    @param trDArray : array that will be emptied
*/
void trDArrayMakeEmpty(TRDArray *trDArray)
{
    while(trDArrayGetSize(trDArray) > 0)
    {
        trDArray->size -= 1;
    }
}

/**
    Function to insert all element of any next-state-array to other next-state-array
    @param destination : destination of insertion
    @param source : array that it's element will be inserted to other array
*/
void trDArrayInsertAll(TRDArray *destionaton, TRDArray *source)
{
    for (int i = 0; i < trDArrayGetSize(source); i++)
    {
        trDArrayPush(destionaton, trDArrayGetElement(source, i));
    }
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
    TRDArray *garbageCollector = trDArrayInit((char)0, EMPTY);
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

/**
    function to set element in specific index of next-state-array
    @param array : array that will be accessed
    @param newData : new data that will be set to specific index
    @param index : array index that will be changed
*/
void trDArraySetElement(TRDArray *array, TRDArray *newData, int index)
{
    if (index < trDArrayGetSize(array))
    {
        array->nextState[index] = newData;
    }
}

/**
    struct for regex
    @attribute code : regular expression string notation
    @attribute compiled : 0 if not compiled, 1 if compiled (compiled means graph already built from string notation)
    @attribute startingState : start state for graph
*/
typedef struct TRegex
{
    char code[255];
    int compiled;
    TRDArray *startingState;
} TRegex;

/**
    Function to initialize TRegex with string notation
    @param code : regular expression string notation to initialized TRegex
    @return initialized regex
*/
TRegex tRegexInitCode(char *code)
{
    TRegex regex;
    regex.compiled = 0;
    regex.startingState = trDArrayInit(0, EMPTY);
    strcpy(regex.code, code);
    return regex;
}

/**
    Function to initialized TRegex without string notation
    @return initialized regex without regex string notation
*/
TRegex tRegexInit()
{
    TRegex regex;
    regex.compiled = 0;
    regex.startingState = trDArrayInit(0, START);
    strcpy(regex.code, "");
    return regex;
}

/**
    Function to tell if TRegex already compiled
    @return 1 if already compiled, 0 if not compiled
*/
int tRegexIsCompiled(TRegex *regex)
{
    return regex->compiled;
}

/**
    Function to set/change code of TRegex (regex string notation)
    @param regex : regex that it's code will be changed
    @param code : new regex string notation
*/
void tRegexSetCode(TRegex *regex, char *code)
{
    regex->compiled = 0;
    trDArrayDeleteAll(&(regex->startingState)); // delete existing graph if already compiled
    regex->startingState = trDArrayInit(0, START);
    strcpy(regex->code, code);
}

/**
    Function to decode regex's notation to graph for regex
    @param startState : array to store start state of sub graph
    @param endState : array to store end state of sub graph
    @param list : array that contain all graph state from converted code
    @param pos : index of list to be converted
*/
void tRegexCompileFuncRec(TRDArray *startState, TRDArray *endState, TRDArray *list, int *pos)
{
//    printf("%d\n", *pos);
    int bound[100]; // to save upper and lower bound of one state in endstate
    int size = 0;
    int push = 1;

    while(*pos < trDArrayGetSize(list) && (trDArrayGetData(trDArrayGetElement(list, *pos)) != ')' || trDArrayGetType(trDArrayGetElement(list, *pos)) != SYMBOL))
    {
        if (trDArrayGetData(trDArrayGetElement(list, *pos)) == '(' && trDArrayGetType(trDArrayGetElement(list, *pos)) == SYMBOL)
        {
            *pos += 1;

            int type = 0;

            TRDArray *startStateForThis = trDArrayInit(0, EMPTY);
            TRDArray *endStateForThis = trDArrayInit(0, EMPTY);
            tRegexCompileFuncRec(startStateForThis, endStateForThis, list, pos);

            if (*pos + 1 < trDArrayGetSize(list))
            {
                if (trDArrayGetData(trDArrayGetElement(list, *pos + 1)) == '+' && trDArrayGetType(trDArrayGetElement(list, *pos + 1)) == SYMBOL)
                {
                    type = 1;
                }
                else if (trDArrayGetData(trDArrayGetElement(list, *pos + 1)) == '*' && trDArrayGetType(trDArrayGetElement(list, *pos + 1)) == SYMBOL)
                {
                    type = 2;
                }
                else if (trDArrayGetData(trDArrayGetElement(list, *pos + 1)) == '?' && trDArrayGetType(trDArrayGetElement(list, *pos + 1)) == SYMBOL)
                {
                    type = 3;
                }
                else
                {
                    type = 0;
                }
            }

//            for (int j = 0; j < trDArrayGetSize(startState); j++)
//            {
//                printf("char: %c\n", trDArrayGetData(trDArrayGetElement(startStateForThis, j)));
//            }

            if (push == 1)
            {
                push = 0;
                size += 1;


                for (int j = 0; j < trDArrayGetSize(startStateForThis); j++)
                {
                    trDArrayPush(startState, trDArrayGetElement(startStateForThis, j));
                }

                bound[size-1] = trDArrayGetSize(endState);

                if (type == 1)
                {
                    for (int j = 0; j < trDArrayGetSize(endStateForThis); j++)
                    {
                        trDArrayPush(endState, trDArrayGetElement(endStateForThis, j));

                        for (int k = 0; k < trDArrayGetSize(startStateForThis); k++)
                        {
                            trDArrayPush(trDArrayGetElement(endStateForThis, j), trDArrayGetElement(startStateForThis, k));
                        }
                    }

                    *pos += 1;

                }
                else if (type == 2)
                {
                    for (int j = 0; j < trDArrayGetSize(endStateForThis); j++)
                    {
                        trDArrayPush(endState, trDArrayGetElement(endStateForThis, j));

                        for (int k = 0; k < trDArrayGetSize(startStateForThis); k++)
                        {
                            trDArrayPush(trDArrayGetElement(endStateForThis, j), trDArrayGetElement(startStateForThis, k));
                        }
                    }

                    TRDArray *empty = trDArrayInit(0, EMPTY);
                    trDArrayPush(startState, empty);
                    trDArrayPush(endState, empty);

                    *pos += 1;

                }
                else if (type == 3)
                {
                    for (int j = 0; j < trDArrayGetSize(endStateForThis); j++)
                    {
                        trDArrayPush(endState, trDArrayGetElement(endStateForThis, j));
                    }

                    TRDArray *empty = trDArrayInit(0, EMPTY);
                    trDArrayPush(startState, empty);
                    trDArrayPush(endState, empty);

                    *pos += 1;

                }
                else
                {
                    for (int j = 0; j < trDArrayGetSize(endStateForThis); j++)
                    {
                        trDArrayPush(endState, trDArrayGetElement(endStateForThis, j));
                    }

                }

                bound[size] = trDArrayGetSize(endState);
            }
            else
            {
                if (type == 1)
                {

                    for (int j = bound[size] - 1; j >= bound[size-1]; j--)
                    {
                        for (int k = 0; k < trDArrayGetSize(startStateForThis); k++)
                        {
                            trDArrayPush(trDArrayGetElement(endState, j), trDArrayGetElement(startStateForThis, k));
                        }
                        trDArrayPop(endState);
                    }

                    for (int j = 0; j < trDArrayGetSize(endStateForThis); j++)
                    {
                        trDArrayPush(endState, trDArrayGetElement(endStateForThis, j));
                        for (int k = 0; k < trDArrayGetSize(startStateForThis); k++)
                        {
                            trDArrayPush(trDArrayGetElement(endStateForThis, j), trDArrayGetElement(startStateForThis, k));
                        }
                    }
                    bound[size] = bound[size-1] + trDArrayGetSize(endStateForThis);
                    *pos += 1;
                }
                else if (type == 2)
                {
                    TRDArray *empty = trDArrayInit(0, EMPTY);

                    for (int j = bound[size] - 1; j >= bound[size-1]; j--)
                    {
                        for (int k = 0; k < trDArrayGetSize(startStateForThis); k++)
                        {
                            trDArrayPush(trDArrayGetElement(endState, j), trDArrayGetElement(startStateForThis, k));
                        }
                        trDArrayPush(trDArrayGetElement(endState, j), empty);
                        trDArrayPop(endState);
                    }

                    for (int j = 0; j < trDArrayGetSize(endStateForThis); j++)
                    {
                        trDArrayPush(endState, trDArrayGetElement(endStateForThis, j));
                        for (int k = 0; k < trDArrayGetSize(startStateForThis); k++)
                        {
                            trDArrayPush(trDArrayGetElement(endStateForThis, j), trDArrayGetElement(startStateForThis, k));
                        }
                    }

                    trDArrayPush(endState, empty);
                    bound[size] = bound[size-1] + trDArrayGetSize(endStateForThis) + 1;

                    *pos += 1;
                }
                else if (type == 3)
                {

                    TRDArray *empty = trDArrayInit(0, EMPTY);

                    for (int j = bound[size] - 1; j >= bound[size - 1]; j--)
                    {
                        for (int k = 0; k < trDArrayGetSize(startStateForThis); k++)
                        {
                            trDArrayPush(trDArrayGetElement(endState, j), trDArrayGetElement(startStateForThis, k));
                        }
                        trDArrayPush(trDArrayGetElement(endState, j), empty);
                        trDArrayPop(endState);
                    }

                    for (int j = 0; j < trDArrayGetSize(endStateForThis); j++)
                    {
                        trDArrayPush(endState, trDArrayGetElement(endStateForThis, j));
                    }

                    trDArrayPush(endState, empty);
                    bound[size] = bound[size-1] + trDArrayGetSize(endStateForThis) + 1;
                    *pos += 1;
                }
                else
                {
                    for (int j = bound[size] - 1; j >= bound[size-1]; j--)
                    {
                        for (int k = 0; k < trDArrayGetSize(startStateForThis); k++)
                        {
                            trDArrayPush(trDArrayGetElement(endState, j), trDArrayGetElement(startStateForThis, k));
                        }
                        trDArrayPop(endState);
                    }
                    for (int j = 0; j < trDArrayGetSize(endStateForThis); j++)
                    {
                        trDArrayPush(endState, trDArrayGetElement(endStateForThis, j));
                    }
                    bound[size] = bound[size-1] + trDArrayGetSize(endStateForThis);
                }
            }

            trDArrayDelete(startStateForThis);
            trDArrayDelete(endStateForThis);
        }
        else if (trDArrayGetData(trDArrayGetElement(list, *pos)) == '|' && trDArrayGetType(trDArrayGetElement(list, *pos)) == SYMBOL)
        {
            if (push == 1)
            {
                size += 1;
                TRDArray *empty = trDArrayInit(0, EMPTY);
                trDArrayPush(startState, empty);
                trDArrayPush(endState, empty);
                bound[size - 1] = trDArrayGetSize(endState) - 1;
                bound[size] = trDArrayGetSize(endState);
            }
//            printf("| : %d\n", *pos);
            push = 1;
            // broken here if "a|(bc)"
            // | followed with ( break the function
        }
        else
        {
            int type = 0;
            if (*pos + 1< trDArrayGetSize(list))
            {
                if (trDArrayGetData(trDArrayGetElement(list, *pos + 1)) == '+' && trDArrayGetType(trDArrayGetElement(list, *pos + 1)) == SYMBOL)
                {
                    type = 1;
                }
                else if (trDArrayGetData(trDArrayGetElement(list, *pos + 1)) == '*' && trDArrayGetType(trDArrayGetElement(list, *pos + 1)) == SYMBOL)
                {
                    type = 2;
                }
                else if (trDArrayGetData(trDArrayGetElement(list, *pos + 1)) == '?' && trDArrayGetType(trDArrayGetElement(list, *pos + 1)) == SYMBOL)
                {
                    type = 3;
                }
            }
//            TRDArray *prevState = trDArrayInit(0);
            if (push == 1)
            {
                //push new state
                push = 0;
                size += 1;
//                printf("PUSH\n");
                trDArrayPush(startState, trDArrayGetElement(list, *pos));
                trDArrayPush(endState, trDArrayGetElement(list, *pos));
                bound[size - 1] = trDArrayGetSize(endState) - 1;
                bound[size] = trDArrayGetSize(endState);

                if (type == 1)
                {
                    trDArrayPush(trDArrayGetElement(endState, bound[size-1]), trDArrayGetElement(list, *pos));
                    *pos += 1;
                }
                else if (type == 2)
                {
                    trDArrayPush(trDArrayGetElement(endState, bound[size-1]), trDArrayGetElement(list, *pos));
                    TRDArray *empty = trDArrayInit(0, EMPTY);
                    trDArrayPush(startState, empty);
                    trDArrayPush(endState, empty);
                    bound[size - 1] = trDArrayGetSize(endState) - 1;
                    bound[size] = bound[size - 1] + 1;
                    *pos += 1;
                }
                else if (type == 3)
                {
                    TRDArray *empty = trDArrayInit(0, EMPTY);
                    trDArrayPush(startState, empty);
                    trDArrayPush(endState, empty);
                    bound[size - 1] = trDArrayGetSize(endState) - 1;
                    bound[size] = bound[size - 1] + 1;
                    *pos += 1;
                }
            }
            else
            {
                // connecting new state to group of endstate
                if (type == 0)
                {
                    for (int j = bound[size] - 1; j >= bound[size-1]; j--)
                    {
                        trDArrayPush(trDArrayGetElement(endState, j), trDArrayGetElement(list, *pos));
                        trDArrayPop(endState);
                    }

                    trDArrayPush(endState, trDArrayGetElement(list, *pos));
                    bound[size] = bound[size-1] + 1;
                }
                else if (type == 1)
                {
                    for (int j = bound[size] - 1; j >= bound[size-1]; j--)
                    {
                        trDArrayPush(trDArrayGetElement(endState, j), trDArrayGetElement(list, *pos));
                        trDArrayPop(endState);
                    }

                    trDArrayPush(endState, trDArrayGetElement(list, *pos));
                    bound[size] = bound[size-1] + 1;

                    trDArrayPush(trDArrayGetElement(endState, bound[size-1]), trDArrayGetElement(list, *pos));

                    *pos += 1;
                }
                else if (type == 2)
                {
                    TRDArray *empty = trDArrayInit(0, EMPTY);
                    for (int j = bound[size] - 1; j >= bound[size-1]; j--)
                    {
                        trDArrayPush(trDArrayGetElement(endState, j), trDArrayGetElement(list, *pos));
                        trDArrayPush(trDArrayGetElement(endState, j), empty);
                        trDArrayPop(endState);
                    }

                    trDArrayPush(trDArrayGetElement(list, *pos), trDArrayGetElement(list, *pos));

                    trDArrayPush(endState, trDArrayGetElement(list, *pos));
                    trDArrayPush(endState, empty);
                    bound[size] = bound[size-1] + 2;

                    *pos += 1;
                }
                else if (type == 3)
                {
                    TRDArray *empty = trDArrayInit(0, EMPTY);

                    for (int j = bound[size] - 1; j >= bound[size-1]; j--)
                    {
                        trDArrayPush(trDArrayGetElement(endState, j), trDArrayGetElement(list, *pos));
                        trDArrayPush(trDArrayGetElement(endState, j), empty);
                        trDArrayPop(endState);
                    }

                    trDArrayPush(endState, trDArrayGetElement(list, *pos));
                    trDArrayPush(endState, empty);
                    bound[size] = bound[size-1] + 2;

                    *pos += 1;
                }
            }
//            trDArrayDelete(prevState);
        }
        *pos += 1;
    }
    if (push == 1)
    {
        size += 1;
        push = 0;
        TRDArray *empty = trDArrayInit(0, EMPTY);
        trDArrayPush(startState, empty);
        trDArrayPush(endState, empty);
        bound[size - 1] = trDArrayGetSize(endState) - 1;
        bound[size] = trDArrayGetSize(endState);
    }
//    for (int j = 0; j < trDArrayGetSize(startState); j++)
//    {
//        printf("%c ", trDArrayGetData(trDArrayGetElement(startState, j)));
//    }
}

/**
    Recursive function to check all graph by traversal depth first
    @param startingState : location of current state
    @param string : string to be checked
    @param pos : position of string to be checked
    @param length : length of string
    @return 1 if there's minimum 1 pattern match, 0 if no patter match
*/
int tRegexCompareFuncRec(TRDArray *startingState, char *string, int pos, int length)
{
//    printf("%c ", trDArrayGetData(startingState));
//    printf("%d ", pos);

    if (pos == length && trDArrayGetType(startingState) != START && trDArrayGetType(startingState) != EMPTY)
    {
        if (trDArrayGetType(startingState) == END)
        {
            return 1;
        }
        return 0;
    }
    else if (trDArrayGetType(startingState) == EMPTY || trDArrayGetType(startingState) == START)
    {
        int result = 0;
        for (int i = 0; i < trDArrayGetSize(startingState); i++)
        {
            result = tRegexCompareFuncRec(trDArrayGetElement(startingState, i), string, pos, length);
            if (result == 1)
            {
                return result;
            }
        }
        return 0;
    }
    else if (trDArrayGetType(startingState) == NUMBERS)
    {
        if (string[pos] >= 48 && string[pos] <= 57)
        {
            int result = 0;
            for (int i = 0; i < trDArrayGetSize(startingState); i++)
            {
                result = tRegexCompareFuncRec(trDArrayGetElement(startingState, i), string, pos+1, length);
                if (result == 1)
                {
                    return result;
                }
            }
        }
        else
        {
            return 0;
        }
    }
    else if (trDArrayGetType(startingState) == LETTERS)
    {
        if ((string[pos] >= 65 && string[pos] <= 90) || (string[pos] >= 97 && string[pos] <= 122) || string[pos] == '_')
        {
            int result = 0;
            for (int i = 0; i < trDArrayGetSize(startingState); i++)
            {
                result = tRegexCompareFuncRec(trDArrayGetElement(startingState, i), string, pos+1, length);
                if (result == 1)
                {
                    return result;
                }
            }
        }
        else
        {
            return 0;
        }
    }
    else if (trDArrayGetData(startingState) == string[pos] || trDArrayGetType(startingState) == ANYTHING)
    {
        int result = 0;

//        printf("pos: %c\n", string[pos]);
        for (int i = 0; i < trDArrayGetSize(startingState); i++)
        {
            result = tRegexCompareFuncRec(trDArrayGetElement(startingState, i), string, pos+1, length);
            if (result == 1)
            {
                return result;
            }
        }
        return 0;
    }
    return 0;
}

/**
    Function to compare pattern of string with regex
    @param regex : regex that will used
    @param string : string to check is it match with regex
    @return 1 if there's minimum 1 pattern match, 0 if no patter match
*/
int tRegexComparePattern(TRegex regex, char *string)
{
    return tRegexCompareFuncRec(regex.startingState, string, 0, strlen(string));
}


/**
    Function to convert code of regex's notation to state node
    @param code : code that will be converted
    @param graphNode : dynamic array of node to store all state
*/
void convertCode(char *code, TRDArray *graphNode)
{
    int position = 0;
    int length = strlen(code);
    int bracket = 0;
    while(position < length)
    {
        if (code[position] == '\\')
        {
            position += 1;
            if (code[position] == 'w')
            {
                trDArrayPush(graphNode, trDArrayInit('w', LETTERS));
            }
            else if (code[position] == 'd')
            {
                trDArrayPush(graphNode, trDArrayInit('d', NUMBERS));
            }
            else
            {
                trDArrayPush(graphNode, trDArrayInit(code[position], NORMAL));
            }
        }
        else if (code[position] == '(' || code[position] == ')' || code[position] == '*' || code[position] == '|' || code[position] == '+' || code[position] == '?')
        {
            trDArrayPush(graphNode, trDArrayInit(code[position], SYMBOL));
        }
        else if (code[position] == '.')
        {
            trDArrayPush(graphNode, trDArrayInit(code[position], ANYTHING));
        }
        else
        {
            trDArrayPush(graphNode, trDArrayInit(code[position], NORMAL));
        }

        position++;
    }
}

/**
    Function to compile regex graph from regex string notation
    @param regex : regex to be compiled
*/
void tRegexCompile(TRegex *regex)
{
    if (tRegexIsCompiled(regex) == 0)
    {
        char appendedCode[257] = "";
        strcpy(appendedCode, "(");
        strcat(appendedCode, regex->code);
        strcat(appendedCode, ")");

        TRDArray *allGraphNode = trDArrayInit(0, EMPTY);
        convertCode(appendedCode, allGraphNode);

//        TRDArray *currentState = trDArrayInit(0);
//        TRDArray *nextState = trDArrayInit(0);
//        TRDArray *endState = trDArrayInit(0);
//        trDArrayPush(currentState, regex->startingState);
//        int start = 0;
//        dummy2(currentState, nextState, endState, allGraphNode, &start);

        TRDArray *nextState = trDArrayInit(0, EMPTY);
        TRDArray *endState = trDArrayInit(0, EMPTY);
//        trDArrayPush(endState, regex->startingState);
        int start = 0;
        tRegexCompileFuncRec(nextState, endState, allGraphNode, &start);
        for (int i = 0; i < trDArrayGetSize(nextState); i++)
        {
            trDArrayPush(regex->startingState, trDArrayGetElement(nextState, i));
        }

        TRDArray *endStateNode = trDArrayInit(0, END);
        for (int i = 0; i < trDArrayGetSize(endState); i++)
        {
            trDArrayPush(trDArrayGetElement(endState, i), endStateNode);
        }


        //int pos = 0;
        //tRegexCompileFuncRec(regex->startingState, appendedCode, &pos, strlen(appendedCode));

        trDArrayDelete(allGraphNode);
        regex->compiled = 1;
    }
}

int main()
{
    TRDArray *trDArray = trDArrayInit('a', NORMAL);
//    for (int i = 33; i < 61; i++)
//    {
//        trDArrayPush(trDArray, trDArrayInit(i));
//    }
//    for (int i = 0; i < trDArrayGetSize(trDArray); i++)
//    {
//        printf("%c ", trDArrayGetElement(trDArray,i)->data);
//    }

    TRegex regex = tRegexInit();
//    TRDArray *a = trDArrayInit('a');
//    TRDArray *b = trDArrayInit('b');
//    TRDArray *c = trDArrayInit('c');
//    TRDArray *d = trDArrayInit('d');
//    TRDArray *e = trDArrayInit('e');
//    TRDArray *f = trDArrayInit('f');
//    trDArrayPush(c, f);
//    trDArrayPush(c, b);
//    trDArrayPush(e, f);
//    trDArrayPush(b, c);
//    trDArrayPush(d, e);
//    trDArrayPush(a, b);
//    trDArrayPush(a, d);

    // Tes DummyTest (traversal graph to check regex)
//    trDArrayPush(regex.startingState, a);
//    printf("%s\n", dummyTest(regex.startingState, "abcf", -1, 4) == 1 ? "True" : "False");

//    // @Test 1
//    tRegexSetCode(&regex, "(abcd|ef(gh|ij))");
//    tRegexCompile(&regex);
//    printf("%s\n", tRegexComparePattern(regex, "efij") == 1 ? "True" : "False");

    // @Test 2
    tRegexSetCode(&regex, "(a(b(c(d))))");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "abcd") == 1 ? "True" : "False");

    // @Test 3
    tRegexSetCode(&regex, "((((d)c)b)a)");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "dcba") == 1 ? "True" : "False");

    // @Test 4
    tRegexSetCode(&regex, "abc(d(ef)g)h");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "abcdefgh") == 1 ? "True" : "False");

    // @Test 5
    tRegexSetCode(&regex, "abc(d(ef|gh)(ij|kl))mn");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "abcdghklmn") == 1 ? "True" : "False");

    // @Test 6
    tRegexSetCode(&regex, "(ab|cd|ef(ef|gh)(kl|mnop|qe|okokdso))|zx");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "efghmnop") == 1 ? "True" : "False");

    // @Test 7 (Wrong result, zx not in graph) FIXED! maybe....
    tRegexSetCode(&regex, "(ab|cd|ef(ef|gh)(kl|mnop|qe|okokdso))|zx");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "zx") == 1 ? "True" : "False");

    // @Test 8 (Broken, make application crash because '|' followed by '(' )
    tRegexSetCode(&regex, "(ab|cd|ef|(ef|gh)(kl|mnop|qe|okokdso))|zx");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "zx") == 1 ? "True" : "False");
    printf("%s\n", tRegexComparePattern(regex, "ghokokdso") == 1 ? "True" : "False");

    // @Test 9
    tRegexSetCode(&regex, "employ(er|ee|ment|ing|able|)");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "employer") == 1 ? "True" : "False");
    printf("%s\n", tRegexComparePattern(regex, "employee") == 1 ? "True" : "False");
    printf("%s\n", tRegexComparePattern(regex, "employment") == 1 ? "True" : "False");
    printf("%s\n", tRegexComparePattern(regex, "employable") == 1 ? "True" : "False");
    printf("%s\n", tRegexComparePattern(regex, "employ") == 1 ? "True" : "False"); //still broken FIXED! maybe...

    TRDArray *a = trDArrayGetElement(trDArrayGetElement(regex.startingState, 0), 0);

    // @Test 10
    tRegexSetCode(&regex, "(||(||a)|)");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "a") == 1 ? "True" : "False");

    // @Test 11
    tRegexSetCode(&regex, "a|(|b)");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "a") == 1 ? "True" : "False");

    // @Test 12
    tRegexSetCode(&regex, "a+");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "aaaaaa") == 1 ? "True" : "False");

    // @Test 13
    tRegexSetCode(&regex, "ab*c");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "abbbbbc") == 1 ? "True" : "False");

    // @Test 14
    tRegexSetCode(&regex, "ab*c*");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "a") == 1 ? "True" : "False");

    // @Test 15
    tRegexSetCode(&regex, "ab*cd*efg");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "acefg") == 1 ? "True" : "False");

    // @Test 16
    tRegexSetCode(&regex, "ab*cd*efg");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "aefg") == 1 ? "True (seharusnya false)" : "False (memang hasil harus false)");

    tRegexSetCode(&regex, "\\w+@\\w+\\.com");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "hans_sean@gmail.com") == 1 ? "True" : "False");

    tRegexSetCode(&regex, "h.t");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "hit") == 1 ? "True" : "False");

    tRegexSetCode(&regex, "a(h|l)*k");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "ahllhlhhhllhk") == 1 ? "True" : "False");

    tRegexSetCode(&regex, "ah*o");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "ao") == 1 ? "True" : "False");

    tRegexSetCode(&regex, "colou?r");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "color") == 1 ? "True" : "False");
    printf("%s\n", tRegexComparePattern(regex, "colour") == 1 ? "True" : "False");

    tRegexSetCode(&regex, "\\w+(\\w|\\.|\\d)*@\\w+\\.com");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "hans.sean22@gmail.com") == 1 ? "True" : "False");


    tRegexSetCode(&regex, "\\w+(\\w|\\.|\\d)*@\\w+\\.(\\w+\\.)*\\w+");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "hans.sean22@mhsits.ac.id") == 1 ? "True" : "False");

//    tRegexSetCode(&regex, "\\w+(\\w|\\.)*@\\w+\\.com");
//    tRegexCompile(&regex);
//    printf("%s\n", tRegexComparePattern(regex, "hans.sean@gmail.com") == 1 ? "True" : "False");

//    trDArrayDeleteAll(&trDArray);
//    printf("%s\n", trDArray == NULL ? "NULL" : "Tidak Otomatis");

//    int *p = (int*)malloc(sizeof(int) * 8);
//    printf("%s\n", p == NULL ? "NULL" : "Tidak Otomatis");
    return 0;
}
