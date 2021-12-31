#ifndef STACK_H
#define STACK_H

// A structure that represents a single stack
// The stack is full when the variable numOfItems is equal to -1
// The stack is full when numOfItems is equal to the last index
//
// int numOfItems = the int representing what level of the stack the highest object is.
// unsigned capacity = an unsigned integer that represents the capacity of objects in the stack.
// char** stackList = an array of string objects
struct Stack {
    int top;
    unsigned cap;
    char **array;
};

// function to create a stack with the capacity given.
// unsigned Cap = a unsigned number representing the cappacity for the stack
struct Stack *createStack(unsigned capacity);

// Function to add an item to the stack. It increases numOfItems by 1
// Stack* stack = The stack to push to
// char* item = the item to be pushed onto the stack
void push(struct Stack *stack, char *item);

// Function to remove an item from the stack. It decreases numOfItems by 1
// Stack* stack = the stack to be popped from
char *pop(struct Stack *stack);

// Function to return the numOfItems from the stack without removing it
// Stack* stack = the stack to peep
char *peek(struct Stack *stack);

#endif //STACK_H