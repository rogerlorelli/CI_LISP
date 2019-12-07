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

- (neg 1)
  - TYPE: INT_TYPE
  - VALUE: -1
  
- (neg -1)
  - TYPE: INT_TYPE
  - VALUE: 1
  
- (neg 1.1)
  - TYPE: DOUBLE_TYPE
  - VALUE: -1.10
  
- (neg -1.1)
  - TYPE: DOUBLE_TYPE
  - VALUE: 1.10
  
- (abs 1)
  - TYPE: INT_TYPE
  - VALUE: 1
  
- (abs -1)
  - TYPE: INT_TYPE
  - VALUE: 1
  
- (abs 1.25)
  - TYPE: DOUBLE_TYPE
  - VALUE: 1.25
  
- (abs -1.5)
  - TYPE: DOUBLE_TYPE
  - VALUE: 1.50
  
- (exp 1)
  - TYPE: DOUBLE_TYPE
  - VALUE: 2.72
  
- (exp 1.1)
  - TYPE: DOUBLE_TYPE
  - VALUE: 3.00
  
- (exp 66)
  - TYPE: DOUBLE_TYPE
  - VALUE: 46071866343312917806440251392.00
  
- (exp -6.6)
  - TYPE: DOUBLE_TYPE
  - VALUE: 0
  
- (exp -2)
  - TYPE: DOUBLE_TYPE
  - VALUE: 0.14

- (sqrt 2)
	- TYPE: DOUBLE_TYPE
	- VALUE: 1.41
	
- (sqrt 4)
	- TYPE: DOUBLE_TYPE
	- VALUE: 2

- (sqrt -100)
	- TYPE: DOUBLE_TYPE
	- VALUE: -nan

- (sqrt 100)
	- TYPE: DOUBLE_TYPE
	- VALUE: 10 

- (cbrt 9)
	- TYPE: DOUBLE_TYPE
	- VALUE:  2.08

- (cbrt 8)
	- TYPE: DOUBLE_TYPE
	- VALUE:  2.00

- (cbrt 5.5123)
	- TYPE: DOUBLE_TYPE
	- VALUE:  1.77

- (cbrt -8)
	- TYPE: DOUBLE_TYPE
	- VALUE:  -2.00

- (cbrt -3)
	- TYPE: DOUBLE_TYPE
	- VALUE:  -1.44

- (log 1)
	- TYPE: DOUBLE_TYPE
	- VALUE:  0.00
	
- (log -1)
	- TYPE: DOUBLE_TYPE
	- VALUE: nan
	
- (log -515)
	- TYPE: DOUBLE_TYPE
	- VALUE:  nan

- (log 515)
	- TYPE: DOUBLE_TYPE
	- VALUE:  6.24

- (log 52.644)
	- TYPE: DOUBLE_TYPE
	- VALUE:  3.96

- (log 52)
	- TYPE: DOUBLE_TYPE
	- VALUE:  3.95
  
- (exp2 1)
  - TYPE: DOUBLE_TYPE
  - VALUE: 2
  
- (exp2 2.2)
  - TYPE: DOUBLE_TYPE
  - VALUE: 4.59
  
- (exp2 -1)
  - TYPE: DOUBLE_TYPE
  - VALUE: 0.50
  
- (exp2 -1.1)
  - TYPE: DOUBLE_TYPE
  - VALUE: 0.47
 
- (add 1 1)
  - TYPE: INT_TYPE
  - VALUE: 2
  
- (add 1.1 2.2)
  - TYPE: DOUBLE_TYPE
  - VALUE: 3.30
  
- (add 1 2.2)
  - TYPE: DOUBLE_TYPE
  - VALUE: 3.20
   
- (add -1 1)
  - TYPE: INT_TYPE
  - VALUE: 0
  
- (add -1.1 2.2)
  - TYPE: DOUBLE_TYPE
  - VALUE: 1.10
 
- (sub 1 1)
  - TYPE: INT_TYPE
  - VALUE: 2
  
- (add 1.1 2.2)
  - TYPE: DOUBLE_TYPE
  - VALUE: 3.30
  
- (add 1 2.2)
  - TYPE: DOUBLE_TYPE
  - VALUE: 3.20
   
- (add -1 1)
  - TYPE: INT_TYPE
  - VALUE: 0
  
- (add -1.1 2.2)
  - TYPE: DOUBLE_TYPE
  - VALUE: 1.10
  
- (add -1 2.2)
  - TYPE: DOUBLE_TYPE
  - VALUE: 1.20
  
- (sub 1 2)
  - TYPE: INT_TYPE
  - VALUE: 3.20
   
- (sub 1 -2)
  - TYPE: INT_TYPE
  - VALUE: 3
  
- (sub -2.2 1.1)
  - TYPE: DOUBLE_TYPE
  - VALUE: -3.30
  
- (sub 1.1 -2.2)
  - TYPE: DOUBLE_TYPE
  - VALUE: -3.30

- (neg (mult 5 (add 1 2)))
	- TYPE: INT_TYPE
	- VALUE: -15

- (remainder 5.62 1)
	- TYPE: DOUBLE_TYPE
	- VALUE: 0.62

- (reminder (add 16 52) (mult 5 6))
	- TYPE: INT_TYPE
	- VALUE: 8
	
- (pow 2 3)
	- TYPE: INT_TYPE
	- VALUE: 8

- (pow 2 3.3)
	- TYPE: DOUBLE_TYPE
	- VALUE: 9.85

- (pow 2 -1)
	- TYPE: INT_TYPE
	- VALUE: 1

- (pow 2 -2)
	- TYPE: INT_TYPE
	- VALUE: 0

- (pow 2.0 -2)
	- TYPE: DOUBLE_TYPE
	- VALUE: 0.25

- (max 5 1)
	- TYPE: INT_TYPE
	- VALUE: 5

- (max -5 1)
	- TYPE: INT_TYPE
	- VALUE: 1

- (max 5.5 1)
	- TYPE: DOUBLE_TYPE
	- VALUE: 5.50

- (max 5.0 5)
	- TYPE: DOUBLE_TYPE
	- VALUE: 5.00

- (max -5 -1)
	- TYPE: INT_TYPE
	- VALUE: -1
## TASK 2 Implementation
Implements grammar to allow for variables, called symbols in ciLisp. Requires modification to the lex file, the Bison file, the ciLisp.h & .c files. 

1. **lex file** - added symbol and let tokens
	   - also modifications to the **regex** to included letters and symbols that accepts 0 or 1 letters 
	   - edited the double regex to allow for more than 1 number before the decimal
	   - fixed the regex to include all functions needed for task two up to "hypot"
2. **Yacc/Bison** file 

	* extended the token to include the LET %token
	* added symbol to the ast node %type 
	* added let_section let_list let_elem to the symNode %type
	* extended the grammar in the s-expr to include symbol and let_section
	* let_section, let_list, let_elem and symbol were also added to the grammar in their own sections with appropriate definition function calls.

3. **ciLisp.h** file
- extended the ciLisp.h file to support the modifications to the lex and bison files.
	- modifications include the creation of or modifications to the following functions and structs:
	- NEW: SYMBOL_TABLE_NODE
		- an ident string
		- a value field that holds an ast_node, so a value can be anything from another symbol to a number to a function
		- a next field that references another symbol_table_node to create a linked list of variables.
	- NEW: SYMBOL_AST_NODE
		- an ident string
	- MOD: AST_NODE 
		- added:
			-  a symbol_table_node to allow a linked list of variable definitions to be attached.
			- a parent, to reference any "higher" ast_nodes that may contain a symbol_table_node list
			- a SYMBOL_AST_NODE field to the union along side the number and function ast nodes.
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
	
#### Added changes from task that were made in task two 
	
- **createFunctionNode**
	- attaches the new parent nodes of the operands op1 and op2 to the newly created node.
- **eval**
	- adds support for the evaulation of the new SYMBOL_AST_NODE using evaSymbolNode().
	- 

**Testing functionality code output**
//TODO

## TASK 3 Implementation
Adds the ability to type cast numbers and symbols
- **Yacc file**
	- Extended let_elem to accept an int symbol and and double symbol in the yacc file and to pass them to the createSymbolTableNode function
	 
- **Lex file**
	- added the support for the type casting to numbers directly looking for keyword "int" and "double".
	
- **ciLisp.c**
	- MOD: createSymbolTableNode
		- adds a parameter to the function for the number TYPE.
		- since all numbers are saved as an double, if the number is a double but the cast type is INT the number will be floored before saving.
	- MOD: evalSymbol
		- added type checking, if the val_type is cast as an INT but the stored value is evaluated as a DOUBLE a precision loss message will print and the value is floored.


**Testing functionality code output**

//TODO
## TASK 4 Implementation
Adds a print operation into the function list.
- **Lex file**
	- Added print to function list
- **ciLisp.c**
	- evalFunctionNode
		- added print to switch options
**Testing functionality code output**
## Task 5
This task for me was straight forward and I knew intuitivley what I needed to change to make this work. 

1. **Yacc file**

	*At this point I am starting to get the hang of how to add code to Lisp now. I have been always adding to the yacc or lex files first so this is how I started with this one too
	 
	* First checking the changed grammar given to us then adding those changes to the yacc file 
	
	* **f_expr** - Changed the fexpr to only accept a FUNC and a s_expr_list now it only takes two parameters instead of 3 
	* **s _ expr_list**	- Made a new ast type called s_expr _list and using the directions given in task 5 fully extended the grammar for this type
2. **lex files**

	* no changes were made to this during task 5
3. **FUNC_ AST_NODE**
	
	* We were asked to change this struct, it was first only supposed to take in two operands but we were asked to change that and to put in just one ast node called *opList.
	* From this extension to the FUNC_ AST_ NODE i had to then go through all segments of code that previously used op1 and op2 and change them to opList and opList.next 
4. **AST_ NODE**

	* To allow creations of lists of s-expressions, a link next is added to AST_NODE
	
5. **Finding Arity**

	* At this point we find out that we have three operation functions that are different than the other 20 or so that we are dealing with for this task. MULT_ OPER, ADD_ OPER AND PRINT _OPER
	* To make things a little easier kieth and I came to a conclusion that seperating all the math functions that only have one operand to one pile and setting the math functions that take in two opoerands into another pile and setting the other three into their own place as well would make things easier to read
	* this was accomplished with the helper function called **getArity()** this function sepereates all OPER_ TYPE to different FUNCTION_ ARITY enums calls
6. **evalFuncNode**
	* this function was heavily refractored and does not call the same thing it did previously
	* now there are three cases in evalFuncNode a Unary, Binary and Nary case which all call other subroutines that are called evalUnary, evalBinary, and evalNary 
7. **evalUnary**
	
	* In this helper method we go check if there are too few parameters and too many parameters before going into a switch statement 
	* In this switch it segragates all math operations that only take in one operand.
	* ther are 7 math functions so far that only take in one operand
	* within each case we call more helper functions where all the math is actually going on
8. **evalBinary**

	* In this helper method we go check if there are too few parameters and too many parameters before going into a switch statement 
	* In the switch it segragates all math operations that only take in two operands.
	* there are 10 math functions so far that only take in two operands
	* within each case we call more helper functions where all the math is actually going on

9. **evalNary**
	
	* In this helper method we go check if there are too few parameters before going into a switch statement 
	* In the switch it segragates all math operations that more than two operands.
	* there are 3 math functions so far that take in more than 2
	* within each case we call more helper functions where all the math is actually going on

10. **add mult print**

 Now finally at this point i needed to fix the three functions that take in more than two operands they essentially are all the same code for each of the three as well. These three again are **MULT, ADD** and **PRINT** I go into there respective helper functions and change how the math works for each of them

11. **freeNode**

	* now that task 5 finally started working before moving onto the next task I free all allocated space for freeNode
	* for this function I made about 4 small helper methods 
	* freeSymTableNode, freeSymNode, freeFuncNode, freeFuncOpList which are all used within the freeNode() function this part is necessary to help free all the allocated space that we have been using this entire project.
	* they recurively free the entore syntax tree and is only called after the execution of the code

**Testing functionality code output**

```
(mult ((let (double a (print(add 4 5 2)))) (sub (sub 100 a) a 5)) 4 2.5)
WARNING: too many parameters for the function sub
=> Integer value is 11

=> Integer value is 11

Double Value 780.00


> (add 1 2 3 4 5)
Integer value is 15

> ((let (int a 1)(double b 2))(print a b 3))
=> Integer value is 1
Double Value 2.00
Integer value is 3

> (add 1 2 3(sub 5 2))
Integer value is 9

```


##Task 6 


1. **yacc files** 
	* went though and added the new COND token
	* and also added a new ast node called s_expr_list
	* after this i went on and extended the grammar given to us	

**Testing functionality code output**





	
```
> ((let (int a (read)) (double b (read)) (c (read)) (d (read))) (print a b c d))
=> read := 3
Integer value is 3
read := 5.0
Double Value 5.00
read := 10
Integer value is 10
read := 5.175
Double Value 5.17

Double Value 5.17

> ((let (a 0)) (cond (less (rand) 0.5) (add a 1) (sub a 1)))
Integer value is 1

> ((let (myA (read))(myB (rand)))(cond (less myA myB) (print myA) (print myB)))
read := -1
=> Integer value is -1

Integer value is -1


``` 		
 		
