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
        ""
};

OPER_TYPE resolveFunc(char *funcName)
{
    int i = 0;
    while (funcNames[i][0] != '\0')
    {
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
AST_NODE *createNumberNode(double value, NUM_TYPE type)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, assign values to contained NUM_AST_NODE - done
    node->type = NUM_NODE_TYPE;
    switch(type)
    {
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
AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
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
    if(node->data.function.oper == CUSTOM_OPER)
    {
        node->data.function.ident = funcName;
    }
    else
    {
        free(funcName);
    }
    node->data.function.op1 = op1;
    node->data.function.op2 = op2;
    if(op1 != NULL)
    {
        node->data.function.op1->parent = node;
    }
    if(op2 != NULL)
    {
        node->data.function.op2->parent = node;
    }
    return node;
}

// Called after execution is done on the base of the tree.
// (see the program production in ciLisp.y)
// Recursively frees the whole abstract syntax tree.
// You'll need to update and expand freeNode as the project develops.
void freeNode(AST_NODE *node)
{
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE)
    {
        // Recursive calls to free child nodes
        freeNode(node->data.function.op1);
        freeNode(node->data.function.op2);

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER)
        {
            free(node->data.function.ident);
        }
    }
    if (node->type == SYM_NODE_TYPE)
    {
        free(node->data.symbol.ident);
    }
    free(node);
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch. - done
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type)
    {
        case NUM_NODE_TYPE:
            result = evalNumNode(&node->data.number);
            break;
        case FUNC_NODE_TYPE:
            result = evalFuncNode(&node->data.function);
            break;
        case SYM_NODE_TYPE:
            result = evalSymbNode(node);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
    }

    return result;
}

RET_VAL evalSymbNode(AST_NODE *symbNode)
{
    if (!symbNode)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};
    AST_NODE *outerNode = symbNode;
    while(outerNode != NULL)
    {
        SYMBOL_TABLE_NODE *currNode = outerNode->symbolTable;
        while(currNode != NULL)
        {
            if(strcmp(symbNode->data.symbol.ident,currNode->ident) == 0)
            {
                result = eval(currNode->val);
                if(outerNode->symbolTable->val_type == INT_TYPE)
                {
                    result.value.dval = lround(result.value.dval);
                    result.type = INT_TYPE;
                    printf("WARNING: precision loss in the assignment for variable %s\n",outerNode->symbolTable->ident);
                }
                else if(outerNode->symbolTable->val_type == INT_TYPE)
                {
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


// returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// DOES NOT allocate space for a new RET_VAL.
RET_VAL evalNumNode(NUM_AST_NODE *numNode)
{
    if (!numNode)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the values stored in the node. -done
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE
    result.type = numNode->type;
    result.value.dval = numNode->value.dval;
    return result;
}



RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode)
{
    if (!funcNode)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE
    RET_VAL op1 = eval(funcNode->op1);
    RET_VAL op2 = eval(funcNode->op2);
    switch(funcNode->oper)
    {
        case NEG_OPER:
            result = negHelper(&op1);
            break;
        case ABS_OPER:
            result.value.dval = fabs(funcNode->op1->data.number.value.dval);
            result.type = funcNode->op1->data.number.type;
            break;
        case EXP_OPER:
            result.value.dval = exp(funcNode->op1->data.number.value.dval);
            result.type = numTypeHelper1(&op1);
            break;
        case SQRT_OPER:
            result = sqrtHelper(&op1);
            break;
        case ADD_OPER:
            result = addHelper(&op1,&op2);
            break;
        case SUB_OPER:
            result = subHelper(&op1,&op2);
            break;
        case MULT_OPER:
            result = multHelper(&op1,&op2);
            break;
        case DIV_OPER:
            result = divHelper(&op1,&op2);
            break;
        case REMAINDER_OPER:
            result = remHelper(&op1,&op2);
            break;
        case LOG_OPER:
            result.value.dval = log(op1.value.dval);
            result.type = numTypeHelper1(&op1);
            break;
        case POW_OPER:
            result = powHelper(&op1,&op2);
            break;
        case MAX_OPER:
            result = maxHelper(&op1,&op2);
            break;
        case MIN_OPER:
            result = minHelper(&op1,&op2);
            break;
        case EXP2_OPER:
            result.value.dval = exp2(op1.value.dval);
            result.type = op1.type;
            break;
        case CBRT_OPER:
            result = cbrtHelper(&op1);
            break;
        case HYPOT_OPER:
            result = hypotHelper(&op1,&op2);
            break;
        default:
            printf("Some other operation!\n");

    }
    return result;
}
RET_VAL negHelper(RET_VAL *op1)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = op1->type;
    result.value.dval = -1 * op1->value.dval;
    return result;
}

RET_VAL addHelper(RET_VAL *op1,RET_VAL *op2)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = numTypeHelper2(op1,op2);
    result.value.dval = op1->value.dval + op2->value.dval;
    return result;
}

RET_VAL subHelper(RET_VAL *op1,RET_VAL *op2)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = numTypeHelper2(op1,op2);
    result.value.dval = op1->value.dval - op2->value.dval;
    return result;
}

RET_VAL multHelper(RET_VAL *op1,RET_VAL *op2)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = numTypeHelper2(op1,op2);
    result.value.dval = op1->value.dval * op2->value.dval;
    return result;
}

RET_VAL divHelper(RET_VAL *op1,RET_VAL *op2)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = numTypeHelper2(op1,op2);
    result.value.dval = op1->value.dval / op2->value.dval;
    return result;
}

RET_VAL remHelper(RET_VAL *op1,RET_VAL *op2)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.value.dval = remainder(op1->value.dval, op2->value.dval);
    result.type = numTypeHelper1(op1);
    return result;
}

RET_VAL powHelper(RET_VAL *op1,RET_VAL *op2)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.type = numTypeHelper2(op1,op2);
    result.value.dval = pow(op1->value.dval, op2->value.dval);
    return result;
}

RET_VAL maxHelper(RET_VAL *op1,RET_VAL *op2)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(op1->value.dval > op2->value.dval)
    {
        result.type = op1->type;
        result.value.dval = op1->value.dval;
    }
    else if(op1->value.dval < op2->value.dval)
    {
        result.type = op2->type;
        result.value.dval = op2->value.dval;
    }
    else
    {
        if(op1->type == DOUBLE_TYPE || op2->type == DOUBLE_TYPE)
        {
            result.type = DOUBLE_TYPE;
        }
        else
        {
            result.type = INT_TYPE;
        }
        result.value.dval = op1->value.dval;
    }
    return result;
}

RET_VAL minHelper(RET_VAL *op1,RET_VAL *op2)
{
    RET_VAL result = {INT_TYPE, NAN};
    if(op1->value.dval < op2->value.dval)
    {
        result.type = op1->type;
        result.value.dval = op1->value.dval;
    }
    else if(op1->value.dval > op2->value.dval)
    {
        result.type = op2->type;
        result.value.dval = op2->value.dval;
    }
    else
    {
        if(op1->type == DOUBLE_TYPE || op2->type == DOUBLE_TYPE)
        {
            result.type = DOUBLE_TYPE;
        }
        else
        {
            result.type = INT_TYPE;
        }
        result.value.dval = op1->value.dval;
    }
    return result;
}

RET_VAL hypotHelper(RET_VAL *op1,RET_VAL *op2)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.value.dval = hypot(op1->value.dval, op2->value.dval);
    result.type = numTypeHelper2(op1,op2);
    return result;
}

RET_VAL sqrtHelper(RET_VAL *op1)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.value.dval = sqrt(op1->value.dval);
    result.type = numTypeHelper1(op1);
    return result;
}

RET_VAL cbrtHelper(RET_VAL *op1)
{
    RET_VAL result = {INT_TYPE, NAN};
    result.value.dval = cbrt(op1->value.dval);
    result.type = numTypeHelper1(op1);
    return result;
}

NUM_TYPE numTypeHelper2(RET_VAL *op1,RET_VAL *op2)
{
    if(op1->type == DOUBLE_TYPE || op2->type == DOUBLE_TYPE)
    {
        return DOUBLE_TYPE;
    }
    else
    {
        return INT_TYPE;
    }
}

NUM_TYPE numTypeHelper1(RET_VAL *op1)
{
    if(remainder(op1->value.dval,1) == 0)
    {
        return INT_TYPE;
    }
    else
    {
        return DOUBLE_TYPE;
    }
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    char* numNames[] = {"NO_TYPE","INT_TYPE","DOUBLE_TYPE"};
    // TODO print the type and value of the value passed in. - done
    printf("TYPE: %s\n",numNames[val.type]);
    if(val.type == INT_TYPE)
    {
        printf("VALUE: %.0ld\n",lround(val.value.dval));
    }
    else
    {
        printf("VALUE: %.4f\n",val.value.dval);
    }
}

SYMBOL_TABLE_NODE *createSymbolTableNode(char *id, AST_NODE *op1,NUM_TYPE type)
{
    SYMBOL_TABLE_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");
    node->val_type = type;
    node->ident = calloc(sizeof(id), 1);
    strcpy(node->ident,id);
    node->val = op1;
    return node;
}

AST_NODE *createSymbolNode(char *id)
{
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

SYMBOL_TABLE_NODE *linkSymbolNode(SYMBOL_TABLE_NODE *node1, SYMBOL_TABLE_NODE *node2)
{
    node2->next = node1;
    return node2;
}

AST_NODE *linkSymbolTableToAST(SYMBOL_TABLE_NODE *symbNode, AST_NODE *node)
{
    node->symbolTable = symbNode;
    SYMBOL_TABLE_NODE *curNode = symbNode;
    while(curNode != NULL)
    {
        curNode->val->parent = node;
        curNode = curNode->next;
    }
    return node;
}
