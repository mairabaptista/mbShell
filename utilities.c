/*
	Maira Baptista de Almeida - RA: 111816
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

#define BUFFERSIZE 64
#define TOKEN_DELIM " \t\r\n\a"

char *readInput(void){
	char *linha = NULL;
	size_t buffer = 0;	//ssize_t
	getline(&linha, &buffer, stdin);
	return linha;
}

char *proximoVazio(char **linha){
	char *token;
	while((token = strsep(linha, TOKEN_DELIM)) && !*token){

	}
	return token;
}

struct_coms *commandParser(char *Args){
	int buffersize = BUFFERSIZE;
	int pos = 0;
	//int i = 0;
	char *token;

	struct_coms *out = calloc(sizeof(struct_coms)+buffersize*sizeof(char*),1);

	if(!out){
		fprintf(stderr, "erro de alocacao\n");
		exit(EXIT_FAILURE);
	}

	while ((token = proximoVazio(&Args))){
		out->args[pos] = token;
		pos++;

		if(pos >= buffersize){
			buffersize += BUFFERSIZE;
			out = realloc(out, sizeof(struct_coms)+buffersize*sizeof(char*));
			if(!out){
				fprintf(stderr, "erro de alocacao\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	out->name = out->args[0];
	out->fd[0] = -1;
	out->fd[1] = -1;
	out->args[pos] = NULL;

	return out;

}

struct_pipe *pipeParser(char *Args, struct_pipe *Pipes){
	char *coms;
	int nComs = 0;
	int i = 0;

	//conta quantos pipes
	while(Args[i]){
		if(Args[i] == '|'){
			nComs++;
		}
		else if(Args[i] == '<'){	//redirect de input
			//nComs++;
			Pipes->redirFlag = IN_REDIR;
		}
		else if(Args[i] == '>'){	//redirect de output
			//nComs++;
			Pipes->redirFlag = OUT_REDIR;
		}
		i++;
	}

	nComs++;
	i = 0;

	struct_pipe *out = calloc(sizeof(struct_pipe) + nComs*sizeof(struct_coms), 1);
	out->nComs = nComs;

	while((coms = strsep(&Args, "|")) || (coms = strsep(&Args, ">")) || (coms = strsep(&Args, "<"))){
		out->Coms[i] = commandParser(coms);
		i++;
	}
	// /printPipe(out);

	return out;

}

struct_pipe printPipe(struct_pipe *Pipes){
	int i, j = 0;
	while(Pipes->Coms[i] != NULL){
		while(Pipes->Coms[i]->args[j] != NULL){
			printf("%s ", Pipes->Coms[i]->args[j]);
			j++;
		}
		i++;	
		j = 0;
		printf("\n");
	}
	printf("Total de %d comandos\n", Pipes->nComs);
}

//remove < ou > dos argumetos
void removeChar(struct_coms *Comandos, char *garbage) {
	int j = 0 , i = 0, k=0;
	int foundc = 0;

	if(foundc == 0){
		while(Comandos->args[i] != NULL){
			if(strcmp(Comandos->args[i], garbage) != 0 ){
				strcpy(Comandos->args[j++], Comandos->args[i]);
				//Comandos->args[i] = NULL;
				
			}
			i++;
		}	
		
	
		Comandos->args[j] = NULL;
	}
}
void removeBackground(struct_coms *Comandos){
	int i =0;
	while(strcmp(Comandos->args[i], "&") != 0){
		i++;
	}
	Comandos->args[i++] = NULL;
	i=0;
	while(Comandos->args[i] != NULL){
		printf("%s\n", Comandos->args[i]);
		//printf("%d\n", k);	
		i++;
	}
}
void getCommand(struct_coms *Comandos, char *garbage) {
	int i = 0;

	while(strcmp(Comandos->args[i], garbage) != 0){
		i++;
	}
	
	Comandos->args[i++] = NULL;
	i = 0 ;
	
	
}

//remove < e > da string de argumentos
void removeCharIO(struct_coms *Comandos) {		//nao funciona por algum motivo misterioso

	int j = 0 , i = 0, k=0;
	while(Comandos->args[i] != NULL){
		if(strcmp(Comandos->args[i], "<") != 0 ){
			strcpy(Comandos->args[j++], Comandos->args[i]);
			
		}
		printf("%s\n", Comandos->args[i]);
		i++;

	}	
	Comandos->args[j] = NULL;
	/*while(Comandos->args[k] != NULL){
		printf("%s\n", Comandos->args[k]);
		//printf("%d\n", k);	
		k++;
	}
	k  = 0;*/	
	i = 0;
	j = 0;

	while(Comandos->args[i] != NULL){
		if(strcmp(Comandos->args[i], ">") != 0 ){
			strcpy(Comandos->args[j++], Comandos->args[i]);
		}
		i++;
	}	
	Comandos->args[j] = NULL;

	
}