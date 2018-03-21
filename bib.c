/* --------------------------------------------------------------------------------------------------------------
					biblioteca para uso do pipe pelo vesh.c
	Funcoes
inclui
	criacao do pipe
	execucao dos processos com e sem pipe
	separacao das strings em comandos

-----------------------------------------------------------------------------------------------------------------*/

#include "bib.h"
#include <errno.h>
#include <fcntl.h>

#define FD_WRITE 1
#define FD_READ 0
void redirecionaEscrita(char *cmd[100],char *arquivo, int flag);
//pegando todos os comandos
void Interpretador(char *linguagem){

	/*todos os comandos da linha digitada estara aki*/
	char *cmd[100], *cmdarq;
	/*o comando em se e os seus argumentos estarao aqui*/
	char *cmd_args[100];
	/*sinal para uso ou nao de pipe*/
	short flag = 0;
	/* variaveis auxiliares */
	int n, aux=0, j, i=0,aux2=0, aux3=0;
	/* o tubo de comunicacao de dados e aqui*/
	int **comunicacao;
	int pid;
	/*Primeiro comando*/
	cmd[0] = strtok(linguagem,"|");  
	/* leitura dos n comandos */
	while(((cmd[++i]) = strtok(NULL,"|")) != NULL);
	/*atribuicao de valores
	 n e o numero de comandos e o auxiliar recebe o mesmo para controle
	 */
	n = i;
	aux = n;
	
	/*alocacao de dois pipes com duas posicoes de tamanho 32 bits*/
	comunicacao = (int**)malloc(2 * sizeof(int*));
	for(i=0;i<2;i++)
		comunicacao[i] = (int*)malloc(2 * sizeof(int));
	
	/*se numero de comandos for maior que 1 e ativado a flag */
	if(n > 1){
		//criacao de 2 pipes para comunicacao entre varios processos
		criaPipe((&(comunicacao[0])));
		criaPipe((&(comunicacao[1])));	
		flag = 1;
	} 

	for(i=0;i<n;i++){
		/*j sempre e zerado para substituir o cmd_args anterior*/
		j=0;
		/* divisao do comando e seus argumentos em um vetor de strings*/
		
		if(strstr(linguagem,">") != NULL){
			cmdarq = strtok(linguagem,">");
			char *arquivo = strtok(NULL,"");
			cmd_args[0] = strtok(cmdarq," ");
			while(((cmd_args[++j]) = strtok(NULL," ")) != NULL);
			redirecionaEscrita(cmd_args,arquivo,1);
			break;
		}
		if(strstr(linguagem,"<") != NULL){
			cmdarq = strtok(linguagem,"<");
			char *arquivo = strtok(NULL,"");
			cmd_args[0] = strtok(cmdarq," ");
			while(((cmd_args[++j]) = strtok(NULL," ")) != NULL);
			redirecionaEscrita(cmd_args,arquivo,0);
			break;
		}
		cmd_args[0] = strtok(cmd[i]," ");
		while(((cmd_args[++j]) = strtok(NULL," ")) != NULL);
		
		/*uso da flag para separar os que precisam e os que nao precisam de pipe*/
		if(flag){
			/*
			para que os canais de comunicacao(pipe) fiquem alternando entre se o uso de duas variaveis
			aux3 e aux2 foi utilizado
			*/
			aux3=i;
			if(aux3%2 == 0) aux2 = 1;
			else aux2 = 0;
			/*aux e o numero restante de programas a serem executados*/
			pid = executarComPipe(cmd_args,&aux,&(comunicacao[aux3%2]),&(comunicacao[aux2]),n);		
		}else{
			pid = executarSemPipe(cmd_args);
		}
		
	}
	waitpid(pid,NULL,0);
	
}

int executarSemPipe(char *cmd[100]){
	
	pid_t pid = fork();
	if(pid < 0) {
		perror("erro fork");
		exit(-1);
	}

	if(pid == 0) {
		execvp(cmd[0],cmd);
		exit(-1);
	}
	return pid;
}
int executarComPipe(char *cmd[100],int *p,int **comunicacao,int **comunicacao2, int n){
	
	pid_t pid  = fork();
	if(pid < 0){
		perror("erro fork");
		exit(-1);
	}
	
	if(*p == 1){//aqui e o ultimo filho executado
		if(pid == 0){//filho	
			
			dup2((*comunicacao2)[FD_READ],STDIN_FILENO);
			
			close((*comunicacao2)[FD_WRITE]);
			close((*comunicacao2)[FD_READ]);
			close((*comunicacao)[FD_WRITE]);
			close((*comunicacao)[FD_READ]);
			execvp(cmd[0],cmd);
			perror("erro exec");
			exit(-1);
		}
		//fechando somente o canal utilizado pelo filho
		//close((*comunicacao)[FD_WRITE]);
		close((*comunicacao2)[FD_WRITE]);
	}
	if(*p == n){//aqui e o primeiro filho executado
		if(pid == 0){	
			dup2((*comunicacao)[FD_WRITE],STDOUT_FILENO);
			
			close((*comunicacao)[FD_READ]);
			close((*comunicacao)[FD_WRITE]);
			close((*comunicacao2)[FD_READ]);
			close((*comunicacao2)[FD_WRITE]);
			
			execvp(cmd[0],cmd);
			perror("erro exec");
			exit(-1);
		}	
		//fechando somente o canal utilizado pelo filho
		close((*comunicacao)[FD_WRITE]);
		//close((*comunicacao2)[FD_WRITE]);
	}else{
		if(pid == 0){	
			
			dup2((*comunicacao)[FD_WRITE],STDOUT_FILENO);
			dup2((*comunicacao2)[FD_READ],STDIN_FILENO);
			
			close((*comunicacao)[FD_WRITE]);
			close((*comunicacao2)[FD_READ]);
			close((*comunicacao2)[FD_WRITE]);
			close((*comunicacao)[FD_READ]);
			
			execvp(cmd[0],cmd);
			perror("erro exec");
			exit(-1);
		}	
		//fechando somente o canal utilizado pelo filho
		close((*comunicacao)[FD_WRITE]);
		close((*comunicacao2)[FD_WRITE]);
	}
	/*
	fechando as entradas do pai se nao o filho fica no aguardo da escrita como se nao houvesse terminado
	*/
	
	*p -= 1; //um programa a menos a ser executado
	return pid;//retorno do pid do filho criado
}

void criaPipe(int **comunicacao){
	if(pipe(*comunicacao) == -1){
		perror("erro pipe");
		exit(-1);
	}
}

void redirecionaEscrita(char *cmd[100],char *arquivo, int flag){
	

	FILE *file = fopen(arquivo,"w");
	fclose(file);
	int newfd;
	pid_t pid  = fork();
	if(pid < 0){
		perror("erro fork");
		exit(-1);
	}
	if(flag){
		newfd = open(arquivo,O_WRONLY);
		if(pid == 0){
			dup2(newfd,STDOUT_FILENO);
			close(newfd);
			
			//close((*comunicacao)[FD_WRITE]);O_RDONLY
			execvp(cmd[0],cmd);
			perror("Exec erro");
		}
	}else{
		newfd = open(arquivo,O_RDONLY);
		if(pid == 0){
			dup2(newfd,STDIN_FILENO);
			close(newfd);
			
			//close((*comunicacao)[FD_WRITE]);O_RDONLY
			execvp(cmd[0],cmd);
			perror("Exec erro");
		}
		
	}
	close(newfd);

}
