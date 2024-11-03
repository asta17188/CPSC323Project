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
    if(OptFunctionDefinitions()) {
        if (current_word == "@") {
            if(OptFunctionDefinitions()) {
                if(StatementList()) {
                    if (current_word == "@") {
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

    if(FunctionDefinitions()) {
        return 1;
    } else {
        // hit empty 
        return 1;
    }
}

int FunctionDefinitions() {

    if(Function()) {
        return 1;
    } else {
        if(Function()) {
            if(FunctionDefinitions()) {
                return 1;
            }
        }
    }
}

int Function() {
    
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

    if(ParameterList()) {
        return 1;
    }
    else {
        //hit empty
        return 1;
    }
}

int ParameterList() {

    if (Parameter()) {
        return 1;
    } else if(current_word == ",") {
        file >> token;
        file >> current_word;
        line_number++;
        if(ParameterList()) {
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
    if(IDs()) {
        if(Qualifier()) {
            return 1;
        }
    } else { 
        errors("an identifier", "Ensure there is a valid identifier for the parameter");
        return 0;
    }
}

int Qualifier() {

    if(current_word == "integer") {
        file >> token;
        file >> current_word; 
        return 1;
    } else if(current_word == "boolean") {
        file >> token;
        file >> current_word; 
        return 1;
    } else if(current_word == "real") {
        file >> token;
        file >> current_word; 
        return 1;
    } else {
        errors("valid qualifier", "Check for type (e.g., 'integer', 'boolean', 'real')");
        return 0;
    }
}

int Body() {

    if(current_word == "{") {
        file >> token;
        file >> current_word; 
        line_number++;
        if(StatementList()) {
            if (current_word == "}") {
                file >> token;
                file >> current_word;
                line_number++;
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

    if(DeclarationList()) {
        return 1;
    } else {
        //hit empty
        return 1;
    }
}

int DeclarationList(){
    
    if(Declaration()) {
        if(current_word == ";") {
            return 1;
        } else if(DeclarationList()) {
            return 1;
        } else {
            // errors("';'", "Add a semicolon, ';'");
            return 0;
        }
    } //questionable    
}

int Declaration(){
    if(Qualifier()) {
        if(IDs()) {
            return 1;
        }
    } else {
        errors("a valid qualifier","Ensure it starts with a valid type (e.g., 'int', 'real', 'boolean')");
        return 0;
    }
} 

int IDs() {
    if (current_word == "identifier") {
        return 1;
    } else if(current_word == ",") {
        file >> token;
        file >> current_word;
        line_number++;
        if(IDs()) {
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

int StatementList() {

    if(Statement()) {
        return 1;
    }
    else if(StatementList());

}

int Statement() {
    if(Compound()) {
        return 1;
    } else if(Assign()) {
        return 1;
    } else if(If()) {
        return 1;
    } else if (Return()) {
        return 1;
    }  else if (Print()) {
        return 1;
    } else if(Scan()) {
        return 1;
    } else if(While()) {
        return 1;
    } else {
        return 0;
    }
}

int Compound() {
    if(current_word == "{") {
        file >> token;
        file >> current_word; 
        line_number++;
        if(StatementList()) {
            if (current_word == "}") {
                file >> token;
                file >> current_word;
                line_number++;
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

    if(current_word == "identifier") {
        file >> token;
        file >> current_word;
        line_number++;
        if (current_word == "=") {
            if(Expression()) {
                if(current_word == ";") {
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

int If() {
//im so sorry this looks so awful but idk if theres another way to write it lmao
//maybe this is left recursion cause this feels diabolical 
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

int Return() {
    if(current_word == "return") {
        line_number++;
        file >> token;
        file >> current_word;
        if(current_word == ";") {
            line_number++;
            file >> token;
            file >> current_word;
            return 1;
        } else if(Expression()) {
            if(current_word == ";") {
                line_number++;
                file >> token;
                file >> current_word;
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

    if(Expression()) {
        if(Relop()) {
            if(Expression()) {
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

    if(current_word == "==" || 
       current_word =="!=" || 
       current_word ==">" || 
       current_word =="<" || 
       current_word =="<=" || 
       current_word =="=>") {
        line_number++;
        file >> token;
        file >> current_word;
        return 1;
    } else {
        errors("a valid relational operator", "Ensure there is a valid relational operator (e.g., '==', '!=', '<', '>', '<=', '>=')");
        return 0;
    }
}

//left recursion 
int Expression() {

}

int Term() {

}

int Factor() {

    if(current_word == "-") {
        line_number++;
        file >> token;
        file >> current_word;
        if(Primary()) {
            return 1;
        } else {
            errors("a valid primary value", "Ensure there is a valid value following the '-'");
            // return 0;
        }
    } else if(Primary()) {
        return 1;
    } else {
        errors("a valid primary value", "Ensure it starts with a valid primary value");
        return 0;
    }
}

int Primary() {

}