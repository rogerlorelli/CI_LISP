%{
    #include "ciLisp.h"
%}

%union {
    int ival;
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node  *symbolTableNode;
    struct arg_table_node  *argTableNode
}

%token <sval> FUNC SYMBOL
%token <dval> INT DOUBLE
%token LPAREN RPAREN EOL QUIT LET TYPE_INT TYPE_DOUBLE COND LAMBDA

%type <astNode> s_expr s_expr_list f_expr number
%type <symbolTableNode> let_section let_elem let_list
%type <argTableNode> arg_list
%type <ival> type
%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
    };

s_expr:
    SYMBOL {
        fprintf(stderr, "yacc: s_expr ::= SYMBOL\n");
	$$ = createSymbolNode($1);
    }
    | LPAREN SYMBOL RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPREN SYMBOL RPAREN\n");
    	$$ = createSymbolNode($2);
    }
    | LPAREN let_section s_expr RPAREN {
    	$$ = linkSymbolTableToAST($2,$3);
    }
    | LPAREN COND s_expr s_expr s_expr RPAREN {
        $$ = createCondASTNode($3,$4,$5);
    }
    | number {
        fprintf(stderr, "yacc: s_expr ::= number\n");
        $$ = $1;
    }
    | f_expr {
        $$ = $1;
    }
    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

number:
    INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($1, INT_TYPE);
    }
    | DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($1, DOUBLE_TYPE);
    }
    | type INT{
        fprintf(stderr, "yacc: number ::= CAST INT\n");
        $$ = createNumberNode($2, $1);
    }
    | type DOUBLE{
        fprintf(stderr, "yacc: number ::= CAST DOUBLE\n");
        $$ = createNumberNode($2, $1);
    };

f_expr:
    LPAREN FUNC s_expr_list RPAREN {
    	fprintf(stderr, "yacc: f_expr ::= LPAREN FUNC expr RPAREN\n");
    	$$ = createFunctionNode($2, $3);
    }
    | LPAREN SYMBOL s_expr_list RPAREN {
    	fprintf(stderr, "yacc: f_expr ::= LPAREN SYMBOL expr RPAREN\n");
    	$$ = createFunctionNode($2, $3);
    };

s_expr_list:
    s_expr s_expr_list {
	$$ = linkASTNodes($1,$2);
    }
    | s_expr {
	$$ = $1;
    };

let_section:
     LPAREN let_list RPAREN {
	fprintf(stderr, "yacc: let_section ::= LPAREN let_list RPAREN\n");
	$$ = $2;
    };

let_list:
    LET let_elem {
    	fprintf(stderr, "yacc: let_list ::= let let_elem\n");
    	$$ = $2;
    }
    | let_list let_elem {
    	fprintf(stderr, "yacc: let_list ::= let_list let_elem\n");
    	$$ = linkSymbolTableNode($1,$2);
    };

let_elem:
    LPAREN type SYMBOL s_expr RPAREN {
	fprintf(stderr, "yacc: let_elm ::= LPAREN symbol expr RPAREN\n");
    	$$ = createSymbolTableNode($3,$4,$2);
    }
    | LPAREN SYMBOL s_expr RPAREN {
      	fprintf(stderr, "yacc: let_elm ::= LPAREN symbol expr RPAREN\n");
        $$ = createSymbolTableNode($2,$3,NO_TYPE);
    }
    | LPAREN type SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
    	fprintf(stderr, "yacc: let_elem ::= LPAREN type SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN\n");
        $$ = createCustomFunctionNode($2, $3, $6, $8);
    }
    | LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
    	fprintf(stderr, "yacc: let_elem ::= LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN\n");
        $$ = createCustomFunctionNode(NO_TYPE, $2, $5, $7);
    };

arg_list:
    SYMBOL arg_list {
    	fprintf(stderr, "yacc: arg_list ::= SYMBOL arg_list\n");
        $$ = linkArgList($1,$2);
    }
    | SYMBOL {
    	fprintf(stderr, "yacc: arg_list ::= SYMBOL\n");
        $$ = createArgNode($1);
    };

type:
    TYPE_INT {
    	$$ = INT_TYPE;
    }
    | TYPE_DOUBLE {
    	$$ = DOUBLE_TYPE;
    }

%%

