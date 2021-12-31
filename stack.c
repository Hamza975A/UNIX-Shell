#include <stdio.h>
#include <stdlib.h>

// A structure that represents a single stack
// The stack is full when the variable numOfItems is equal to -1
// The stack is full when numOfItems is equal to the last index
//
// int numOfItems = the int representing what level of the stack the highest object is.
// unsigned capacity = an unsigned integer that represents the capacity of objects in the stack.
// char** stackList = an array of string objects
struct Stack {
    int numOfItems;
    int capacity;
    char **stackList;
};

// function to create a stack with the capacity given.
// unsigned Cap = a unsigned number representing the capacity for the stack
struct Stack *createStack(int Cap) {
    struct Stack *stack = (struct Stack *) malloc(sizeof(struct Stack));
    stack->capacity = Cap;
    stack->numOfItems = -1;
    stack->stackList = (char **) malloc(stack->capacity * sizeof(char));
    return stack;
}

// Function to add an item to the stack. It increases numOfItems by 1
// Stack* stack = The stack to push to
// char* item = the item to be pushed onto the stack
void push(struct Stack *stack, char *item) {
    //If the stack is full, dont push the item because the stack does not have space
    if (stack->capacity - 1 == stack->numOfItems)
        return;
    //Else, push the item onto the top of the stack.
    stack->stackList[++stack->numOfItems] = item;
}

// Function to remove an item from the stack. It decreases numOfItems by 1
// Stack* stack = the stack to be popped from
char *pop(struct Stack *stack) {
    //If the stack is empty, there is nothing to pop from the stack.
    if (stack->numOfItems == -1)
        return "EMPTY";
    //Else, pop the numOfItems item from the stack and return it.
    return stack->stackList[stack->numOfItems--];
}

// Function to return the numOfItems from the stack without removing it
// Stack* stack = the stack to peep
char *peek(struct Stack *stack) {
    //If the stack is empty, there is nothing to pop from the stack.
    if (stack->numOfItems == -1)
        return "EMPTY";
    //Else return the value at the numOfItems of the stack without removing it.
    return stack->stackList[stack->numOfItems];
}