// FOR EASE OF USE:
// ctrl + F -> Find, type in rule or function you want to see
// R3) Function Definitions
// R6) Parameter List
// R11) Declaration List
// R13) IDs
// R14) Statement List
// R18) If
// R19) Return

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

//main function will read word by word
std::string current_word;
std::string token;
int line_number;
std::ifstream file("output.txt"); 
bool switch{true};

// Rule 3
// <FD> → <F> <FD’>
int FunctionDefinitions() {
    if(switch)
        std::cout << "<Function Definitions>  ::= <Function> | <Function> <Function Definitions>\n";
    if(Function()) {
        if(FunctionDefinitionPrime()) {
            return 1;
        }
    } 
    return 0;
}

// <FD’> → <FD> | ε
int FunctionDefinitionsPrime() {
    if(FunctionDefinitions()) {
        return 1;
    } else {    // hit empty
        return 1;
    }
}

// Rule 6
// <PL> → <P><PL’>
int ParameterList() {
    if(switch)
        std::cout << "<Parameter List>  ::=  <Parameter>    |     <Parameter> , <Parameter List>\n";
        
    if (Parameter()) {
        if(ParameterListPrime()) {
            return 1;
        }
        return 0;
    }
}

// <PL’> → ,<PL> | ε
int ParameterListPrime() {
    if(current_word == ",") {   // ,
        file >> token;
        file >> current_word;
        line_number++;
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
}

// unchanged, just here for the ParameterList() Function so errors don't pop up
int Parameter() {
    if(switch)
        std::cout << "<Parameter> ::=  <IDs >  <Qualifier>\n";
    if(IDs()) {
        if(Qualifier()) {
            return 1;
        }
    } else { 
        errors("an identifier", "Ensure there is a valid identifier for the parameter");
        return 0;
    }
}


// Rule 11
// <DL> → <D>; <DL’>
int DeclarationList(){
    if(switch)
        std::cout << "<Declaration List>  := <Declaration> ;     |      <Declaration> ; <Declaration List>\n";   
        
    if(Declaration()) {     // <D>
        if(current_word == ";") {   // ;
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

// unchanged, just here for the DeclarationList() Function so errors don't pop up
int Declaration(){
    if(switch)
        std::cout << "<Declaration> ::=   <Qualifier > <IDs>\n";
    if(Qualifier()) {
        if(IDs()) {
            return 1;
        }
    } else {
        errors("a valid qualifier","Ensure it starts with a valid type (e.g., 'int', 'real', 'boolean')");
        return 0;
    }
} 

// Rule 13
// <ID> → <I><ID’>
int IDs() {
    if(switch)
        std::cout << "<IDs> ::=     <Identifier>    | <Identifier>, <IDs>\n";

    if (current_word == "identifier") { // <I>
        if(IDsPrime()) // <ID'>
            return 1;
    }  else {
        errors("an identifier", "Ensure the current token is valid");
        return 0;
    }
}

// <ID’> → ,<ID> | ε
int IDsPrime() {
    if(current_word == ",") {   // ,
        file >> token;
        file >> current_word;
        line_number++;
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

// Rule 14
// <SL> → <S> <SL’>
int StatementList() {
    if(switch)
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
        return 1;
    } else {    // hit empty
        return 1;
    }
}

// unchanged, just here for the StatementList() Function so errors don't pop up
int Statement() {
    if(switch)
        std::cout << "<Statement> ::=   <Compound>  |  <Assign>  |   <If>  |  <Return>   | <Print>   |   <Scan>   |  <While>\n";
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

// Rule 18
// <I> → <i (<C>)> <S> <f’>
int If() {
    if(switch)
        std::cout << "<If> ::=     if  ( <Condition>  ) <Statement>   fi  |   if  ( <Condition>  ) <Statement>   else  <Statement>  fi\n";
    if(current_word == "if") {              // if 1
        line_number++;
        file >> token;
        file >> current_word;
        if(current_word == "(") {           // ( 2
            line_number++;
            file >> token;
            file >> current_word;
            if(Condition()) {               // <C> 3
                if(current_word == ")") {   // ) 4
                    line_number++;
                    file >> token;
                    file >> current_word;
                    if(Statement()) {       // <S> 5
                        if (fiPrime())
                            return 1;
                    } else { // 5
                        errors("a valid statement", "Ensure there is a valid statement after 'if' condition");
                        return 0;
                    }
                } else { // 4
                    errors("')'", "Ensure the condition is closed");
                    return 0;
                }
            } else {    // 3
                errors("a valid condition", "Ensure the statement contains a valid condition");
                return 0;
            }
        } else {    // 2
            errors("'('", "Ensure the condition contains an opening parenthesis");
            return 0;
        } 
    }  // 1
    return 0; // means it is not an if statement
}

// <f’> → <f> | <e> <S> <f>
int fiPrime() {
    if(current_word == "fi") {  // <f>
        line_number++;
        file >> token;
        file >> current_word;
        return 1;
    } else if (current_word == "else") {    //  <e> 6
        line_number++;
        file >> token;
        file >> current_word;
        if(Statement()) {       // 7
            if(current_word == "fi") {  // <f> 8
                line_number++;
                file >> token;
                file >> current_word;
                return 1;
            } else {    // 8
                errors("'fi'", "Ensure the block is closed with 'fi'");
                return 0;
            }
        } else {    // 7
            errors("a valid statement", "Ensure there is a valid statement after 'else'");
            return 0;
        } 
    } else { // 6
        errors("'fi' or 'else'", "Ensure there is a valid statement after the condition");
        return 0;
    }  
}

// Rule 19
// <R> → <r>; <E’>
int Return() {
    if(switch)
        std::cout << "<Return> ::=  return ; |  return <Expression> ;\n";
    if(current_word == "return") {  // <r>
        line_number++;
        file >> token;
        file >> current_word;
        if(current_word == ";") {   // ,
            line_number++;
            file >> token;
            file >> current_word;
            if(ExpressionPrime()) { // <E'>
                return 1;
            } else {
                errors("';'", "Add a semicolon");
                return 0;
            }
        } else {
            errors("valid expression", "Check the expression for missing parts");
            return 0;
        }
    } 
    return 0;
}
// (!!!) NOTE: Left factorization for backtracking doesn't match left factorization for left recursion

// Left Recursion! Unsure if this is correct. only here to prevent error popup
int Expression() {
    if(switch)
        std::cout << "<Expression>  ::=    <Expression> + <Term>    | <Expression>  - <Term>    |    <Term>\n";
    if(Term()) {
        if(ExpressionPrime()) {
            return 1;
        }
        // Error Handling here?
    }
    return 0;
}

// Left Recursion! Unsure if this is correct. only here to prevent error popup
int ExpressionPrime() {
    if(current_word == "+" || current_word == "-") {
        return 1;
    } else { //  followed current formatting, unsure if this is correct logic. because its either if (+/-) 
             //  or else, which could be anything. It could be a random char under the else statement and it'd be true
        return 1;   // hit empty
    }
}

// Left Recursion! Unsure if this is correct. only here to prevent error popup
int Term() {
    if(switch)
        std::cout << "<Term>    ::=      <Term>  *  <Factor>     |   <Term>  /  <Factor>     |     <Factor>\n";
    
    if(Factor()){
        if(TermPrime()) {
            return 1;
        }
    } else {
        // Error Handling Here?
    }
    return 1;   // temp
}

// Left Recursion! Unsure if this is correct. only here to prevent error popup
int TermPrime() {
    if(current_word == "*" || current_word == "/") {
        if(Factor()) {
            if (TermPrime()) {
                return 1;
            }  
        }
    } else {    // hit empty
        return 1;
    }
}

// only here to prevent error popup
int Factor() {
    if(switch)
        std::cout << "<Factor> ::=      -  <Primary>    |    <Primary>\n";
        
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

