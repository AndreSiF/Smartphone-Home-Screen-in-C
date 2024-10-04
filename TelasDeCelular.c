#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//ESTRUTURAS E SUAS DEFINICOES USADAS NO PROGRAMA

struct Apps{
	char nome[30];
	struct Apps *prox;
};

struct Telas{
	int pos;
	struct Telas *prox;
	struct Telas *ant;
	struct Apps *primApp;
};

typedef struct Telas tela;
typedef struct Apps app;

//ESTRUTURAS E SUAS DEFINICOES USADAS NO PROGRAMA

tela *deletarTelas(tela *tel){									// FUNCAO QUE DELETA TELAS VAZIAS
	if(tel == NULL){											// DEPOIS DE MOVER OS APLICATIVOS, AS TELAS QUE NAO POSSUEM AO MENOS UM APP
		return tel;												// SERAO DELETADAS NESSA FUNCAO
	}
	if(tel->primApp == NULL){
		free(tel);
		tel = NULL;
		return tel;
	}
	tela *p;
	for(p = tel; p->prox != NULL; p = p->prox){
		if(p->primApp == NULL){
			p->ant->prox = NULL;
			free(p);
		}
	}
	if(p->primApp == NULL){
		p->ant->prox = NULL;
		free(p);
	}
	return tel;
}

app *moverApps(tela *p){										// FUNCAO QUE MOVE APPS ENTRE AS TELAS
	app *p1;													// PEGA UM APLICATIVO DA TELA SEGUINTE E MOVE PARA A ANTERIOR CASO A ANTERIOR NAO
	p1 = p->prox->primApp;										// ESTEJA COM O LIMITE MAXIMO DE APLICATIVOS
	p->prox->primApp = p1->prox;
	p1->prox = NULL;
	return p1;
}

void organizarTelas(tela *tel){									// FUNCAO PARA ORGANIZAR OS APPS AO DESINSTALAR
	if(tel == NULL){											// E USADA PARA VERIFICAR SE CADA UMA DAS TELAS ATUAIS ESTAO COM O NUMERO
		return;													// ADEQUADO DE APLICATIVO QUE SERIAM 3 CASO POSSUA TELA SEGUINTE
	}
	if(tel->primApp == NULL){
		return;
	}
	int cont;
	tela *p;
	app *p1;
	for(p = tel; p->prox != NULL; p = p->prox){
		for(cont = 0, p1 = p->primApp; p1->prox != NULL; p1 = p1->prox, cont++);
		if(cont < 2){
			p1->prox = moverApps(p);
		}
	}
}

void liberarPrimApp(tela *p){									// FUNCAO SEPARADA PARA DELETAR O PRIMEIRO APP DA LISTA
	app *p1;													// A FUNCAO PARA EXCLUIR O PRIMEIRO APLICATIVO DE UMA TELA E UM POUCO MAIS
	if(p->primApp->prox == NULL){								// COMPLICADA JA QUE NAO USEI LISTA DUPLAMENTE ENCADEADA PARA OS APLICATIVOS
		free(p->primApp);
		p->primApp = NULL;
	}
	else{
		p1 = p->primApp;
		p->primApp = p1->prox;
		free(p1);
	}
}

void liberarApp(app *p){										// FUNCAO QUE LIBERA O ESPACO DO APP DELETADO DA MEMORIA
	app *p2;													// PEGA O APLICATIVO QUE FOI DELETADO E LIBERA SUA POSICAO NA MEMORIA
	p2 = p->prox;
	p->prox = p2->prox;	
	free(p2);
}

tela *removerApp(tela *tel){									// FUNCAO QUE INICIA A REMOCAO DO APP
	char remover[30];											// ELA PERGUNTA O NOME DO APLICATIVO QUE DEVE SER DELETADO E O PROCURA NAS LISTAS
	tela *p, *achado = NULL;									// DE APLICATIVOS ENTRE AS TELAS ALEM DE CHAMAR A FUNCAO DE ORGANIZACAO E LIBERACAO
	app *p1, *p2;												// DAS TELAS. O RETORNO DEVE SEMPRE SER A PRIMEIRA TELA PARA CASO ELA SEJA LIBERADA
	printf("\nLista de APPs instalados: \n");					// O PROGRAMA POSSA IDENTIFICAR QUE ESTA VAZIA
	for(p = tel; p != NULL; p = p->prox){
		for(p1 = p->primApp; p1 != NULL; p1 = p1->prox){
			printf("%s\n", p1->nome);
		}
	}
	printf("\nDigite o nome do APP que deseja desinstalar: ");
	fflush(stdin);
	gets(remover);
	for(p = tel; p != NULL; p = p->prox){
		for(p1 = p->primApp; p1 != NULL; p1 = p1->prox){
			if(!strcmp(p1->nome, remover)){
				if(p1 == p->primApp){
					printf("App sendo removido: %s\n", p1->nome);
					achado = p;
					liberarPrimApp(p);
				}
				else{
					for(p2 = p->primApp; p2->prox->nome != p1->nome; p2 = p2->prox);
					printf("App sendo removido: %s\n", p2->prox->nome);
					achado = p;
					liberarApp(p2);
				}
			}
		}
	}
	if(achado == NULL){
		printf("APP nao existente.\n");
		return tel;
	}
	organizarTelas(tel);
	tel = deletarTelas(tel);
	return tel;
}

tela *instalarPrimApp(){										// FUNCAO QUE INSTALA O PRIMEIRO APP CASO A LISTA DE APPS ESTIVER VAZIA
	tela *tel = (tela *) malloc (sizeof(tela));					// ELA PRIMEIRO ALOCA UMA NOVA TELA, ALOCA O NOVO APP E PEDE SEU NOME
	tel->prox = NULL;											// DEVE SEMPRE RETORNAR A NOVA TELA CRIADA POIS ELA SERA A PRIMEIRA TELA
	tel->ant = NULL;
	tel->pos = 1;
	app *novoApp = (app *) malloc (sizeof(app));
	printf("\nDigite o nome do primeiro app a ser instalado: ");
	fflush(stdin);
	gets(novoApp->nome);
	novoApp->prox = NULL;
	tel->primApp = novoApp;
	return tel;
}

void criarTela(tela *tel, app *novoApp){						// FUNCAO PARA CRIACAO DE NOVAS TELAS QUANDO O LIMITE DE 3 APPS E ALCANCADO
	tela *novaTela = (tela *) malloc (sizeof(tela));			// ELA SIMPLESMENTE ALOCA UMA NOVA TELA, COLOCA O SEU PRIMEIRO APLICATIVO E CONECTA ELA 
	tel->prox = novaTela;										// A TELA ANTERIOR POR ISSO NAO E NECESSARIO RETORNO
	novaTela->ant = tel;
	novaTela->prox = NULL;
	novaTela->primApp = novoApp;
	novaTela->pos = tel->pos + 1;
}

void instalarApp(tela *tel){									// FUNCAO QUE ADICIONA UM APLICATIVO NA MEMORIA DO PROGRAMA
	int c;														// ELA ALOCA UM NOVO APLICATIVO, PERGUNTA O NOME E O COLOCA NA LISTA DE
	app *p;														// APLICATIVOS E RETORNA, CASO TENHA MAIS DE 3 APLICATIVOS ELA CRIA UMA NOVA TELA
	tela *p1;
	app *novoApp = (app *) malloc (sizeof(app));
	printf("\nDigite o nome do app a ser instalado: ");
	fflush(stdin);
	gets(novoApp->nome);
	novoApp->prox = NULL;
	if(tel->primApp == NULL){
		tel->primApp = novoApp;
	}
	else{
		for(p1 = tel; p1 != NULL; p1 = p1->prox){
			for(c = 0, p = p1->primApp; p->prox != NULL; c++, p = p->prox);
			if(c < 2){
				p->prox = novoApp;
				return;
			}
		}
		for(p1 = tel; p1->prox != NULL; p1 = p1->prox);
		criarTela(p1, novoApp);
	}
}

void mostrarApps(app *appAtual){								//FUNCAO RECURSIVA PARA MOSTRAR OS APPS AO USUARIO
	puts(appAtual->nome);										// ELA VARRE A LISTA DE APLICATIVOS DA TELA ESPECIFICA QUE FOI ENVIADA POR PARAMETRO
	if(appAtual->prox != NULL){
		mostrarApps(appAtual->prox);
	}
	return;
}

void mostrarTelas(tela *tel){									// FUNCAO QUE MOSTRA AS DIFERENTES TELAS DO CELULAR, ELA MOSTRA A TELA AO USUARIO, DIZ QUAL
	tela *p = tel;												// TELA ELE SE ENCONTRA ATUALMENTE, DA OPCOES DE NAVEGACAO E EVITA QUE O USUARIO ACESSE
	char escolha[30];											// UMA TELA NAO ALOCADA
	system("cls");
	printf("\nprox -> \tProxima tela\nant ->  \t Tela anterior\nsair -> \t Voltar ao menu\n");
	while(strcmp(escolha, "sair")){	
		printf("\nTela %d:\n", p->pos);
		mostrarApps(p->primApp);
		printf("\nDigite sua escolha: ");
		fflush(stdin);
		gets(escolha);
		strlwr(escolha);
		if(!strcmp(escolha, "prox")){
			if(p->prox != NULL){
				p = p->prox;
			}
			else{
				printf("Ultima tela alcancada.\n");
			}
		}
		else if(!strcmp(escolha, "ant")){
			if(p->ant != NULL){
				p = p->ant;
			}
			else{
				printf("Primeira tela alcancada.\n");
			}
		}
		else if(!strcmp(escolha, "sair")){
			return;
		}
		else{
			printf("Erro, tente novamente.\n");
		}
	}
}

app *criarAppsIniciais(){										// FUNCAO DE ALOCACAO DE DOIS APPS INICIAIS
	app *app1 = (app *) malloc (sizeof(app));					// SO ALOCA UNS APLICATIVOS INICIAIS, O LIMITE DE APLICATIVOS E 3
	app *app2 = (app *) malloc (sizeof(app));
	strcpy(app1->nome, "Configuracao");
	strcpy(app2->nome, "Videos");
	app1->prox = app2;
	app2->prox = NULL;
	return app1;
}

tela *appsIniciais(tela *tel){									// FUNCAO DE ALOCACAO DA TELA INICIAL
	tel = (tela *) malloc (sizeof(tela));						// ALOCA A PRIMEIRA TELA DO APLICATIVO QUE PODE SER DELETADA CASO NECESSARIO
	tel->pos = 1;
	tel->ant = NULL;
	tel->prox = NULL;
	tel->primApp = criarAppsIniciais();
	return tel;
}

int main(){														// DECIDI DEIXAR AS ESCOLHAS MAIS BASICAS DO USUARIO NA MAIN
	tela *telaInicial = NULL;
	tela *p;
	telaInicial = appsIniciais(telaInicial);
	char escolha[20];
	while(strcmp(escolha, "sair")){
		system("cls");
		printf("\nver ->\t\tMostra lista de apps\ninstal ->\tInstala novo app\nsair ->\t\tFinaliza o programa\ndesin ->\tDesinstala um app\nDigite o que deseja fazer: ");
		fflush(stdin);
		gets(escolha);
		strlwr(escolha);
		if(!strcmp(escolha, "ver")){
			if(telaInicial == NULL){
				printf("\nNenhum APP instalado, tente instalar um APP com o comando instal.\n");
			}
			else{
				mostrarTelas(telaInicial);	
			}
		}
		else if(!strcmp(escolha, "instal")){
			if(telaInicial != NULL){
				instalarApp(telaInicial);	
			}
			else{
				telaInicial = instalarPrimApp();
			}
		}
		else if(!strcmp(escolha, "desin")){
			if(telaInicial == NULL){
				printf("\nNenhum APP instalado, tente instalar um APP com o comando instal. \n");
			}
			else{
				telaInicial = removerApp(telaInicial);	
			}
		}
		else if(!strcmp(escolha, "sair")){
			printf("Saindo...");
		}
		else{
			printf("Erro, tente novamente\n");
		}
	}
	for(p = telaInicial; p != NULL; p = p->prox){
		free(p->primApp);
	}
	free(telaInicial);
	return 0;
}