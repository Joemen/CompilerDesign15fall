#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// This file is for reference only, you are not required to follow the implementation. //

int HASH(char * str) {
	int idx=0;
	while (*str){
		idx = idx << 1;
		idx+=*str;
		str++;
	}
	return (idx & (HASH_TABLE_SIZE-1));
}

SymbolTable symbolTable;

SymbolTableEntry* newSymbolTableEntry(int nestingLevel)
{
    SymbolTableEntry* symbolTableEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
    symbolTableEntry->nextInHashChain = NULL;
    symbolTableEntry->prevInHashChain = NULL;
    symbolTableEntry->nextInSameLevel = NULL;
    symbolTableEntry->sameNameInOuterLevel = NULL;
    symbolTableEntry->attribute = NULL;
    symbolTableEntry->name = NULL;
    symbolTableEntry->nestingLevel = nestingLevel;
    return symbolTableEntry;
}

void removeFromHashTrain(int hashIndex, SymbolTableEntry* entry)
{
}

void enterIntoHashTrain(int hashIndex, SymbolTableEntry* entry)
{
}

void initializeSymbolTable()
{
	symbolTable.scopeDisplay = (SymbolTableEntry**)malloc(100*sizeof(SymbolTableEntry*));
	symbolTable.currentLevel = 0;
	symbolTable.scopeDisplayElementCount = 1;
	// 先將 int, float, void 加入
	SymbolAttribute *symAttr;

	symAttr = malloc(sizeof(SymbolAttribute));
	symAttr->attributeKind = TYPE_ATTRIBUTE;
	symAttr->attr.typeDescriptor = malloc(sizeof(TypeDescriptor));
	symAttr->attr.typeDescriptor->properties.dataType = INT_TYPE;
	enterSymbol("int", symAttr);

	symAttr = malloc(sizeof(SymbolAttribute));
	symAttr->attributeKind = TYPE_ATTRIBUTE;
	symAttr->attr.typeDescriptor = malloc(sizeof(TypeDescriptor));
	symAttr->attr.typeDescriptor->properties.dataType = FLOAT_TYPE;
	enterSymbol("float", symAttr);

	symAttr = malloc(sizeof(SymbolAttribute));
	symAttr->attributeKind = TYPE_ATTRIBUTE;
	symAttr->attr.typeDescriptor = malloc(sizeof(TypeDescriptor));
	symAttr->attr.typeDescriptor->properties.dataType = VOID_TYPE;
	enterSymbol("void", symAttr);	
}

void symbolTableEnd()
{
}

SymbolTableEntry* retrieveSymbol(char* symbolName)
{
	int hashnumber = HASH(symbolName);
	if(symbolTable.hashTable[hashnumber] == NULL)
		return NULL;
	else{
		SymbolTableEntry* entry = symbolTable.hashTable[hashnumber];
		while(entry != NULL){
			if(strcmp(entry->name, symbolName) == 0)
				return symbolTable.hashTable[hashnumber];
			else
				entry = entry->nextInHashChain;
		}
		return NULL;
	}
}

SymbolTableEntry* enterSymbol(char* symbolName, SymbolAttribute* attribute)
{
	int hashnumber = HASH(symbolName);
	SymbolTableEntry* new_entry = newSymbolTableEntry(symbolTable.currentLevel);
	//In big hash table part
	if(symbolTable.hashTable[hashnumber] == NULL){
		//New node
		new_entry->name = symbolName;
		new_entry->attribute = attribute;
		symbolTable.hashTable[hashnumber] = new_entry;
	}
	else{
		//Existed node
		new_entry->name = symbolName;
		new_entry->attribute = attribute;
		new_entry->nextInHashChain = symbolTable.hashTable[hashnumber];
		symbolTable.hashTable[hashnumber]->prevInHashChain = new_entry;
		symbolTable.hashTable[hashnumber] = new_entry;
	}
	//In scope display
	new_entry->nextInSameLevel = symbolTable.scopeDisplay[symbolTable.scopeDisplayElementCount-1];
	symbolTable.scopeDisplay[symbolTable.scopeDisplayElementCount-1] = new_entry;
	return new_entry;
}

SymbolTableEntry* getfunction(){
	for(int i = symbolTable.scopeDisplayElementCount-2; i >=0; i--)
		if(symbolTable.scopeDisplay[i]->attribute->attributeKind == FUNCTION_SIGNATURE)
			return symbolTable.scopeDisplay[i];
	return NULL;
}


//remove the symbol from the current scope
void removeSymbol(char* symbolName)
{
}

int declaredLocally(char* symbolName)
{
	int hashnumber = HASH(symbolName);
	if(symbolTable.hashTable[hashnumber] == NULL)
		return 0;
	SymbolTableEntry* entry = symbolTable.hashTable[hashnumber];
	while(entry != NULL){
		if(strcmp(entry->name, symbolName) == 0){
			if(entry->nestingLevel == symbolTable.currentLevel)
				return 1;
			else
				return 0;
		}
		entry = entry->nextInHashChain;
	}
	return 0;
}

void openScope()
{
	symbolTable.scopeDisplayElementCount += 1;
	symbolTable.currentLevel += 1;
//	symbolTable.scopeDisplay = (SymbolTableEntry**)realloc(symbolTable.scopeDisplay, symbolTable.currentLevel*sizeof(SymbolTableEntry*));
	symbolTable.scopeDisplay[symbolTable.scopeDisplayElementCount-1] = NULL;
}

void closeScope()
{
	symbolTable.scopeDisplayElementCount -= 1;
	SymbolTableEntry* removing_symbol = symbolTable.scopeDisplay[symbolTable.scopeDisplayElementCount];
	while(removing_symbol != NULL){
		//remove it from big table: connect its prev and next
		int hashnumber = HASH(removing_symbol->name);
		if(removing_symbol->nextInHashChain != NULL){
			removing_symbol->nextInHashChain->prevInHashChain = NULL;
			symbolTable.hashTable[hashnumber] = removing_symbol->nextInHashChain;
		}
		else
			symbolTable.hashTable[hashnumber] = NULL;
		SymbolTableEntry* old_symbol = removing_symbol;
		removing_symbol = removing_symbol->nextInSameLevel;
//		free(old_symbol);
	}
//	symbolTable.scopeDisplay = (SymbolTableEntry**)realloc(symbolTable.scopeDisplay, symbolTable.currentLevel*sizeof(SymbolTableEntry*));
	symbolTable.scopeDisplay[symbolTable.scopeDisplayElementCount] = NULL;
	symbolTable.currentLevel -= 1;
}





