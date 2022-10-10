#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define SIZE 1024


int ur=0;
int first=0;
int numC=0;
int lastl=0;
int lendiff=0;

typedef struct node{
	char tipo;
	int ind1,ind2;
    int len;
	char **saveU,**saveR;
	struct node *next;
	struct node *prev;
}element;

typedef struct list{
    int count;
    struct node *head;
    struct node *tail;
}lista;

void init_list(lista *l);
void list_insert(lista *l, char *t, int ind1, int ind2, char **cont);
void add_toElem(lista *l,char **cont);
void delete_list(lista *l);
int control_list(lista *l);
void modify(lista *l);
void change(int ind1, int ind2, char **cont, char **save);
void del(int ind1, int ind2, char **cont);
void print(int ind1, int ind2, char **cont);
void undo(int numero, lista *l, lista *u, char **cont);
void redo(int numero, lista *l, lista *r, char **cont);

int main(){
	int ind1,ind2,i;
	char* str=(char*)malloc(10*sizeof(char));
	char* rest=(char*)malloc(8*sizeof(char));
	char* op=" ";
	char** content=(char**)malloc(sizeof(char**)*(10*SIZE));
	str[0]=' ';
	rest[0]=' ';

    
	for(i=0;i<10;i++){
		content[i]=".";
	}
   	
	lista l,l_edit;
    	init_list(&l);
	init_list(&l_edit);
	
	while(str[0]!='q'){
		if(!fgets(str,sizeof(str)+5,stdin))
			printf("ERROR!\n");
		if(str[strlen(str)-1]=='\n')
			str[strlen(str)-1]='\0';
	
		ind1=strtol(str,&rest,10);
		
		if(rest[0]==','){
			rest++;
			ind2=strtol(rest,&op,10);
			
			if(!strcmp(op,"c")){
				if(control_list(&l)==1){
                    			modify(&l);
					delete_list(&l_edit);
					init_list(&l_edit);
                    first=0;
                    ur=0;
				}
				numC++;
				list_insert(&l,op,ind1,ind2,content);
				change(ind1,ind2,content,NULL);
				add_toElem(&l,content);
			}
			
			else if(!strcmp(op,"d")){
				if(control_list(&l)==1){
                    			modify(&l);
					delete_list(&l_edit);
					init_list(&l_edit);
                    first=0;
                    ur=0;
				}
				list_insert(&l,op,ind1,ind2,content);
                if(numC>0)
                    del(ind1,ind2,content);
			}
				
			else if(!strcmp(op,"p"))
				print(ind1,ind2,content);
			
		}
		
		else{
			if(ind1!=0){
			
				if(!strcmp(rest,"u")){
                    if(first==0)
                        list_insert(&l,rest,ind1,0,NULL);
                    first=1;
                    ur+=ind1;
                    if(ur>0)
                        undo(ind1,&l,&l_edit,content);
                }
				
				else if(!strcmp(rest,"r")){
					if(control_list(&l)==1){
                        ur-=ind1;
                        if(ur<=0)
                            ur=0;
                        redo(ind1,&l,&l_edit,content);
                    }
    			}
			}
		}
	}
	
	return 0;
}


void init_list(lista *l){
    l->count=0;
    l->head= l->tail=NULL;
}

void list_insert(lista *l, char *t, int ind1, int ind2, char **cont){
    element *nodo;
	nodo=malloc(sizeof(element));
	
	nodo->tipo=t[0];
    nodo->ind1=ind1;
    nodo->ind2=ind2;
    if(nodo->tipo=='c'){
        nodo->saveU=(char**)malloc(sizeof(char**)*(ind2-ind1+1));
	    int i=0,l=strlen(cont[ind1-1+i]);
	    for(i=0;i<(ind2-ind1+1);i++){
            nodo->saveU[i]=cont[ind1-1+i];
	    }
	}
	else if(nodo->tipo=='d' && numC>0){
		int i=0,l;
		nodo->saveU=(char**)malloc(sizeof(char**)*lastl);
        if(ind1==0)
            ind1++;
        while(ind1<lastl){
            nodo->saveU[i]=cont[ind1-1];
            i++;
            ind1++;
		}
	}
	else
		nodo->saveU=NULL;
	
	nodo->saveR=NULL;
    nodo->next=NULL;

    if (l->count==0){
        nodo->prev=NULL;
        l->head=l->tail=nodo;
    }
    else{
        nodo->prev=l->tail;
        l->tail->next=nodo;
        l->tail=nodo;
    }
    l->count++;
    lendiff=lastl;
    if(nodo->tipo=='c' && numC>0){
		if(numC==1)
            nodo->len=ind2;
	    else{
            if(lastl<=ind2)
                nodo->len=ind2;
            else
                nodo->len=lastl;
        }
    }
    if(nodo->tipo=='d'){
        if(nodo->ind1>lastl)
            nodo->len=lastl;
        else{
            if(ind2>lastl)
                ind2=lastl;
            int diff=ind2-nodo->ind1+1;
            nodo->len=lastl-diff;
        }
    }
    lastl=nodo->len;
}

void add_toElem(lista *l,char **cont){
	int i=0;
	element *nodo;
	nodo=l->tail;
	nodo->saveR=(char**)malloc(sizeof(char**)*(nodo->ind2-nodo->ind1+1));
	while(i<(nodo->ind2-nodo->ind1+1)){
		nodo->saveR[i]=cont[nodo->ind1-1+i];
        i++;
	}
}

void delete_list(lista *l){
    element *el1,*el2;
    el1=l->head;
    int i;
    for(i=0;i<l->count;i++){
        el2=el1->next;
        free(el1);
        el1=el2;
    }
}

int control_list(lista *l){
	element *el;
    el= l->tail;
    	
    if(el->tipo=='u')
		return 1;
	
	return 0;
}

void modify(lista *l){
    element *el=l->tail->prev;
    l->tail=el;
    l->count--;
}

void change(int ind1, int ind2, char **cont, char **save){
	int i=0,l=0;
	char* s=(char*)malloc(300);
	s[0]=' ';
	if(ind1==0)
		ind1++;
	ind1--;
	while(ind1<ind2){
		if(save==NULL){
			if(!fgets(s,300,stdin))
				printf("ERROR!\n");
			l=strlen(s);
			if(s[l-1]=='\n')
				s[l-1]='\0';
			cont[ind1]=(char*)malloc(l);
			memcpy(cont[ind1],s,l);
			
		}
		else if(save!=NULL){
			cont[ind1]=save[i];
        }
		i++;
		ind1++;
	}
	if(save==NULL){
		if(scanf("%s",s)!=1)
			printf("Failed to read the string.\n");
		if(s[0]!='.')
			printf("ERROR\n");
	}
    free(s);
}

void del(int ind1, int ind2, char **cont){
    if(ind1==0 && ind2==0 || (ind1>lastl || lendiff==0))
        return;
    if(ind1!=0)
        ind1--;
    if(ind2>lendiff)
        ind2=lendiff;
    int diff=ind2-ind1+2;
    
    while(ind1+diff<lendiff){
        if(cont[ind1]!="." || cont!=NULL)
            cont[ind1]=cont[ind1+diff];
        else{
            cont[ind1]=".";
        }
        ind1++;
    }
    while(ind1<ind2){
        cont[ind1]=".";
        ind1++;
    }
    
}

void print(int ind1, int ind2, char **cont){
	if(ind1==0){
		printf(".\n");
		ind1++;
	}
	ind1--;
	if(cont[ind1]==NULL)
            printf("PRINT ===> Cont[%d]=.\n",ind1);
	else
            printf("PRINT ===> Cont[%d]=%s\n",ind1,cont[ind1]);
	ind1++;
	}
	if(ind1>lastl)
        while(ind1<ind2){
            printf(".\n");
            ind1++;
        }
}

void undo(int numero, lista *l, lista *u, char **cont){
	if(ur>(l->count-1+u->count)){
        ur=l->count-1+u->count;
        numero=l->count-1;
    }
	if(numero==0 || l->head->tipo=='u')
		return;

	element *el;
	el=l->tail->prev;
    int i;
    if(u->count!=0){
    	el=u->tail;
        for(i=0;i<u->count;i++)
            el=el->prev;
    }
    for(i=0;i<numero;i++){
       lendiff=el->len;
        if(l->count>1)
            lastl=el->prev->len;
        else
            lastl=0;
		if(el->tipo=='c'){
			change(el->ind1,el->ind2,cont,el->saveU);
            numC--;
        }
		else if(el->tipo=='d' && numC>0){
			int j=0,k=el->ind1-1;
            
            while(k<lendiff){
                if(el->saveU!=NULL){
                    cont[k]=el->saveU[j];
                    k++;
                }
                j++;
            }
		}
		
		el=el->prev;
	}
	
	el=l->tail->prev;
    if(u->count==0){
    	if((l->count-1)<=numero){
          	u->count=l->count-1;
		u->head=l->head;
		u->tail=el;
            l->count=1;
            l->head=l->tail;
	}
	else{
		l->count-=numero;
		u->count+=numero;
		u->tail=el;
			
		    while(numero!=0){
		    	u->head=el;
		    	el=el->prev;			   
		        numero--;
		    }
		    el->next=l->tail;
		    l->tail->prev=el;
		}
	}
	else{
		if((l->count-1)<=numero){
			u->count=u->count+l->count-1;
			el->next=u->head;
			u->head=l->head;
            l->count=1;
            l->head=l->tail;
		}
		else{
			l->count-=numero;
			u->count+=numero;
            el->next=u->head;
		    while(numero!=0){
		    	u->head=el;
                el=el->prev;
		        numero--;
		    }
		    el->next=l->tail;
		    l->tail->prev=el;
		}
    	
	}
}

void redo(int numero, lista *l, lista *r, char **cont){
	if(numero==0 || r->count==0 || ur>r->count)
		return;	
	element *el;
	
	el=r->head;
    int i,min=0;
    if(r->count<numero)
    	min=r->count;
    else
    	min=numero;
    
	for(i=0;i<min;i++){
        lendiff=lastl;
        lastl=el->len;
        
		if(el->tipo=='c'){
			change(el->ind1,el->ind2,cont,el->saveR);
            numC++;
        }
		else if(el->tipo=='d'){
        	del(el->ind1,el->ind2,cont);
        }
		el=el->next;
	}
	
	if(l->count==1){
		l->head=r->head;
		if(r->count<=numero){
			l->count=r->count;
			l->tail=r->tail;
			init_list(r);
            first=0;
		}
		else{
			el=r->head;
			l->count+=numero;
			r->count-=numero;
			while(numero!=1){
		    	el=el->next;
		        numero--;
		    }
		    r->head=el->next;
		    el->next=l->tail;
		    l->tail->prev=el;
		}
	}
	else{
		el=l->tail->prev;
		el->next=r->head;
		if(r->count<=numero){
			l->tail=r->tail;
			l->count=l->count+r->count-1;
			init_list(r);
            first=0;
		}
		else{
			r->count-=numero;
			l->count+=numero;
		    while(numero!=0){
		    	el=el->next;
		        numero--;
		    }
		    r->head=el->next;
		    el->next=l->tail;
            l->tail->prev=el;
		}
	}
}
