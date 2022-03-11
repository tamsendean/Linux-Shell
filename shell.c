/**
 Tamsen Dean
 Professor Cenek
 CS 334
 27 February 2022
 HW #3
 */

#include "shell.h"
#define MAX 1024

void cd(char* change_dir) {
    char dir[MAX] = "\0"; //init directory
    
    while(change_dir != NULL) { //cd to string tokens
        strcat(dir, change_dir);    //concat string with each character
        change_dir = strtok(NULL, " \n"); //get next character while no delim
    }
    
    char* crnt_dir = dir+2; // attempt to remove "cd" from the directory name
    
    if(strcmp(crnt_dir, "")==0) { // if no argument, cd to user's home path -- results in seg fault
        crnt_dir = getenv("HOME");
    }
    
    if(chdir(crnt_dir) != 0) { //check error with chdir
        perror(crnt_dir);
    }
}

void path(char* pathname, char* input) {
    char dir[MAX] = "\0";
    char file[MAX];
    getcwd(file, sizeof(file));
    
    while(pathname != NULL) {
        strcat(dir, pathname);
        pathname = strtok(NULL, " \n");
    }
    char* crnt_dir = dir+4;
    char* mod = dir+6; //modifier
    
    if(strcmp(crnt_dir, "") == 0) {
        printf("%s> %s\n", input, file);
    }
    else if(strchr(crnt_dir, '+') != NULL) {
        strcat(file, mod);
        printf("%s> %s\n", input, file);
    }
    else if(strchr(crnt_dir, '-') != NULL) {
        printf("%s> %s\n", input, file);
    }
    return;
}

void execFlags(char** flags) {
    
    pid_t pid = fork(); //fork processes
    if(pid == -1) {
        perror("Fork error"); //check for fork error
    }//error handling
    else if(pid == 0) {
        execvp(flags[0], flags); //execute the command with listed flags
        printf("Command %s not found\n", flags[0]); //print error if it does not execute
        exit(1); //exit child process
    }//child
    else wait((int *)0); //wait for the child process to complete
    
}

void pipecmd(char* line) {
    int status;
    int i = 0;
    char* args_c[3];
    char* flags1[MAX];
    char* flags2[MAX];
    char* flags3[MAX];
    int pipe_num = 0;
    pid_t pid1, pid2, pid3;
    
    // get the number of pipes in argument to know how many to open/too many pipes given
    for (i = 0; i < strlen(line); i++) {
        if (line[i] == '|') {
            pipe_num++;
        }
    }
    line = strtok(line, "|");   //delimit line by the | character
    while(line != NULL) {
        args_c[i] = line;
        line = strtok(NULL, "|");
        i++;
    }
    line = strtok(args_c[0], " "); //delimit first index of the array for first command
    while(line != NULL) {
        flags1[i] = line;
        line = strtok(NULL, " ");
        i++;
    }
    line = strtok(args_c[1], " "); //delimit second index
    while(line != NULL) {
        flags2[i] = line;
        line = strtok(NULL, " ");
        i++;
    }
    
    //if there are two pipes provided
    if(pipe_num == 2) {
        line = strtok(args_c[2], " "); //delimit third index
        while(line != NULL) {
            flags3[i] = line;
            line = strtok(NULL, " ");
            i++;
        }
    }
    int fd1[2], fd2[2];
    
    if(pipe(fd1) == -1) { // make first pipe
        perror("pipe");
    }
    
    if(pipe_num == 2) { // make second pipe if needed
        if(pipe(fd2) == -1) {
            perror("pipe");
        }
    }
    
    if ((pid1 = fork()) == 0) { // fork first child
        dup2(fd1[1], 1); // duplicate write end of first pipe to stdout
        close(fd1[0]); // close both ends of first pipe
        close(fd1[1]);
        if(pipe_num == 2) { // close both ends of second pipe if needed
            close(fd2[0]);
            close(fd2[1]);
        }
        execvp(*flags1, flags1); //execute first command
    }
    else if(pid1 < 0) { //error check first fork
        perror("fork");
    }
    else { // parent
        // fork second child
        if ((pid2 = fork()) == 0) { // duplicate read end of first pipe to stdin
            dup2(fd1[0], 0);  // close both ends of first pipe
            close(fd1[0]);
            close(fd1[1]);
            
            if(pipe_num == 2) {   //duplicate write end and close second pipe if needed
                dup2(fd2[1], 1);
                close(fd2[0]);
                close(fd2[1]);
            }
            
            execvp(*flags2, flags2); //execute second command
        }
        else if(pid2 < 0) { //error check second fork
            perror("fork");
        }
        else { // parent
            if(pipe_num == 2) {
                if ((pid3 = fork()) == 0) { // fork third child if needed
                    dup2(fd2[0], 0);    // duplicate read end of second pipe to stdin
                    close(fd1[0]);  // close both ends of all pipes
                    close(fd1[1]);
                    close(fd2[0]);
                    close(fd2[1]);
                    execvp(*flags3, flags3); //execute third command
                }
                else if(pid3 < 0){ //error check third fork
                    perror("fork");
                }
            }
        }
    }
    close(fd1[0]); //close first pipe
    close(fd1[1]);
    
    if(pipe_num == 2) { //close second pipe
        close(fd2[0]);
        close(fd2[1]);
    }
    for (i = 0; i < pipe_num + 1; i++) {  //wait for all children
        wait(&status);
    }
}

void redir_cmd(char* line) {
    char *token;
    int i = 0;
    int j = 0;
    char *flags[512];
    const char delim[2] = " \n";
    int fd[10];
    int checking = 0;
    
    if(fork() == 0){
        token = strtok(line, delim);
        while(token != NULL) {
            
            if(checking == 0) {
                flags[i] = token;
                i++;
                token = strtok(NULL, delim);
            }//breaks by spacing
            
            if(token != NULL) {
                checking = 0;
                if(strcmp(token, ">") == 0){
                    token = strtok(NULL, delim);
                    fd[j] = open(token, O_CREAT | O_TRUNC | O_WRONLY, 0600);
                    
                    dup2(fd[j], 1);
                    close(fd[j]); //performs writing
                    j++;
                    
                    token = strtok(NULL, delim); //get next index
                    if(token != NULL){
                        if(strcmp(token, "<") == 0 || strcmp(token, ">") == 0) checking = 1;
                    }//checks if next index is redirection
                }//checks for > redirection
                
                else if(strcmp(token, "<") == 0){
                    token = strtok(NULL, delim);
                    fd[j] = open(token, O_RDONLY);
                    
                    dup2(fd[j], 0);
                    close(fd[j]); //performs reading
                    j++;
                    
                    token = strtok(NULL, delim); //gets next index
                    if(token != NULL){
                        if(strcmp(token, "<") == 0 || strcmp(token, ">") == 0) checking = 1;
                    }
                }
            }
        }
        flags[i] = NULL;    //sets last to NULL
        
        execvp(flags[0], flags); //exec
    }   //child
    else {
        wait( (int *)0 );
        return;
    }   //parent
}

int ispipe(char* line) {
    char dir[MAX] = "\0"; //char array to hold the concat string to check for pipes
    int i = 0;
    
    while(line != NULL) {   //concat line to check for pipes
        strcat(dir, line);
        line = strtok(NULL, ";");
        if(i == 0) strcat(dir, " ");
        i++;
    }
    
    if(strchr(dir, '|') != NULL && strchr(dir, '<') == NULL && strchr(dir, '>') == NULL) {  //puts(dir);
        pipecmd(dir); //execute pipecmd if pipes found
        return 1;
    }
    
    if((strchr(dir, '|') == NULL && strchr(dir, '<') != NULL) || strchr(dir, '>') != NULL) {
        redir_cmd(dir);
        return 1;
    }
    return 0;
}

/**
 Sources:
 https://www.daniweb.com/programming/software-development/threads/389378/chdir-in-c-program
 
 https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf
 */
