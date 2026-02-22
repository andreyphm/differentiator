#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <differentiator.h>

node_t* GetG(char** current_character_ptr);
node_t* GetN(char** current_character_ptr);
node_t* GetP(char** current_character_ptr);
node_t* GetE(char** current_character_ptr);
node_t* GetT(char** current_character_ptr);
node_t* GetV(char** current_character_ptr);
node_t* GetS(char** current_character_ptr);
node_t* GetF(char** current_character_ptr, const char* string);

bool is_func_name(const char* string);

#endif // PARSER_H
