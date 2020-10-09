#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#define Red "\x1b[31m"
#define Yellow "\x1b[33m"
#define Blue "\x1b[34m"
#define Cyan "\x1b[36m"
#define RESET "\x1b[0m"
#define SIZE 100000
#define db printf(Red "debug" RESET "\n")

int min(int a, int b){
    if(a < b) return a;
    return b;
}

int may(int a, int b){
    if(a > b) return a;
    return b;
}

typedef struct Command{
    char *command;
    char **args;
    int mod1, mod2, mod3;
    char *input, *output;
    int length_args;
}Command;

typedef struct Commands_Split_Pipes{
    Command *_command;
    int length_commands;
}Commands_Split_Pipes;

void Constructor_Command(Command *_command){
    _command->command = malloc(sizeof(char) * SIZE);
    _command->args = malloc(sizeof(char) * SIZE);
    _command->length_args = 0;
    _command->mod1 = _command->mod2 = _command->mod3 = 0;
    _command->input = malloc(sizeof(char) * SIZE);
    _command->output = malloc(sizeof(char) * SIZE);
}

void Constructor_Commands_Split_Pipes(Commands_Split_Pipes *input_process){
    input_process->_command = malloc(SIZE);
    input_process->length_commands = 0;
}

char special_characters[] = {' ', '|', '>', '<', '&'};

int Is_Special(char x){
    for(int i = 0; i < strlen(special_characters); i++){
        if(special_characters[i] == x)
            return 1;
    }
    return 0;
}

void Copy_To(char a[], char b[]){ //Copy from a to b
    for(int i = 0; i < strlen(a); i++)
        b[i] = a[i];
    b[strlen(a) + 1] = 0;
}