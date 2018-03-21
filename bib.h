/* --------------------------------------------------------------------------------------------------------------
					biblioteca para uso do pipe pelo vesh.c
	Cabecario
inclui
	criacao do pipe
	execucao dos processos com e sem pipe
	separacao das strings em comandos

-----------------------------------------------------------------------------------------------------------------*/

#ifndef _BIB_H
#define _BIB_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define FD_WRITE 1
#define FD_READ 0
	
void Interpretador(char *linguagem);
int executarSemPipe(char *cmd[100]);
void criaPipe(int **comunicacao);
int executarComPipe(char *cmd[100],int *p,int **comunicacao,int **comunicacao2, int n);


#endif //_BIB_H
