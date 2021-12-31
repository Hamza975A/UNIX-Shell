#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "stack.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

/**
 * Function that prints the custom shell prompt for weird shell.
 */
void printPrompt()
{
    printf("wrdsh> ");
}

/**
 * This function takes in a string and separates it by space. Returns an array of strings.
 * @param string : The string to be parsed by space
 * @return : Parsed string separated by space as an array of strings
 */
char **parseBySpace(char *string)
{
    char *token;
    char delimiter[2] = " ";

    // First token
    token = strtok(string, delimiter);

    // Dynamically allocating memory to store commands in an array of strings (2D array)
    // Allocating memory to store 100 commands of 100 char each
    char **commands = (char **)malloc(sizeof(char *) * 100);
    for (unsigned int i = 0; i < 100; i++)
    {
        commands[i] = (char *)malloc(sizeof(char) * 100);
    }

    // Parsing through the string and separating it by space and storing it into the array
    int i = 0;
    while (token != NULL)
    {
        strcpy(commands[i], token);
        token = strtok(NULL, delimiter);
        i++;
    }
    // set last item in the array to be null | needed for execvp to work
    commands[i] = NULL;
    return commands;
}

/**
 * This function takes an array of strings (2D Array) as input and runs it via multiple processes. 
 * Has support for multiple pipes, STDOUT and Redirection.
 * @param commands : Array of strings (2D Array) which contains commands and arguments to run. 
 */
void runCommands(char **commands)
{
    // Counters
    int numberOfPipes = 0;
    int numberOfCommands = 0;

    // Array to store just the commands in (remove pipes from the strings)
    // Allocated memory to store 100 arrays
    char ***onlyCommands = malloc(sizeof(char *) * 100);

    /**
     * @brief Loop to count the number of pipes and number of commands there are and save 
     * the commands separately. 
     */
    int i = 0;
    while (commands[i] != NULL)
    {
        if (strcmp(commands[i], "|") == 0)
        {
            numberOfPipes++;
        }
        else
        {
            // Parse the command by space and save it as an array
            onlyCommands[numberOfCommands] = parseBySpace(commands[i]);
            numberOfCommands++;
        }
        i++;
    }

    // Create arrays to store pipe and pids of the processes forked
    int fd[numberOfCommands + 1][2];
    int pids[numberOfCommands];

    // Initialize all the pipes
    for (int p = 0; p <= numberOfPipes + 1; p++)
    {
        if (pipe(fd[p]) < 0)
        {
            fprintf(stderr, "Pipe failed\n");
            exit(-1);
        }
    }

    /**
     * @brief loop that creates pipes and sets them up for the current and the next command. 
     * Also runs the commands. Checks last command for redirection and duplicates the letter 'c' 'm' 'p' 't'
     * if present in STDOut for the user.
     */
    for (int k = 0; k < numberOfCommands; k++)
    {
        // Close the pipes used by the last iteration of the loop
        if (k > 0)
        {
            close(fd[k - 1][0]);
            close(fd[k][1]);
        }

        // Fork a child process
        pids[k] = fork();
        if (pids[k] < 0)
        { // Fork failed
            fprintf(stderr, "Fork failed\n");
            exit(-2);
        }
        else if (pids[k] == 0)
        { // Child process
            // Close every pipe except the one to be used
            for (int j = 0; j < numberOfCommands + 1; j++)
            {
                // Read pipe to be used is for the current iteration, dont close needed File Descriptors.
                if (k != j)
                {
                    close(fd[j][0]);
                }

                // Write pipe to be used is for the next iteration, dont close needed File Descriptors.
                if (k + 1 != j)
                {
                    close(fd[j][1]);
                }
            }

            // Connect STDIn to the read from the last iteration of the loop
            dup2(fd[k][0], STDIN_FILENO);

            // If not running the last command then connect STDOut to the write end for the next iteration
            // of the loop.
            if (k != numberOfCommands - 1)
            {
                // connect STDOut to the write end for the next iteration of the loop
                dup2(fd[k + 1][1], STDOUT_FILENO);
            }
            else
            { // Current process is on last command.
                char **lastCommand = malloc(sizeof(char) * 100);

                // Check if last command is a redirection case.
                // If yes then save the file name and remove the < symbol and keep the commands
                if ((onlyCommands[k][1] != NULL) && (strcmp(onlyCommands[k][1], "<") == 0))
                {
                    int x = 2;

                    // Starting from 3rd element(after the redirection symbol), grab every item until
                    // the end of the array has been reached.
                    while (onlyCommands[k][x] != NULL)
                    {
                        // Copy strings over.
                        lastCommand[x - 2] = onlyCommands[k][x];
                        x++;
                    }

                    // Save file name
                    char fileName[100];
                    strcpy(fileName, onlyCommands[k][0]);

                    // Permissions for new file and redirect STDOut to the new file.
                    int file = open(fileName, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                    dup2(file, STDOUT_FILENO);

                    // Run final command (redirection case)
                    execvp(lastCommand[0], lastCommand);
                    int errorCode = errno;
                    exit(errorCode);
                }
                else
                { // Case to duplicate cmpt letters
                    // pipe the output of the last command
                    close(fd[k + 1][0]);
                    dup2(fd[k + 1][1], STDOUT_FILENO);
                }
                free(lastCommand);
            }
            // Run the current command
            execvp(onlyCommands[k][0], onlyCommands[k]);
            int errorCode = errno;
            exit(errorCode);
        }
        // Wait for the child to be finished
        int childStatus;
        wait(&childStatus);
        if (!WIFEXITED(childStatus))
        {
            exit(errno);
        }

        /**
         * @brief Duplicate the letter c, m, p, t and print the output
         */
        if (k == numberOfCommands - 1)
        {
            // Assumed max size of output 1000 chars and each char if duplicated would be 2000 chars
            char output[1000];
            char cmptOutput[2000];

            // clear char arrays before each command run
            memset(output, 0, 1000);
            memset(cmptOutput, 0, 2000);

            close(fd[numberOfCommands][1]);
            // Read the output of the last command from the pipe
            if (read(fd[numberOfCommands][0], &output, 1000) == -1)
            {
                exit(-3);
            }
            close(fd[numberOfCommands][0]);

            // Duplicate the characters if found
            int counter = 0;
            int num = 0;
            while (output[num] != '\0')
            {
                if (output[num] == 'c')
                {
                    cmptOutput[counter] = 'c';
                    cmptOutput[counter + 1] = 'c';
                    counter++;
                }
                else if (output[num] == 'm')
                {
                    cmptOutput[counter] = 'm';
                    cmptOutput[counter + 1] = 'm';
                    counter++;
                }
                else if (output[num] == 'p')
                {
                    cmptOutput[counter] = 'p';
                    cmptOutput[counter + 1] = 'p';
                    counter++;
                }
                else if (output[num] == 't')
                {
                    cmptOutput[counter] = 't';
                    cmptOutput[counter + 1] = 't';
                    counter++;
                }
                else
                {
                    // General case: cmpt letter not found, copy as is
                    cmptOutput[counter] = output[num];
                }
                num++;
                counter++;
            }
            // Add a newline and print the new output
            cmptOutput[counter] = '\0';
            printf("%s", cmptOutput);
        }
    }

    for (int f = 0; f < 100; f++)
    {
        free(onlyCommands[f]);
    }
    free(onlyCommands);
}

/**
 * Function that reads the commands from right to left and flips the order.
 * @param string : Input from user
 * @return : array of strings in flipped order of the input
 */
char **flipOrder(char *string)
{
    /* get the first token */
    char delimiter[] = "|";
    char *token;
    token = strtok(string, delimiter);
    int numberOfCommands = 0;

    // Assuming max size of 100 commands
    struct Stack *myStack = createStack(100);

    /* walk through other tokens */
    while (token != NULL)
    {
        // push each command between pipes to the stack
        push(myStack, token);

        // get the next token
        token = strtok(NULL, delimiter);

        // if there is another command add the pipe symbol
        if (token != NULL)
        {
            char *pipeDelimiter = "|";
            push(myStack, pipeDelimiter);
        }

        numberOfCommands++;
    }

    // new array to store all the commands once they are flipped
    char **flippedCommands = (char **)malloc(sizeof(char *) * 100);
    for (unsigned int i = 0; i < 100; i++)
    {
        flippedCommands[i] = (char *)malloc(sizeof(char *) * 100);
    }

    // pop each command from stack to flip it and add it to the array
    int i = numberOfCommands + (numberOfCommands - 1);
    int j = 0;
    while (i > 0)
    {
        strcpy(flippedCommands[j], pop(myStack));
        i--;
        j++;
    }
    flippedCommands[j] = NULL;
    return flippedCommands;
}

/**
 * Main function which runs when this shell file is executed. Starts the shell with a prompt, takes input from the user
 * and can handle pipes, STDOut redirection and reads commands from right to left instead of the default left to right.
 * Also duplicates the letter c, m, p, t if found on stdout for the user.
 * @return : exit code
 */
int main(void)
{
    char *inputLine = NULL;
    char **commands;

    // Repeat until exit is entered
    while (1)
    {
        printPrompt();
        size_t length = 0;
        ssize_t numberOfChars = getline(&inputLine, &length, stdin);

        // remove trailing new inputLine character from inputLine
        numberOfChars--;
        inputLine[numberOfChars] = '\0';

        // check if exit is entered
        if (strcmp(inputLine, "exit") == 0)
        {
            // Edge case: If no commands exists then dont free and simply just exit.
            if (commands != NULL)
            {
                // Free all the allocated memory used by commands array
                for (int i = 0; i < 100; i++)
                {
                    free(commands[i]);
                }
                free(commands);
            }
            exit(0);
        }
        // call function to flip the order of commands between pipes
        commands = flipOrder(inputLine);

        // run and parse the commands
        runCommands(commands);
    }
}