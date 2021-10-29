#include "stdio.h"
#include "stdlib.h"
#include "string.h"

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
    regex.startingState = trDArrayInit(0);
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
    regex.startingState = trDArrayInit(0);
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
    regex->startingState = trDArrayInit(0);
    strcpy(regex->code, code);
}

//TRDArray *tRegexCompileFuncRec2(TRDArray *parentNode, char *code, int *pos, int length)
//{
//    TRDArray *currentNode;
//    if (code[*pos] != '(' && code[*pos] != ')' && code[*pos] != '(')
//    {
//        currentNode = trDArrayInit(code[*pos]);
//
//    }
//}
//
//void tRegexCompileFuncRec(TRDArray *parentNode, char *code, int *pos, int length)
//{
//    if (code[*pos] != ')' && *pos < length)
//    {
//        if (code[*pos] == '(')
//        {
//            *pos += 1;
//            while(code[*pos] != ')')
//            {
//                tRegexCompileFuncRec(parentNode, code, pos, length);
//            }
//        }
//        else
//        {
////            TRDArray *currentNode = trDArrayInit(code[*pos]);
////            *pos += 1;
////            trDArrayPush(currentNode, tRegexCompileFuncRec(code, pos, length));
////            return currentNode;
////
//
//            TRDArray *currentNode = trDArrayInit(code[*pos]);
//            trDArrayPush(parentNode, currentNode);
//            *pos += 1;
//            tRegexCompileFuncRec(currentNode, code, pos, length);
//        }
//    }
//}
//
//void dummy(TRDArray *currentState, TRDArray *startState, TRDArray *endState, TRDArray *list, int *pos)
//{
//    printf("%d\n", *pos);
//    if (*pos >= trDArrayGetSize(list))
//    {
//        return;
//    }
//    if (trDArrayGetData(trDArrayGetElement(list, *pos)) == '(')
//    {
//        *pos += 1;
//        TRDArray *endStateForThis = trDArrayInit(0);
//        TRDArray *startStateForThis = trDArrayInit(0);
//        dummy(currentState, startStateForThis, endStateForThis, list, pos);
//        for (int i = 0; i < trDArrayGetSize(currentState); i++)
//        {
//            for (int j = 0; j < trDArrayGetSize(startStateForThis); j++)
//            {
//                trDArrayPush(trDArrayGetElement(currentState, i), trDArrayGetElement(startStateForThis, j));
//            }
//        }
//        trDArrayMakeEmpty(currentState);
//        trDArrayInsertAll(currentState, endStateForThis);
//        *pos += 1;
//        trDArrayMakeEmpty(startStateForThis);
//        trDArrayMakeEmpty(endStateForThis);
//
//        dummy(currentState, startStateForThis, endStateForThis, list, pos);
//        trDArrayDelete(startStateForThis);
//        trDArrayDelete(endStateForThis);
//
//
//    }
//    else if (trDArrayGetData(trDArrayGetElement(list, *pos)) == ')')
//    {
//        *pos += 1;
//        return;
//    }
//    else if (trDArrayGetData(trDArrayGetElement(list, *pos)) == '|')
//    {
//
//    }
//    else
//    {
//        trDArrayPush(startState, trDArrayGetElement(list, *pos));
//        TRDArray *walker = trDArrayGetElement(list, *pos);
//        *pos += 1;
//        for (int i = *pos; i < trDArrayGetSize(list); i++)
//        {
//            if (trDArrayGetData(trDArrayGetElement(list, i)) == '(')
//            {
//
//            }
//            else if (trDArrayGetData(trDArrayGetElement(list, i)) == ')')
//            {
//                *pos = i;
//                return;
//            }
//            else if (trDArrayGetData(trDArrayGetElement(list, i)) == '|')
//            {
//
//            }
//            else
//            {
//                trDArrayPush(walker, trDArrayGetElement(list, i));
//                walker= trDArrayGetElement(list, i);
//            }
//            *pos = i;
//            trDArrayPush(endState, walker);
//        }
//    }
////
////    trDArrayDelete(startStateForThis);
//
////    for (int i = *pos; i < trDArrayGetSize(list) -1; i++)
////    {
////        if (trDArrayGetData(trDArrayGetElement(list, i)) == '(')
////        {
////            trDArrayPush(stack, trDArrayGetElement(list, i));
////        }
////        else if (trDArrayGetData(trDArrayGetElement(list, i)) == ')')
////        {
////
////        }
////        else if (trDArrayGetData(trDArrayGetElement(list, i)) == '|')
////        {
////            for (int i = trDArrayGetSize(stack); i >= 0; i++)
////            {
////                char data = trDArrayGetData(trDArrayGetElement(stack, i));
////                if (data != '(')
////                {
////                    trDArrayPush(trDArrayGetElement(stack, i), trDArrayGetElement(list, i));
////                    break;
////                }
////            }
////        }
////        else
////        {
////
////        }
////    }
////
////    trDArrayDelete(currentState);
////    trDArrayDelete(stack);
//}

/**
    Still not finished, function to build regex graph
*/
void dummy2(TRDArray *startState, TRDArray *endState, TRDArray *list, int *pos)
{
    printf("%d\n", *pos);
    int bound[100];
    int size = 0;
    int push = 1;

    while(*pos < trDArrayGetSize(list) && trDArrayGetData(trDArrayGetElement(list, *pos)) != ')')
    {
        if (trDArrayGetData(trDArrayGetElement(list, *pos)) == '(')
        {
            *pos += 1;
            TRDArray *startStateForThis = trDArrayInit(0);
            TRDArray *endStateForThis = trDArrayInit(0);
            dummy2(startStateForThis, endStateForThis, list, pos);

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
                for (int j = 0; j < trDArrayGetSize(endStateForThis); j++)
                {
                    trDArrayPush(endState, trDArrayGetElement(endStateForThis, j));
                }
                bound[size] = trDArrayGetSize(endState);
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

            trDArrayDelete(startStateForThis);
            trDArrayDelete(endStateForThis);
        }
        else if (trDArrayGetData(trDArrayGetElement(list, *pos)) == '|')
        {
            push = 1;
            printf("| : %d\n", *pos);
            // broken here if "a|(bc)"
            // | followed with ( break the function
        }
        else
        {
            if (push == 1)
            {
                push = 0;
                size += 1;
//                printf("PUSH\n");
                trDArrayPush(startState, trDArrayGetElement(list, *pos));
                trDArrayPush(endState, trDArrayGetElement(list, *pos));
                bound[size - 1] = trDArrayGetSize(endState) - 1;
                bound[size] = trDArrayGetSize(endState);
            }
            else
            {
                for (int j = bound[size] - 1; j >= bound[size-1]; j--)
                {
                    trDArrayPush(trDArrayGetElement(endState, j), trDArrayGetElement(list, *pos));
                    trDArrayPop(endState);
                }
                trDArrayPush(endState, trDArrayGetElement(list, *pos));
                bound[size] = bound[size-1] + 1;
            }
        }
        *pos += 1;
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
    printf("%c ", trDArrayGetData(startingState));
    if (trDArrayGetData(startingState) == string[pos] || trDArrayGetData(startingState) == 0)
    {
        int result = 0;
        if (trDArrayGetSize(startingState) == 0)
        {
            if (pos == length -1)
            {
                return 1;
            }
            return 0;
        }
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
    @return 1 if there's minimum 1 pattern match, 0 if no patter match
*/
int tRegexComparePattern(TRegex regex, char *string)
{
    return tRegexCompareFuncRec(regex.startingState, string, -1, strlen(string));
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

        TRDArray *allGraphNode = trDArrayInit(0);
        for (int i = 0; i < strlen(appendedCode); i++)
        {
            trDArrayPush(allGraphNode, trDArrayInit(appendedCode[i]));
        }
//        TRDArray *currentState = trDArrayInit(0);
//        TRDArray *nextState = trDArrayInit(0);
//        TRDArray *endState = trDArrayInit(0);
//        trDArrayPush(currentState, regex->startingState);
//        int start = 0;
//        dummy2(currentState, nextState, endState, allGraphNode, &start);

        TRDArray *nextState = trDArrayInit(0);
        TRDArray *endState = trDArrayInit(0);
//        trDArrayPush(endState, regex->startingState);
        int start = 0;
        dummy2(nextState, endState, allGraphNode, &start);
        for (int i = 0; i < trDArrayGetSize(nextState); i++)
        {
            trDArrayPush(regex->startingState, trDArrayGetElement(nextState, i));
        }


        //int pos = 0;
        //tRegexCompileFuncRec(regex->startingState, appendedCode, &pos, strlen(appendedCode));

        trDArrayDelete(allGraphNode);
        regex->compiled = 1;
    }
}

int main()
{
    TRDArray *trDArray = trDArrayInit('a');
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

//    // @Test 2
//    tRegexSetCode(&regex, "(a(b(c(d))))");
//    tRegexCompile(&regex);
//    printf("%s\n", tRegexComparePattern(regex, "abcd") == 1 ? "True" : "False");

//    // @Test 3
//    tRegexSetCode(&regex, "((((d)c)b)a)");
//    tRegexCompile(&regex);
//    printf("%s\n", tRegexComparePattern(regex, "dcba") == 1 ? "True" : "False");

//    // @Test 4
//    tRegexSetCode(&regex, "abc(d(ef)g)h");
//    tRegexCompile(&regex);
//    printf("%s\n", tRegexComparePattern(regex, "abcdefgh") == 1 ? "True" : "False");

//    // @Test 5
//    tRegexSetCode(&regex, "abc(d(ef|gh)(ij|kl))mn");
//    tRegexCompile(&regex);
//    printf("%s\n", tRegexComparePattern(regex, "abcdghklmn") == 1 ? "True" : "False");

//    // @Test 6
//    tRegexSetCode(&regex, "(ab|cd|ef(ef|gh)(kl|mnop|qe|okokdso))|zx");
//    tRegexCompile(&regex);
//    printf("%s\n", tRegexComparePattern(regex, "efghmnop") == 1 ? "True" : "False");

//    // @Test 7 (Wrong result, zx not in graph) FIXED! maybe....
//    tRegexSetCode(&regex, "(ab|cd|ef(ef|gh)(kl|mnop|qe|okokdso))|zx");
//    tRegexCompile(&regex);
//    printf("%s\n", tRegexComparePattern(regex, "zx") == 1 ? "True" : "False");

//    // @Test 8 (Broken, make application crash because '|' followed by '(' )
//    tRegexSetCode(&regex, "(ab|cd|ef|(ef|gh)(kl|mnop|qe|okokdso))|zx");
//    tRegexCompile(&regex);
//    printf("%s\n", tRegexComparePattern(regex, "zx") == 1 ? "True" : "False");

    // @Test 9
    tRegexSetCode(&regex, "employ(|er|ee|ment|ing|able)");
    tRegexCompile(&regex);
    printf("%s\n", tRegexComparePattern(regex, "employer") == 1 ? "True" : "False");
    printf("%s\n", tRegexComparePattern(regex, "employee") == 1 ? "True" : "False");
    printf("%s\n", tRegexComparePattern(regex, "employment") == 1 ? "True" : "False");
    printf("%s\n", tRegexComparePattern(regex, "employable") == 1 ? "True" : "False");


//    trDArrayDeleteAll(&trDArray);
//    printf("%s\n", trDArray == NULL ? "NULL" : "Tidak Otomatis");

//    int *p = (int*)malloc(sizeof(int) * 8);
//    printf("%s\n", p == NULL ? "NULL" : "Tidak Otomatis");
    return 0;
}
