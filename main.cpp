//
//  main.cpp
//  COP3530_Project1
//
//  Created by Jason Cochran on 10/24/16.
//  Copyright (c) 2016 Jason Cochran. All rights reserved.
//

#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <cmath>
#include <list>
#include <stack>
using namespace std;

// Global variable hash map
std::unordered_map<string, double> variables;
bool errorEncountered;

bool isdouble(const std::string& s) {
    std::istringstream stream(s);
    double d;
    char c;
    return stream >> d && !(stream >> c);
}

// Constant time
bool isOperator( string s ) {
    if(s == "+") {
        return true;
    } else if( s == "-") {
        return true;
    } else if( s == "*") {
        return true;
    } else if( s == "/") {
        return true;
    } else if( s == "^") {
        return true;
    } else {
        return false;
    }
}

// Constant time
bool isFunction(string s ) {
    if(s == "cos") {
        return true;
    } else if(s == "sin") {
        return true;
    } else if(s == "log") {
        return true;
    } else {
        return false;
    }
}

bool isAssociative(string operand, string associativity) {
    if( operand == "+" && associativity == "left" ) {
        return true;
    } else if(operand == "-" && associativity == "left" ) {
        return true;
    } else if(operand == "*" && associativity == "left" ) {
        return true;
    } else if(operand == "/" && associativity == "left" ) {
        return true;
    } else if(operand == "^" && associativity == "right" ) {
        return true;
    } else if(operand == "sin" && associativity == "right" ) {
        return true;
    } else if(operand == "cos" && associativity == "right" ) {
        return true;
    } else if(operand == "log" && associativity == "right" ) {
        return true;
    } else {
        // Didn't match anything... we are good
        return false;
    }
}

int precedenceComp(string first, string second) {
    int firstPrec, secondPrec;
    
    if( first == "+") {
        firstPrec = 1;
    } else if(first == "-") {
        firstPrec = 1;
    } else if(first == "*") {
        firstPrec = 2;
    } else if(first == "/") {
        firstPrec = 2;
    } else if(first == "^") {
        firstPrec = 3;
    } else if(first == "sin") {
        firstPrec = 4;
    } else if(first == "cos") {
        firstPrec = 4;
    } else if(first == "log") {
        firstPrec = 4;
    } else {
        // Didn't match anything... we are good
        firstPrec = 0;
    }
    
    if( second == "+") {
        secondPrec = 1;
    } else if(second == "-") {
        secondPrec = 1;
    } else if(second == "*") {
        secondPrec = 2;
    } else if(second == "/") {
        secondPrec = 2;
    } else if(second == "^") {
        secondPrec = 3;
    } else if(first == "sin") {
        secondPrec = 4;
    } else if(first == "cos") {
        secondPrec = 4;
    } else if(first == "log") {
        secondPrec = 4;
    } else {
        // Didn't match anything... we are good
        secondPrec = 0;
    }
    
    return (firstPrec - secondPrec);
    
}

// Shunting yard is O(n) because everything is pushed no more than 2 times
vector<string> ShuntingYard(vector<string> input) {
    vector<string> output;
    vector<string> function;
    stack<string> stack;
    
    // While there are tokens to be read
    for (int i = 0; i < input.size(); i++) {
        string token = input[i];
        // Implement as default case...
        if( isOperator(token) ) { // If the token is an operator
            // pop o2 off the operator stack, onto the output queue;
            string o1 = token;
            if( !stack.empty() ) {
                string o2 = stack.top();
                
                
                while (isOperator(o2) &&
                    ( ( precedenceComp( o1, o2 ) < 0  && isAssociative(o1, "right") ) ||
                    ( isAssociative( o1, "left" ) &&  precedenceComp( o1, o2 ) <= 0 ) ) ) {
                    // bool test1 = ( precedenceComp( o1, o2 ) < 0  && isAssociative(o1, "right") );
                    // bool test2 = ( isAssociative( o1, "left" ) &&  precedenceComp( o1, o2 ) <= 0 );
                    // For above statement:
                    // while there is an operator token o2, at the top of the operator stack and either
                    // o1 is left-associative and its precedence is less than or equal to that of o2, or
                    // o1 is right associative, and has precedence less than that of o2,
                    
                    // pop o2 off the stack, onto the output queue;
                    stack.pop();
                    output.push_back(o2);
                    
                    if ( !stack.empty() ) {
                        o2 = stack.top();
                    } else {
                        break;
                    }
                }
            }
            
            // at the end of iteration push o1 onto the operator stack.
            stack.push(token);
            
        } else if(isFunction(token)) { // If the token is a function token, then push it onto the stack.
            // For this we must take all inputs until we read left '(' and right ')' in order to get all of the operators that must be included in the function
            stack.push(token);
            
        } else if(token == ")" ) { // If the token is a right parenthesis (i.e. ")"):
            // Until the token at the top of the stack is a left parenthesis,
            // pop operators off the stack onto the output queue.
            string tokenTop = stack.top();
            
            while( tokenTop != "(") {
                output.push_back(tokenTop);
                stack.pop();
                if (stack.empty()) {
                    break;
                }
                tokenTop = stack.top();
            }
            
            // Pop the left parenthesis from the stack, but not onto the output queue.
            if( !stack.empty()) {
                stack.pop();
            }
            // If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
            if ( tokenTop != "(" )
            {
                //mismatch
            }
        } else if(token == "(") { // If the token is a left parenthesis (i.e. "(" ), then push it onto the stack.
            stack.push(token);
        } else { // must be a numba
            // If the token is a number, then push it to the output queue
            output.push_back(token);
        }
    }
    
    // When there are no more tokens to read:
    // While there are still operator tokens in the stack:
    while (!stack.empty()) {
        // Pop the operator onto the output queue.
        string lastToken = stack.top();
        output.push_back(lastToken);
        stack.pop();
    }
    
    // Return as vector<string> so toDouble can do the final conversion to get us answer
    return output;
}

// O(n) - not possible for everything to be popped/pushed n times
double toDouble(vector<string> tokens, bool isAssignment) {
    stack<string> tokenS;
    for(int i = (tokens.size() - 1) ; i >= 0; i--) {
        tokenS.push(tokens[i]);
    }
    
    //string temp = tokenS.top();
    //string token = tokens.front();
    
    stack<double> stack;
    while(tokenS.size() != 0){
        // string token = tokens.front();
        string token = tokenS.top();
        if( isdouble(token) ) { // if its a value
            // push onto stack
            stack.push(stod(token));
            // tokens.erase(tokens.begin());
            tokenS.pop();
        } else if( isOperator(token) ) { // if its an operator
            // It is already known that the operator takes n arguments
            int numArgs = 2;
            double firstOp, secondOp;
            if( stack.size() < numArgs  ) {// If there are fewer than n values on the stack
                // (Error) The user has not input sufficient values in the expression.
                break;
            } else { // Else... every operator uses 2 operands so pop twice
                //Pop the top n values from the stack.
                // Swap the order of these for respect to PEMDAS subtraction
                secondOp = stack.top();
                stack.pop();
                firstOp = stack.top();
                stack.pop();
            }
            
            // Evaluate the operator, with the values as arguments
            if (token == "*") {
                stack.push(firstOp * secondOp);
            } else if (token == "/") {
                if( secondOp == 0 && !isAssignment ) { // Division by zero in assignments are silently ignored
                    cout << "Division-By-Zero" << endl;
                    errorEncountered = true;
                    for(int i = 0; i < stack.size(); i++) {
                        stack.pop();
                    }
                    return 0;
                }
                stack.push( firstOp / secondOp);
            } else if (token == "-") {
                stack.push( firstOp - secondOp);
            } else if (token == "+") {
                stack.push(firstOp + secondOp);
            } else if (token == "^") {
                stack.push(std::pow(firstOp, secondOp));
            } else {
                cout << "Error" << endl;
                break;
            }
            // tokens.erase(tokens.begin()); // Erase the operator we just computed with
            tokenS.pop();
            // Push the returned results, if any, back onto the stack.
        } else if( isFunction(token) ) { // Must be sin, cos, log; we'll pop just one item from the stack?
            double firstOp = (double) stack.top();
            stack.pop();
            if (token == "sin") {
                stack.push( sin(firstOp) );
            } else if(token == "cos") {
                stack.push( cos(firstOp) );
            } else if(token == "log") {
                stack.push( log(firstOp) );
            } else {
                // Do nothing... this is an error catcher
            }
            // tokens.erase(tokens.begin());
            tokenS.pop();
        } else if( stack.size() == 1 ) { // If there is only one value in the stack
            // That value is the result of the calculation.
            // strangley we don't need this case
        } else { // Otherwise, there are more values in the stack
            // could be mismatch but we don't care
        }
    }
    if(errorEncountered != true) {
        double final = stack.top();
        return final;
    } else {
        return 0;
    }
}

// Also functions as tokenizer
// O(n) - goes through every char in string... no repeats (additional functions are constant time)
vector<string> expressionCleaner(string input, bool isAssignment) {
    vector<string> cleaned;
    
    // push_back onto string is constant time (see C++ standard manual for this)
    
    string tempStore; // For numbers
    bool tempInUse = false; // For numbers
    string tempVar; // For variables
    bool tempVarInUse = false; // For variables

    for(int i = 0; i <= input.length(); i++) {
        switch( input[i] ) {
            case '0' :	case '1' :	case '2' :	case '3' : 	case '4' :
            case '5' :	case '6' :	case '7' :	case '8' :	case '9' :
            case '.' :
                tempStore.push_back(input[i]);
                tempInUse = true;
                break;
            // case 0:
            case '*':   case '/':   case '+':   case '-':   case '^':
            case '(':   case ')':   case '=':
            {
                // isAssignment = false;
                if(tempVarInUse == true) { // for variables
                    std::unordered_map<std::string,double>::const_iterator found = variables.find(tempVar);
                    bool check = ( found != variables.end() );
                    
                    if( isFunction(tempVar) ) {
                        cleaned.push_back(tempVar);
                        tempVarInUse = false;
                        tempVar = "";
                    } else if( check ) {
                        // double temp = variables[tempVar];
                        ostringstream stream;
                        stream << variables[tempVar];
                        string str = stream.str();
                        cleaned.push_back( str );
                        tempVarInUse = false;
                        tempVar = "";
                    } else { // variable not found
                        if( !isAssignment) {
                            cout << "Undeclared-Variable" << endl;
                            tempVarInUse = false;
                            tempVar = "";
                            errorEncountered = true;
                            cleaned.clear();
                        }
                        
                    }
                } else if(tempInUse == true) { // for numbers
                    cleaned.push_back(tempStore);
                    tempInUse = false;
                    tempStore = "";
                } else {
                    // do nothing
                }
                stringstream ss;
                string s;
                ss << input[i];
                ss >> s;
                cleaned.push_back(s);
                break;
            }
            default:
                // Must be a char variable or special function...
                if( isalpha( input[i] ) ) {
                    tempVar.push_back( input[i] );
                    tempVarInUse = true;
                }
                // Do nothing... its a space
        }
    }
    if(tempVarInUse == true) {
        std::unordered_map<std::string,double>::const_iterator found = variables.find(tempVar);
        bool check = ( found != variables.end() );
        
        if( isFunction(tempVar) ) {
            cleaned.push_back(tempVar);
            tempVarInUse = false;
            tempVar = "";
            // How to break...
        } else if( check ) {
            // double temp = variables[tempVar];
            ostringstream stream;
            stream << variables[tempVar];
            string str = stream.str();
            cleaned.push_back( str );
            tempVarInUse = false;
            tempVar = "";
        } else { // variable not found?
            if( !isAssignment) {
                cout << "Undeclared-Variable" << endl;
                tempVarInUse = false;
                tempVar = "";
                errorEncountered = true;
                cleaned.clear();
            }
            errorEncountered = true;
        }

    } else if(tempInUse == true) {
        cleaned.push_back(tempStore);
        tempInUse = false;
    } else {
        // do nothing
    }
    
    return cleaned;
}

int main() {
    variables["Pi"] = 3.14169;
    variables["e"] = 2.718;
    
    string input;
    do{
        getline(cin,input);

        if( input.find("=") != std::string::npos ) { // Assignment
             int equalPosition = -1;
             for(int i = 3; i < input.size(); i++) {
                 if(input[i] == '=') {
                     equalPosition = i;
                 }
             }
             vector<string> cleaned = expressionCleaner( input.substr(equalPosition + 1, input.size() - 1), true );
             vector<string> shunted = ShuntingYard(cleaned);
             double final = toDouble(shunted, true);
             if( errorEncountered == false) {
                variables[ input.substr(4,equalPosition - 5 ) ] = final;
             } else {
                errorEncountered = false;
             }
             // variables[ input.substr(4,equalPosition - 5 ) ] = final; // 3 for let... plus 2 spaces
        } else if(input != "quit" ) { // Expression
            vector<string> cleaned = expressionCleaner(input, false);
            vector<string> shunted = ShuntingYard(cleaned);
            double final = toDouble(shunted, false);
            if( errorEncountered == false) {
                cout << final << endl;
            } else {
                errorEncountered = false;
            }
        } else {
            // do nothing
        }
        
    }while( input != "quit" );
    
    return 0;
}