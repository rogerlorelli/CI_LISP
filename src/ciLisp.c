#include "ciLisp.h"
#include "math.h"

void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

// Array of string values for operations.
// Must be in sync with funcs in the OPER_TYPE enum in order for resolveFunc to work.
char *funcNames[] = {
        "neg",
        "abs",
        "exp",
        "sqrt",
        "add",
        "sub",
        "mult",
        "div",
        "remainder",
        "log",
        "pow",
        "max",
        "min",
        "exp2",
        "cbrt",
        "hypot",
        "read",
        "rand",
        "print",
        "equal",
        "less",
        "greater",
        "print",
        ""
};

OPER_TYPE resolveFunc(char *funcName) {
    int i = 0;
    while (funcNames[i][0] != '\0') {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the number.
// Sets the AST_NODE's type to number.
// Populates the value of the contained NUMBER_AST_NODE with the argument value.
// SEE: AST_NODE, NUM_AST_NODE, AST_NODE_TYPE.
AST_NODE *createNumberNode(double value, NUM_TYPE type) {
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, assign values to contained NUM_AST_NODE - done
    node->type = NUM_NODE_TYPE;
    switch (type) {
        case INT_TYPE:
            node->data.number.type = INT_TYPE;
            //node->data.number.value.ival = (long)value;
            break;
        case DOUBLE_TYPE:
            node->data.number.type = DOUBLE_TYPE;
            //node->data.number.value.dval = value;
            break;
        default:
            printf("Invalid NUM_NODE_TYPE\n");
    }
    node->data.number.value.dval = value;
    return node;
}

// Called when an f_expr is created (see ciLisp.y).
// Creates an AST_NODE for a function call.
// Sets the created AST_NODE's type to function.
// Populates the contained FUNC_AST_NODE with:
//      - An OPER_TYPE (the enum identifying the specific function being called)
//      - 2 AST_NODEs, the operands
// SEE: AST_NODE, FUNC_AST_NODE, AST_NODE_TYPE.
AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1) {
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, populate contained FUNC_AST_NODE - done
    // NOTE: you do not need to populate the "ident" field unless the function is type CUSTOM_OPER.
    // When you do have a CUSTOM_OPER, you do NOT need to allocate and strcpy here.
    // The funcName will be a string identifier for which space should be allocated in the tokenizer.
    // For CUSTOM_OPER functions, you should simply assign the "ident" pointer to the passed in funcName.
    // For functions other than CUSTOM_OPER, you should free the funcName after you're assigned the OPER_TYPE.
    node->type = FUNC_NODE_TYPE;
    node->data.function.oper = resolveFunc(funcName);
    if (node->data.function.oper == CUSTOM_OPER) {
        node->data.function.name = funcName;
    } else {
        free(funcName);
    }
    node->data.function.opList = op1;
    AST_NODE *currNode = op1;
    while (currNode != NULL) {
        currNode->parent = node;
        currNode = currNode->next;
    }
    return node;
}

AST_NODE *createCondASTNode(AST_NODE *condNode, AST_NODE *trueNode, AST_NODE *falseNode){
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");
    node->type = COND_NODE_TYPE;
    node->data.condition.cond = condNode;
    node->data.condition.ifTrue = trueNode;
    node->data.condition.ifFalse = falseNode;
    condNode->parent = node;
    trueNode->parent = node;
    falseNode->parent = node;
    return node;
}

// Called after execution is done on the base of the tree.
// (see the program production in ciLisp.y)
// Recursively frees the whole abstract syntax tree.
// You'll need to update and expand freeNode as the project develops.
void freeNode(AST_NODE *node) {
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE) {
        // Recursive calls to free child nodes
        AST_NODE *currNode = node->next;
        AST_NODE *prevNode;
        while (currNode != NULL) {
            prevNode = currNode;
            currNode = currNode->next;
            freeNode(prevNode);
        }

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER) {
            free(node->data.function.name);
        }
    }
    if (node->type == SYM_NODE_TYPE) {
        free(node->data.symbol.ident);
    }
    free(node);
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node) {
    if (!node)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch. - done
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type) {
        case NUM_NODE_TYPE:
            result = evalNumNode(&node->data.number);
            break;
        case FUNC_NODE_TYPE:
            result = evalFuncNode(node);
            break;
        case SYM_NODE_TYPE:
            result = evalSymbNode(node);
            break;
        case COND_NODE_TYPE:
            result = evalCondNode(&node->data.condition);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
    }

    return result;
}


RET_VAL evalCondNode(COND_AST_NODE *condNode){
    if (!condNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    switch((int)eval(condNode->cond).value.dval)
    {
        case 1:
            result = eval(condNode->ifTrue);
            break;
        case 0:
            result = eval(condNode->ifFalse);
            break;
        default:
            printf("Bug at eval Condition\n");
            break;
    }

    return result;
}


RET_VAL evalSymbNode(AST_NODE *symbNode) {
    if (!symbNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};
    AST_NODE *outerNode = symbNode;

    while (outerNode != NULL) {
        if (outerNode->isCustom) {
            ARG_TABLE_NODE *argNode = outerNode->arg_list;
            while(argNode != NULL){
                if (strcmp(symbNode->data.symbol.ident, argNode->ident) == 0) {
                    result = argNode->value;
                    return result;
                }
                argNode = argNode->next;
            }
        } else {
            SYMBOL_TABLE_NODE *currNode = outerNode->symbolTable;
            while (currNode != NULL) {
                if (strcmp(symbNode->data.symbol.ident, currNode->ident) == 0) {
                    result = eval(currNode->val);
                    if (currNode->val_type == INT_TYPE) {
                        if (result.type == DOUBLE_TYPE) {
                            printf("WARNING: precision loss in the assignment for variable %s\n",
                                   outerNode->symbolTable->ident);
                        }
                        result.value.dval = lround(result.value.dval);
                        result.type = INT_TYPE;
                    } else if (currNode->val_type == DOUBLE_TYPE) {
                        result.type = DOUBLE_TYPE;
                    }
                    return result;
                }
                currNode = currNode->next;
            }
        }
        outerNode = outerNode->parent;
    }
    return result;
}


// returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// DOES NOT allocate space for a new RET_VAL.
RET_VAL evalNumNode(NUM_AST_NODE *numNode) {
    if (!numNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the values stored in the node. -done
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE
    result.type = numNode->type;
    result.value.dval = numNode->value.dval;
    return result;
}

RET_VAL evalFuncNode(AST_NODE *funcNode) {
    if (!funcNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE

    RET_VAL op1;
    RET_VAL op2;
    AST_NODE *currNode = funcNode->data.function.opList;
    if (currNode != NULL && funcNode->data.function.oper != PRINT_OPER) {
        op1 = eval(currNode);
        currNode = currNode->next;
        op2 = eval(currNode);
    }
    bool hasOperands = (funcNode->data.function.opList != NULL);
    bool unary = (currNode == NULL);
    bool unaryFunc = false;
    bool binaryFunc = false;
    do {
        switch (funcNode->data.function.oper) {
            case NEG_OPER:
                result.value.dval = -1 * op1.value.dval;
                unaryFunc = true;
                break;
            case ABS_OPER:
                result.value.dval = fabs(op1.value.dval);
                result.type = op1.type;
                unaryFunc = true;
                break;
            case EXP_OPER:
                result.value.dval = exp(op1.value.dval);
                unaryFunc = true;
                break;
            case SQRT_OPER:
                result.value.dval = sqrt(op1.value.dval);
                unaryFunc = true;
                break;
            case CBRT_OPER:
                result.value.dval = cbrt(op1.value.dval);
                unaryFunc = true;
                break;
            case LOG_OPER:
                result.value.dval = log(op1.value.dval);
                unaryFunc = true;
                break;
            case EXP2_OPER:
                result.value.dval = exp2(op1.value.dval);
                unaryFunc = true;
                break;
            case PRINT_OPER:
                currNode = funcNode->data.function.opList;
                printf("=>");
                while (currNode != NULL) {
                    op1 = eval(currNode);
                    if (op1.type == INT_TYPE) {
                        if (isnan(op1.value.dval)) {
                            printf(" %lf ", op1.value.dval);
                        } else {
                            printf(" %ld ", lround(op1.value.dval));
                        }
                    } else {
                        printf(" %.2f ", op1.value.dval);
                    }
                    currNode = currNode->next;
                    result = op1;
                }
                printf("\n");
                break;
            case ADD_OPER:
                if (unary) {
                    binaryFunc = true;
                } else {
                    result.value.dval = op1.value.dval + op2.value.dval;
                }
                break;
            case SUB_OPER:
                if (unary) {
                    binaryFunc = true;
                } else {
                    result.value.dval = op1.value.dval - op2.value.dval;
                }
                break;
            case MULT_OPER:
                if (unary) {
                    binaryFunc = true;
                } else {
                    result.value.dval = op1.value.dval * op2.value.dval;
                }
                break;
            case DIV_OPER:
                if (unary) {
                    binaryFunc = true;
                } else {
                    result.value.dval = op1.value.dval / op2.value.dval;
                }
                break;
            case REMAINDER_OPER:
                if (unary) {
                    binaryFunc = true;
                } else {
                    result.value.dval = remainder(op1.value.dval, op2.value.dval);
                    if(result.value.dval < 0)
                    {
                        result.value.dval += op2.value.dval;
                    }
                }
                break;
            case POW_OPER:
                if (unary) {
                    binaryFunc = true;
                } else {
                    result.value.dval = pow(op1.value.dval, op2.value.dval);
                }
                break;
            case MAX_OPER:
                if (unary) {
                    binaryFunc = true;
                } else {
                    result = maxHelper(&op1, &op2);
                }
                break;
            case MIN_OPER:
                if (unary) {
                    binaryFunc = true;
                } else {
                    result = minHelper(&op1, &op2);
                }
                break;
            case HYPOT_OPER:
                if (unary) {
                    binaryFunc = true;
                } else {
                    result.value.dval = hypot(op1.value.dval, op2.value.dval);
                }
                break;
            case READ_OPER:
                result = readHelper(funcNode);
                break;
            case RAND_OPER:
                result.value.dval = (double)rand() / RAND_MAX;
                result.type = DOUBLE_TYPE;
                funcNode->type = NUM_NODE_TYPE;
                funcNode->data.number.type = DOUBLE_TYPE;
                funcNode->data.number.value = result.value;
                break;
            case LESS_OPER:
                result.value.dval = (op1.value.dval < op2.value.dval) ? 1 : 0;
                break;
            case GREATER_OPER:
                result.value.dval = (op1.value.dval > op2.value.dval) ? 1 : 0;
                break;
            case EQUAL_OPER:
                result.value.dval = (op1.value.dval == op2.value.dval) ? 1 : 0;
                break;
            case CUSTOM_OPER:
                result = evalCustomFunc(funcNode);
            default:
                break;
        }
        if (hasOperands) {
            if (funcNode->data.function.opList->next == NULL) {
                result.type = numTypeHelper1(&op1);
            } else {
                result.type = numTypeHelper2(&op1, &op2);
            }
        }
        if (binaryFunc) {
            printf("ERROR: too few parameters for the function %s\n", funcNames[funcNode->data.function.oper]);
            break;
        }
        op1 = result;
        if (currNode != NULL) {
            currNode = currNode->next;
            op2 = eval(currNode);
        }
    } while (!unaryFunc && currNode != NULL);
    if (!unary && unaryFunc) {
        printf("WARNING: too many parameters for the function <name>\n");
    }
    return result;
}

RET_VAL readHelper(AST_NODE *funcNode) {
    if (!funcNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    char buffer[128];
    char **ptr = NULL;
    printf("read := ");
    scanf("%s", buffer);
    result.value.dval = strtod(buffer, ptr);
    bool found = false;
    result.type = INT_TYPE;
    for (int i = 0; !found && buffer[i] != '\0' && i < sizeof(buffer); i++) {
        if (buffer[i] == '.') {
            result.type = DOUBLE_TYPE;
            found = true;
        }
    }
    funcNode->type = NUM_NODE_TYPE;
    funcNode->data.number.type = result.type;
    funcNode->data.number.value.dval = result.value.dval;
    return result;
}

RET_VAL maxHelper(RET_VAL *op1, RET_VAL *op2) {
    RET_VAL result = {INT_TYPE, NAN};
    if (op1->value.dval > op2->value.dval) {
        result.type = op1->type;
        result.value.dval = op1->value.dval;
    } else if (op1->value.dval < op2->value.dval) {
        result.type = op2->type;
        result.value.dval = op2->value.dval;
    } else {
        if (op1->type == DOUBLE_TYPE || op2->type == DOUBLE_TYPE) {
            result.type = DOUBLE_TYPE;
        } else {
            result.type = INT_TYPE;
        }
        result.value.dval = op1->value.dval;
    }
    return result;
}

RET_VAL minHelper(RET_VAL *op1, RET_VAL *op2) {
    RET_VAL result = {INT_TYPE, NAN};
    if (op1->value.dval < op2->value.dval) {
        result.type = op1->type;
        result.value.dval = op1->value.dval;
    } else if (op1->value.dval > op2->value.dval) {
        result.type = op2->type;
        result.value.dval = op2->value.dval;
    } else {
        if (op1->type == DOUBLE_TYPE || op2->type == DOUBLE_TYPE) {
            result.type = DOUBLE_TYPE;
        } else {
            result.type = INT_TYPE;
        }
        result.value.dval = op1->value.dval;
    }
    return result;
}

NUM_TYPE numTypeHelper2(RET_VAL *op1, RET_VAL *op2) {
    if (op1->type == DOUBLE_TYPE || op2->type == DOUBLE_TYPE) {
        return DOUBLE_TYPE;
    } else {
        return INT_TYPE;
    }
}

NUM_TYPE numTypeHelper1(RET_VAL *op1) {
    if (remainder(op1->value.dval, 1) == 0) {
        return INT_TYPE;
    } else {
        return DOUBLE_TYPE;
    }
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val) {
    char *numNames[] = {"NO_TYPE", "INT_TYPE", "DOUBLE_TYPE"};
    // TODO print the type and value of the value passed in. - done
    printf("TYPE: %s\n", numNames[val.type]);

    if (val.type == INT_TYPE) {
        if (isnan(val.value.dval)) {
            printf("VALUE: %lf\n", val.value.dval);
        } else {
            printf("VALUE: %ld\n", lround(val.value.dval));
        }
    } else {
        printf("VALUE: %.2f\n", val.value.dval);
    }
}

SYMBOL_TABLE_NODE *createSymbolTableNode(char *id, AST_NODE *op1, NUM_TYPE type) {
    SYMBOL_TABLE_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");
    node->val_type = type;
    node->ident = calloc(sizeof(id), 1);
    strcpy(node->ident, id);
    node->val = op1;
    return node;
}

AST_NODE *createSymbolNode(char *id) {
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(1, nodeSize)) == NULL)
        yyerror("Memory allocation failed!");

    node->type = SYM_NODE_TYPE;
    node->data.symbol.ident = id;
    return node;
}

SYMBOL_TABLE_NODE *linkSymbolTableNode(SYMBOL_TABLE_NODE *node1, SYMBOL_TABLE_NODE *node2) {
    node2->next = node1;
    return node2;
}

AST_NODE *linkASTNodes(AST_NODE *node1, AST_NODE *node2) {
    node1->next = node2;
    return node1;
}

AST_NODE *linkSymbolTableToAST(SYMBOL_TABLE_NODE *symbNode, AST_NODE *node) {
    node->symbolTable = symbNode;
    SYMBOL_TABLE_NODE *curNode = symbNode;
    while (curNode != NULL) {
        curNode->val->parent = node;
        curNode = curNode->next;
    }
    return node;
}

SYMBOL_TABLE_NODE *createCustomFunctionNode(NUM_TYPE numType, char *funcName, ARG_TABLE_NODE *arg_list, AST_NODE *funcNode){
    SYMBOL_TABLE_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");
    node->val_type = numType;
    node->type = LAMBDA_TYPE;
    node->ident = calloc(sizeof(funcName), 1);
    strcpy(node->ident, funcName);
    node->val = funcNode;
    node->val->arg_list = arg_list;
    node->val->isCustom = true;
    return node;
}

ARG_TABLE_NODE *createArgNode(char *id)
{
    ARG_TABLE_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(ARG_TABLE_NODE);
    if ((node = calloc(1, nodeSize)) == NULL)
        yyerror("Memory allocation failed!");

    node->ident = calloc(sizeof(id), 1);
    strcpy(node->ident, id);
    return node;
}

ARG_TABLE_NODE *linkArgList(char *id1, ARG_TABLE_NODE *node2){
    ARG_TABLE_NODE *node1 = createArgNode(id1);
    node1->next = node2;
    return node1;
}

RET_VAL evalCustomFunc(AST_NODE *funcNode){
    if (!funcNode)
        return (RET_VAL) {INT_TYPE, NAN};
    RET_VAL result = {INT_TYPE, NAN};
    AST_NODE *outerNode = funcNode;
    while (outerNode != NULL) {
        SYMBOL_TABLE_NODE *currNode = outerNode->symbolTable;
        while (currNode != NULL) {
            if (strcmp(funcNode->data.function.name, currNode->ident) == 0) {
                AST_NODE *opNode = funcNode->data.function.opList;
                size_t nodeSize;
                ARG_TABLE_NODE *head;
                nodeSize = sizeof(ARG_TABLE_NODE);
                if ((head = calloc(1, nodeSize)) == NULL)
                    yyerror("Memory allocation failed!");
                ARG_TABLE_NODE *currArgNode = head;
                while(opNode != NULL) {
                    currArgNode->value = eval(opNode);
                    if ((opNode = opNode->next) != NULL) {
                        currArgNode->next = calloc(1, nodeSize);
                        currArgNode = currArgNode->next;
                    }
                }
                currArgNode = head;
                ARG_TABLE_NODE *argNode = currNode->val->arg_list;
                while(argNode != NULL && currArgNode != NULL) {
                    argNode->value = currArgNode->value;
                    currArgNode = currArgNode->next;
                    argNode = argNode->next;
                }
                if(argNode != NULL)
                {
                    printf("ERROR: too few parameters for the function %s\n",funcNode->data.function.name);
                    return result;
                }
                if(currArgNode != NULL)
                {
                    printf("WARNING: too many parameters for the function %s\n",funcNode->data.function.name);
                }
                result = eval(currNode->val);
                if (currNode->val_type == INT_TYPE) {
                    if (result.type == DOUBLE_TYPE) {
                        printf("WARNING: precision loss in the assignment for variable %s\n",
                               outerNode->symbolTable->ident);
                    }
                    result.value.dval = lround(result.value.dval);
                    result.type = INT_TYPE;
                } else if (currNode->val_type == DOUBLE_TYPE) {
                    result.type = DOUBLE_TYPE;
                }
                return result;
            }
            currNode = currNode->next;
        }
        outerNode = outerNode->parent;
    }
    return result;
}