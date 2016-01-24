#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include"header.h"

#define TABLE_SIZE	256

symtab * hash_table[TABLE_SIZE];
extern int linenumber;
int count = 0 ;
int HASH(char * str){
	int idx=0;
	while(*str){
		idx = idx << 1;
		idx+=*str;
		str++;
	}	
	return (idx & (TABLE_SIZE-1));
}

/*returns the symbol table entry if found else NULL*/

symtab * lookup(char *name){
	int hash_key;
	symtab* symptr;
	if(!name)
		return NULL;
	hash_key=HASH(name);
	symptr=hash_table[hash_key];

	while(symptr){
		if(!(strcmp(name,symptr->lexeme)))
			return symptr;
		symptr=symptr->front;
	}
	return NULL;
}


void insertID(char *name){
	int hash_key;
	symtab* ptr;
	symtab* symptr=(symtab*)malloc(sizeof(symtab));	
	
	hash_key=HASH(name);
	ptr=hash_table[hash_key];
	
	if(ptr==NULL){
		/*first entry for this hash_key*/
		hash_table[hash_key]=symptr;
		symptr->front=NULL;
		symptr->back=symptr;
	}
	else{
		symptr->front=ptr;
		ptr->back=symptr;
		symptr->back=symptr;
		hash_table[hash_key]=symptr;	
	}
	
	strcpy(symptr->lexeme,name);
	symptr->line=linenumber;
	symptr->counter=1;
	count++ ;
}

void printSym(symtab* ptr) 
{
	    printf(" Name = %s \n", ptr->lexeme);
	    printf(" References = %d \n", ptr->counter);
}

void printSymTab()
{
    int i;
    printf("----- Symbol Table ---------\n");
    for (i=0; i<TABLE_SIZE; i++)
    {
        symtab* symptr;
	symptr = hash_table[i];
	while (symptr != NULL)
	{
            printf("====>  index = %d \n", i);
	    printSym(symptr);
	    symptr=symptr->front;
	}
    }
}
int comp(const void *a,const void *b){
	return strcmp((*(symtab**)a)->lexeme ,(*(symtab**)b)->lexeme);
}

void print_Frequency()
{
	printf("Frequency of identifiers:\n");
	symtab **tmp,*ptr;
	int i ,now_count = 0 ;
	tmp = (symtab**)malloc(count * sizeof(symtab*));
	for( i = 0 ; i < TABLE_SIZE; i++){
		for(ptr = hash_table[i] ; ptr!=NULL ; ptr = ptr -> front){
			tmp[now_count++] = ptr ;				
		}
	}
	qsort(tmp,count,sizeof(symtab*),comp);
	for( i = 0 ; i < count ;i++)
		printf("%s %d\n",tmp[i]->lexeme,tmp[i]->counter);
	free(tmp);					

	
}
