
# CILISP README

                                                   ### Tasks Completetd ###
- [x] TASK 1 
- [x] TASK 2
- [x] TASK 3
- [x] TASK 4
- [x] TASK 5
- [x] TASK 6
- [x] TASK 7
- [x] TASK 8
- [x] EXT: TASK 9

## TASK 1 implementation

*Finished TODO's provided within the ciLisp.c file
Implements the execution/evaluation of simple mathematical operations on integers and doubles, allows for the composition of functions as operators.

1. **createNumberNode**
	* assign number type based on the passed type value.
	* assign value to value field of the num_ast_node. 
	* Value field has been updated, from a union of a double value and a long int, into a single double value for ease of computation.
2. **createFunctionNode**
	* Assigns function type to type field
	* Assigns operands passed to the function to the operands fields op1 & op2.
	* using the passed funcName, calls resolveFun to get the operation enum and assign to oper field.
	  - if the operation enum is for Custom_oper, assigns the funcName to the name fields, else frees the funcName.   
3. **eval** 
 	* The foreman of the evaluation function, uses a switch statement calls one of the specialized evaluation functions based on the type of the node.
 	  - As of now it can only be a FUNCTION or NUM_NODE TYP
 	* Returns a RET_VAL that had been returned from the prior specialized evaluation function. 
4. **evalNumNode** 
	* Returns a RET_VAL containing the value and num type of the evaluated number node. 
5. **evalFunctionNode** 
	* Creates a result RET_VAL node with type INT and value NAN.
    - If the following instructions fail the function will return this node unchanged.
	* Uses a switch statement based on the oper enum value to determine the math operation to compute.
    - The majority of the operation have a helper function that is called when the operation is chosen that handles the computation and type checking for the return value.
      - Functions such as exp(), sqrt(), and log() auto assign the type DOUBEL_TYPE. 
	      - log() is the natural log
	* Creates temporary operands op1 & op2 that hold the values of the evaluated operands contained in the function node.
	* Runs the mathematical operation determined by the oper enum value on the temp operands and assigns the return value to the result variable. 
6. **printRetVal**
     - Is passed the RET_VAL from the root evaluation and prints the type and value.
       - The value print will be formatted according to the type of the number, INT or DOUBLE.
         - If the number is type INT lround() will be called on the value, else DOUBLE will print the number with no modification.

**Testing functionality code output**

  - Below are sample runs with outputs.
    - I believe that I have tested all the combinations for this level of the project


## TASK 2 Implementation
Implements grammar to allow for variables, called symbols in ciLisp. Requires modification to the lex file, the Bison file, the ciLisp.h & .c files. 

- **lex file** 
	- **MOD** 
		- modifications to the **regex** to included letters and symbols that accepts 0 or 1 letters 
		- edited the double regex to allow for more than 1 number before the decimal
		 - fixed the regex to include all functions needed for task two up to "hypot"
	- **NEW**
		- added symbol and let tokens
- **Yacc/Bison** file
	- **MOD**
		- extended the token to include the LET %token
		- extended the grammar in the s-expr to include symbol and let_section
	- **NEW**
		- added symbol to the ast node %type 
		- added let_section let_list let_elem to the symNode %type
		- let_section, let_list, let_elem and symbol were also added to the grammar in their own sections with appropriate definition function calls.

- **ciLisp.h** file
	- **MOD**
		- **AST_NODE** 
			-  a symbol_table_node to allow a linked list of variable definitions to be attached.
			- a parent, to reference any "higher" ast_nodes that may contain a symbol_table_node list
			- a SYMBOL_AST_NODE field to the union along side the number and function ast nodes.
	- **NEW** 
		- **SYMBOL_TABLE_NODE**
			- an ident string
			- a value field that holds an ast_node, so a value can be anything from another symbol to a number to a function
			- a next field that references another symbol_table_node to create a linked list of variables.
	- **SYMBOL_AST_NODE**
			- an ident string - for variable name
- **ciLisp.c**
	- **MOD**
		- **createFunctionNode()**
			- attaches the new parent nodes of the operands op1 and op2 to the newly created node.
		- **eval()**
			- adds support for the evaulation of the new SYMBOL_AST_NODE using evaSymbolNode().
	- **NEW**
		- **createSymbolTableNode()**
			- creates an instance of the new SYMBOL_TABLE_NODE 
			- sets the value type to the type passed in the parameters
			- sets the ident to the passed parametere
			- sets the value to the ast_node passed.
		- **linkSymbolTableNode()**
			- links the next field of the second node passed to the first node and returns the pointer to the second node.
		- **linkSymbolTableToAST()**
			- links the symbol list to the ast_node
		- **createSymbolNode()** 
			- creates an instance of the SYMBOL_AST_NODE with its ident set by the passed string.
		- **evalSymbolNode**
			- called by eval(), searches the symbolTableNode list of the AST_NODE and it's parent node.
				- if the symbol is found, evaluates the node's value, an ast_node itself, and returns the information in a RET_VAL
				- if the symbol is not found prints an error message and returns a RET_VAL with value nan. 
	

**Testing functionality code output**
>  ((let (a 5) (b c) (c 2) (d 4)) (neg (add (mult a c) (div d c))))
TYPE: INT_TYPE
VALUE: -12

>  ((let (a 5.0) (b c) (c 2) (d 4)) (neg (add (mult a c) (div d c))))
TYPE: DOUBLE_TYPE
VALUE: -12.00
	
## TASK 3 Implementation
Adds the ability to type cast numbers and symbols
- **Yacc file**
	- Extended let_elem to accept an int symbol and and double symbol in the yacc file and to pass them to the createSymbolTableNode function
	 
- **Lex file**
	- added the support for the type casting to numbers directly looking for keyword "int" and "double".
	
- **ciLisp.c**
	- **MOD** 
		- **createNumberNode()**
			- adds a Boolean flag, if a double is type cast into an INT a precision loss warning will print
		- **createSymbolTableNode()**
			- adds a parameter to the function for the number TYPE.
			- since all numbers are saved as an double, if the number is a double but the cast type is INT the number will be floored before saving.
		- **evalSymbol()**
			- added type checking, if the val_type is cast as an INT but the stored value is evaluated as a DOUBLE a precision loss message will print and the value is floored.


**Testing functionality code output**
- ((let (int a 5.0) (b c) (double c 2) (d 4)) (neg (add (mult a c) (div d c))))
	- WARNING: precision loss in the assignment for variable a
	- TYPE: DOUBLE_TYPE
	- VALUE: -12.00

- (abs (neg(add (mult 5 double 2) (div 4 2))))
	- TYPE: DOUBLE_TYPE
	- VALUE: 12.00

- (abs (neg(add (mult 5 double 2) (div double 5 2))))
	- TYPE: DOUBLE_TYPE
	- VALUE: 12.50

- (add int 2.2 4)
	- WARNING: precision loss during number node creation
	- TYPE: INT_TYPE
	- VALUE: 6
## TASK 4 Implementation
Adds a print operation into the function list.
- **Lex file**
	- Added print to function list
- **ciLisp.c**
	- evalFunctionNode
		- added print to switch options
		
**Testing functionality code output**
> (print (neg(add (mult 5 double 2) (div double 5 2))))
=> -12.50 
TYPE: DOUBLE_TYPE
VALUE: -12.50

> (print (abs (neg(add (mult 5 double 2) (div double 5 2)))))
=> 12.50 
TYPE: DOUBLE_TYPE
VALUE: 12.50

>  ((let (a 5)) (print a))
=> 5 
TYPE: INT_TYPE
VALUE: 5

> (print (add int 2.0 double 5))
WARNING: precision loss during number node creation
=> 7.00 
TYPE: DOUBLE_TYPE
VALUE: 7.00
## Task 5
Implements the ability for the language to support an operand list of arbitrary size.
- **Yacc file**
	- **MOD**:
		- **f_expr** 
			- changed f_expr from 2 expressions, having 1 or 2 operands, to a single expression operand list on undetermined size.
	- **NEW**	
		- **s _ expr_list**
			- new grammar that allows for single, multiple, and no s-expressions to be attached to a function expression.
			
- **ciLisp.h**
	- **MOD**:
		- **FUNC_ AST_NODE**
			- **MOD**
				- removed AST_NODEs op1 & op2 
			- **NEW**
				- replaced by op_list, an AST_NODE pointer, which will now link the list of the operands that are supposed to be evaluated and computed by the attached function.
	- **NEW**
		- **linkASTNode()**
			- links the chain of ast_nodes that make up the op_list to each other, making the linked-list.
		- **AST_ NODE**
			- **NEW**
				- added an AST_NODE pointer to the struct, allowing for the linking of AST_NODEs to act as an op_list.
				
- **ciLisp.c**
	- **MOD**
		- **createFunctionNode**
			- modified, removing the operand parameters and replaced with an AST_NODE pointer op_list to be assigned to the new op_list field in the new func_ast_node. Traverses the list and sets the parent of each operand to this new node. 
		- **evalFuncNode**
			- modified to evaluate an op_list now instead of a set number of operands.
			- prior helper methods were deconstructed and integrated back into the function except for the add and multiplication helpers. 
				- ADD, MULT, and PRINT operations now have the ability to accept any number of operands.
					- done by passing the helper methods the ast node containing the op_list and treating it like a linked list. The first pass will evaluate the first and second operands, the result will be assigned to the first operand, the second will evaluate the next operand in the list and the computation will repeat.   
				- logic is now implemented to check for the minimum appropriate number of operands to compute an operation, printing an error message and exiting if the minimum is not met. There is also logic to check for too many operands being passed to any operations, that are not the previously mentioned ADD, Mult, or PRINT, printing a warning message, but will compute the operation once with the necessary operands.

**Testing functionality code output**
> (print 1 2 3 4 5 6 7 8 9)
=> 1  2  3  4  5  6  7  8  9 
TYPE: INT_TYPE
VALUE: 9

> (add 1 2 3 4 5 6 7 8 9.0)
TYPE: DOUBLE_TYPE
VALUE: 45.00

> (mult 1 2 3 4 5.0)
TYPE: DOUBLE_TYPE
VALUE: 120.00

> (div 1 2 3 4 5 6 7 8 9)
eval function WARNING: too many parameters for the function div
TYPE: INT_TYPE
VALUE: 0

> (div 1.0 2 3 4 5 6 7 8 9)
eval function WARNING: too many parameters for the function div
TYPE: DOUBLE_TYPE
VALUE: 0.50

> (abs -5 2 3 6)
eval function WARNING: too many parameters for the function abs
TYPE: INT_TYPE
VALUE: 5

>(mult (add 1 1) (abs -5) (div double 5 2) (exp2 3))
TYPE: DOUBLE_TYPE
VALUE: 200.00

> ((let (a c) (b 5) (c (div 2 2)) (d (add a a c))) (mult (add a c) (abs (neg 5)) (div b 2) (exp2 d)))
Warning: Loss of Precision
TYPE: INT_TYPE
VALUE: 160

> ((let (a c) (double b 5) (c (div 2 2)) (d (add a a c))) (mult (add a c) (abs (neg 5)) (div b 2) (exp2 d)))
TYPE: DOUBLE_TYPE
VALUE: 200.00
## Task 6
- Adds the read function
	- allows for user input 
- Adds the rand function
	- allows for random values
- Adds conditional statements
	- requires the addition of conditional evaluation functions
		- lesser
		- greater
		- equal
		
- **Lex File**
	- **MOD**
		- adds the ability to parse a new keyword "cond" which will return a COND token.
		- added rand, read, equal, less, and greater to the function name list so the function creation would recognize the function calls

- **Yacc File**
	- **MOD**
		- Changes the s-expression grammar to allow for conditional statements.
			- cond keyword will start the statement, the following s-expr is the conditional function to check, the next s-expr is the expression that is evaluated if the conditional returned true, else the third expression is evaluated.

- **ciLisp.h**
	- **MOD**
		- AST_NODE_TYPE
			- added a condition node type to the enum list for use in eval().
		- AST_NODE
			- added a COND_AST_NODE to the union data, called condition. Will hold the three expressions for evaluation.
	- **NEW**
		- COND_AST_NODE
			- contains three fields:
				- cond
					- conditional expression to decide which of the following expressions is evaluated
				- ifTrue
					- expression to evaluate if the conditional is true.
				- ifFalse
					- expression to evaluate if the conditional is false.
			- all are ast_node pointers
- **ciLisp.c**
	- **MOD**
		- **eval()**
			- added support for calling evalCondNode() when needed.
		- **evalFuncNode()**
			- added functions:
				- read
					- reads a value from the user, changes the ast_node from a function node to a number node, in the number field of the data union the read in value is assigned to the value field and the type is dependent on whether or not the read value contained a decimal point.
				- rand
					- generates a random number from 0 to 1, changes the ast_node from a function node to a number node, in the number field of the data union the random value is assigned to the value field and the type is set as a double.
				- greater
					- returns 1 if the first operand is greater than the second operand, 0 if not.
				- less
					- returns 1 if the first operand is smaller than the second operand, 0 if not.
				- equal
					- returns 1 if the first operand is equal than the second operand, 0 if not.
	- **NEW**
		- **createCondNode()**
			- creates an ast_node of COND_TYPE and assigns the provided expressions to the appropriate fields. Sets the parents of the expressions this node. 
		- **evalCondNode()**
			- creates a new RET_VAL and uses a switch statement, based on the evaluation of the conditional expression either the ifTrue expression will be evaluated and the value assigned to RET_VAL or the ifFalse expression will be.
		
 **Testing functionality code output**
- Basic Function Use
>
	- Positive Integers
> (greater 10 2)
TYPE: INT_TYPE
VALUE: 1

> (greater 2 10)
TYPE: INT_TYPE
VALUE: 0

> (less 2 10)
TYPE: INT_TYPE
VALUE: 1

> (less 10 2)
TYPE: INT_TYPE
VALUE: 0

> (equal 10 10)
TYPE: INT_TYPE
VALUE: 1

> (equal 10 2)
TYPE: INT_TYPE
VALUE: 0

	- Negative Integers
	
> (equal -10 -10)
TYPE: INT_TYPE
VALUE: 1

> (less -5 -2)
TYPE: INT_TYPE
VALUE: 1

> (less -2 -5)
TYPE: INT_TYPE
VALUE: 0

> (greater -5 -1)
TYPE: INT_TYPE
VALUE: 0

> (greater -1 -5)
TYPE: INT_TYPE
VALUE: 1

> (equal -10 2)
TYPE: INT_TYPE
VALUE: 0

	- DOUBLES
	
> (greater 2.5 2.25)
TYPE: INT_TYPE
VALUE: 1

> (less 2.25 2.5)
TYPE: INT_TYPE
VALUE: 1

> (equal 2.5 2.5)
TYPE: INT_TYPE
VALUE: 1

	- RAND & READ

> (rand)
ERROR CHECKING PRINTING RAND: 0.84
TYPE: DOUBLE_TYPE
VALUE: 0.84

> (read 5.5)
eval function WARNING: too many parameters for the function read
read := 5
TYPE: INT_TYPE
VALUE: 5

> (read)
read := 5.0
TYPE: DOUBLE_TYPE
VALUE: 5.00

	- CONDITIONAL EXPRESSION TEST
		- also tests Task 4 & 5

> ((let (a (rand)) (b (read))) (cond (greater b a) (print a) (print b a)))
read := 0.5
ERROR CHECKING PRINTING RAND: 0.84
=> 0.50  0.84 
TYPE: DOUBLE_TYPE
VALUE: 0.84

	- Testing Prior Tasks

		- Tests Tasks 1, 2 & 3
> ((let (a b) (int b 2.5) (double c 3)) (equal (add a c) (div double 10 a)))
WARNING: precision loss in the assignment for variable b
WARNING: precision loss in the assignment for variable b
TYPE: INT_TYPE
VALUE: 1




>
## Task 7 & 8 & 9
DEVELOPERS NOTE:
Initially this task was supposed to be broken into three separate tasks, 2 required 1 optional, but I saw that it would be more logical to implement the end goal which removed the requirement of sections of 2 of the tasks. 

- Adds the ability to create user defined functions and allows tail-end recursive behavior. The function will follow same rules as the majority of already implemented function: 
	- when more operands are passed to the function then are in formal parameter of the function a warning will print but the function will still execute using the appropriate number of operands once. If less than the required parameters are passed an error will print and the program will exit safely. 


- **Lex File**
	- added support for a new keyword "lambda" and the return of a new LAMBDA token.
- **Yacc File**
	- added support for the LAMBA token.
		- **MOD**
			- let_elem
				- added a grammar that would allow the creation of custom functions, both type cast and not, that uses the SYMBOL token for passing it's name, and the LAMBDA token to delineate that it is a custom function.
		- **NEW**
			- struct arg_table_node
				- added to the union to allow for the return of arg_table_node pointers 
			- arg_list
				- a list of args, or the formal parameters, of the new custom function if any. The args follow the same rules as symbols are defined after the lambda keyword.
- **ciLisp.h**
	- **MOD** 
		- **AST_NODE**
			- added a ARG_TABLE_NODE pointer to the struct allowing for the attachment of an arg_list if the node contains a custom function.
			- added a Boolean flag isCustom to be set when a custom function is created to help in symbol evaluation, directing the search to check the arg_list instead of the symbol_table_node list.
	- **NEW** 
		- **ARG_TABLE_NODE**
			- This node, or a list of these nodes, will be attached to a custom function ast node and act as the parameter list of the function. 
			- new struct with three fields:
				- String ident: for a parameter name
				- NUM_AST_NODE value: acts as a stack to hold the evaluated values that are passed when the custom function is called.
					- DEVELOPERS NOTE: Since these tasks were so closely linked I implemented this section to be tail-recursive so all passed operands are evaluated before they are put onto the stack and the stack is cleared before any further operands are added. This eliminated the need for the stack to be a linked list, since there would only ever be 1 value per argument.
				- ARG_TABLE_NODE next: like the symbol table, an arg_list is a linked list of all the arguments needed for the function to operate. 
- **ciLisp.c**
	- **MOD** 
		- **evalFuncNode()**
			- added support to call evalCustomFuncNode(), also implements logic so if a custom function is to be evaluated the operands are not, this is handled within evalCustomFuncNode() and would lead to redundant evaluations. 
		- **evalSymbNode()**
			- adds support for searching arg_lists if a custom function is being evaluated.
	- **NEW**
		- **createArgNode()**
			- creates and returns the pointer to a new arg_table_node, passing it an id string, and leaving the value field null. 
		- **linkArgNode()**
			- points the next pointer of the first passed arg_table_node to a second arg_table_node and returns the pointer of the first node. 
		- **createCustomFunctionNode()**
			- unlike normal functions a custom function is stored as a symbol_table_node, the function name is the ident, the algorithm of the function is stored as an ast_node in the value field and the arg_list is attached to the ast_node's arg_list field. The isCustom flag is set to true, where is is normally false, and since the structure of the node is still a symbol_table_node it retains the num_type field so type casting is allowed.
		- **evalCustomFunc()**
			- called by evalFuncNode(), functions much like evalSymbolNode() in that it searches the attached ast_node and it's parents for the symbol_table_node that contains the definition of what the function name is. If found a temporary arg_list is created and the arguments passed from the function call are evaluated and copied. This evaluation of the passed arguments before the custom function is evaluated is one of the key features that allows for this grammar to be tail-recursive. The temporary arg_list then transfers its values to the arg_list of the function definition, here is also where the error checking for the appropriate number of parameters passed vs number of formal parameters happens. Finally the custom function definition is evaluated. 
			
					
 **Testing functionality code output**
 >
	> Task 1: use of remainder function
	> Task 2: use of symbol variables
	>Task 3: Type Casting
	> Task 4: use of the print function
	>Task 5: use of an arbitrary number of operations and the warnings that come from them if misused
	 > Task 6: Conditional Expressions and Operations & Functions: RAND & READ
	 
> ((let (gcd lambda (x y) (cond (greater y x) (gcd y x) (cond (equal y 0) (x) (gcd y (remainder x y)))))) (gcd 95 55))
	- TYPE: INT_TYPE
	- VALUE: 5

> ((let (gcd lambda (x y) (cond (greater y x) (gcd y x) (cond (equal y 0) (x) (gcd y (remainder x y)))))) (gcd 95 ))
	- ERROR: too few parameters for the function gcd
	- Process finished with exit code 1
 
 > ((let (gcd lambda (x y) (cond (greater y x) (gcd y x) (cond (equal y 0) (x) (gcd y (remainder x y)))))) (gcd 95 55 55))
	- eval custom function WARNING: too many parameters for the function gcd
	- TYPE: INT_TYPE
	- VALUE: 5
	

> ((let (gcd lambda (x y) (cond (greater y x) (gcd y x) (cond (equal y 0) (x) (gcd y (remainder x y)))))) (gcd double 95 55))
	- TYPE: DOUBLE_TYPE
	- VALUE: 5.00

> ((let (int gcd lambda (x y) (cond (greater y x) (gcd y x) (cond (equal y 0) (x) (gcd y (remainder x y)))))) (gcd 95 55))
	- TYPE: INT_TYPE
	- VALUE: 5

> ((let (int gcd lambda (x y) (cond (greater y x) (gcd y x) (cond (equal y 0) (x) (gcd y (remainder x y)))))) (gcd double 95 55))
	- WARNING: precision loss in the assignment for variable gcd
	- TYPE: INT_TYPE
	- VALUE: 5

> ((let (countdown lambda (x) (cond (greater x 0) (countdown (print (sub x 1))) (print x)))) (countdown 10))
=> 9 
=> 8 
=> 7 
=> 6 
=> 5 
=> 4 
=> 3 
=> 2 
=> 1 
=> 0 
=> 0 
	- TYPE: INT_TYPE
	- VALUE: 0
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTE2MzQ2ODAwNiwxMTQ5MTgwMjgxLC05NT
MwNzEzMjgsOTA5NTUyNTQwLDE1MDQ1NDY1NzYsNDE0ODA0MDky
LC0yMTE2MTgzNjIzLC0xOTQ1NTMwMzYxLDEwMDQ2NTUyNTIsMT
I3Njg4MjU3MiwtMTA5MTc3Njc1NCwtNTY1MTkzNzgzXX0=
-->