
//***************************************************************************************************************//
//                                     																   			 //                                                                                                                         //
//                                     MEMBROS														   			 //
//						   GUSTAVO DIAS DE CASTRO        - 0027180                                     			 //
//                         IGOR ANTÔNIO PEDROSO          - 0026754                                     			 //
//                         JOSE LUIS	MACIEL           - 0026892									   			 //
//                      			                                                                   			 //
//                                                                                                     			 //
//                                                                                                     			 //
//                                                                                                     			 //
// Trabalho sobre interface de comandos, utilisando funcoes fork	() e exec()						   			 // 
//                                                                                                     			 //
// Este arquivo foi desenvolvido no sistema operacional Linux distribuição Ubuntu na linguagem C.                //
//                                                                                                               //
// Data: 05/10/2016.                                                                                             //
//                                                                                                               //                                                                                                                                                             //
//***************************************************************************************************************//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "bib.h"
	
#define BLU   "\x1B[34m"
#define RED   "\x1B[31m"
#define RESET "\x1B[0m"




int main(int argc, char* argv[]){

	int sinal = 0;
	char *user = getenv("USER");//pwd
	char *dir  = getenv("PWD"); //diretorio
	char *dir2[100];
	char key[100];
	int i=0;

	
	/* Pegando o ultimo diretorio */	
		dir2[i] = strtok(dir,"/");
		while((dir2[++i] = strtok(NULL,"/")) != NULL);
	/*pegando os comandos digitados pelo usuario*/
	do{		

		printf("["BLU"%s "RED" %s]"RESET"->",user, dir2[i-1] );
		fgets(key,100,stdin);
		/*como o fgets pega o \n como caracter, ele entao e retirado*/
		key[strlen(key)-1] = '\0';
		/*comando de saida do console*/
		if(!strcmp(key,"exit")) 
			return 0;
		
		Interpretador(key);		
		printf("\n");
		/*caso haja mudanca no usuario para kernel ou caso haja mudanca de diretorio*/
		user 	= getenv("USER");
		dir 	= getenv("PWD");
	}while(!sinal);



	return 0;
}

