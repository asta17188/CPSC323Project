#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

//main function will read word by word
std::string current_word;
std::string token;
int line_number;
    //ifstream has special functions and you dont need 
    //to open and close the file but you still need to make sure the file opened correctly, we can add that later
std::ifstream file("output.txt"); 
bool switch{true};

void errors(const std::string& expected, const std::string suggested) {
    std::cerr << "Error on line " << line_number << ": Unexpected token " << current_word << ".\n";
    std::cerr << "Expected " << expected << ".\n";
    std::cerr << "Suggested: " << suggested << ".\n"; 
    // std::cerr is similar to cout, but meant for errors. there is no buffer so it will print immediately!
    exit(1);
}

int main() {    

    
    
    
    file >> token;
    file >> current_word;
    line_number++;
    //skip first 2 words because the first two words are just lexeme and token



    return 0;
}

// Issue: find a way to store token type each time because whenever <Identifier>,<Integer>,<Real> are in
// the rules we need to check if the current word is those types. 
// i think the token and current word vars fixes this

int Rat24F() {

    //print rule
    // if (switch == true) {
    //     std::cout << "<Rat24F>  ::=   <Opt Function Definitions>   @  <Opt Declaration List>  <Statement List>  @\n";
    // }
    if(OptFunctionDefinitions()) {
        if (current_word == "@") {
            if(OptFunctionDefinitions()) {
                if(StatementList()) {
                    if (current_word == "@") {
                        if (switch == true) {
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
    // if (switch)
    //     std::cout << "<Opt Function Definitions> ::= <Function Definitions>     |  <Empty>\n";
    if(FunctionDefinitions()) {
        if (switch) {
            std::cout << "<Opt Function Definitions> ::=  <Function Definitions>\n";
        }
        return 1;
    } else {
        if (switch) {
            std::cout << "<Opt Function Definitions> ::=  <Empty>\n";
        }
        // hit empty 
        return 1;
    }
}

// Backtracking
int FunctionDefinitions() {
    // if(switch)
    //     std::cout << "<Function Definitions>  ::= <Function> | <Function> <Function Definitions>\n";
    if(Function()) {
        if(switch) {
            std::cout << "<Function Definitions>  ::=  <Function>\n";
        }
        return 1;
    } else {
        if(Function()) {
            if(FunctionDefinitions()) {
                if(switch) {
                    std::cout << "<Function Definitions>  ::=  <Function> <Function Definitions>\n";
                }
                return 1;
            }
        }
    }
}

int Function() {
    // if(switch)
    //     std::cout << "<Function> ::= function  <Identifier>   ( <Opt Parameter List> )  <Opt Declaration List>  <Body>\n";

    if(current_word == "function") {
        //print <identifier> i think
        file >> token;
        file >> current_word; // move position
        line_number++;
        if(current_word == "(") {
            file >> token;
            file >> current_word;
            line_number++;
            if(OptParameterList()) {
                if(current_word == ")") {
                    if(switch) {
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
    // if(switch)
    //     std::cout << "<Opt Parameter List> ::=  <Parameter List>    |     <Empty>\n";
    if(ParameterList()) {
        if(switch) {
            std::cout << "<Opt Parameter List> ::=  <Parameter List>\n";
        }
        return 1;
    }
    else {
        //hit empty
        if(switch) {
            std::cout << "<Opt Parameter List> ::=  <Empty>\n";
        }
        return 1;
    }
}

// Backtracking
int ParameterList() {
    // if(switch)
    //     std::cout << "<Parameter List>  ::=  <Parameter>    |     <Parameter> , <Parameter List>\n";
        
    if (Parameter()) {
        if(switch) {
            std::cout << "<Parameter List>  ::=  <Parameter>\n";
        }
        return 1;
    } else if(current_word == ",") {
        file >> token;
        file >> current_word;
        line_number++;
        if(ParameterList()) {
            if(switch) {
                std::cout << "<Parameter List>  ::=  <Parameter> , <Parameter List>\n";
            }
            return 1;
        } else {
            errors("','", "Parameter after comma");
            return 0;
        }
    } else {
        errors("parameter or ','", "Check parameters and format");
        return 0;
    }
}

int Parameter() {
    // if(switch)
    //     std::cout << "<Parameter> ::=  <IDs >  <Qualifier>\n";
    if(IDs()) {
        if(Qualifier()) {
            if(switch) {
                std::cout << "<Parameter> ::=  <IDs >  <Qualifier>\n";
            }
            return 1;
        }
    } else { 
        errors("an identifier", "Ensure there is a valid identifier for the parameter");
        return 0;
    }
}

int Qualifier() {
    // if(switch)
    //     std::cout << "<Qualifier> ::= integer    |    boolean    |  real \n";
        
    if(current_word == "integer") {
        file >> token;
        file >> current_word; 
        if(switch) {
            std::cout << "<Qualifier> ::= integer \n";
        }
        return 1;
    } else if(current_word == "boolean") {
        file >> token;
        file >> current_word; 
        if(switch) {
            std::cout << "<Qualifier> ::= boolean \n";
        }
        return 1;
    } else if(current_word == "real") {
        file >> token;
        file >> current_word; 
        if(switch) {
            std::cout << "<Qualifier> ::=  real \n";
        }
        return 1;
    } else {
        errors("valid qualifier", "Check for type (e.g., 'integer', 'boolean', 'real')");
        return 0;
    }
}

int Body() {
    // if(switch)
    //     std::cout << "<Body>  ::=  {  < Statement List>  }\n";
        
    if(current_word == "{") {
        file >> token;
        file >> current_word; 
        line_number++;
        if(StatementList()) {
            if (current_word == "}") {
                file >> token;
                file >> current_word;
                line_number++;
                if(switch) {
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
    // if(switch)
    //     std::cout << "<Opt Declaration List> ::= <Declaration List>   |    <Empty>\n";
        
    if(DeclarationList()) {
        if(switch) {
            std::cout << "<Opt Declaration List> ::= <Declaration List>\n";
        }
        return 1;
    } else {
        //hit empty
        if(switch) {
            std::cout << "<Opt Declaration List> ::= <Empty>\n";
        }
        return 1;
    }
}

// Backtracking
int DeclarationList(){
    // if(switch)
    //     std::cout << "<Declaration List>  := <Declaration> ;     |      <Declaration> ; <Declaration List>\n";   
        
    if(Declaration()) {
        if(current_word == ";") {
            if(switch) {
                std::cout << "<Declaration List>  := <Declaration> \n"; 
            }
            return 1;
        } else if(DeclarationList()) {
            if(switch) {
                std::cout << "<Declaration List>  := <Declaration> ; <Declaration List>\n"; 
            }
            return 1;
        } else {
            // errors("';'", "Add a semicolon, ';'");
            return 0;
        }
    } //questionable    
}

int Declaration(){
    // if(switch)
    //     std::cout << "<Declaration> ::=   <Qualifier > <IDs>\n";
    if(Qualifier()) {
        if(IDs()) {
            if(switch) {
                std::cout << "<Declaration> ::=   <Qualifier > <IDs>\n";
            }
            return 1;
        }
    } else {
        errors("a valid qualifier","Ensure it starts with a valid type (e.g., 'int', 'real', 'boolean')");
        return 0;
    }
} 

// Backtracking
int IDs() {
    // if(switch)
    //     std::cout << "<IDs> ::=     <Identifier>    | <Identifier>, <IDs>\n";

    if (current_word == "identifier") {
        if(switch) {
            std::cout << "<IDs> ::=     <Identifier>\n";
        }
        return 1;
    } else if(current_word == ",") {
        file >> token;
        file >> current_word;
        line_number++;
        if(IDs()) {
            if(switch) {
                std::cout << "<IDs> ::=     <Identifier>, <IDs>\n";
            }
            return 1;
        } else {
            errors("an identifier", "Check if token is a valid identifier following ','");
            return 0;
        }
    } else {
        errors("an identifier", "Ensure the current token is valid");
        // guys am i going crazy, is this correct, my brain is confused.
        return 0;
    }
}

// Backtracking
int StatementList() {
    // if(switch)
    //     std::cout << "<Statement List> ::=   <Statement>   | <Statement> <Statement List>\n";
    if(Statement()) {
        if(switch) {
            std::cout << "<Statement List> ::=   <Statement>\n";
        }
        return 1;
    }
    else if(StatementList()) {
        if(switch) {
            std::cout << "<Statement List> ::=   <Statement> <Statement List>\n";
        }
    }
}

int Statement() {
    // if(switch)
    //     std::cout << "<Statement> ::=   <Compound>  |  <Assign>  |   <If>  |  <Return>   | <Print>   |   <Scan>   |  <While>\n";
    if(Compound()) {
        if(switch) {
            std::cout << "<Statement> ::=   <Compound>\n";
        }
        return 1;
    } else if(Assign()) {
        if(switch) {
            std::cout << "<Statement> ::=   <Assign>\n";
        }
        return 1;
    } else if(If()) {
        if(switch) {
            std::cout << "<Statement> ::=   <If>\n";
        }
        return 1;
    } else if (Return()) {
        if(switch) {
            std::cout << "<Statement> ::=   <Return>\n";
        }
        return 1;
    }  else if (Print()) {
        if(switch) {
            std::cout << "<Statement> ::=   <Print>\n";
        }
        return 1;
    } else if(Scan()) {
        if(switch) {
            std::cout << "<Statement> ::=   <Scan>\n";
        }
        return 1;
    } else if(While()) {
        if(switch) {
            std::cout << "<Statement> ::=   <While>\n";
        }
        return 1;
    } else {
        errors("statement", "Ensure a valid statement is provided"); // keep?
        return 0;
    }
}

int Compound() {
    // if(switch)
    //     std::cout << "<Compound> ::=   {  <Statement List>  }\n";
    if(current_word == "{") {
        file >> token;
        file >> current_word;
        line_number++;
        if(StatementList()) {
            if (current_word == "}") {
                file >> token;
                file >> current_word;
                line_number++;
                if(switch) {
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
    // if(switch)
    //     std::cout << "<Assign> ::=     <Identifier> = <Expression> ;\n";
    if(current_word == "identifier") {
        file >> token;
        file >> current_word;
        line_number++;
        if (current_word == "=") {
            if(Expression()) {
                if(current_word == ";") {
                    if(switch) {
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
}

// Backtracking
int If() {
//im so sorry this looks so awful but idk if theres another way to write it lmao
//maybe this is left recursion cause this feels diabolical 
    // if(switch)
    //     std::cout << "<If> ::=     if  ( <Condition>  ) <Statement>   fi  |   if  ( <Condition>  ) <Statement>   else  <Statement>  fi\n";
    if(current_word == "if") {
        line_number++;
        file >> token;
        file >> current_word;
        if(current_word == "(") {
            line_number++;
            file >> token;
            file >> current_word;
            if(Condition()) {
                if(current_word == ")") {
                    line_number++;
                    file >> token;
                    file >> current_word;
                    if(Statement()) {
                        if(current_word == "fi") {
                            line_number++;
                            file >> token;
                            file >> current_word;
                            if(switch) {
                                std::cout << "<If> ::=     if  ( <Condition>  ) <Statement>   fi\n";
                            }
                            return 1;
                        } else if (current_word == "else") {
                            line_number++;
                            file >> token;
                            file >> current_word;
                            if(Statement()) {
                                if(current_word == "fi") {
                                    line_number++;
                                    file >> token;
                                    file >> current_word;
                                    if(switch) {
                                        std::cout << "<If> ::=     if  ( <Condition>  ) <Statement>   else  <Statement>  fi\n";
                                    }
                                    return 1;
                                } else {
                                    errors("'fi'", "Ensure the block is closed with 'fi'");
                                    return 0;
                                }
                            } else {
                                errors("a valid statement", "Ensure there is a valid statement after 'else'");
                                return 0;
                            }
                        } else {
                            errors("'fi' or 'else'", "Ensure there is a valid statement after the condition");
                            return 0;
                        }
                    } else {
                        errors("a valid statement", "Ensure there is a valid statement after 'if' condition");
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
        return 0; // means it is not an if statement
    }
}

// Backtracking
int Return() {
    // if(switch)
    //     std::cout << "<Return> ::=  return ; |  return <Expression> ;\n";
    if(current_word == "return") {
        line_number++;
        file >> token;
        file >> current_word;
        if(current_word == ";") {
            line_number++;
            file >> token;
            file >> current_word;
            if(switch) {
                std::cout << "<Return> ::=  return ;\n";
            }
            return 1;
        } else if(Expression()) {
            if(current_word == ";") {
                line_number++;
                file >> token;
                file >> current_word;
                if(switch) {
                    std::cout << "<Return> ::=  return <Expression> ;\n";
                }
                return 1;
            } else {
                errors("';'", "Add a semicolon");
                return 0;
            }
        } else {
            errors("valid expression", "Check the expression for missing parts");
            return 0;
        }
    } else {
        return 0;
    }
}

int Print() {
    // if(switch)
    //     std::cout << "<Print> ::=     put ( <Expression>);\n";
    if (current_word == "put") {
        line_number++;
        file >> token;
        file >> current_word;
        if(current_word == "(") {
            line_number++;
            file >> token;
            file >> current_word;
            if(Expression()) {
                if(current_word == ")") {
                    line_number++;
                    file >> token;
                    file >> current_word;
                    if(current_word == ";") {
                        line_number++;
                        file >> token;
                        file >> current_word; 
                        if(switch) {
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
    // if(switch)
    //     std::cout << "<Scan> ::=    get ( <IDs> );\n";
    if(current_word == "get") {
        line_number++;
        file >> token;
        file >> current_word;
        if(current_word == "(") {
            line_number++;
            file >> token;
            file >> current_word;
            if(IDs()) {
                if(current_word == ")") {
                    line_number++;
                    file >> token;
                    file >> current_word;
                    if(current_word == ";") {
                        if(switch) {
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
    // if(switch)
    //     std::cout << "<While> ::=  while ( <Condition>  )  <Statement>\n";
    if(current_word == "while") {
        line_number++;
        file >> token;
        file >> current_word;
        if(current_word == "(") {
            line_number++;
            file >> token;
            file >> current_word;
            if(Condition()) {
                if(current_word == ")") {
                    line_number++;
                    file >> token;
                    file >> current_word;
                    if(Statement()) {
                        if(switch) {
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
    // if(switch)
    //     std::cout << "<Condition> ::=     <Expression>  <Relop>   <Expression>\n";
        
    if(Expression()) {
        if(Relop()) {
            if(Expression()) {
                if(switch) {
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
    if(switch)
        std::cout << "<Relop> ::=        ==   |   !=    |   >     |   <    |  <=   |    =>\n";
        
    if (current_word == "==") {
        line_number++;
        file >> token;
        file >> current_word;
        if(switch) {
            std::cout << "<Relop> ::=        ==\n";
        }
        return 1;
    } else if (current_word == "!=") {
        line_number++;
        file >> token;
        file >> current_word;
        if(switch) {
            std::cout << "<Relop> ::=        !=\n";
        }
        return 1;
    } else if (current_word == ">") {
        line_number++;
        file >> token;
        file >> current_word;
        if(switch) {
            std::cout << "<Relop> ::=        >\n";
        }
        return 1;
    } else if (current_word == "<") {
        line_number++;
        file >> token;
        file >> current_word;
        if(switch) {
            std::cout << "<Relop> ::=        <\n";
        }
        return 1;
    } else if (current_word == "<=") {
        line_number++;
        file >> token;
        file >> current_word;
        if(switch) {
            std::cout << "<Relop> ::=        <=\n";
        }
        return 1;
    } else if (current_word == "=>") {
        line_number++;
        file >> token;
        file >> current_word;
        if(switch) {
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
    // if(switch)
    //     std::cout << "<Expression>  ::=    <Expression> + <Term>    | <Expression>  - <Term>    |    <Term>\n";
    if(Term()) {
        if(ExpressionPrime()){
            if(switch) {
                std::cout << "<Expression>  ::=    <Expression> + <Term>    | <Expression>  - <Term>    |    <Term>\n";
            } // shall we add more statements to accompany the other statements? (+, -, and just the term) (im just adding comments to keep track of stuff)
            return 1;
        }
    } 
    return 0;
    
}

int ExpressionPrime() {
    if(current_word == "+" || current_word == "-") {
        line_number++;
        file >> token;
        file >> current_word;
        if(Term()) {
            if(ExpressionPrime()) {
                return 1;
            } else {
                errors("prime expression","Ensure there is a valid Prime Expression");
                return 0;
            }
        } else {
            errors("term","Ensure there is a valid Term used");
            return 0;
        }
    } else if(Term()) {
        return 1;
    }
    return 1;

}


// left recursion
int Term() {
    if(switch)
        std::cout << "<Term>    ::=      <Term>  *  <Factor>     |   <Term>  /  <Factor>     |     <Factor>\n";
} // shall we add more statements to accompany the other statements? (im just adding comments to keep track of stuff)


int Factor() {
    // if(switch)
    //     std::cout << "<Factor> ::=      -  <Primary>    |    <Primary>\n";
        
    if(current_word == "-") {
        line_number++;
        file >> token;
        file >> current_word;
        if(Primary()) {
            if(switch) {
                std::cout << "<Factor> ::=      -  <Primary>\n";
            }
            return 1;
        } else {
            errors("a valid primary value", "Ensure there is a valid value following the '-'");
            // return 0;
        }
    } else if(Primary()) {
        if(switch) {
                std::cout << "<Factor> ::=      <Primary>\n";
            }
        return 1;
    } else {
        errors("a valid primary value", "Ensure it starts with a valid primary value");
        return 0;
    }
}

int Primary() {
    if(switch)
        std::cout << "<Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   |  <Real>  |   true   |  false\n";

} // shall we add more statements to accompansy the other statements? (im just adding comments to keep track of stuff)
