/**
 Tamsen Dean
 Professor Cenek
 CS 334
 27 February 2022
 HW #3
 */

#include "shell.h"

#define MAX 1024

int main(int argc, char* argv[]) {
    char cmd[MAX];  //taking in commands
    char cmdSent[MAX];
    char input[MAX];
    
    char *flags[MAX];   //var for parsing
    char* breakpt[MAX];
    char *token;  //var for separating

    FILE *fp = stdin;

    const char semiDelim[2] = ";";  //delimiting
    const char lineDelim[2] = " \n";
    
    int i, j, k = 0;    //counters
    int p1_check = 0;   //switches
    int p2_check = 0;
    
    printf("Please cd into your current directory: ");
    gets(input);
    
    if(argc == 2){
        fp = fopen(argv[1], "r");   //open the given file
        if(fp == NULL) {
            printf("Failed to open %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    while(1) { //infinite loop
        printf("%s> ", input);
        
        if(fgets(cmd, MAX, fp) == NULL) { //get user input
            p2_check = 1;
            p1_check = 1;
        }

        token = strtok(cmd, semiDelim); //get first token
        if(argc == 2 && p1_check == 0 && p2_check == 0) {
            printf("Command: %s\n", token);
        }
        i = 0;
        while(token != NULL){
            breakpt[i] = token;
            i++;
            token = strtok(NULL, semiDelim);
        }
    
        j = 0;
        while(j < i){
            strcpy(cmdSent, breakpt[j]);
            token = strtok(cmdSent, lineDelim); //breaks up by spacing
        
            if(p1_check == 0){
                if(ispipe(token) == 1) {
                    p1_check = 1;    //if pipe, redirect
                }
            }
            
            strcpy(cmdSent, breakpt[j]); //original state
            token = strtok(cmdSent, lineDelim);
            
            if(strcmp(token, "cd") == 0){ //change directory
                cd(token);
                p1_check = 1;
            }
            else if(strcmp(token, "exit") == 0){ //exit
                p2_check = 1;
                p1_check = 1;
            }
            else {
                k = 0;
                while(token != NULL) {
                    flags[k] = token;
                    k++;
                    token = strtok(NULL, lineDelim);
                }
                flags[k] = token;
            }
        
            if(p1_check == 0) {
                execFlags(flags); //execute if normal
            }
            j++;
            p1_check = 0;
        }

        if(p2_check == 1){
            kill(getppid(), SIGINT);
            exit(0);
        }
    }
}
/**
 Sources used:
 https://wiki.sei.cmu.edu/confluence/display/c/STR06-C.+Do+not+assume+that+strtok%28%29+leaves+the+parse+string+unchanged
 
 https://stackoverflow.com/questions/13078926/is-there-a-way-to-count-tokens-in-c
 
 https://www.delftstack.com/howto/c/sigint-in-c/
 */
