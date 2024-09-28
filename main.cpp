#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>

using u32 = uint32_t;

namespace fs = std::filesystem;

//validating instructions
int valid(const std::string& str, int*);
int posOfWord(const std::string& str, int n);
bool invalidHex(const std::string& str);

//performing instructions
std::string getOperation(const std::string& str);


int main(int argc, char** argv) {
    //Check Arguments
    if(argc != 2) {
        std::cerr << "Usage: ./Luc_DeWitt_Project1 <text_file>\n";
        std::cerr << "Text_file is a text file containing various assembly instructions. \n";
        std::cerr << std::endl;
        return 1;
    }
    std::string fileName(argv[1]);
    if(!fs::exists(fileName)){
        std::cerr << "The specified file does not exist. \n";
        std::cerr << std::endl;
        return 2;
    }

    //Vector for storing the instructions
    std::vector<std::string> lines;

    //file IO
    std::ifstream fin;
    std::string buffer;
    int lineNum = 0;
    int errorPos = 0;
    int posToEnd = 0;

    //opening the file and reading
    fin.open(fileName);
    while(std::getline(fin,buffer)){
        if(buffer.length() != 0){
            lines.push_back(buffer);
        }
    
        //check if instructions are valid
        //point out the error if it can be found
        if((errorPos = valid(buffer, &posToEnd)) ){
            if(errorPos < 0){
                errorPos == 0;
            }
            for(char ch : buffer){
                if(ch ==' '){
                    errorPos++;
                }
                else
                    break;
            }
            std::cerr << "Invalid instruction found on line " << lineNum + 1 << ":\n";
            std::cerr << buffer << std::endl;
            for (int i = 0; i < errorPos; i++){
                std::cerr << " ";
            }
            std::cerr << "\x1b[38;2;240;0;0m";
            std::cerr << "^";
            for (int i = 0, len = posToEnd; i < len; i++){
                std::cerr << "~";
            }
            std::cerr << "\x1b[0m";
            std::cerr << std::endl;
            return 3;
        }
        lineNum++;
    }

    //Close the file stream
    fin.close();

    //perform each instruction
    for(auto it = lines.begin(); it < lines.end(); it++){
        //store each instruction in a buffer
        std::stringstream ss(*it);
        std::string token_buffer;
        std::vector<std::string> tokens;

        //read instructions into the buffer
        while(ss >> token_buffer){
            tokens.push_back(token_buffer);
        }

        //ADD instruction
        if(tokens[0] == "ADD" || tokens[0] == "add"){
            u32 operand1 = 0;
            u32 operand2 = 0;
            u32 sum = 0;
            bool willOverflow = false;

            //extract operand 1 from file
            for (int i = 2, len = tokens[1].length(); i < len; i++){
                int digit = 0;
                if('0' <= tokens[1][i] && tokens[1][i] <= '9'){
                    digit = tokens[1][i] - '0';
                }
                else if ('a' <= tokens[1][i] && tokens[1][i] <= 'f'){
                    digit = tokens[1][i] - 'a' + 10;
                }
                else if ('A' <= tokens[1][i] && tokens[1][i] <= 'F'){
                    digit = tokens[1][i] - 'A' + 10;
                }
                operand1 *= 16;
                operand1 += digit;
            }
            //extract operand 2 from file
            for (int i = 2, len = tokens[2].length(); i < len; i++){
                int digit = 0;
                if('0' <= tokens[2][i] && tokens[2][i] <= '9'){
                    digit = tokens[2][i] - '0';
                }
                else if ('a' <= tokens[2][i] && tokens[2][i] <= 'f'){
                    digit = tokens[2][i] - 'a' + 10;
                }
                else if ('A' <= tokens[2][i] && tokens[2][i] <= 'F'){
                    digit = tokens[2][i] - 'A' + 10;
                }
                operand2 *= 16;
                operand2 += digit;
            }

            //check if addition will overflow
            willOverflow = (operand2 > std::numeric_limits<u32>::max() - operand1);

            //store the sum
            sum = operand1 + operand2;

            //print instruction and result
            std::cout << *it << ": 0x";
            std::cout << std::hex << std::uppercase << sum << std::nouppercase << std::dec;
            std::cout << "\nOverflow: ";
            std::cout << (willOverflow ? "yes" : "no" ) << "\n" << std::endl;
        }
    }
    return 0;
}

int valid(const std::string& str, int* posToEnd){
    //forgive empty lines
    if(str.length() == 0){
        return 0;
    }

    //counting the words in each instruction line

    //store elements of the instruction in a buffer
    std::vector<std::string> words;
    std::string buffer;
    std::stringstream ss(str);
    std::string full = ss.str();

    //read into the buffer
    while (ss >> buffer){
        words.push_back(buffer);
    }

    //insturctions require three elements ( operation, operand1, operand2)
    if(words.size() < 3){
        return str.length();
    }
    else if(words.size() > 3){
        return posOfWord(full,4);
    }

    //validate the elements
    else{
        //confirm that first element is a valid operation
        if ( words[0] != "ADD" && words[0] != "add" ){
            *posToEnd = words[0].length() - 1;
            return -1;
        }
        //confirm that secoond element is an operand 
        if(invalidHex(words[1])){
            *posToEnd = words[1].length()-1;
            return posOfWord(full,2);
        }
        //confirm that third element is a valid operand
        if(invalidHex(words[2])){
            *posToEnd = words[2].length() - 1;
            return posOfWord(full, 3);
    }
}
return 0;
}

//find the location of a word for error marking
int posOfWord(const std::string& str, int n){
    int pos = 0;
    int len = 0;
    int target = 0;
    int startOfWordNum = 0;
    bool inWord = false;
    for(pos = 0, len = str.length(); pos < len; ++pos){
        if (!inWord && !std::isspace(str[pos])){
            startOfWordNum++;
            inWord = true;
        }
        else if (inWord && std::isspace(str[pos])){
            inWord = false;
        }
        if(startOfWordNum >= n){
            target = pos;
            break;
        }
    }
    return target;
}

//check if a purported hexadecimal value is invalid
bool invalidHex(const std::string& str){
    if(str[0] != '0'){
        return true;
    }
    if(str[1] != 'x'){
        return true;
    }
    if(str.length() > 10){
        return true;
    }
    if(str.length() <= 2){
        return true;
    }
    for(int i = 2, len = str.length(); i < len; ++i){
        if(str[i] < '0' || str[i] > '9'){
            if(str[i] < 'a' || str[i] > 'f'){
                if(str[i] < 'A' || str[i] > 'F'){
                    return true;
                }
            }
        }
    }
    return false;
}