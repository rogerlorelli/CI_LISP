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
AST_NODE *createNumberNode(double value, NUM_TYPE type, bool warningFlag) {
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
            node->data.number.value = floor(value);
            if (warningFlag) {
                printf("WARNING: precision loss during number node creation\n");
            }
            break;
        case DOUBLE_TYPE:
            node->data.number.type = DOUBLE_TYPE;
            node->data.number.value = value;
            break;
        default:
            printf("Invalid NUM_NODE_TYPE\n");
    }
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
/**
 * Creates a condition AST node that contains 3 expressions: 1 condition to check, 1 expression to evaluate if true, 1 if false
 * @param condNode - expression to evaluate to determine which of the following expressions to evaluate after.
 * @param trueNode - expression to eval if cond is true
 * @param falseNode - expression t0 eval if cond is false
 * @return an AST_NODE with condition data field filled with the params
 */
AST_NODE *createCondASTNode(AST_NODE *condNode, AST_NODE *trueNode, AST_NODE *falseNode) {
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

    //free everything on the ARG_TABLE
    ARG_TABLE_NODE *tempNode = node->arg_list;
    ARG_TABLE_NODE *tempPrevNode;
    while (tempNode != NULL) {
        tempPrevNode = tempNode;
        tempNode = tempNode->next;
        free(tempPrevNode->ident);
        free(tempPrevNode);
    }

    //free everything on the SYMB_TABLE
    SYMBOL_TABLE_NODE *tempSYMBNode = node->symbolTable;
    SYMBOL_TABLE_NODE *tempSYMBPrevNode;
    while (tempSYMBNode != NULL) {
        tempSYMBPrevNode = tempSYMBNode;
        tempSYMBNode = tempSYMBNode->next;
        free(tempSYMBPrevNode->ident);
        freeNode(tempSYMBPrevNode->val);
        free(tempSYMBPrevNode);
    }

    //if the node is a funcNode go through the
    AST_NODE *currNode;
    AST_NODE *prevNode;
    if (node->type == FUNC_NODE_TYPE) {
        // Recursive calls to free child nodes
        currNode = node->data.function.opList;
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

    if (node->type == COND_NODE_TYPE) {
        // Recursive calls to free child nodes
        currNode = node->data.condition.cond;
        while (currNode != NULL) {
            prevNode = currNode;
            currNode = currNode->next;
            freeNode(prevNode);
        }
        currNode = node->data.condition.ifFalse;
        while (currNode != NULL) {
            prevNode = currNode;
            currNode = currNode->next;
            freeNode(prevNode);
        }
        currNode = node->data.condition.ifTrue;
        while (currNode != NULL) {
            prevNode = currNode;
            currNode = currNode->next;
            freeNode(prevNode);
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

/**
 * Calls eval on the conditional expression and dependant on the output calls eval on 1 of the others
 * @param condNode node containing the three expressions, 2 of which should be evaluated.
 * @return RET_VAL with the answer from either the ifTrue expression or the ifFalse expression
 */
RET_VAL evalCondNode(COND_AST_NODE *condNode) {
    if (!condNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    switch ((int) eval(condNode->cond).value) {
        case 1:
            result = eval(condNode->ifTrue);
            break;
        case 0:
            result = eval(condNode->ifFalse);
            break;
        default:
            printf("Bug at eval Condition, idiot\n");
            break;
    }

    return result;
}

/**
 * Checks the AST_Node and it's parents for the definition of the symbol node being evaluated
 * @param symbNode - node to evaluate
 * @return RET_VAL containing the evaluated value of the symbol if found. NAN if not found.
 */
RET_VAL evalSymbNode(AST_NODE *symbNode) {
    if (!symbNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};
    AST_NODE *outerNode = symbNode;

    while (outerNode != NULL) {
        if (outerNode->isCustom) {
            ARG_TABLE_NODE *argNode = outerNode->arg_list;
            while (argNode != NULL) {
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
                    if (currNode->val_type == INT_TYPE && result.type == DOUBLE_TYPE) {
                        printf("WARNING: precision loss in the assignment for variable %s\n",
                               symbNode->data.symbol.ident);
                        result.value = floor(result.value);
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
    if (isnan(result.value)) {
        printf("ERROR: VARIABLE NOT FOUND %s\n", symbNode->data.symbol.ident);
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
    result.value = numNode->value;
    return result;
}
/**
 * Either calls a helper method to compute a mathematical equation or computes it locally. Checks for appropriate number
 * of operands being passed to an equation and prints either an error message or warning when approriate.
 * @param funcNode - node containing a function operation and possibly an op_list if operands are present
 * @return RET_VAL with the value and type of the evaluated operation.
 */
RET_VAL evalFuncNode(AST_NODE *funcNode) {
    if (!funcNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE

    RET_VAL op1;
    RET_VAL op2;
    AST_NODE *currNode = funcNode->data.function.opList;
    bool hasOperands = (currNode != NULL);
    if (currNode != NULL && funcNode->data.function.oper != PRINT_OPER && funcNode->data.function.oper != ADD_OPER &&
        funcNode->data.function.oper != MULT_OPER && funcNode->data.function.oper != CUSTOM_OPER) {
        op1 = eval(currNode);
        currNode = currNode->next;
        op2 = eval(currNode);
    }
    bool unaryOp_list = (currNode == NULL);
    bool unaryFunc = true;
    switch (funcNode->data.function.oper) {
        case NEG_OPER:
            if (hasOperands) {
                result.value = -1 * op1.value;
                result.type = op1.type;
            }
            break;
        case ABS_OPER:
            if (hasOperands) {
                result.value = fabs(op1.value);
                result.type = op1.type;
            }
            break;
        case EXP_OPER:
            if (hasOperands) {
                result.value = exp(op1.value);
                result.type = DOUBLE_TYPE;
            }
            break;
        case SQRT_OPER:
            if (hasOperands) {
                result.value = sqrt(op1.value);
                result.type = DOUBLE_TYPE;
            }
            break;
        case CBRT_OPER:
            if (hasOperands) {
                result.value = cbrt(op1.value);
                result.type = DOUBLE_TYPE;
            }
            break;
        case LOG_OPER:
            if (hasOperands) {
                result.value = log(op1.value);
                result.type = DOUBLE_TYPE;
            }
            break;
        case EXP2_OPER:
            if (hasOperands) {
                result.value = exp2(op1.value);
                result.type = numTypeHelper1(&result);
            }
            break;
        case PRINT_OPER:
            currNode = funcNode->data.function.opList;
            size_t nodeSize;
            AST_NODE *head;
            nodeSize = sizeof(AST_NODE);
            if ((head = calloc(1, nodeSize)) == NULL)
                yyerror("Memory allocation failed!");
            AST_NODE *currTempNode = head;
            while (currNode != NULL) {
                currTempNode->data.number = eval(currNode);
                if ((currNode = currNode->next) != NULL) {
                    currTempNode->next = calloc(1, nodeSize);
                    currTempNode = currTempNode->next;
                }
            }
            currTempNode = head;
            printf("=>");
            while (head != NULL) {
                op1 = head->data.number;
                if (op1.type == INT_TYPE) {
                    if (isnan(op1.value)) {
                        printf(" %lf ", op1.value);
                    } else {
                        printf(" %d ", (int) floor(op1.value));
                    }
                } else {
                    printf(" %.2f ", op1.value);
                }
                head = head->next;
                result = op1;
            }
            printf("\n");
            AST_NODE *prevNode;
            //free temp op list
            while (currTempNode != NULL) {
                prevNode = currTempNode;
                currTempNode = currTempNode->next;
                free(prevNode);
            }
            return result;
        case ADD_OPER:
            unaryFunc = false;
            if (hasOperands) {
                unaryOp_list = (currNode->next == NULL);
                if (unaryOp_list) {
                    break;
                } else {
                    result = addHelper(funcNode->data.function.opList);
                    return result;
                }
            }
            break;
        case SUB_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            } else {
                result.value = op1.value - op2.value;
                result = numTypeHelper2(&op1, &op2, &result);
            }
            break;
        case MULT_OPER:
            unaryFunc = false;
            if (hasOperands) {
                unaryOp_list = (currNode->next == NULL);
                if (unaryOp_list) {
                    break;
                } else {
                    result = multHelper(funcNode->data.function.opList);
                    return result;
                }
            }
            break;
        case DIV_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            } else {
                if (op1.value == 0) {
                    result.value = 0;
                } else if (op2.value == 0) {
                    result.value = NAN;
                    return result;
                } else {
                    result.value = op1.value / op2.value;
                    result = numTypeHelper2(&op1, &op2, &result);
                }
            }
            break;
        case REMAINDER_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            } else {
                result.value = remainder(op1.value, op2.value);
                if (isnan(result.value)) {
                    result.value = 0;
                }
                if (result.value < 0) {
                    result.value += op2.value;
                }
                result = numTypeHelper2(&op1, &op2, &result);
            }
            break;
        case POW_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            } else {
                result.value = pow(op1.value, op2.value);
                result = numTypeHelper2(&op1, &op2, &result);
            }
            break;
        case MAX_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            } else {
                result = maxHelper(&op1, &op2);
                return result;
            }
            break;
        case MIN_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            } else {
                result = minHelper(&op1, &op2);
                return result;
            }
            break;
        case HYPOT_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            } else {
                result.value = hypot(op1.value, op2.value);
                result.type = DOUBLE_TYPE;
            }
            break;
        case READ_OPER:
            if (hasOperands) {
                printf("eval function WARNING: too many parameters for the function %s\n",
                       funcNames[funcNode->data.function.oper]);
            }
            result = readHelper(funcNode);
            return result;
        case RAND_OPER:
            if (hasOperands) {
                printf("eval function WARNING: too many parameters for the function %s\n",
                       funcNames[funcNode->data.function.oper]);
            }
            result.value = (double) rand() / RAND_MAX;
            result.type = DOUBLE_TYPE;
            funcNode->type = NUM_NODE_TYPE;
            funcNode->data.number.type = DOUBLE_TYPE;
            funcNode->data.number.value = result.value;
            printf("ERROR CHECKING PRINTING RAND: %.2f\n", result.value);
            return result;
        case LESS_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            }
            result.value = (op1.value < op2.value) ? 1 : 0;
            break;
        case GREATER_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            }
            result.value = (op1.value > op2.value) ? 1 : 0;
            break;
        case EQUAL_OPER:
            unaryFunc = false;
            if (unaryOp_list) {
                break;
            }
            result.value = (op1.value == op2.value) ? 1 : 0;
            break;
        case CUSTOM_OPER:
            result = evalCustomFunc(funcNode);
            return result;
        default:
            break;
    }
    if ((unaryOp_list && !unaryFunc) ||
        (!hasOperands && funcNode->data.function.oper != RAND_OPER && funcNode->data.function.oper != READ_OPER)) {
        printf("ERROR: too few parameters for the function %s\n FOR TESTING EXIT IS COMMENTED OUT\n",
               funcNames[funcNode->data.function.oper]);
        //exit(1);
    }
    if ((hasOperands && !unaryOp_list && unaryFunc) || (unaryOp_list && !unaryFunc)) {
        printf("eval function WARNING: too many parameters for the function %s\n",
               funcNames[funcNode->data.function.oper]);
    }
    return result;
}
/**
 * helper method to get an input from the user and overwrites the func_node as a num_node containing the input
 * value & type.
 * @param funcNode - func_node to be overwritten with new input value
 * @return RET_VAL of the input value with appropriate type
 */
RET_VAL readHelper(AST_NODE *funcNode) {
    if (!funcNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    char buffer[128];
    char **ptr = NULL;
    printf("read := ");
    scanf("%s", buffer);
    result.value = strtod(buffer, ptr);
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
    funcNode->data.number.value = result.value;
    return result;
}
/**
 * max helper method to find the greater of the 2 operands, determines the type as well.
 * @param op1 operand to be compared
 * @param op2 operand to be compared
 * @return RET_VAL containing the information of the greater operand.
 */
RET_VAL maxHelper(RET_VAL *op1, RET_VAL *op2) {
    RET_VAL result = {INT_TYPE, NAN};
    if (op1->value > op2->value) {
        result.type = op1->type;
        result.value = op1->value;
    } else if (op1->value < op2->value) {
        result.type = op2->type;
        result.value = op2->value;
    } else {
        if (op1->type == DOUBLE_TYPE || op2->type == DOUBLE_TYPE) {
            result.type = DOUBLE_TYPE;
        } else {
            result.type = INT_TYPE;
        }
        result.value = op1->value;
    }
    return result;
}
/**
 * max helper method to find the smaller of the 2 operands, determines the type as well.
 * @param op1 operand to be compared
 * @param op2 operand to be compared
 * @return RET_VAL containing the information of the smaller operand.
 */
RET_VAL minHelper(RET_VAL *op1, RET_VAL *op2) {
    RET_VAL result = {INT_TYPE, NAN};
    if (op1->value < op2->value) {
        result.type = op1->type;
        result.value = op1->value;
    } else if (op1->value > op2->value) {
        result.type = op2->type;
        result.value = op2->value;
    } else {
        if (op1->type == DOUBLE_TYPE || op2->type == DOUBLE_TYPE) {
            result.type = DOUBLE_TYPE;
        } else {
            result.type = INT_TYPE;
        }
        result.value = op1->value;
    }
    return result;
}
/**
 * determines the type of the result dependant on the type of the 2 passed operands
 * @param op1 - operand 1 to be compared
 * @param op2 - operand 2 to be compared
 * @param result RET_VAL to be updated
 * @return updated result
 */
RET_VAL numTypeHelper2(RET_VAL *op1, RET_VAL *op2, RET_VAL *result) {
    if (op1->type == DOUBLE_TYPE || op2->type == DOUBLE_TYPE) {
        result->type = DOUBLE_TYPE;
    } else {
        result->type = INT_TYPE;
        if (remainder(result->value, 1) > 0) {
            result->value = floor(result->value);
        }
    }
    return *result;
}
/**
 * When called checks if the value is a non-whole number
 * @param op1 operand to be checked
 * @return INT if the number is whole, Double if not
 */
NUM_TYPE numTypeHelper1(RET_VAL *op1) {
    if (remainder(op1->value, 1) == 0) {
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
        if (isnan(val.value)) {
            printf("VALUE: %lf\n", val.value);
        } else {
            printf("VALUE: %d\n", (int) floor(val.value));
        }
    } else {
        printf("VALUE: %.2f\n", val.value);
    }
}
/**
 * Creates a SYMBOL_TABLE_NODE with the passed information
 * @param id - variable name
 * @param op1 - value of the variable
 * @param type - Type casting
 * @return SYMBOL_TABLE_NODE
 */
SYMBOL_TABLE_NODE *createSymbolTableNode(char *id, AST_NODE *op1, NUM_TYPE type) {
    SYMBOL_TABLE_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");
    node->val_type = type;
    node->ident = id;
    node->val = op1;
    return node;
}
/**
 * Creates a SYMBOL_NODE with the passed id, will be attached to a function_node as an operand
 * @param id name of variable.
 * @return AST_NODE with symbol data field containing the variable name
 */
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
/**
 * Links 2 Symbol_Table_nodes together
 * @param node1
 * @param node2
 * @return returns the head of the linked list
 */
SYMBOL_TABLE_NODE *linkSymbolTableNode(SYMBOL_TABLE_NODE *node1, SYMBOL_TABLE_NODE *node2) {
    node2->next = node1;
    return node2;
}
/**
 * links 2 AST nodes to begin or append to a op_list
 * @param node1 new head of the op_list
 * @param node2 linked list to be appended to
 * @return head of the linked list
 */
AST_NODE *linkASTNodes(AST_NODE *node1, AST_NODE *node2) {
    node1->next = node2;
    return node1;
}
/**
 * linkes a symbol table node linked list to an ast node
 * @param symbNode linked list to be attached
 * @param node AST node for linked list to be attached to
 * @return AST node with attached linked list
 */
AST_NODE *linkSymbolTableToAST(SYMBOL_TABLE_NODE *symbNode, AST_NODE *node) {
    node->symbolTable = symbNode;
    SYMBOL_TABLE_NODE *curNode = symbNode;
    while (curNode != NULL) {
        curNode->val->parent = node;
        curNode = curNode->next;
    }
    return node;
}
/**
 * creates a custom symbol table node, the ident is the function name, the val is the function definition,
 * and the arg_list is attached to the arg_list in the definiton's ast_Node. The isCustom flag is also set to true.
 * @param numType allows for type casting
 * @param funcName - name of the function
 * @param arg_list - formal parameters of the function
 * @param funcNode - definition of the custom function
 * @return SYMBOL_TABLE_NODE containing all of the custom functions information
 */
SYMBOL_TABLE_NODE *
createCustomFunctionNode(NUM_TYPE numType, char *funcName, ARG_TABLE_NODE *arg_list, AST_NODE *funcNode) {
    SYMBOL_TABLE_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");
    node->val_type = numType;
    node->ident = funcName;
    node->val = funcNode;
    node->val->arg_list = arg_list;
    node->val->isCustom = true;
    return node;
}
/**
 * creates an ARG_TABLE_NODE with the ident set by the passed id
 * @param id name to be set to the ident field
 * @return ARG_TABLE with a filled name but blank value field, value field will be filled at evaluation
 */
ARG_TABLE_NODE *createArgNode(char *id) {
    ARG_TABLE_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(ARG_TABLE_NODE);
    if ((node = calloc(1, nodeSize)) == NULL)
        yyerror("Memory allocation failed!");

    node->ident = id;
    return node;
}
/**
 * creates an Arg_Node and links it to another node
 * @param id1 name of new argNode to be created
 * @param node2 running list
 * @return ARG_TABLE_NODE holding the head of the linked list
 */
ARG_TABLE_NODE *linkArgList(char *id1, ARG_TABLE_NODE *node2) {
    ARG_TABLE_NODE *node1 = createArgNode(id1);
    node1->next = node2;
    return node1;
}
/**
 * Like evalSymbol, this function will check the node and it's parents for the definition of the function name. If found
 * will create a temporary linked list containing the evaluated op_list and stores it into the custom function's arg_list.
 * Then calls eval on the custom function's definition.
 * @param funcNode custom function to be evaluated
 * @return RET_VAL containing the evaluated function's return information
 */
RET_VAL evalCustomFunc(AST_NODE *funcNode) {
    if (!funcNode)
        return (RET_VAL) {INT_TYPE, NAN};
    RET_VAL result = {INT_TYPE, NAN};
    AST_NODE *outerNode = funcNode;

    //while loop to search AST_NODE and parents for the definition of the Custom function.
    while (outerNode != NULL) {
        SYMBOL_TABLE_NODE *currNode = outerNode->symbolTable;
        while (currNode != NULL) {
            //when found begin creating a linked list of ARG_TABLE_NODES that will contain the evaluated operands of the
            //custom function while keeping track of the head of the temp list with 'head'
            if (strcmp(funcNode->data.function.name, currNode->ident) == 0) {
                AST_NODE *opNode = funcNode->data.function.opList;
                size_t nodeSize;
                ARG_TABLE_NODE *head;
                nodeSize = sizeof(ARG_TABLE_NODE);
                if ((head = calloc(1, nodeSize)) == NULL)
                    yyerror("Memory allocation failed!");
                ARG_TABLE_NODE *currArgNode = head;
                while (opNode != NULL) {
                    currArgNode->value = eval(opNode);
                    if ((opNode = opNode->next) != NULL) {
                        currArgNode->next = calloc(1, nodeSize);
                        currArgNode = currArgNode->next;
                    }
                }
                //reset the current temp node back to head so we can now transfer the evaluated operands to the arg_list of the custom function.
                currArgNode = head;
                ARG_TABLE_NODE *argNode = currNode->val->arg_list;
                while (argNode != NULL && currArgNode != NULL) {
                    argNode->value = currArgNode->value;
                    currArgNode = currArgNode->next;
                    argNode = argNode->next;
                }
                //check if the number of operands passed to the custom function meet the formal parameters of the custom function
                //printing the appropriate error or warning, and exiting if need be.
                if (argNode != NULL) {
                    printf("ERROR: too few parameters for the function %s\n", funcNode->data.function.name);
                    exit(1);
                }
                if (currArgNode != NULL) {
                    printf("eval custom function WARNING: too many parameters for the function %s\n",
                           funcNode->data.function.name);
                }
                //now evaluate the custom function definition
                result = eval(currNode->val);
                //attempt to free temp list
                ARG_TABLE_NODE *prevNode;
                do {
                    prevNode = head;
                    head = head->next;
                    free(&prevNode->ident);
                } while (head != NULL);
                if (currNode->val_type == INT_TYPE) {
                    if (result.type == DOUBLE_TYPE) {
                        printf("WARNING: precision loss in the assignment for variable %s\n",
                               outerNode->symbolTable->ident);
                    }
                    result.value = floor(result.value);
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
    printf("ERROR: Function %s not found.\n", funcNode->data.function.name);
    return result;
}
/**
 * helper method that takes in the AST_NODE and runs the operation as many times as there are operands
 * @param op_list linked list containing the operands
 * @return RET_VAL containing the sum of all the operands
 */
RET_VAL addHelper(AST_NODE *op_list) {
    RET_VAL result = {INT_TYPE, NAN};
    RET_VAL op1 = eval(op_list);
    RET_VAL op2;
    op_list = op_list->next;
    while (op_list != NULL) {
        op2 = eval(op_list);
        result.value = op1.value + op2.value;
        result = numTypeHelper2(&op1, &op2, &result);
        op1 = result;
        op_list = op_list->next;
    }
    return result;
}
/**
 * helper method that takes in the AST_NODE and runs the operation as many times as there are operands
 * @param op_list linked list containing the operands
 * @return RET_VAL containing the product of all the operands
 */
RET_VAL multHelper(AST_NODE *op_list) {
    RET_VAL result = {INT_TYPE, NAN};
    RET_VAL op1 = eval(op_list);
    RET_VAL op2;
    op_list = op_list->next;
    while (op_list != NULL) {
        op2 = eval(op_list);
        result.value = op1.value * op2.value;
        result = numTypeHelper2(&op1, &op2, &result);
        op1 = result;
        op_list = op_list->next;
    }
    return result;
}