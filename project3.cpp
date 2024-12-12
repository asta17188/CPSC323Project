// Project 3: Object Code Generation
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iomanip>
#include <array>

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

// Holds name of temp file, which will hold results from lexer
const std::string tempName = "temp.txt";

std::fstream tempFile(tempName, std::ios::in | std::ios::out);

int memoryAddress{9000};
int instructionAddress{1};
std::string current_type;
std::string relationalOp;
//for get()
bool isSTDIN;
bool runElse{true};

const int TABLE_COL = 3;
const int TABLE_ROW = 1000;

// NOTE -> arrayName[row][col]
// Symbol Table Fields: Name/Lexeme, Data Type, Memory Address
std::array<std::array<std::string, TABLE_COL>, TABLE_ROW> symbolTable;
// Instruction Table Fields: Instruction Address/Number (Starts at 1), Op, Operand
std::array<std::array<std::string, TABLE_COL>, TABLE_ROW> instructionTable;

std::array<int, TABLE_ROW> stack;
int stackCount{0};

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
void findOperator(Token& , std::fstream&, std::string&);
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
int Rat24F(std::fstream&);
int OptFunctionDefinitions(std::fstream&);
int FunctionDefinitions(std::fstream&);
int FunctionDefinitionsPrime(std::fstream&);
int Function(std::fstream&);
int OptParameterList(std::fstream&);
int ParameterList(std::fstream&);
int ParameterListPrime(std::fstream&);
int Parameter(std::fstream&);
int Qualifier(std::fstream&);
int Body(std::fstream&);
int OptDeclarationList(std::fstream&);
int DeclarationList(std::fstream&);
int DeclarationListPrime(std::fstream&);
int Declaration(std::fstream&);
int IDs(std::fstream&);
int IDsPrime(std::fstream&);
int StatementList(std::fstream&);
int StatementListPrime(std::fstream&);
int Statement(std::fstream&);
int Compound(std::fstream&);
int Assign(std::fstream&);
int If(std::fstream&);
int fiPrime(std::fstream&);
int Return(std::fstream&);
int ReturnPrime(std::fstream&);
int Print(std::fstream&);
int Scan(std::fstream&);
int While(std::fstream&);
int Condition(std::fstream&);
int Relop(std::fstream&);
int Expression(std::fstream&);
int ExpressionPrime(std::fstream&);
int Term(std::fstream&);
int TermPrime(std::fstream&);
int Factor(std::fstream&);
int Primary(std::fstream&);
std::string getSymbolAddress(std::string saveID);

// Table Functions
void addSymbol(std::string, std::string);
void addInstruction(std::string, std::string);
bool checkSymbol(std::string);
void backPatch(int);
void printSymbols(std::fstream&);
void printInstruction(std::fstream&);

// Stack Functions
void pushStack(int);
int popStack();

//std::fstream dstFile;
std::string add;
std::ifstream file;
std::string current_word;
std::string token;
int line_number;

bool switcher{false};

void errors(const std::string& expected, const std::string suggested) {
    std::cerr << "Error on line " << line_number << ": Unexpected token " << current_word << ".\n";
    std::cerr << "Expected " << expected << ".\n";
    std::cerr << "Suggested: " << suggested << ".\n"; 
    // std::cerr is similar to cout, but meant for errors. there is no buffer so it will print immediately!
    //exit(1);
}


void printToken(std::string tok, std::string lex, std::fstream& dst) {
    std::cout << "Token: " << tok << "     Lexeme: " << lex << "\n";
    // dst << "Token: " << tok << "     Lexeme: " << lex << "\n";
    
}

void moveFile() {
    tempFile >> token;
    tempFile >> token;
    tempFile >> current_word;
    tempFile >> current_word;
    line_number++;
}

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


    //dstFile << "-----RESULTS OF SYNTAX ANALYZER-----\n";
    std::cout << "-----RESULTS OF SYNTAX ANALYZER-----\n";
    tempFile.seekg(0);                      // sets file pointer to beginning of tempFile
    moveFile();
    dstFile.clear();
    Rat24F(dstFile);                        // starts syntax analyzer

    tempFile.seekg(0);                      // sets file pointer to beginning of tempFile
    //dstFile << "\n\n-----RESULTS OF LEXICAL ANALYZER-----\n";
    std::cout << "\n\n-----RESULTS OF LEXICAL ANALYZER-----\n";
    while (getline(tempFile, temp)) {
        std::cout << temp << std::endl;     // prints results of lexical analyzer to terminal
        //dstFile << temp << std::endl;       // writes results of lexical analyzer to dstFile (i.e. output1.txt, output2.txt, output3.txt)
    }

    printInstruction(dstFile);
    printSymbols(dstFile);
    
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
    int state = 0; // starting state
    // int accepting_state = 1; // accepting state

    if(lexeme.empty()) {
        return false;
    }

    for (size_t i = 0; i < lexeme.size() - 1; ++i) {
        char character = lexeme[i];
        switch (state)
        {
            case 0: // starting
                if (isalpha(character)) {
                    state = 1; // state 1 if first character is a letter
                } else {
                    return false; // means first char is not a letter
                }
                break;
            case 1:
                if (isalnum(character)) { // if letter & or digit
                    state = 1; // stay in this state
                } else {
                    return false;
                }
                break;
        }
        if (!isalpha(lexeme[lexeme.size() - 1])) {
            return false;
        }
    }
    return true;
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





int Rat24F(std::fstream& dst) {
    if(OptFunctionDefinitions(dst)) {
        if (current_word == "@") {
            printToken(token, current_word, dst);
            moveFile();
            if(OptDeclarationList(dst)) {
                if(StatementList(dst)) {
                    if (current_word == "@") {
                        printToken(token, current_word, dst);
                        if (switcher == true) {
                            std::cout << "<Rat24F>  ::=   <Opt Function Definitions>   @  <Opt Declaration List>  <Statement List>  @\n";
                            dst << "<Rat24F>  ::=   <Opt Function Definitions>   @  <Opt Declaration List>  <Statement List>  @\n";
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
    return 0;
}

int OptFunctionDefinitions(std::fstream& dst) {
    // if (switcher)
    //     std::cout << "<Opt Function Definitions> ::= <Function Definitions>     |  <Empty>\n";
    if(FunctionDefinitions(dst)) {
        if (switcher) {
            std::cout << "<Opt Function Definitions> ::=  <Function Definitions>\n";
            dst << "<Opt Function Definitions> ::=  <Function Definitions>\n";
        }
        return 1;
    } else {
        if (switcher) {
            std::cout << "<Opt Function Definitions> ::=  <Empty>\n";
            dst << "<Opt Function Definitions> ::=  <Empty>\n";
        }
        // hit empty 
        return 1;
    }
    return 1;
}

// Backtracking
int FunctionDefinitions(std::fstream& dst) {
    if (Function(dst)) {
        if (FunctionDefinitionsPrime(dst)) {
            if(switcher) {
                std::cout << "<Function Definitions> ::= <Function><FunctionDefinitionsPrime>\n";
                dst << "<Function Definitions> ::= <Function><FunctionDefinitionsPrime>\n";
            }
            return 1;
        }
    }
    return 0;
}



int FunctionDefinitionsPrime(std::fstream& dst) {
    
    if(Function(dst)) {
        if(FunctionDefinitions(dst)) {
        if(switcher) std::cout << "<FunctionDefinitionsPrime> ::= <FunctionDefinitions>";
        dst << "<FunctionDefinitionsPrime> ::= <FunctionDefinitions>";
        return 1;
    }
    }
    
    return 1;
}


int Function(std::fstream& dst) {
    if(current_word == "function") {
        //print <identifier> i think
        printToken(token, current_word, dst);
        moveFile();
        if(token == "ID") {
            printToken(token, current_word, dst);
            moveFile();
            if(current_word == "(") {
            printToken(token, current_word, dst);
            moveFile();
            if(OptParameterList(dst)) {
                if(current_word == ")") {
                    printToken(token, current_word, dst);
                    moveFile();
                    if(OptDeclarationList){
                        if(Body(dst)) {
                            if(switcher) {
                                 std::cout << "<Function> ::= function  <Identifier>   ( <Opt Parameter List> )  <Opt Declaration List>  <Body>\n";
                            }
                            return 1;
                        } else return 0;
                    } else return 0;
                    
                } else {
                    errors("')'", "Add a closing parenthesis");
                    return 0;
                }
            } else {
                //errors("parameters or empty parameters", "Check your parameters");
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
        
    return 0;
}



int OptParameterList(std::fstream& dst){
    if(ParameterList(dst)) {
        if(switcher) {
            std::cout << "<Opt Parameter List> ::=  <Parameter List>\n";
            dst << "<Opt Parameter List> ::=  <Parameter List>\n";
        }
        return 1;
    }
    else {
        //hit empty
        if(switcher) {
            std::cout << "<Opt Parameter List> ::=  <Empty>\n";
            dst << "<Opt Parameter List> ::=  <Empty>\n";
        }
        return 1;
    }
    return 1;
}

// Backtracking
int ParameterList(std::fstream& dst) {

        
    if (Parameter(dst)) {
        if(ParameterListPrime(dst)) {
            if(switcher) {
            std::cout << "<Parameter List>  ::=  <Parameter> <Parameter List Prime>\n";
            dst << "<Parameter List>  ::=  <Parameter> <Parameter List Prime>\n";
    }
            return 1;
        }
        return 0;
    }
    return 0;
}

int ParameterListPrime(std::fstream& dst) {
    if(current_word == ",") {   // ,
        printToken(token, current_word, dst);
        moveFile();
        if(ParameterList(dst)) {   // <PL>
            std::cout << "<Parameter List Prime>  ::=  ,<Parameter List>\n";
            //dst << "<Parameter List Prime>  ::=  ,<Parameter List>\n";
            return 1;
        } else {
            errors("','", "Parameter after comma");
            return 0;
        }
    } else {    // | ε 
        std::cout << "<Parameter List Prime>  ::=  <Empty>\n";
        //dst << "<Parameter List Prime>  ::=  <Empty>\n";
        // hit empty
        return 1;
    }
    return 1;
}

int Parameter(std::fstream& dst) {
    if(IDs(dst)) {
        if(Qualifier(dst)) {
            if(switcher) {
                std::cout << "<Parameter> ::=  <IDs >  <Qualifier>\n";
                dst << "<Parameter> ::=  <IDs >  <Qualifier>\n";
            }
            return 1;
        }
    } else { 
        //errors("an identifier", "Ensure there is a valid identifier for the parameter");
        return 0;
    }
    return 0;
}

int Qualifier(std::fstream& dst) {        
    if(current_word == "integer") {
        current_type = "Integer";
        printToken(token, current_word, dst);
        moveFile();
        if(switcher) {
            std::cout << "<Qualifier> ::= integer \n";
            dst << "<Qualifier> ::= integer \n";
        }
        return 1;
    } else if(current_word == "boolean") {
        current_type = "boolean";
        printToken(token, current_word, dst);
        moveFile();
        if(switcher) {
            std::cout << "<Qualifier> ::= boolean \n";
            dst << "<Qualifier> ::= boolean \n";
        }
        return 1;
    } else if(current_word == "real") {
        printToken(token, current_word, dst);
        moveFile();
        if(switcher) {
            std::cout << "<Qualifier> ::=  real \n";
            dst << "<Qualifier> ::=  real \n";
        }
        return 1;
    } else {
        errors("valid qualifier", "Check for type (e.g., 'integer', 'boolean', 'real')");
        return 0;
    }
    return 0;
}

int Body(std::fstream& dst) {        
    if(current_word == "{") {
        printToken(token, current_word, dst);
        moveFile();
        if(StatementList(dst)) {
            if (current_word == "}") {
                printToken(token, current_word, dst);
                moveFile();
                if(switcher) {
                    std::cout << "<Body>  ::=  {  < Statement List>  }\n";
                    dst << "<Body>  ::=  {  < Statement List>  }\n";
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
    return 0;
}

int OptDeclarationList(std::fstream& dst){        
    if(DeclarationList(dst)) {
        if(switcher) {
            std::cout << "<Opt Declaration List> ::= <Declaration List>\n";
            dst << "<Opt Declaration List> ::= <Declaration List>\n";
        }
        return 1;
    } else {
        //hit empty
        if(switcher) {
            std::cout << "<Opt Declaration List> ::= <Empty>\n";
            dst << "<Opt Declaration List> ::= <Empty>\n";
        }
        return 1;
    }
    return 1;
}

// Backtracking
int DeclarationList(std::fstream& dst){
        
    if(Declaration(dst)) {     // <D>
        if(current_word == ";") {   // ;
            printToken(token, current_word, dst);
            moveFile();
            if (DeclarationListPrime(dst)) // <DL'>
                if(switcher) {
                    std::cout << "<Declaration List>  := <Declaration> ; <Declaration List Prime>\n";  
                    dst << "<Declaration List>  := <Declaration> ; <Declaration List Prime>\n";  
                } 
                return 1;
                    
        } else {
            // errors("';'", "Add a semicolon, ';'");
            return 0;
        }
    } 
    return 0;
}


// <DL’> → <DL> | ε
int DeclarationListPrime(std::fstream& dst) {
    if(Declaration(dst)) {
        
        return 1;
        // if(current_word == ";") {
        //     printToken(token, current_word, dst);
        //     moveFile();
        //     if(DeclarationListPrime(dst)) {
        //         std::cout << "<Declaration List Prime>  := <Declaration> ; <Declaration List>\n";  
        //         dst << "<Declaration List Prime>  := <Declaration> ; <Declaration List Prime>\n"; 
        //         return 1;
        //     }
        // }
    } 
    std::cout << "<Declaration List Prime>  := <Declaration> ; <Declaration List>\n";  
    //dst << "<Declaration List Prime>  := <Empty>\n"; 
    return 1;
}


int Declaration(std::fstream& dst){
    if(Qualifier(dst)) {
        if(IDs(dst)) {
            if(switcher) {
                std::cout << "<Declaration> ::=   <Qualifier > <IDs>\n";
                dst << "<Declaration> ::=   <Qualifier > <IDs>\n";
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
int IDs(std::fstream& dst) {
    if (token == "ID") { // <I>
        addSymbol(current_word, current_type);
        
        if(isSTDIN) {
            //add current ID to stack
            addInstruction("POPM", getSymbolAddress(current_word));
        }
        
        printToken(token, current_word, dst);
        moveFile();
        if(IDsPrime(dst)) // <ID'>
            if(switcher) {
            std::cout << "<IDs> ::=     <Identifier> <IDs Prime>\n";
            dst << "<IDs> ::=     <Identifier> <IDs Prime>\n";
            }
            
            return 1;
    }  else {
        //errors("an identifier", "Ensure the current token is valid");
        
        return 0;
    }
    return 0;
}


// <ID’> → ,<ID> | ε
int IDsPrime(std::fstream& dst) {
    if(current_word == ",") {   // ,
        printToken(token, current_word, dst);
        moveFile();
        if(IDs(dst)) { // <ID>
            return 1;
            // if(IDsPrime(dst)) {
                
            //     if(switcher) {
            //     std::cout << "<IDs Prime> ::=   , <Identifier> <IDs Prime>\n";
            //     dst << "<IDs Prime> ::=    , <Identifier> <IDs Prime>\n";
            //     }
            //     return 1;
            // }
        }else {
             errors("an identifier", "Check if token is a valid identifier following ','");
             return 0;
         }
    // } else {    // hit empty
    //     return 1;
    }
    return 1;
}


// Backtracking
int StatementList(std::fstream& dst) {
    
    if(Statement(dst)) {
        if(StatementListPrime(dst)) {
            
            if(switcher) {
            std::cout << "<Statement List> ::=  <Statement> <Statement List Prime>\n";
            dst << "<Statement List> ::=   <Statement> <Statement List Prime>\n";
            }
            return 1;

        }
    } 
    return 0;
}

// <SL’> → <SL> | ε
int StatementListPrime(std::fstream& dst) {
    if(StatementList(dst)) {
        if(switcher) std::cout <<"<Statement List Prime> ::=   <Statement> <Statement List Prime>";
        return 1;
        
    } else {    // hit empty
        if(switcher) std::cout <<"<Statement List Prime> ::=   <Empty>";
        return 1;
    }
    return 1;
}

int Statement(std::fstream& dst) {
    if(Compound(dst)) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Compound>\n";
            dst << "<Statement> ::=   <Compound>\n";
        }
        return 1;
    } else if(Assign(dst)) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Assign>\n";
            dst << "<Statement> ::=   <Assign>\n";
        }
        return 1;
    } else if(If(dst)) {
        if(switcher) {
            std::cout << "<Statement> ::=   <If>\n";
            dst << "<Statement> ::=   <If>\n";
        }
        return 1;
    } else if (Return(dst)) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Return>\n";
            dst << "<Statement> ::=   <Return>\n";
        }
        return 1;
    }  else if (Print(dst)) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Print>\n";
            dst << "<Statement> ::=   <Print>\n";
        }
        return 1;
    } else if(Scan(dst)) {
        if(switcher) {
            std::cout << "<Statement> ::=   <Scan>\n";
            dst << "<Statement> ::=   <Scan>\n";
        }
        return 1;
    } else if(While(dst)) {
        if(switcher) {
            std::cout << "<Statement> ::=   <While>\n";
            dst << "<Statement> ::=   <While>\n";
        }
        return 1;
    } else {
        errors("statement", "Ensure a valid statement is provided"); // keep?
        return 0;
    }
    return 0;
}

int Compound(std::fstream& dst) {
    if(current_word == "{") {
        printToken(token, current_word, dst);
        moveFile();
        addInstruction("LABEL", ""); // start one

        if(StatementList(dst)) {
            if (current_word == "}") {
                printToken(token, current_word, dst);
                moveFile();
                addInstruction("LABEL", ""); // end one

                if(switcher) {
                    std::cout << "<Compound> ::=   {  <Statement List>  }\n";
                    dst << "<Compound> ::=   {  <Statement List>  }\n";
                }
                return 1;
            } else {
                errors("'}'", "Add a closing brace");
                return 0;
            }
        } else if (current_word == "}") {
            printToken(token, current_word, dst);
            moveFile();
            addInstruction("LABEL", ""); // empty one

            if(switcher) { // empty case
                    std::cout << "<Compound> ::=   {  <Empty>  }\n";
                    dst << "<Compound> ::=   {  <Empty>  }\n";
                }
            return 1;
        } else {
            errors("a valid statements","Ensure the statements are valid in the body");
            return 0;
        }
    } else { 
        errors("'{'", "Add an opening brace");
        return 0;
    }
}

std::string getSymbolAddress(std::string saveID) {
    for (size_t i = 0; i < (memoryAddress - 9000); i++) {
        if (symbolTable[i][0] == saveID)
            return symbolTable[i][2];  // Return the memory address
    }
    errors("symbol", saveID + " not found in symbol table");
    return "";
}

int Assign(std::fstream& dst) {
    std::string saved;
    if(token == "ID") {
        saved = current_word;
        printToken(token, current_word, dst); // log token
        moveFile();

        if (current_word == "=") {
            printToken(token, current_word, dst); // log '='
            moveFile();
            // addInstruction("POPM", getSymbolAddress(saved));

            if(Expression(dst)) {
                addInstruction("POPM", getSymbolAddress(saved));
                if(current_word == ";") {
                    printToken(token, current_word, dst); // logs ';'
                    moveFile();

                    if(switcher) {
                        std::cout << "<Assign> ::=     <Identifier> = <Expression> ;\n";
                        dst << "<Assign> ::=     <Identifier> = <Expression> ;\n";
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
            errors("'='","Needs an assignment operator '=' after identifier");
            return 0;
        }
    } else {
        errors("identifier", "Needs a starting identifier");
        return 0;
    }
    return 0;
}



int If(std::fstream& dst){
    if(current_word == "if") {              // if 1
        printToken(token, current_word, dst);
        moveFile();
        if(current_word == "(") {           // ( 2
            printToken(token, current_word, dst);
            moveFile();
            if(Condition(dst)) {               // <C> 3
                if(current_word == ")") {   // ) 4
                    printToken(token, current_word, dst);
                    moveFile();
                    // if tos is false run else .. put state in statement
                    if(stack[stackCount == 0]) runElse = false;
                    if(Statement(dst)) {       // <S> 5
                        runElse = true; 
                        backPatch(instructionAddress);
                        if (fiPrime(dst))
                            if(current_word == "fi") {
                                addInstruction("LABEL", "");
                                printToken(token, current_word, dst);
                                moveFile();
                                if(switcher) {
                                    std::cout << "<If> ::=    if ( <Condition> ) <Statement> <OptionalElse> fi\n";
                                    dst << "<If> ::=    if ( <Condition> ) <Statement> <OptionalElse> fi\n";
                                
                                }
                                return 1;
                            }
                    }
                }
            }
        }
    }
    return 0;
}

int fiPrime(std::fstream& dst) {
    //std::cout << "fiprime";
    if(current_word == "else") {
        printToken(token, current_word, dst);
        moveFile();
        if(Statement(dst)) {
            std::cout << "<If Prime> ::=    else <Statement>\n";
            //dst << "<If Prime> ::=    else <Statement>\n";
            return 1;
        }
    }
    std::cout << "<If Prime> ::=    <Empty>\n";
    //dst << "<If Prime> ::=    <Empty>\n";
    return 1;
}

// Backtracking
int Return(std::fstream& dst) {
    if(current_word == "return") {  // <r>
        printToken(token, current_word, dst);
        moveFile();
        
            if (ReturnPrime(dst)) {
                
                if(current_word == ";") {
                    printToken(token, current_word, dst);
                    moveFile(); 
                    std::cout << "<Return> ::=    return <Return'> ;\n";
                    dst << "<Return> ::=    return <Return'> ;\n";
                    return 1;
                }
                
                
            }
         
        
    } 
    return 0;
}

int ReturnPrime(std::fstream& dst) {

    if(Expression(dst)) {
        std::cout << "<Return Prime> ::=    <Expression>\n";
        dst << "<Return Prime> ::=    <Expression>\n";
        return 1;
    }
    
    std::cout << "<Return Prime> ::=    <Empty>\n";
    dst << "<Return Prime> ::=    <Empty>\n";
    return 1;
}

int Print(std::fstream& dst) {
    // if(switcher)
    //     std::cout << "<Print> ::=     put ( <Expression>);\n";
    if (current_word == "put") {
        printToken(token, current_word, dst);
        moveFile();
        if(current_word == "(") {
            printToken(token, current_word, dst);
            moveFile();
            if(Expression(dst)) {
                if(current_word == ")") {
                    addInstruction("STDOUT", "");
                    popStack();
                    printToken(token, current_word, dst);
                    moveFile();
                    if(current_word == ";") {
                        printToken(token, current_word, dst);
                        moveFile();
                        if(switcher) {
                            std::cout << "<Print> ::=     put ( <Expression>);\n";
                            dst << "<Print> ::=     put ( <Expression>);\n";
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
    return 0;
}

int Scan(std::fstream& dst) {
    if(current_word == "get") {
        isSTDIN = true;
        printToken(token, current_word, dst);
        moveFile();
        if(current_word == "(") {
            addInstruction("STDIN", "");
            printToken(token, current_word, dst);
            moveFile();
            if(IDs(dst)) {
                if(current_word == ")") {
                    isSTDIN = false;
                    printToken(token, current_word, dst);
                    moveFile();
                    if(current_word == ";") {
                        printToken(token, current_word, dst);
                        moveFile();
                        if(switcher) {
                            std::cout << "<Scan> ::=    get ( <IDs> );\n";
                            dst << "<Scan> ::=    get ( <IDs> );\n";
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
    return 0;
}

int While(std::fstream& dst) {
    if(current_word == "while") {
        int address{instructionAddress};
        addInstruction("LABEL", "");
        printToken(token, current_word, dst);
        moveFile();
        if(current_word == "(") {
            printToken(token, current_word, dst);
            moveFile();
            if(Condition(dst)) {
                if(current_word == ")") {
                    printToken(token, current_word, dst);
                    moveFile();

                    // these two lines were added before statement instead of after (according to partial solutions from prof)
                    // because when program hits statement, it returns before executing 
                    
                    if(Statement(dst)) {
                        if(switcher) {
                            std::cout << "<While> ::=  while ( <Condition>  )  <Statement>\n";
                            dst << "<While> ::=  while ( <Condition>  )  <Statement>\n";
                        }
                        addInstruction("JUMP", std::__cxx11::to_string(address));
                        backPatch(instructionAddress);
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
    return 0;
}

int Condition(std::fstream& dst) {        
    if(Expression(dst)) {
        if(Relop(dst)) {
            if(Expression(dst)) {
                if(switcher) {
                    std::cout << "<Condition> ::=     <Expression>  <Relop>   <Expression>\n";
                    dst << "<Condition> ::=     <Expression>  <Relop>   <Expression>\n";
                }

                if (relationalOp == "<")
                {
                    addInstruction("LES", "");
                    pushStack(instructionAddress);
                    addInstruction("JUMPZ", "");
                } else if (relationalOp == ">")
                {
                    addInstruction("GRT", "");
                    pushStack(instructionAddress);
                    addInstruction("JUMPZ", "");
                } else if (relationalOp == "==")
                {
                    addInstruction("EQU", "");
                    pushStack(instructionAddress);
                    addInstruction("JUMPZ", "");
                } else if (relationalOp == "!=")
                {
                    addInstruction("NEQ", "");
                    pushStack(instructionAddress);
                    addInstruction("JUMPZ", "");
                } else if (relationalOp == "<=")
                {
                    addInstruction("LEQ", "");
                    pushStack(instructionAddress);
                    addInstruction("JUMPZ", "");
                } else if (relationalOp == ">=")
                {
                    addInstruction("GEQ", "");
                    pushStack(instructionAddress);
                    addInstruction("JUMPZ", "");
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
    return 0;
}

int Relop(std::fstream& dst) {
    if(switcher) {
        std::cout << "<Relop> ::=        ==   |   !=    |   >     |   <    |  <=   |    =>\n";
        dst << "<Relop> ::=        ==   |   !=    |   >     |   <    |  <=   |    =>\n";
    }
        
    if (current_word == "==") {
        relationalOp = current_word;
        printToken(token, current_word, dst);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        ==\n";
            dst << "<Relop> ::=        ==\n";
        }
        return 1;
    } else if (current_word == "!=") {
        relationalOp = current_word;
        printToken(token, current_word, dst);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        !=\n";
            dst << "<Relop> ::=        !=\n";
        }
        return 1;
    } else if (current_word == ">") {
        relationalOp = current_word;
        printToken(token, current_word, dst);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        >\n";
            dst << "<Relop> ::=        >\n";
        }
        return 1;
    } else if (current_word == "<") {
        relationalOp = current_word;
        printToken(token, current_word, dst);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        <\n";
            dst << "<Relop> ::=        <\n";
        }
        return 1;
    } else if (current_word == "<=") {
        relationalOp = current_word;
        printToken(token, current_word, dst);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        <=\n";
            dst << "<Relop> ::=        <=\n";
        }
        return 1;
    } else if (current_word == "=>") {
        relationalOp = current_word;
        printToken(token, current_word, dst);
        moveFile();
        if(switcher) {
            std::cout << "<Relop> ::=        =>\n";
            dst << "<Relop> ::=        =>\n";
        }
        return 1;
    } else {
        errors("a valid relational operator", "Ensure there is a valid relational operator (e.g., '==', '!=', '<', '>', '<=', '>=')");
        return 0;
    }
    return 0;
}

//left recursion 
int Expression(std::fstream& dst) {
    if(Term(dst)) {
        if(ExpressionPrime(dst)){
            if(switcher) {
                std::cout << "<Expression>  ::=    <Term> <ExpressionPrime> \n";
                dst << "<Expression>  ::=    <Term> <ExpressionPrime> \n";
            }
            return 1;
        }
    } 
    return 0; 
    
}

int ExpressionPrime(std::fstream& dst) {
    if(current_word == "+" || current_word == "-") {
        std::string saved = current_word;
        printToken(token, current_word, dst);
        moveFile();
        
        // addInstruction("ADD", "");
        if(Term(dst)) {
            if (saved == "+") {
                addInstruction("ADD", "");
            } else if (saved == "-") {
                addInstruction("SUB", "");
            }

            if(ExpressionPrime(dst)) {
                std::cout << "<Expression Prime>  ::=    +<Term> <ExpressionPrime> | -<Term> <ExpressionPrime> \n";
                //dst << "<Expression Prime>  ::=    +<Term> <ExpressionPrime> | -<Term> <ExpressionPrime>\n";
                return 1;
            } 
        } 
    } else {
        std::cout << "<Expression Prime>  ::=    <Empty>\n";
        //dst << "<Expression Prime>  ::=    <Empty>\n";
        return 1;
    }
    return 0;
}


// left recursion
int Term(std::fstream& dst) {
    if(Factor(dst)){
        if(TermPrime(dst)) {
            if(switcher) {
            std::cout << "<Term>    ::=      <Factor> <Term Prime>\n";
            dst << "<Term>    ::=      <Factor> <Term Prime>\n";
            }
            return 1;
        }
    } 
    return 0;   // temp
}


int TermPrime(std::fstream& dst) {
    if(current_word == "*" || current_word == "/") {
        printToken(token, current_word, dst);
        moveFile();

        // addInstruction("MUL", "");
        if(Factor(dst)) {
            if (current_word == "*") {
                addInstruction("MUL", "");
            } else if (current_word == "/") {
                addInstruction("DIV", "");
            }
            if (TermPrime(dst)) {
                std::cout << "<Term Prime>    ::=      *<Factor> <Term Prime> | /<Factor> <Term Prime>\n";
                //dst << "<Term Prime>    ::=       *<Factor> <Term Prime> | /<Factor> <Term Prime>\n";
                return 1;
            }  
        }
    } else {    // hit empty
        std::cout << "<Term Prime>    ::=      <Empty>\n";
        //dst << "<Term Prime>    ::=      <Empty>\n";
        return 1;
    }
    return 0;
}

int Factor(std::fstream& dst) {   
    if(current_word == "-") {
        printToken(token, current_word, dst);
        moveFile();

        if(Primary(dst)) {
            addInstruction("NEG", "");
            if(switcher) {
                std::cout << "<Factor> ::=      -  <Primary>\n";
                dst << "<Factor> ::=      -  <Primary>\n";
            }
            return 1;
        } else {
            errors("a valid primary value", "Ensure there is a valid value following the '-'");
            // return 0;
        }
    } else if(token == "ID") {
        addInstruction("PUSHM", getSymbolAddress(current_word));
        printToken(token, current_word, dst);
        moveFile();

        if(switcher) {
                std::cout << "<Factor> ::=      <Primary>\n";
                dst << "<Factor> ::=      <Primary>\n";
        }
        return 1;

    } else if(Primary(dst)) {
        if(switcher) {
                std::cout << "<Factor> ::=      <Primary>\n";
                dst << "<Factor> ::=      <Primary>\n";
        }
        return 1;
    } else {
        errors("a valid primary value", "Ensure it starts with a valid primary value");
        return 0;
    }
    return 0;
}

int Primary(std::fstream& dst) {
    //if(switcher) {
        //std::cout << "<Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   |  <Real>  |   true   |  false\n";
        //dst << "<Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   |  <Real>  |   true   |  false\n"; 
    //}

    if(current_word == "(") {
        printToken(token, current_word, dst);
        moveFile();

        if(Expression(dst)) {
            if(current_word == ")") {
                printToken(token, current_word, dst);
                moveFile();
                if (switcher) {
                    std::cout << "<Primary> ::=     ( <Expression> )\n";
                    dst << "<Primary> ::=     ( <Expression> )\n";
                }
                return 1;
            } else {
                errors("')", "Missing closing parenthesis");
                return 0;
            }
        } else {
            errors("expression","Check your expression in the parenthesis");
            return 0;
        }
    }

    if(token == "ID") {
        std::string nam = current_word;
        // addInstruction("PUSHM", getSymbolAddress(current_word));
        printToken(token, current_word, dst);
        moveFile();

        if (current_word == "(") {
            printToken(token, current_word, dst);
            moveFile();
            if (IDs(dst)) {
                if(current_word == ")") {
                    printToken(token, current_word, dst);
                    moveFile();
                    if (switcher) {
                        std::cout << "<Primary> ::=     <Identifier> (IDs)\n";
                        dst << "<Primary> ::=     <Identifier> (IDs)\n";
                    }
                    return 1;
                } else {
                    errors("')", "Missing closing parenthesis");
                    return 0;
                }
            } else {
                errors("ID", "Invalid argument in function");
                return 0;
            }
        } else {
            addInstruction("PUSHM", getSymbolAddress(nam));
        }

        if(/*token == "ID" ||*/token == "Integer" || token == "Real" || current_word == "true" || current_word == "false") {
            std::string saved;
            if (current_word == "true") {
                saved = "1";
            } else if (current_word == "false") {
                saved = "0";
            } else {
                saved = current_word;
            }
            addInstruction("PUSHI", saved);
            if (switcher) {
                std::cout << "<Primary> ::=     <Identifier> \n";
                dst << "<Primary> ::=     <Identifier> \n";
            }
            return 1;
        }
    }

    if(/*token == "ID" ||*/token == "Integer" || token == "Real" || current_word == "true" || current_word == "false") {
        std::string saveBool;
        if (current_word == "true" || current_word == "false") {
            if (current_word == "true") {
                saveBool = "1";
            } else if (current_word == "false") {
                saveBool = "0";
            }
        } else {
            saveBool = current_word;
        }
        addInstruction("PUSHI", saveBool);
        printToken(token, current_word, dst);
        moveFile();

        if (switcher) {
            std::cout << "<Primary> ::=     <Integer>  |  <Real>  |   true   |  false\n";
            dst << "<Primary> ::=     <Integer>  |  <Real>  |   true   |  false\n";
        }
        return 1;
    }
    errors("Primary value", "Expected a primary value");
    return 0;
}


void addSymbol(std::string lexeme, std::string type)
{
    if(runElse){
        int row{memoryAddress - 9000};
        if (!checkSymbol(lexeme))
        {
            errors("non-duplicate variable", "make sure variables all have distinct names");
            return;
        }
        std::string address = std::__cxx11::to_string(memoryAddress++);

        symbolTable[row][0] = lexeme;
        symbolTable[row][1] = type;
        symbolTable[row][2] = address;

        return;  
    }
    return;
}

void addInstruction(std::string op, std::string operand)
{
    if(runElse){
        int row{instructionAddress - 1};
        std::string address = std::__cxx11::to_string(instructionAddress++);

        instructionTable[row][0] = address;
        instructionTable[row][1] = op;
        instructionTable[row][2] = operand;
        return;
    }
    return;
}

bool checkSymbol(std::string name)
{
    for (size_t i = 0; i < (memoryAddress - 9000); i++)
    {
        if (symbolTable[i][0] == name)
            return false;
    }
    
    return true;
}

void backPatch(int jumpAddress)
{
    int address = popStack();
    instructionTable[address - 1][2] = std::__cxx11::to_string(jumpAddress);   

    return;
}

void printSymbols(std::fstream& dst)
{
    std::cout << "---SYMBOL TABLE---\n";
    dst << "\n\n---SYMBOL TABLE---\n";
    std::cout << "LEXEME     " << "DATA TYPE     " << "MEMORY ADDRESS\n";
    dst << "LEXEME     " << "DATA TYPE     " << "MEMORY ADDRESS\n";
    for (size_t i = 0; i < (memoryAddress - 9000); i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            std::cout << symbolTable[i][j] << "          ";
            dst << symbolTable[i][j] << "          ";
        }
        std::cout << std::endl;
        dst << std::endl;
    }
    std::cout << "\n\n";
    dst << "\n\n";
    return;
}

void printInstruction(std::fstream& dst)
{
    std::cout << "---INSTRUCTION TABLE---\n";
    dst << "---INSTRUCTION TABLE---\n";
    std::cout << "ADDRESS     " << "OP     "  << "OPERAND\n";
    dst << "ADDRESS     " << "OP     " << "OPERAND\n";
    for (size_t i = 0; i < (instructionAddress - 1); i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            std::cout << instructionTable[i][j] << "           ";
            dst << instructionTable[i][j] << "           ";
        }
        std::cout << std::endl;
        dst << std::endl;
    }
    std::cout << "\n\n";
    dst << "\n\n";
    return;
}

void pushStack(int address)
{
    if(runElse){
        if(stackCount == TABLE_ROW)
            return;
        
        stack[stackCount++] = address;
        return;
    }
    return;
}

int popStack()
{
    if(runElse){
        if(stackCount == 0)
        {
            std::cout << "ERROR: unable to pop on an empty stack\n";
            return 0;   // error value
        }
        return stack[--stackCount];
    }
    return 0;
}
