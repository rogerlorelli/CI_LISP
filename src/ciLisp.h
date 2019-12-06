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

typedef enum {LAMBDA_TYPE} SYMBOL_TYPE;


OPER_TYPE resolveFunc(char *);

// Types of Abstract Syntax Tree nodes.
// Initially, there are only numbers and functions.
// You will expand this enum as you build the project.
typedef enum {
    NUM_NODE_TYPE,
    FUNC_NODE_TYPE,
    SYM_NODE_TYPE,
    COND_NODE_TYPE
} AST_NODE_TYPE;

typedef struct {
    struct ast_node *cond;
    struct ast_node *ifTrue; // to eval if cond is nonzero
    struct ast_node *ifFalse; // to eval if cond is zero
} COND_AST_NODE;

// Types of numeric values
typedef enum { NO_TYPE, INT_TYPE, DOUBLE_TYPE } NUM_TYPE;

// Node to store a number.
typedef struct {
    NUM_TYPE type;
    double value;
} NUM_AST_NODE;

// Values returned by eval function will be numbers with a type.
// They have the same structure as a NUM_AST_NODE.
// The line below allows us to give this struct another name for readability.
typedef NUM_AST_NODE RET_VAL;

// Node to store a function call with its inputs
typedef struct {
    OPER_TYPE oper;
    char *name;
    struct ast_node *opList;
} FUNC_AST_NODE;

typedef struct symbol_table_node {
    SYMBOL_TYPE type;
    NUM_TYPE val_type;
    char *ident;
    struct ast_node *val;
    struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;

typedef struct arg_table_node {
    char *ident;
    NUM_AST_NODE value;
    struct arg_table_node *next;
} ARG_TABLE_NODE;

typedef struct symbol_ast_node {
    char *ident;
} SYMBOL_AST_NODE;

// Generic Abstract Syntax Tree node. Stores the type of node,
// and reference to the corresponding specific node (initially a number or function call).
typedef struct ast_node {
    AST_NODE_TYPE type;
    SYMBOL_TABLE_NODE *symbolTable;
    ARG_TABLE_NODE *arg_list;
    struct ast_node *parent;
    union {
        NUM_AST_NODE number;
        FUNC_AST_NODE function;
        SYMBOL_AST_NODE symbol;
        COND_AST_NODE condition;
    } data;
    struct ast_node *next;
    bool isCustom;
} AST_NODE;

AST_NODE *createCondASTNode(AST_NODE *condNode, AST_NODE *trueNode, AST_NODE *falseNode);

SYMBOL_TABLE_NODE *createSymbolTableNode(char *id, AST_NODE *op1, NUM_TYPE type);

SYMBOL_TABLE_NODE *linkSymbolTableNode(SYMBOL_TABLE_NODE *node1, SYMBOL_TABLE_NODE *node2);

AST_NODE *linkSymbolTableToAST(SYMBOL_TABLE_NODE *symbNode, AST_NODE *node);

AST_NODE *createSymbolNode(char *id);

AST_NODE *linkASTNodes(AST_NODE *node1, AST_NODE *node2);

AST_NODE *createNumberNode(double value, NUM_TYPE type);

AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1);

SYMBOL_TABLE_NODE *createCustomFunctionNode(NUM_TYPE numType, char *funcName, ARG_TABLE_NODE *arg_list, AST_NODE *funcNode);

ARG_TABLE_NODE *createArgNode(char *id);

ARG_TABLE_NODE *linkArgList(char* id1, ARG_TABLE_NODE *node2);

void freeNode(AST_NODE *node);

RET_VAL eval(AST_NODE *node);
RET_VAL evalNumNode(NUM_AST_NODE *numNode);
RET_VAL evalFuncNode(AST_NODE *funcNode);
RET_VAL evalCondNode(COND_AST_NODE *condNode);
RET_VAL evalSymbNode(AST_NODE *symbNode);
RET_VAL evalCustomFunc(AST_NODE *funcNode);
RET_VAL readHelper(AST_NODE *funcNode);

void printRetVal(RET_VAL val);

RET_VAL maxHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL minHelper(RET_VAL *op1,RET_VAL *op2);
RET_VAL addHelper(AST_NODE *op_list);
RET_VAL multHelper(AST_NODE *op_list);

NUM_TYPE numTypeHelper1(RET_VAL *op1);
NUM_TYPE numTypeHelper2(RET_VAL *op1,RET_VAL *op2);


#endif
