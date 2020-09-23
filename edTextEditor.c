#include <stdio.h>
#include <stdlib.h>
#include <string.h>




enum Command
{
    change,
    delete,
    print,
    undo,
    redo,
    quit
};
typedef enum Command Command;

enum CycleState
{
    nil,
    set,
    unset
};
typedef enum CycleState CycleState;

struct Action {
    Command command;
    int a;
    int b;
    char **arguments;
    int argumentsSize;
    char **redoArguments;
    int redoArgumentsSize;
    char **undoDelete;
    int undoDeleteCounter;
    char **redoDelete;
    int redoDeleteCounter;
};
typedef struct Action Action;

char **text;
int textCounter = 0;
int textSize = 1000;

Action *undoStack;
int undoStackSize = 1000;
int undoStackTop = -1;

Action *redoStack;
int redoStackSize = 1000;
int redoStackTop = -1;

int undoCounter = 0;



char **readLines(int n);
Command readCommand(int *p1, int *p2);
void executeChange(int start, int end);
void executeDelete(int start, int end);
void executePrint(int start, int end);
void executeUndo(int num);
void undoChange(Action action);
void undoDelete(Action action);
void executeRedo(int num);
void redoChange(Action action);
void redoDelete(Action action);

void undoStackPush(Action action);
Action undoStackPop();
void redoStackPush(Action action);
Action redoStackPop();




int main() {

    text = malloc(textSize * sizeof(char *));
    undoStack = malloc(undoStackSize * sizeof(Action));
    redoStack = malloc(redoStackSize * sizeof(Action));

    int a, b;
    Command command;


    while ((command = readCommand(&a, &b)) != quit)
    {

        switch (command)
        {
        case change:
            executeChange(a, b);
            break;
        case delete:
            executeDelete(a, b);
            break;
        case print:
            executePrint(a, b);
            break;
        case undo:
            if (undoCounter + a > undoStackTop + 1)
            {
                undoCounter = undoStackTop + 1;
            }
            else
            {
                undoCounter += a;
            }
            break;
        case redo:
            if (undoCounter - a < 0)
            {
                if (a - undoCounter > redoStackTop + 1)
                {
                    undoCounter = -(redoStackTop + 1);
                }
                else
                {
                    undoCounter -= a;
                }
            }
            else
            {
                undoCounter -= a;
            }
            break;
        case quit:
            break;
        }
    }



    return 0;
}




char **readLines(int n) {

    char **lines = malloc(n * sizeof(char *));

    for (int i = 0; i < n; i++)
    {
        lines[i] = malloc(1028);
        (void) fgets(lines[i], 1028, stdin);
        lines[i] = realloc(lines[i], strlen(lines[i]) + 1);
    }

    return lines;

}

Command readCommand(int *p1, int *p2) {

    char command[1028];
    fgets(command, 1028, stdin);
    char commandID = command[strlen(command) - 2];

    if(strchr(command, ',')) 
    {
        sscanf(command, "%d,%d", p1, p2);
    } 
    else if (strchr(command, 'q')) {
        return quit;
    }
    else 
    {
        sscanf(command, "%d", p1);
    }

    switch (commandID)
    {
    case 'c':
        return change;
    case 'd':
        return delete;
    case 'p':
        return print;
    case 'u':
        return undo;
    case 'r':
        return redo;
    default:
        return quit;
    }

}




void executeChange(int start, int end) {

    if (undoCounter > 0)
    {
        executeUndo(undoCounter);
    }
    else if (undoCounter < 0)
    {
        executeRedo(-undoCounter);
    }
    undoCounter = 0;
    redoStackTop = -1;

    int interval = end - 1 < textCounter ? (end - start + 1) : (textCounter - start + 1);
    char **arguments = start - 1 >= textCounter ? NULL : malloc(interval * sizeof(char *));
    
    if (textCounter + (end - start + 1 - interval) >= textSize - 2)
    {
        textSize *= 2;
        text = realloc(text, textSize * sizeof(char *));
    }
    if (arguments != NULL) 
    {
        memcpy(arguments, text + start - 1, interval * sizeof(char *));
    }
    textCounter -= interval;

    int numElements = end - start + 1;
    char **newText = readLines(numElements);
    (void) scanf(".\n");

    int argsCount = arguments == NULL ? 0 : interval;
    Action executeAction = {change, start, end, arguments, argsCount, newText, numElements, NULL, 0, NULL, 0};
    undoStackPush(executeAction);

    memmove(text + start - 1, newText, numElements * sizeof(char *));
    textCounter += numElements;

}

void executeDelete(int start, int end) {

    if (undoCounter > 0)
    {
        executeUndo(undoCounter);
    }
    else if (undoCounter < 0)
    {
        executeRedo(-undoCounter);
    }
    undoCounter = 0;
    redoStackTop = -1;

    if (start - 1 >= textCounter)
    {
        char **redoDelete = malloc(textCounter * sizeof(char *));
        for (int i = 0; i < textCounter; i++) 
        {
            redoDelete[i] = text[i];
        }
        Action deleteAction = {delete, start, end, NULL, 0, NULL, 0, NULL, 0, redoDelete, textCounter};
        undoStackPush(deleteAction);
        return;
    }

    int elementsToMove = end - 1 < textCounter ? (textCounter - end) : 0;
    int interval = end - 1 < textCounter ? (end - start + 1) : (textCounter - start + 1); 
    char **arguments = interval == 0 ? NULL : malloc(interval * sizeof(char *));

    char **undoDelete = malloc(textCounter * sizeof(char *));
    int undoCounter = textCounter;
    for (int i = 0; i < textCounter; i++)
    {
        undoDelete[i] = text[i];
    }

    if (arguments != NULL)
    {
        memcpy(arguments, text + start - 1, interval * sizeof(char *));
    }
    textCounter -= interval;

    memmove(text + start - 1, text + end, elementsToMove * sizeof(char *));

    char **redoDelete = malloc(textCounter * sizeof(char *));
    for (int i = 0; i < textCounter; i++) 
    {
        redoDelete[i] = text[i];
    }
    int redoCounter = textCounter;

    
    Action deleteAction = {delete, start, end, arguments, interval, NULL, 0, undoDelete, undoCounter, redoDelete, redoCounter};
    undoStackPush(deleteAction);

}

void executePrint(int start, int end) {

    if (undoCounter > 0)
    {
        executeUndo(undoCounter);
    }
    else if (undoCounter < 0)
    {
        executeRedo(-undoCounter);
    }
    undoCounter = 0;

    for (int i = start - 1; i < end; i++) 
    {
         if(i >= textCounter || i < 0) 
        {
            fputs(".\n", stdout);
        }
        else 
        {
            fputs(text[i], stdout);
        }
    }

}

void executeUndo(int num) {

    if(num > undoStackTop)
    {
        for (int i = 0; undoStackTop >= 0; i++)
        {
            Action action = undoStackPop();
            redoStackPush(action);
        }
        textCounter = 0;
        return;
    }

    for (int i = 0; i < num && undoStackTop >= 0; i++) 
    {
        Action action = undoStackPop();

        switch (action.command)
        {
        case change:
            undoChange(action);
            redoStackPush(action);
            break;
        case delete:
            undoDelete(action);
            redoStackPush(action);
            break;
        default:
            break;
        }

    }

}

void undoChange(Action action) {

    int start = action.a;
    int end = action.b;
    int argumentsSize = action.argumentsSize;

    memmove(text + start - 1, action.arguments, argumentsSize * sizeof(char *));
    textCounter -= ((end - start + 1) - argumentsSize);

}

void undoDelete(Action action) {

    char **newText = action.undoDelete;
    int newTextCounter = action.undoDeleteCounter;
    if (newText != NULL) 
    {
        text = newText;
        textCounter = newTextCounter;
    }

}

void executeRedo(int num) {

    for (int i = 0; i < num && redoStackTop >= 0; i++) 
    {
        Action action = redoStackPop();

        switch (action.command)
        {
        case change:
            redoChange(action);
            undoStackPush(action);
            break;
        case delete:
            redoDelete(action);
            undoStackPush(action);
            break;
        default:
            break;
        }

    }  

}

void redoChange(Action action) {

    int start = action.a;
    int end = action.b;
    int numElements = action.redoArgumentsSize;

    int interval = end - 1 < textCounter ? (end - start + 1) : (textCounter - start + 1);
    textCounter -= interval;

    memmove(text + start - 1, action.redoArguments, numElements * sizeof(char *));
    textCounter += numElements;

}

void redoDelete(Action action) {

    text = action.redoDelete;
    textCounter = action.redoDeleteCounter;

}

void undoStackPush(Action action) {

    if(undoStackTop + 1 >= undoStackSize - 2)  
    {
        undoStackSize *= 2;
        undoStack = realloc(undoStack, undoStackSize * sizeof(Action));
    }

    undoStack[++undoStackTop] = action;

}

Action undoStackPop() {

    if (undoStackTop < 0) 
    {
        Action emptyAction = {quit, -1, -1, NULL};
        return emptyAction;
    }

    return undoStack[undoStackTop--];

}

void redoStackPush(Action action) {

    if(redoStackTop + 1 >= redoStackSize - 2)
    {
        redoStackSize *= 2;
        redoStack = realloc(redoStack, redoStackSize * sizeof(Action));
    }

    redoStack[++redoStackTop] = action;
}

Action redoStackPop() {

    if (redoStackTop < 0)
    {
        Action emptyAction = {quit, -1, -1, NULL};
        return emptyAction;
    }

    return redoStack[redoStackTop--];

}