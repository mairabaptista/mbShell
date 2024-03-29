/*
	Maira Baptista de Almeida - RA: 111816
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "utilities.h"

int IOHandler(struct_pipe *Pipes, struct_coms *Comandos, char *input_file, char *output_file, int flag){
	pid_t pid;
	pid = fork();
	int fd_in, fd_out;
	if(pid == 0){	//filho
		if(flag == 0){	//IN_REDIR

			getCommand(Comandos, "<");
			
			//abre arquivo
			fd_in = open(input_file, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 	//abre como read only
			if(fd_in < 0){
				perror("open 1");
				return -1;
			}
			//troca o stdin pelo arquivo
			dup2(fd_in, STDIN_FILENO);
			close(fd_in);
		}
		else if(flag == 1){		//OUT_REDIR
			getCommand(Comandos, ">");
			//abre/cria o arquivo e habilita para escrita
			fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);	
			if(fd_out < 0){
				perror("open 2");
				return -1;
			}
			//troca o stdout pelo arquivo
			close(STDOUT_FILENO);
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
		}
		else if(flag == 2){	//	IN_REDIR e OUT_REDIR
			getCommand(Comandos, "<");
			fd_in = open(input_file, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			dup2(fd_in, STDIN_FILENO);
			close(fd_in);

			fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
			
		}
		if(execvp(Comandos->name, Comandos->args) < 0){
			perror("execvp filho\n");
			return -1;
		}
	}
	else if(pid < 0){
		perror("fork");
		return -1;
	}	
	else if(pid > 0){	//pai
		waitpid(pid, NULL, 0);
	}

	return pid;
}

int execNopipe(struct_coms *Comandos, int background){
	pid_t pid;
	
	pid = fork();	
	if(pid < 0){
		perror("fork");
		return -1;
	}
	else if(pid == 0){	//filho
		if(background == 1){
			removeBackground(Comandos);
		}
		if(execvp(Comandos->name, Comandos->args) < 0){
			perror("execvp filho\n");
			return -1;
		}
	}
	else if(pid > 0){	//pai
		if(background == 0){
			waitpid(pid, NULL, 0);
		}
		else{
			printf("Processo criado com PID %d\n", pid);
		}
	}
	

	return 0;
}

int pipeHandler(struct_pipe *Pipes, struct_coms *Comandos, int (*pipes)[2]){
	//se eu chamar IOHandler aqui, cria outro fork, isso ta certo?
	pid_t pid;
	pid = fork();
	int i, j = 0;
	int foundC = 0;
	int fd_in, fd_out;
	int fdesc_input = Comandos->fd[0];
	int fdesc_output = Comandos->fd[1];
	int redF = 0;

	//pid = fork();
	if(pid == 0){  //filho

		while(Comandos->args[j] != NULL){	//IN_OUT_REDIR
			if(Comandos->args[j+2] != NULL){
				if((strcmp(Comandos->args[j], "<") == 0) && (strcmp(Comandos->args[j+2], ">") == 0)){
					
					fd_in = open(Comandos->args[j+1], O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					dup2(fd_in, STDIN_FILENO);
					close(fd_in);

					fd_out = open(Comandos->args[j+3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					dup2(fd_out, STDOUT_FILENO);
					close(fd_out);
					getCommand(Comandos, "<");
					redF = 1;					
					break;
				}			
			}
			j++;
		}
		j=0;
		if(redF == 0){
			while(Comandos->args[j] != NULL){
				if(strcmp(Comandos->args[j], ">") == 0){	//OUT_REDIR
					
					//IOHandler(Pipes, Comandos, NULL, Comandos->args[j], OUT_REDIR);
					//abre/cria o arquivo e habilita para escrita
					fd_out = open(Comandos->args[j+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
						
					if(fd_out < 0){
						perror("open 2");
						return -1;
					}
					//troca o stdout pelo arquivo
					dup2(fd_out, STDOUT_FILENO);	//Qual descritor de arquivo aqui?
					close(fd_out);
					foundC = 1;
					getCommand(Comandos, ">");
					break;

				}
				else if(strcmp(Comandos->args[j], "<") == 0){	//IN_REDIR
					
					//IOHandler(Pipes, Comandos, Comandos->args[j], NULL, IN_REDIR);
					//abre arquivo
					fd_in = open(Comandos->args[j+1], O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 	//abre como read only
					
					if(fd_in < 0){
						perror("open 1");
						return -1;
					}
					//troca o stdin pelo arquivo
					dup2(fd_in, STDIN_FILENO);	//Qual descritor de arquivo aqui?
					close(fd_in);
					foundC = 1;
					getCommand(Comandos, "<");
					break;
				}	
				j++;
			}
		}
		

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

int commandExecute(struct_pipe *Pipes){
	int numPipes = 0;
	int i = 1;
	int j = 0;
	int l, k = 0;
	int background = 0;
	int returnedPID;
	int foundC = 0;
	int redF = 0;

	//Se tem apenas 1 comando, nao possui pipe -> executa direto
	if(Pipes->nComs == 1){
		//checa se temos & (processo em background)
		while(Pipes->Coms[0]->args[j] != NULL){
			if(strcmp(Pipes->Coms[0]->args[j], "&") == 0){
				//printf("testets\n");
				background = 1;
			}
			j++;
		}
		j = 0;

		while(Pipes->Coms[0]->args[j] != NULL){
			if(Pipes->Coms[0]->args[j+2] != NULL){
				if((strcmp(Pipes->Coms[0]->args[j], "<") == 0) && (strcmp(Pipes->Coms[0]->args[j+2], ">") == 0)){
					Pipes->redirFlag = IN_OUT_REDIR;
					IOHandler(Pipes, Pipes->Coms[0], Pipes->Coms[0]->args[j+1], Pipes->Coms[0]->args[j+3], Pipes->redirFlag);
					redF = 1;
					
					break;
				}			
			}
			j++;
		}
		j=0;
		if(redF == 0){
			//checa se temos > ou < para apenas 1 comando
			while(Pipes->Coms[0]->args[j] != NULL){
			if(strcmp(Pipes->Coms[0]->args[j], ">") == 0){	//OUT_REDIR
				Pipes->redirFlag = OUT_REDIR;
				IOHandler(Pipes, Pipes->Coms[0], Pipes->Coms[0]->args[j+1], Pipes->Coms[0]->args[j+1], Pipes->redirFlag);
				foundC = 1;
			}
			else if(strcmp(Pipes->Coms[0]->args[j], "<") == 0){	//IN_REDIR
				Pipes->redirFlag = IN_REDIR;
				IOHandler(Pipes, Pipes->Coms[0], Pipes->Coms[0]->args[j+1], Pipes->Coms[0]->args[j+1], Pipes->redirFlag);
				foundC = 1;
			}
			
			j++;
		}
		}
		
		
		if (foundC == 0){
			execNopipe(Pipes->Coms[0], background);
		}		
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
		printf("\033[1;36m");
		printf("mbShell> ");
		printf("\033[0m");
		inputs = readInput();
		piperino = pipeParser(inputs, piperino);
		rets = commandExecute(piperino);
		free(inputs);
	}
	
	return 0;
}