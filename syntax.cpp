#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

//main function will read word by word

int Rat24F();
int OptFunctionDefinitions();
int FunctionDefinitions();
int FunctionDefinitionsPrime();
int Function();
int OptParameterList();
int ParameterList();
int ParameterListPrime();
int Parameter();
int Qualifier();
int Body();
int OptDeclarationList();
int DeclarationList();
int DeclarationListPrime();
int Declaration();
int IDs();
int IDsPrime();
int StatementList();
int StatementListPrime();
int Statement();
int Compound();
int Assign();
int If();
int fiPrime();
int Return();
int ReturnPrime();
int Print();
int Scan();
int While();
int Condition();
int Relop();
int Expression();
int ExpressionPrime();
int Term();
int TermPrime();
int Factor();
int Primary();

std::ofstream ostrm("test.txt");

std::string current_word;
std::string token;
int line_number;
    //ifstream has special functions and you dont need 
    //to open and close the file but you still need to make sure the file opened correctly, we can add that later
std::ifstream file("temp.txt"); //test line
bool switcher{true};

void errors(const std::string& expected, const std::string suggested) {
    std::cerr << "Error on line " << line_number << ": Unexpected token " << current_word << ".\n";
    std::cerr << "Expected " << expected << ".\n";
    std::cerr << "Suggested: " << suggested << ".\n"; 
    //std::cerr is similar to cout, but meant for errors. there is no buffer so it will print immediately!
    //exit(1);
}

void printToken(std::string tok, std::string lex) {
    std::cout << "Token: " << tok << "     Lexeme: " << lex << "\n";
    ostrm << "Token: " << tok << "     Lexeme: " << lex << "\n";
}

void moveFile() {
    if (file >> token && file >> current_word) {
        std::cout << token << " " << current_word;
    } else {
        std::cerr << "Error: EOF of no more tokens";
        exit (1);
    }
    // file >> token;
    // file >> token;
    // file >> current_word;
    // file >> current_word;
    line_number++;

}
//

int main() {    

    if(!file.is_open()) {
        std::cout<< "Error: File could not be opened.\n"; 
    } else {
        moveFile();
        Rat24F();
        std::cout <<"success";
    }



    return 0;
}

// Issue: find a way to store token type each time because whenever <Identifier>,<Integer>,<Real> are in
// the rules we need to check if the current word is those types. 
// i think the token and current word vars fixes this

int Rat24F() {

    //print rule
    // if (switcher == true) {
    //     std::cout << "<Rat24F>  ::=   <Opt Function Definitions>   @  <Opt Declaration List>  <Statement List>  @\n";
    // }
    if(OptFunctionDefinitions()) {
        if (current_word == "@") {
            printToken(token, current_word);
            moveFile();
            if(OptDeclarationList()) {
                if(StatementList()) {
                    if (current_word == "@") {
                        printToken(token, current_word);
                        moveFile();
                        if (switcher == true) {
                            std::cout << "<Rat24F>  ::=   <Opt Function Definitions>   @  <Opt Declaration List>  <Statement List>  @\n";
                        }
                        return 1;
                    } else {
                        errors("'@'", "Add a '@' to close the struct");
                        return 0;
                    }
                } else {
                    errors("a valid statements","Ensure the statements are valid in the body");
                    return 0;
                }
            } else {
                errors("optional function definitions or empty", "Check for function definitions");
                return 0;
            }
        } else { 
            errors("'@'", "The Rat24F should be delimited by '@'");
            return 0;
        }
    } else { 
        return 0;
    }
}

int OptFunctionDefinitions() {
    // if (switcher)
    //     std::cout << "<Opt Function Definitions> ::= <Function Definitions>     |  <Empty>\n";
    if(FunctionDefinitions()) {
        if (switcher) {
            std::cout << "<Opt Function Definitions> ::=  <Function Definitions>\n";
        }
        return 1;
    } else {
        if (switcher) {
            std::cout << "<Opt Function Definitions> ::=  <Empty>\n";
        }
        // hit empty 
        return 1;
    }
}

// Backtracking
int FunctionDefinitions() {
    // if(switcher)
    //     std::cout << "<Function Definitions>  ::= <Function> | <Function> <Function Definitions>\n";
    if (Function()) {
        if (FunctionDefinitionsPrime()) {
            if(switcher) std::cout << "<Function Definitions> ::= <Function><FunctionDefinitionsPrime>\n";
            return 1;
        }
    }
    return 0;
}


int FunctionDefinitionsPrime() {
    
    if(Function()) {
        if(FunctionDefinitions()) {
        if(switcher) { 
            std::cout << "<FunctionDefinitionsPrime> ::= <FunctionDefinitions>";
        }
        return 1;
        }
    }
    
    return 1;
}


int Function() {
    // if(switcher)
    //     std::cout << "<Function> ::= function  <Identifier>   ( <Opt Parameter List> )  <Opt Declaration List>  <Body>\n";

    if(current_word == "function") {
        //print <identifier> i think
        printToken(token, current_word);
        moveFile();
        if(current_word == "(") {
            printToken(token, current_word);
            moveFile();
            if(OptParameterList()) {
                if(current_word == ")") {
                    printToken(token, current_word);
                    moveFile();
                    if(switcher) {
                        std::cout << "<Function> ::= function  <Identifier>   ( <Opt Parameter List> )  <Opt Declaration List>  <Body>\n";
                    }
                    return 1;
                } else {
                    errors("')'", "Add a closing parenthesis");
                    return 0;
                }
            } else {
                errors("parameters or empty parameters", "Check your parameters");
                return 0;
            }
        } else {
            errors("'('", "Add an opening parenthesis");
            return 0;
        }
    } else {
        return 0;
    }

}



int OptParameterList(){
    // if(switcher)
    //     std::cout << "<Opt Parameter List> ::=  <Parameter List>    |     <Empty>\n";
    if(ParameterList()) {
        if(switcher) {
            std::cout << "<Opt Parameter List> ::=  <Parameter List>\n";
        }
        return 1;
    }
    else {
        //hit empty
        if(switcher) {
            std::cout << "<Opt Parameter List> ::=  <Empty>\n";
        }
        return 1;
    }
}

// Backtracking
int ParameterList() {
    if(switcher)
        std::cout << "<Parameter List>  ::=  <Parameter>    |     <Parameter> , <Parameter List>\n";
        
    if (Parameter()) {
        if(ParameterListPrime()) {
            return 1;
        }
    
    }
    return 0;
}

int ParameterListPrime() {
    if(current_word == ",") {   // ,
        printToken(token, current_word);
        moveFile();
        if(ParameterList()) {   // <PL>
            return 1;
        } else {
            errors("','", "Parameter after comma");
            return 0;
        }
    } else {    // | ε 
        // hit empty
        return 1;
    }
    return 1;
}

int Parameter() {
    // if(switcher)
    //     std::cout << "<Parameter> ::=  <IDs >  <Qualifier>\n";
    if(IDs()) {
        if(Qualifier()) {
            if(switcher) {
                std::cout << "<Parameter> ::=  <IDs >  <Qualifier>\n";
            }
            return 1;
        }
    } else { 
        errors("an identifier", "Ensure there is a valid identifier for the parameter");
        return 0;
    }
    return 0;
}

int Qualifier() {
    // if(switcher)
    //     std::cout << "<Qualifier> ::= integer    |    boolean    |  real \n";
        
    if(current_word == "integer") {
        printToken(token, current_word);
        moveFile();
        if(switcher) {
            std::cout << "<Qualifier> ::= integer \n";
        }
        return 1;
    } else if(current_word == "boolean") {
        printToken(token, current_word);
        moveFile();
        if(switcher) {
            std::cout << "<Qualifier> ::= boolean \n";
        }
        return 1;
    } else if(current_word == "real") {
        printToken(token, current_word);
        moveFile();
        if(switcher) {
            std::cout << "<Qualifier> ::=  real \n";
        }
        return 1;
    } else {
        errors("valid qualifier", "Check for type (e.g., 'integer', 'boolean', 'real')");
        return 0;
    }
}

int Body() {
    // if(switcher)
    //     std::cout << "<Body>  ::=  {  < Statement List>  }\n";
        
    if(current_word == "{") {
        printToken(token, current_word);
        moveFile();
        if(StatementList()) {
            if (current_word == "}") {
                printToken(token, current_word);
                moveFile();
                if(switcher) {
                    std::cout << "<Body>  ::=  {  < Statement List>  }\n";
                }
                return 1;
            } else {
                errors("'}'","Add a closing brace to end the body");
                return 0;
            }
        } else {
            errors("valid statements", "Ensure the statements are valid in the body");
            return 0;
        } 
    } else {
        errors("'{'", "Add an opening brace to the start of the body");
        return 0;
    }
}

int OptDeclarationList(){
    // if(switcher)
    //     std::cout << "<Opt Declaration List> ::= <Declaration List>   |    <Empty>\n";
        
    if(DeclarationList()) {
        if(switcher) {
            std::cout << "<Opt Declaration List> ::= <Declaration List>\n";
        }
        return 1;
    } else {
        //hit empty
        if(switcher) {
            std::cout << "<Opt Declaration List> ::= <Empty>\n";
        }
        return 1;
    }
}

// Backtracking
int DeclarationList(){
    if(switcher)
        std::cout << "<Declaration List>  := <Declaration> ;     |      <Declaration> ; <Declaration List>\n";   
        
    if(Declaration()) {     // <D>
        if(current_word == ";") {   // ;
            printToken(token, current_word);
            moveFile();
            if (DeclarationListPrime()) // <DL'>
                return 1;
        } else {
            // errors("';'", "Add a semicolon, ';'");
            return 0;
        }
    } 
    return 0;
}


// <DL’> → <DL> | ε
int DeclarationListPrime() {
    if(Declaration()) {
        return 1;
    } else {
        return 1;
    }
}

int Declaration(){
    // if(switcher)
    //     std::cout << "<Declaration> ::=   <Qualifier > <IDs>\n";
    if(Qualifier()) {
        if(IDs()) {
            if(switcher) {
                std::cout << "<Declaration> ::=   <Qualifier > <IDs>\n";
            }
            return 1;
        }
    } else {
        errors("a valid qualifier","Ensure it starts with a valid type (e.g., 'int', 'real', 'boolean')");
        return 0;
    }
    return 0;
} 

// Backtracking
// <ID> → <I><ID’>
int IDs() {
    if(switcher)
        std::cout << "<IDs> ::=     <Identifier>    | <Identifier>, <IDs>\n";

    if (token == "ID") { // <I>
        printToken(token, current_word);
        moveFile();
        if(IDsPrime()) // <ID'>
            return 1;
    }  else {
        errors("an identifier", "Ensure the current token is valid");
        return 0;
    }
    return 0;
}

// <ID’> → ,<ID> | ε
int IDsPrime() {
    if(current_word == ",") {   // ,
        printToken(token, current_word);
        moveFile();
        if(IDs()) { // <ID>
            return 1;
        } else {
            errors("an identifier", "Check if token is a valid identifier following ','");
            return 0;
        }
    } else {    // hit empty
        return 1;
    }
}



// Backtracking
int StatementList() {
    if(switcher)
        std::cout << "<Statement List> ::=   <Statement>   | <Statement> <Statement List>\n";
    if(Statement()) {
        if(StatementListPrime()) {
            return 1;
        }
    } 
    return 0;
}

// <SL’> → <SL> | ε
int StatementListPrime() {
    if(StatementList()) {
        if(switcher) std::cout <<"<SL’> → <SL> | ε";
        return 1;
    } else {    // hit empty
        if(switcher) std::cout <<"<SL’> → <SL> | ε";
        return 1;
    }
}

int Statement() {
    // if(switcher)
    //     std::cout << "<Statement> ::=   <Compound>  |  <Assign>  |   <If>  |  <Return>   | <Print>   |   <Scan>   |  <While>\n";
    if(Compound()) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Compound>\n";
        }
        return 1;
    } else if(Assign()) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Assign>\n";
        }
        return 1;
    } else if(If()) {
        if(switcher) {
            std::cout << "<Statement> ::=   <If>\n";
        }
        return 1;
    } else if (Return()) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Return>\n";
        }
        return 1;
    }  else if (Print()) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Print>\n";
        }
        return 1;
    } else if(Scan()) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Scan>\n";
        }
        return 1;
    } else if(While()) {
        if(switcher) {
            std::cout << "<Statement> ::=   <While>\n";
        }
        return 1;
    } else {
        errors("statement", "Ensure a valid statement is provided"); // keep?
        return 0;
    }
}

int Compound() {
    // if(switcher)
    //     std::cout << "<Compound> ::=   {  <Statement List>  }\n";
    if(current_word == "{") {
        printToken(token, current_word);
        moveFile();
        if(StatementList()) {
            if (current_word == "}") {
                printToken(token, current_word);
                moveFile();
                if(switcher) {
                    std::cout << "<Compound> ::=   {  <Statement List>  }\n";
                }
                return 1;
            } else {
                errors("'}'", "Add a closing brace");
                return 0;
            }
        } else {
            errors("a valid statements","Ensure the statements are valid in the body");
            return 0;
        }
    } else { 
        errors("'{'", "Add an opening brace");
        return 0;
    }
}

int Assign() {
    // if(switcher)
    //     std::cout << "<Assign> ::=     <Identifier> = <Expression> ;\n";
    if(token == "ID") {
        printToken(token, current_word);
        moveFile();
        if (current_word == "=") {
            if(Expression()) {
                if(current_word == ";") {
                    printToken(token, current_word);
                    moveFile();
                    if(switcher) {
                        std::cout << "<Assign> ::=     <Identifier> = <Expression> ;\n";
                    }
                    return 1;
                } else {
                    errors("';'", "Add a semicolon");
                    // return 0;
                }
            } else {
                errors("valid expression", "Check the expression for missing parts");
                return 0;
            }
        } else {
            errors("'='","Needs an assignment operator '=' after identifier");
            return 0;
        }
    } else {
        errors("identifier", "Needs a starting identifier");
        return 0;
    }
return 0;
}



int If(){
    if(switcher)
        std::cout << "<If> ::=     if  ( <Condition>  ) <Statement>   fi  |   if  ( <Condition>  ) <Statement>   else  <Statement>  fi\n";
    if(current_word == "if") {              // if 1
        printToken(token, current_word);
        moveFile();
        if(current_word == "(") {           // ( 2
            printToken(token, current_word);
            moveFile();
            if(Condition()) {               // <C> 3
                if(current_word == ")") {   // ) 4
                    printToken(token, current_word);
                    moveFile();
                    if(Statement()) {       // <S> 5
                        if (fiPrime())
                            if(current_word == "fi") {
                                printToken(token, current_word);
                                moveFile();
                                return 1;
                            }
                    }
                }
            }
        }
    }
    return 0;
}

int fiPrime() {
    std::cout << "fiprime";
    if(current_word == "else") {
        printToken(token, current_word);
        moveFile();
        if(Statement()) {
            return 1;
        }
    }
    return 1;
}




// int If() {
//     if(switcher)
//         std::cout << "<If> ::=     if  ( <Condition>  ) <Statement>   fi  |   if  ( <Condition>  ) <Statement>   else  <Statement>  fi\n";
//     if(current_word == "if") {              // if 1
//         printToken(token, current_word);
//         moveFile();
//         if(current_word == "(") {           // ( 2
//             printToken(token, current_word);
//             moveFile();
//             if(Condition()) {               // <C> 3
//                 if(current_word == ")") {   // ) 4
//                     printToken(token, current_word);
//                     moveFile();
//                     if(Statement()) {       // <S> 5
//                         if (fiPrime())
//                             return 1;
//                     } else { // 5
//                         errors("a valid statement", "Ensure there is a valid statement after 'if' condition");
//                         return 0;
//                     }
//                 } else { // 4
//                     errors("')'", "Ensure the condition is closed");
//                     return 0;
//                 }
//             } else {    // 3
//                 errors("a valid condition", "Ensure the statement contains a valid condition");
//                 return 0;
//             }
//         } else {    // 2
//             errors("'('", "Ensure the condition contains an opening parenthesis");
//             return 0;
//         } 
//     }  // 1
//     return 0; // means it is not an if statement
// }

// // <f’> → <f> | <e> <S> <f>
// int fiPrime() {
//     if(current_word == "fi") {  // <f>
//         printToken(token, current_word);
//         moveFile();
//         return 1;
//     } else if (current_word == "else") {    //  <e> 6
//         printToken(token, current_word);
//         moveFile();
//         if(Statement()) {       // 7
//             if(current_word == "fi") {  // <f> 8
//                 printToken(token, current_word);
//                 moveFile();
//                 return 1;
//             } else {    // 8
//                 errors("'fi'", "Ensure the block is closed with 'fi'");
//                 return 0;
//             }
//         } else {    // 7
//             errors("a valid statement", "Ensure there is a valid statement after 'else'");
//             return 0;
//         } 
//     } else { // 6
//         errors("'fi' or 'else'", "Ensure there is a valid statement after the condition");
//         return 0;
//     }  
// }


// Backtracking
int Return() {
    if(current_word == "return") {  // <r>
        printToken(token, current_word);
        moveFile();
        if(current_word == ";") {   // ,
            printToken(token, current_word);
            moveFile(); 
            if(ReturnPrime()) {
                return 1;
            }
        }
    } 
    return 0;
}

int ReturnPrime() {

    if(Expression()) {
        return 1;
    }
    return 1;
}
// int Return() {
//     if(switcher)
//         std::cout << "<Return> ::=  return ; |  return <Expression> ;\n";
//     if(current_word == "return") {  // <r>
//         printToken(token, current_word);
//         moveFile();
//         if(current_word == ";") {   // ,
//             printToken(token, current_word);
//             moveFile();
//             if(ExpressionPrime()) { // <E'>
//                 return 1;
//             } else {
//                 errors("';'", "Add a semicolon");
//                 return 0;
//             }
//         } else {
//             errors("valid expression", "Check the expression for missing parts");
//             return 0;
//         }
//     } 
//     return 0;
// }

int Print() {
    // if(switcher)
    //     std::cout << "<Print> ::=     put ( <Expression>);\n";
    if (current_word == "put") {
        printToken(token, current_word);
        moveFile();
        if(current_word == "(") {
            printToken(token, current_word);
            moveFile();
            if(Expression()) {
                if(current_word == ")") {
                    printToken(token, current_word);
                    moveFile();
                    if(current_word == ";") {
                        printToken(token, current_word);
                        moveFile();
                        if(switcher) {
                            std::cout << "<Print> ::=     put ( <Expression>);\n";
                        }
                        return 1;
                    } else {
                        errors("';'", "Add a semicolon");
                        return 0;
                    }
                } else {
                    errors("')'", "Ensure the condition is closed");
                    return 0;
                }
            } else {
                errors("valid expression", "Check the expression for missing parts");
                return 0;
            }
        } else {
            errors("'('", "Ensure the condition contains an opening parenthesis");
            return 0;
        }
    } else {
        return 0;
    }
}

int Scan() {
    // if(switcher)
    //     std::cout << "<Scan> ::=    get ( <IDs> );\n";
    if(current_word == "get") {
        printToken(token, current_word);
        moveFile();
        if(current_word == "(") {
            printToken(token, current_word);
            moveFile();
            if(IDs()) {
                if(current_word == ")") {
                    printToken(token, current_word);
                    moveFile();
                    if(current_word == ";") {
                        if(switcher) {
                            std::cout << "<Scan> ::=    get ( <IDs> );\n";
                        }
                        return 1;
                    } else {
                        errors("';'", "Add a semicolon");
                        return 0;
                    }
                } else {
                    errors("')'", "Ensure the condition is closed");
                    return 0;
                }
            } else {
                errors("valid identifiers","Ensure the statement includes a valid identifier");
                return 0;
            }
        } else {
            errors("'('", "Ensure the condition contains an opening parenthesis");
            return 0;
        }
    } else {
        return 0;
    }
}

int While() {
    // if(switcher)
    //     std::cout << "<While> ::=  while ( <Condition>  )  <Statement>\n";
    if(current_word == "while") {
        printToken(token, current_word);
        moveFile();
        if(current_word == "(") {
            printToken(token, current_word);
            moveFile();
            if(Condition()) {
                if(current_word == ")") {
                    printToken(token, current_word);
                    moveFile();
                    if(Statement()) {
                        if(switcher) {
                            std::cout << "<While> ::=  while ( <Condition>  )  <Statement>\n";
                        }
                        return 1;
                    } else {
                        errors("a valid statement", "Ensure there is a valid statement after 'while'");
                        return 0;
                    }
                } else {
                    errors("')'", "Ensure the condition is closed");
                    return 0;
                }
            } else {
                errors("a valid condition", "Ensure the statement contains a valid condition");
                return 0;
            }
        } else {
            errors("'('", "Ensure the condition contains an opening parenthesis");
            return 0;
        }
    } else {
        return 0;
    }
}

int Condition() {
    // if(switcher)
    //     std::cout << "<Condition> ::=     <Expression>  <Relop>   <Expression>\n";
        
    if(Expression()) {
        if(Relop()) {
            if(Expression()) {
                if(switcher) {
                    std::cout << "<Condition> ::=     <Expression>  <Relop>   <Expression>\n";
                }
                return 1;
            } else {
                errors("a valid expression", "Ensure there is a valid expression");
                return 0;
            }
        } else {
            errors("a valid relational operator", "Ensure there is a valid relational operator (e.g., '==', '!=', '<', '>', '<=', '>=')");
            return 0;
        }
    } else { 
        errors("a valid expression", "Ensure there is a valid expression");
        return 0;
    }
}

int Relop() {
    if(switcher)
        std::cout << "<Relop> ::=        ==   |   !=    |   >     |   <    |  <=   |    =>\n";
        
    if (current_word == "==") {
        printToken(token, current_word);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        ==\n";
        }
        return 1;
    } else if (current_word == "!=") {
        printToken(token, current_word);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        !=\n";
        }
        return 1;
    } else if (current_word == ">") {
        printToken(token, current_word);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        >\n";
        }
        return 1;
    } else if (current_word == "<") {
        printToken(token, current_word);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        <\n";
        }
        return 1;
    } else if (current_word == "<=") {
        printToken(token, current_word);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        <=\n";
        }
        return 1;
    } else if (current_word == "=>") {
        printToken(token, current_word);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        =>\n";
        }
        return 1;
    } else {
        errors("a valid relational operator", "Ensure there is a valid relational operator (e.g., '==', '!=', '<', '>', '<=', '>=')");
        return 0;
    }

    // if(current_word == "==" || 
    //    current_word =="!=" || 
    //    current_word ==">" || 
    //    current_word =="<" || 
    //    current_word =="<=" || 
    //    current_word =="=>") {
    //     line_number++;
    //     file >> token;
    //     file >> current_word;
    //     return 1;
    // } else {
    //     errors("a valid relational operator", "Ensure there is a valid relational operator (e.g., '==', '!=', '<', '>', '<=', '>=')");
    //     return 0;
    // }

}

//left recursion 
int Expression() {
    // if(switcher)
    //     std::cout << "<Expression>  ::=    <Expression> + <Term>    | <Expression>  - <Term>    |    <Term>\n";
    if(Term()) {
        if(ExpressionPrime()){
            if(switcher) {
                std::cout << "<Expression>  ::=    <Expression> + <Term>    | <Expression>  - <Term>    |    <Term>\n";
            } // shall we add more statements to accompany the other statements? (+, -, and just the term) (im just adding comments to keep track of stuff)
            return 1;
        }
    } 
    return 0; // do we need errors here? -kat
    
}

int ExpressionPrime() {
    if(current_word == "+" || current_word == "-") {
        printToken(token, current_word);
        moveFile();
        if(Term()) {
            if(ExpressionPrime()) {
                return 1;
            } //else {
            //     errors("prime expression","Ensure there is a valid Prime Expression"); // keep? adjust? -kat
            //     return 0;
            // }
        } //else {
        //     errors("term","Ensure there is a valid Term used"); // keep? adjust? -kat
        //     return 0;
        // }
    } else if(Term()) {
        return 1;
    }
    return 1;

}


// left recursion
int Term() {
    if(switcher)
        std::cout << "<Term>    ::=      <Term>  *  <Factor>     |   <Term>  /  <Factor>     |     <Factor>\n";
    
    if(Factor()){
        if(TermPrime()) {
            return 1;
        }
    } else {
        errors("factor","Ensure there is a valid Factor used");
        return 0;
        // Error Handling Here? for surely
    }
    return 0;   // temp
}


int TermPrime() {
    if(current_word == "*" || current_word == "/") {
        printToken(token, current_word);
        moveFile();
        if(Factor()) {
            if (TermPrime()) {
                return 1;
            }  
        }
    } else {    // hit empty
        return 1;
    }
return 1;
}

int Factor() {
    // if(switcher)
    //     std::cout << "<Factor> ::=      -  <Primary>    |    <Primary>\n";
        
    if(current_word == "-") {
        printToken(token, current_word);
        moveFile();
        if(Primary()) {
            if(switcher) {
                std::cout << "<Factor> ::=      -  <Primary>\n";
            }
            return 1;
        } else {
            errors("a valid primary value", "Ensure there is a valid value following the '-'");
            // return 0;
        }
    } else if(Primary()) {
        if(switcher) {
                std::cout << "<Factor> ::=      <Primary>\n";
            }
        return 1;
    } else {
        errors("a valid primary value", "Ensure it starts with a valid primary value");
        return 0;
    }
return 0;
}

int Primary() {
    if(switcher)
        std::cout << "<Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   |  <Real>  |   true   |  false\n";

    if(token == "ID") {
        printToken(token, current_word);
        moveFile();
        if(current_word == "(") {
            printToken(token, current_word);
            moveFile(); 
            if(IDs()) {
                if(current_word == ")") {
                    printToken(token, current_word);
                    moveFile();
                    return 1;  
                }
            }
        }
    }
    
    if(current_word == "(") {
            printToken(token, current_word);
            moveFile();
        if(Expression()) {
            if(current_word == ")") {
                printToken(token, current_word);
                moveFile();
                return 1;
            }
        }
    }

    if(token == "ID" || token == "Integer" || token == "Real" || current_word == "true" || current_word == "false") {
        printToken(token, current_word);
        moveFile();
        return 1;
    }
return 0;
}