#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "ciLispParser.h"

int yyparse(void);

int yylex(void);

void yyerror(char *);

// Enum of all operators.
// must be in sync with funcs in resolveFunc()
typedef enum oper {
    NEG_OPER, // 0
    ABS_OPER,
    EXP_OPER,
    SQRT_OPER,
    ADD_OPER,
    SUB_OPER,
    MULT_OPER,
    DIV_OPER,
    REMAINDER_OPER,
    LOG_OPER,
    POW_OPER,
    MAX_OPER,
    MIN_OPER,
    EXP2_OPER,
    CBRT_OPER,
    HYPOT_OPER,
    READ_OPER,
    RAND_OPER,
    PRINT_OPER,
    EQUAL_OPER,
    LESS_OPER,
    GREATER_OPER,
    CUSTOM_OPER =255
} OPER_TYPE;

OPER_TYPE resolveFunc(char *);

// Types of Abstract Syntax Tree nodes.
// Initially, there are only numbers and functions.
// You will expand this enum as you build the project.
typedef enum {
    NUM_NODE_TYPE,
    FUNC_NODE_TYPE,
    SYM_NODE_TYPE
} AST_NODE_TYPE;

// Types of numeric values
typedef enum { NO_TYPE, INT_TYPE, DOUBLE_TYPE } NUM_TYPE;

// Node to store a number.
typedef struct {
    NUM_TYPE type;
    union{
        double dval;
        long ival;
    } value;
} NUM_AST_NODE;

// Values returned by eval function will be numbers with a type.
// They have the same structure as a NUM_AST_NODE.
// The line below allows us to give this struct another name for readability.
typedef NUM_AST_NODE RET_VAL;

// Node to store a function call with its inputs
typedef struct {
    OPER_TYPE oper;
    char* ident; // only needed for custom functions
    struct ast_node *op1;
    struct ast_node *op2;
} FUNC_AST_NODE;

typedef struct symbol_table_node {
    NUM_TYPE val_type;
    char *ident;
    struct ast_node *val;
    struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;

typedef struct symbol_ast_node {
    char *ident;
} SYMBOL_AST_NODE;

// Generic Abstract Syntax Tree node. Stores the type of node,
// and reference to the corresponding specific node (initially a number or function call).
typedef struct ast_node {
    AST_NODE_TYPE type;
    SYMBOL_TABLE_NODE *symbolTable;
    struct ast_node *parent;
    union {
        NUM_AST_NODE number;
        FUNC_AST_NODE function;
        SYMBOL_AST_NODE symbol;
    } data;
} AST_NODE;



SYMBOL_TABLE_NODE *createSymbolTableNode(char *id, AST_NODE *op1,NUM_TYPE type);

SYMBOL_TABLE_NODE *linkSymbolNode(SYMBOL_TABLE_NODE *node1, SYMBOL_TABLE_NODE *node2);

AST_NODE *linkSymbolTableToAST(SYMBOL_TABLE_NODE *symbNode, AST_NODE *node);

AST_NODE *createSymbolNode(char *id);

AST_NODE *createNumberNode(double value, NUM_TYPE type);

AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1, AST_NODE *op2);

void freeNode(AST_NODE *node);

RET_VAL eval(AST_NODE *node);
RET_VAL evalNumNode(NUM_AST_NODE *numNode);
RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode);
RET_VAL evalSymbNode(AST_NODE *symbNode);

void printRetVal(RET_VAL val);

RET_VAL negHelper(RET_VAL *op1);
RET_VAL absHelper(RET_VAL *op1);
RET_VAL expHelper(RET_VAL *op1);
RET_VAL sqrtHelper(RET_VAL *op1);
RET_VAL addHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL subHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL multHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL divHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL remHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL maxHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL minHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL logHelper(RET_VAL *op1);
RET_VAL powHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL exp2Helper(RET_VAL *op1);
RET_VAL cbrtHelper(RET_VAL *op1);
RET_VAL hypotHelper(RET_VAL *op1,RET_VAL *op2);

NUM_TYPE numTypeHelper1(RET_VAL *op1);
NUM_TYPE numTypeHelper2(RET_VAL *op1,RET_VAL *op2);


#endif
