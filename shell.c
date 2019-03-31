/*
	Maira Baptista de Almeida - RA: 111816
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utilities.h"

int execNopipe(struct_coms *Comandos){
	pid_t pid;
	pid = fork();	
	if(pid == 0){	//filho
		if(execvp(Comandos->name, Comandos->args) < 0){
			perror("execvp filho\n");
			return -1;
		}
	}
	else if(pid > 0){	//pai
		waitpid(pid, NULL, 0);
	}
	else if(pid < 0){
		perror("fork");
		return -1;
	}

	return 0;
}

int pipeHandler(struct_pipe *Pipes, struct_coms *Comandos, int (*pipes)[2]){
	pid_t pid;
	pid = fork();
	int i;
	int fdesc_input = Comandos->fd[0];
	int fdesc_output = Comandos->fd[1];

	if(pid == 0){	//filho
		if(fdesc_input != -1 && fdesc_input != STDIN_FILENO){
			dup2(fdesc_input, STDIN_FILENO);
		}
		if(fdesc_output != -1 && fdesc_output != STDOUT_FILENO){
			dup2(fdesc_output, STDOUT_FILENO);
		}
		//fechar todos os pipes
		for(i = 0; i < (Pipes->nComs - 1); i++){
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		//executa os comandos		
		if(execvp(Comandos->name, Comandos->args) < 0){
				perror("execvp filho\n");
				return -1;
		}
		free(pipes);
		exit(1);
	}

	return pid;
}

int commandExecute(struct_pipe *Pipes/*struct_coms *Comandos*/){
	int numPipes = 0;
	int i = 1;
	int returnedPID;

	//Se tem apenas 1 comando, nao possui pipe -> executa direto
	if(Pipes->nComs == 1){
		execNopipe(Pipes->Coms[0]);
	}

	//Se tem mais de um comando, temos pipe
	else{
		numPipes = Pipes->nComs - 1;
		//vetor de pipes
		int (*pipes)[2] = calloc(numPipes * sizeof(int[2]), 1);
		if(pipes == NULL){
			printf("Erro de alocacao\n");
			return -1;
		}

		Pipes->Coms[0]->fd[STDIN_FILENO] = STDIN_FILENO;
		while(i <= numPipes){
			pipe(pipes[i-1]);
			Pipes->Coms[i]->fd[STDIN_FILENO] = pipes[i-1][STDIN_FILENO];		//seta descritor para input
			Pipes->Coms[i-1]->fd[STDOUT_FILENO] = pipes[i-1][STDOUT_FILENO];	//seta descritor para output
			i++;
		}
		//setar os descritores do primeiro e ultimo comandos		
		Pipes->Coms[numPipes]->fd[STDOUT_FILENO] = STDOUT_FILENO;

		i = 0;

		while(Pipes->Coms[i] != NULL){
			returnedPID = pipeHandler(Pipes, Pipes->Coms[i], pipes);
			i++;
		}
		//fecha todos os pipes
		for(i = 0; i < (Pipes->nComs - 1); i++){
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		i = 0;
		//espera os filhos
		while(i != Pipes->nComs){
			waitpid(returnedPID, NULL, 0);
			i++;
		}

		free(pipes);
	}
	
	//printPipe(Pipes);
	return returnedPID;
}

int main(){
	int rets;
	struct_pipe *piperino;
	char *inputs = NULL;
	while(1){
		printf("mbShell> ");
		inputs = readInput();
		piperino = pipeParser(inputs, piperino);
		rets = commandExecute(piperino);
		free(inputs);
	}
	
	return 0;
}