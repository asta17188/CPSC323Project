// Project 2: Syntax Analyzer
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iomanip>
//#include "syntax.cpp"

// Declaring Enums for Token Types
enum Token
{
    OPERATOR,   // 0
    SEPARATOR,  // 1
    INTEGER,    // 2
    REAL,       // 3
    KEYWORD,    // 4
    ID,         // 5    
    UNKNOWN,    // 6
    DEFAULT     // 7

};

/*
    NOTE: When testing program, make sure to delete the output.txt file
          and create a new one. Otherwise, old (and possibly failed) attempts
          will still be on the output.txt when running the program again
          and create erroneous results.
*/

// Declaring Lists of Operators, Separators, Keywords, and Unknown Chars
const int ARRAY_SIZE = 11;
char unknownCharArray[ARRAY_SIZE] = {'!', '#', '$', '%', '&', '`', '?', '^', '_', '~'};

std::set<std::string> keywords{"if", "switcher", "else","double","fi","new","return","asm",
                            "get","auto","while","extern","put","typedef","break","integer",
                            "break","boolean","operator","real","template","goto","enum",
                            "class", "this","function","case","string","throw","const","float",
                            "try","char","void","for","signed","register","unsigned","friend",
                            "catch","union","public","continue","protected","virtual","private",
                            "default","inline","sizeof","delete","int","static","volatile","do",
                            "long","struct","char16_t","char32_t","explicit","nullptr","static_cast",
                            "overload","short", "true", "false"};

std::set<std::string> operators{"=", "==", ">", "<", "<=",">=","!=","+","-","*","/"};


std::set<std::string> separators{"{","}","(",")",R"(")", ":"," ",";","::","\n","//","[","]","|",",", "@"};

// Function Prototypes
void findOperator(Token& , std::fstream& , std::string&);
void findOperator(Token& , std::fstream& , char&);
void findSeparator(Token& , std::fstream& , std::string&);
void findSeparator(Token& , std::fstream& , char&);
void findKeyword(Token&, std::fstream&, std::string&);
bool IDs(const std::string&);
bool integers(const std::string&);
bool reals(const std::string&);
void findUnknownChar(char&, std::string&, Token&, std::fstream&); 
template <typename T> void logLexeme(Token, std::fstream&, T&);          
void logLexeme(Token, std::fstream&, std::string&);  
void real_output();
void ids_output();
void int_output();

// Syntax Analyzer Functions
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

//std::fstream dstFile;
std::string add;
std::ifstream file;
std::string current_word;
std::string token;
int line_number;

bool switcher{true};

void errors(const std::string& expected, const std::string suggested) {
    // std::cerr << "Error on line " << line_number << ": Unexpected token " << current_word << ".\n";
    // std::cerr << "Expected " << expected << ".\n";
    // std::cerr << "Suggested: " << suggested << ".\n"; 
    // std::cerr is similar to cout, but meant for errors. there is no buffer so it will print immediately!
    //exit(1);
}

void printToken(std::string tok, std::string lex) {
    std::cout << "Token: " << tok << "     Lexeme: " << lex << "\n";
    
}

void moveFile() {
    file >> token;
    file >> token;
    file >> current_word;
    file >> current_word;
    line_number++;
}

// Holds name of temp file, which will hold results from lexer
const std::string tempName = "temp.txt";

std::fstream tempFile(tempName, std::ios::in | std::ios::out);


int main(int argc, char const *argv[])
{
    // Declare Local Variables
    std::ifstream srcFile;      // "Source" File
    char c;                     // Character in Current Loop
    Token type{DEFAULT};        // Type of Token
    std::string temp{""};       // Holds a string of int/real/id/keyword/multi-char unknown
    std::string fileName;
    bool isComment{false};      // True if detects beginning of comment
    bool isEndComment{false};   // True if detects end of comment

    // i.e. "testcase1.txt", "testcase2.txt", "testcase3.txt"
    std::cout << "Enter Test File Name: ";
    std::cin >> fileName;
        
    // Open Test File (src)
    srcFile.open(fileName);        
    if (!srcFile)
        throw std::invalid_argument("ERROR: FILE CANNOT BE FOUND, ENSURE YOU INPUT THE CORRECT FILE NAME");

    // i.e. "output.txt"
    std::cout << "Enter Output File Name: ";
    std:: cin >> fileName;

    // Declare "Destination" File
    std::fstream dstFile(fileName, std::ios::in | std::ios::out);     
    if (!dstFile)
        throw std::invalid_argument("ERROR: FILE CANNOT BE FOUND, ENSURE YOU INPUT THE CORRECT FILE NAME");
    dstFile <<"";
    // Declare Temp File
    //std::fstream tempFile("tempName", std::ios::in | std::ios::out);
    //if (!tempFile)
        //throw std::invalid_argument("ERROR: FILE CANNOT BE FOUND, ENSURE TEMP FILE IS IN SAME FOLDER AS .EXE");
    //tempFile <<"";
    // Print Transition Tables to Terminal
    real_output();
    ids_output();
    int_output();

    // While Loop: Run Through Test File Character By Character (!eof)
    while (!srcFile.eof())
    {
        c = srcFile.get();
        //std::cout << c << ' ';      // TEMP: This line is just for testing purposes, can be removed later

        // CODE START: Ignores comments (4 steps)
        if(isEndComment == true)   //  (4) Skips over ], the end of comment. Afterwards turns isEndComment back to false
        {
            isEndComment = false;
            continue;
        }
        if (srcFile.peek() == ']' && c == '*')  // (3) Detects the end of comments, skips *, marks end of comment true to skip ] in next iteration of loop. isComment is turned back to false
        {
            isComment = false;
            isEndComment = true;
            continue;
        }
        if(srcFile.peek() == '*' && c == '[')   // (1) Detects the start of comment, sets isComment true to skip following iterations until end of comment
        {
            isComment = true;
            continue;
        }
        if (isComment)  // (2) if isComment is true, then it skips following iterations until isComment is false
            continue;
        // CODE END

        /*
            If delimiter is met, it signals the end of the string.
            Afterwards, checks for int/real/id/keyword/multi-char unknown
        */
        if((c == ' ' || c == ';' || c == '}' || c == ')' || c == ']' || c == ',' || c == '*' || c == '(' || c == '\n') && temp != "")
        {
            // Integer Function
            if(integers(temp))
            {
                type = INTEGER;
                logLexeme(type, tempFile, temp);
            }

            // Real Function
            if(reals(temp))
            {
                type = REAL;
                logLexeme(type, tempFile, temp);
            }

            // Keyword Function
            findKeyword(type, tempFile, temp);
            
            // ID Function
            if(IDs(temp))
            {
                type = ID;
                logLexeme(type, tempFile, temp);
            }
            // Unknown Function
            if(type == DEFAULT)
            {
                type = UNKNOWN;
                logLexeme(type, tempFile, temp);
            }

            
            findSeparator(type, tempFile, c);
        }
        
        /*  OPERATOR FUNCTIONS
            If statements checks for multi-char operators (i.e. ==, <=, etc.)
            If Token type still DEFAULT, checks for single char operators
        */
        if (srcFile.peek() == '=' && c == '=')
        {
            type = OPERATOR;
            logLexeme(type, tempFile, "==");
        }
         if (srcFile.peek() == '=' && c == '>')
        {
            type = OPERATOR;
            logLexeme(type, tempFile, ">=");
        }
        if (srcFile.peek() == '=' && c == '<')
        {
            type = OPERATOR;
            logLexeme(type, tempFile, "<=");
        }
        if (srcFile.peek() == '=' && c == '!')
        {
            type = OPERATOR;
            logLexeme(type, tempFile, "!=");
        }
        if (type == DEFAULT)
            findOperator(type, tempFile, c);
        

        /*  SEPARATOR FUNCTIONS
            If statements checks for multi-char separators
            If Token type still DEFAULT, checks for single char separators
        */
        if (srcFile.peek() == ':' && c == ':')
        {
            logLexeme(SEPARATOR, tempFile, "::");
        }
        if (srcFile.peek() == '/' && c == '/')
        {
            logLexeme(SEPARATOR, tempFile, "//");
        }
        if(type == DEFAULT)     
            findSeparator(type, tempFile, c);
        
        // Unknown Character Function
        if (type == DEFAULT)
            findUnknownChar(c, temp, type, tempFile);
        

        // If Token type still DEFAULT at end of loop, 
        // adds current character to string (for multi-char lexemes)
        if (type == DEFAULT)
            temp += c;          // appends current character to string

        // Sets Token type back to DEFAULT at end of loop  
        type = DEFAULT;
    }
    // End of While Loop
    
    // Print Formatting Purposes
    std::cout << "\n\n";  

    // Print Output File Contents
    dstFile.seekg(0);
    while(getline(tempFile, temp))
        std::cout << temp << std::endl;
        file >> temp;
        
    
    moveFile();
    Rat24F();
    dstFile << add;

    // Close Test File (src)
    srcFile.close();
    // Close Output File
    dstFile.close();
    // Close Temp File
    tempFile.close();

    return 0;
}

template <typename T> void logLexeme(Token t, std::fstream& dst, T& lexeme)
{
    std::string str{lexeme};
    if (t == OPERATOR)
        dst << "TOKEN: Operator  \tLEXEME: " << lexeme << std::endl;
    else if(t == SEPARATOR)
    {
        // 10.27.24 - Modified func. to ignore spaces and newline
        if(str == "\n" || str == " ")
            return;
        else
            dst << "TOKEN: Separator\tLEXEME: " << lexeme << std::endl;
    }
            
    return;
}

void logLexeme(Token t, std::fstream& dst, std::string& s)
{
    switch (t)
    {
    case 2:
        dst << "TOKEN: Integer  \tLEXEME: " << s << std::endl;
        break;
    case 3: 
        dst << "TOKEN: Real     \tLEXEME: " << s << std::endl;
        break;
    case 4:
        dst << "TOKEN: Keyword  \tLEXEME: " << s << std::endl;
        break;
    case 5:
        dst << "TOKEN: ID       \tLEXEME: " << s << std::endl;
        break;
    default:
        dst << "TOKEN: Unknown  \tLEXEME: " << s << std::endl;
        break;
    }
        s = "";     // Resets temp string
    
    return;
}

void findOperator(Token& t, std::fstream& file, std::string& word)
{
    if(operators.find(word) != operators.end())
        t = OPERATOR;
        logLexeme(t, file, word);
    return; 
}
void findOperator(Token& t, std::fstream& file, char& ch)
{
    std::string chword(1, ch);
    if(operators.find(chword) != operators.end())
        t = OPERATOR;
        logLexeme(t, file, ch);
    return; 
}

void findSeparator(Token& t, std::fstream& file, std::string& word)
{
    if(separators.find(word) != separators.end())
        t = SEPARATOR;
        logLexeme(t, file, word);
    return; 
}

void findSeparator(Token& t, std::fstream& file, char& ch)
{
    std::string chword(1, ch);
    if(separators.find(chword) != separators.end())
        t = SEPARATOR;
    logLexeme(t, file, ch);
    return; 
}

void findUnknownChar(char& c, std::string& s, Token& t, std::fstream& dst)
{
    for (size_t i = 0; i < ARRAY_SIZE; i++)
    {
        if (unknownCharArray[i] == c)
        {
            t = UNKNOWN;
            dst << "Unknown  \t" << c << std::endl;
        }
    }
    return;
}

bool IDs(const std::string& lexeme) {
    int state = 1; // starting state
    // int accepting_state = 1; // accepting state

    if(lexeme.empty()) {
        return false;
    }

    for (size_t i = 0; i < lexeme.size(); ++i) {
        char character = lexeme[i];
        switch (state) // switcheres
        {
            case 1:
                if (isalpha(character)) {
                    state = 2;
                } else {
                    state = 13; // rejected
                }
                break;
            case 2:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 4;
                } else {
                    state = 13;
                }
                break;
            case 3:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)){
                    state = 5;
                }
                break;
            case 4:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 6;
                } else {
                    state = 13;
                }
                break;
            case 5:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 7;
                } else {
                    state = 13;
                }
                break;
            case 6:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 8;
                } else {
                    state = 13;
                }
                break;
            case 7:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 9;
                } else {
                    state = 13;
                }
                break;
            case 8:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 10;
                } else {
                    state = 13;
                }
                break;
            case 9:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 11;
                } else {
                    state = 13;
                }
                break;
            case 10:
                if(isalpha(character)) {
                    state = 10; // stay in laste letter state check
                } else if (isdigit(character)) {
                    state = 11; // go to digits state
                }
                break;
            case 11:
                if(isalpha(character)) {
                    state = 10; // go to letter state
                } else if (isdigit(character)) {
                    state = 11; // stay in digits state
                } else {
                    state = 13; // reject
                }
                break;
            case 13:
                return false;

            // case 0: // starting
            //     if (isalpha(character)) {
            //         state = 1; // state 1 if first character is a letter
            //     } else {
            //         return false; // means first char is not a letter
            //     }
            //     break;
            // case 1:
            //     if (isalnum(character)) { // if letter & or digit
            //         state = 1; // stay in this state
            //     } else {
            //         return false;
            //     }
            //     break;

        }
        // if (!isalpha(lexeme[lexeme.size() - 1])) {
        //     return false;
        // }
    }
    // return true;
    return (state == 10);
}

bool integers(const std::string& lexeme) {
    if(lexeme.empty()) {
        return false;
    }

    int state = 0;
    int accepting_state = 1;

    for (char character : lexeme) {
        switch (state)
        {
        case 0:
            if (isdigit(character)) {
                state = 1; //goes to integer state
            } else {
                return false;
            }
            break;
        case 1:
            if (isdigit(character)) {
                state = 1; //stays in this state
            } else {
                return false; 
            }
            break;
        }
    }
    return (state==accepting_state);
}

bool reals(const std::string& lexeme) {
    if(lexeme.empty()) {
        return false;
    }

    int state = 0;
    int accepting_state = 3;

    for (char character : lexeme) {
        switch (state)
        {
        case 0: //starting
            if (isdigit(character)) {
                state = 1;
            } else {
                return false;
            }
            break;
        case 1: // integer state
            if (isdigit(character)) {
                state = 1; // stays in integer state
            } else if (character == '.') {
                state = 2; //goes to decimal state
            } else {
                return false;
            }
            break;
        case 2:
            if (isdigit(character)) {
                state = 3;
            } else {
                return false;
            }
            break;
        case 3:
            if (isdigit(character)) {
                state = 3;
            } else {
                return false;
            }
            break;
        }
    }
    return (state == accepting_state);
}

void findKeyword(Token& t, std::fstream& file, std::string& word)
{
    if(keywords.find(word) != keywords.end())
    {
        t = KEYWORD;
        logLexeme(t, file, word);
    }
    return; 
}

void real_output() {
    std::cout << "REALS TRANSITION TABLE\n";
    std::cout << " " << std::setw(5) << "|" << std::setw(5) << "d" << std::setw(10) << ".\n";
    std::cout << "-----|--------------------\n";
    std::cout << "1" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "5\n";
    std::cout << "2" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "3\n";
    std::cout << "3" << std::setw(5) << "|" << std::setw(5) << "4" << std::setw(10) << "5\n";
    std::cout << "4" << std::setw(5) << "|" << std::setw(5) << "4" << std::setw(10) << "5\n";
    std::cout << "5" << std::setw(5) << "|" << std::endl;
}

void ids_output() {
    std::cout << "IDs TRANSITION TABLE\n";
    std::cout << " " << std::setw(5) << "|" << std::setw(5) << "l" << std::setw(10) << "d\n";
    std::cout << "-----|--------------------\n";
    std::cout << "1" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "13\n";
    std::cout << "2" << std::setw(5) << "|" << std::setw(5) << "3" << std::setw(10) << "13\n";
    std::cout << "3" << std::setw(5) << "|" << std::setw(5) << "4" << std::setw(10) << "13\n";
    std::cout << "4" << std::setw(5) << "|" << std::setw(5) << "5" << std::setw(10) << "6\n";
    std::cout << "5" << std::setw(5) << "|" << std::setw(5) << "7" << std::setw(10) << "13\n";
    std::cout << "6" << std::setw(5) << "|" << std::setw(5) << "13" << std::setw(10) << "8\n";
    std::cout << "7" << std::setw(5) << "|" << std::setw(5) << "9" << std::setw(10) << "13\n";
    std::cout << "8" << std::setw(5) << "|" << std::setw(5) << "10" << std::setw(10) << "12\n";
    std::cout << "9" << std::setw(5) << "|" << std::setw(5) << "9" << std::setw(10) << "6\n";
    std::cout << "10" << std::setw(4) << "|" << std::setw(5) << "11" << std::setw(10) << "13\n";
    std::cout << "11" << std::setw(4) << "|" << std::setw(5) << "7" << std::setw(10) << "6\n";
    std::cout << "12" << std::setw(4) << "|" << std::setw(5) << "10" << std::setw(10) << "6\n";
    std::cout << "13" << std::setw(4) << "|" << std::endl;
}

void int_output() {
    std::cout << "INTEGERS TRANSITION TABLE\n";
    std::cout << " " << std::setw(5) << "|" << std::setw(5) << "d" << std::setw(10) << "\n";
    std::cout << "-----|----------\n";
    std::cout << "1" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "\n";
    std::cout << "2" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "\n\n";
}





int Rat24F() {
    //print rule
    // if (switcher == true) {
    //     std::cout << "<Rat24F>  ::=   <Opt Function Definitions>   @  <Opt Declaration List>  <Statement List>  @\n";
    // }
    add += "sssssssssss";
    if(OptFunctionDefinitions()) {
        if (current_word == "@") {
            printToken(token, current_word);
            moveFile();
            if(OptDeclarationList()) {
                if(StatementList()) {
                    if (current_word == "@") {
                        printToken(token, current_word);
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
    if(FunctionDefinitions()) {
        if(switcher) std::cout << "<FunctionDefinitionsPrime> ::= <FunctionDefinitions>";
        return 1;
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
        return 0;
    }
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
        // Error Handling Here?
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

}

