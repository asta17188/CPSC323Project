#include <iostream>
#include <fstream>
#include <string>

//main function will read word by word
std::string current_word;
std::string token;
int line_number;
    //ifstream has special functions and you dont need 
    //to open and close the file but you still need to make sure the file opened correctly, we can add that later
std::ifstream file("output.txt"); 

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
                    }
                    else 
                        return 0;
                }
                else 
                    return 0;
            }
            else 
                return 0;
        }
        else 
            return 0;
    }
    else 
        return 0;

}

int OptFunctionDefinitions() {

    if(FunctionDefinitions()) {
        return 1;
    }
    else 
        // hit empty 
        return 1;
}

int FunctionDefinitions() {

    if(Function()) {
        return 1;
    }
    else 
        if(Function()) {
            if(FunctionDefinitions()) {
                return 1;
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
                }
            }
            else 
                return 0;
        }
        else 
            return 0;
    }
    else 
        return 0;

}



int OptParameterList(){

    if(ParameterList()) {
        return 1;
    }
    else 
        //hit empty
        return 1;
}

int ParameterList() {

    if (Parameter()) {
        return 1;
    } 
    else if(current_word == ",") {
        file >> token;
        file >> current_word;
        line_number++;
        if(ParameterList()) {
            return 1;
        }
        else return 0;
    }
    else return 0;
    
}

int Parameter() {
    if(IDs()) {
        if(Qualifier()) {
            return 1;
        }
    }
    else 
        return 0;
}

int Qualifier() {

    if(current_word == "integer") {
        file >> token;
        file >> current_word; 
        return 1;
    }
    else if(current_word == "boolean"){
        file >> token;
        file >> current_word; 
        return 1;
    } 
    else if(current_word == "real") {
        file >> token;
        file >> current_word; 
        return 1;
    }
    else 
        return 0;
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
            }
            else 
                return 0;
        }
        else 
            return 0;
    }
    else 
        return 0;
}

int OptDeclarationList(){

    if(DeclarationList()) {
        return 1;
    }
    else 
        //hit empty
        return 1;

}

int DeclarationList(){
    
    if(Declaration()) {
        if(current_word == ";") {
            return 1;
        }
        else if(DeclarationList()) {
            return 1;
        }
        else return 0;
    } //questionable    
}

int Declaration(){
    if(Qualifier()) {
        if(IDs()) {
            return 1;
        }
    }
    else 
        return 0;
}

int IDs() {
    if (current_word == "identifier") {
        return 1;
    } 
    else if(current_word == ",") {
        file >> token;
        file >> current_word;
        line_number++;
        if(IDs()) {
            return 1;
        }
        else return 0;
    }
    else return 0;
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
    }
    else if(Assign()) {
        return 1;
    }
    else if(If()) {
        return 1;
    }
    else if (Return()) {
        return 1;
    } 
    else if (Print()) {
        return 1;
    }
    else if(Scan()) {
        return 1;
    }
    else if(While()) {
        return 1;
    }
    else 
        return 0;
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
            }
            else 
                return 0;
        }
        else 
            return 0;
    }
    else 
        return 0;
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
                }
            }
            else 
                return 0;
        }
        else 
            return 0;
    }
    else 
        return 0;
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
                        } 
                        else if (current_word == "else") {
                            line_number++;
                            file >> token;
                            file >> current_word;
                            if(Statement()) {
                                if(current_word == "fi") {
                                    line_number++;
                                    file >> token;
                                    file >> current_word;
                                    return 1;
                                }
                                else return 0;
                            }
                            else return 0;
                        }
                        else return 0;
                    }
                    else return 0;
                }
                else return 0;
            }
            else return 0;
        }
        else return 0;
    }
    else return 0;
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
            }
            else return 0;
        }
        else return 0;
    }
    else return 0;
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
                    }
                    else return 0;
                }
                else return 0;
            }
            else return 0;
        } 
        else return 0;
    }
    else return 0;
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
                    }
                    else return 0;
                }
                else return 0;
            } 
            else return 0;
        }
        else return 0;
    }
    else return 0;
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
                    }
                    else return 0;
                }
                else return 0;
            } 
            else return 0;
        }
        else return 0;
    }
    else return 0;
}

int Condition() {

    if(Expression()) {
        if(Relop()) {
            if(Expression()) {
                return 1;
            }
            else return 0;
        }
        else return 0;
    }
    else return 0;
}

int Relop() {

    if(current_word == "==" | 
       current_word =="!=" | 
       current_word ==">" | 
       current_word =="<" | 
       current_word =="<=" | 
       current_word =="=>") {
        line_number++;
        file >> token;
        file >> current_word;
        return 1;
       }
    return 0;
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
        }
    } 
    else if(Primary()) {
        return 1;
    }
    else return 0;
}

int Primary() {

}