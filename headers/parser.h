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

#endif // PARSER_H
