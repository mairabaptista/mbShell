/*
	Maira Baptista de Almeida - RA: 111816
*/

#define UTILS_H
#define BUFFERSIZE 64
#define TOKEN_DELIM " \t\r\n\a"
#define IN_REDIR 2
#define OUT_REDIR 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Struct que representa um comando e argumentos de um comando
typedef struct {
  char* name;	//nome do executavel
  int fd[2];	//descritores do comando
  char* args[];	//comandos
} struct_coms;

//Struct para um pipe de comandos
typedef struct {
  int nComs; //numero de comandos
  int redirFlag;	//flag de redirect
  struct_coms* Coms[]; //comandos
} struct_pipe;

char *readInput(void);
char *proximoVazio(char **linha);
struct_coms *commandParser(char *args);
struct_pipe *pipeParser(char *args,struct_pipe *Pipes);
int pipeHandler(struct_pipe *Pipes, struct_coms *Comandos, int (*pipes)[2]);
int commandExecute(struct_pipe *Pipes);
int execNopipe(struct_coms *Comandos);
struct_pipe printPipe(struct_pipe *Pipes);