/**
 Tamsen Dean
 Professor Cenek
 CS 334
 27 February 2022
 HW #3
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void cd(char* change_dir);
void path(char* pathname, char* input);
void execFlags(char** flags);
void pipecmd(char* line);
void redir_cmd(char* line);
int ispipe(char* line);
