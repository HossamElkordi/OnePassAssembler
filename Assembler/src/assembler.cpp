#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <regex>
#include <iterator>
#include<string>
#include<vector>
#include<string>
#include <unordered_set>
#include <unordered_map>

using namespace std;

//============== Global Variable =========================================

unordered_map<string, string> OPTAB;
unordered_map<string, string> REGTAB;
unordered_set<string> DIRECTIVES;
bool pc = true;
map<string, vector<string>> symTab;
string current;
bool errorFlag=false;
string FileCode;
int format;

//============== End Of Global Variable ==================================

//============== Functions declarations ==================================

void split(string str, string seperator, list<string> * strings);
string getObjectOpcode(string opcode, string operand1, string operand2);
string flags(string operand, const string& indexed);
string hex2bin(string hexa);
string bin2hex(string bin);
void setOPTAB();
void setREGTAB();
void setDirectives();
void memoryLocationAdder(string identifier,int location);
void labelAdder(string label,int location);
string ReadFile(string path);
int displacementCalculator(int address);

//============== End Of Functions declarations ===========================

int main() {
    setOPTAB();
    setREGTAB();
	setDirectives();
	string path;
	cout<<"Enter the path of the file";
    cin>>path;
	ReadFile(path);
    return 0;
}

int displacementCalculator(int address){
    if(abs(address-pc)>2048){
        errorFlag= true;
        return 0;

    } else{
        pc=true;
        return address-pc;
    }
}

void labelAdder(string label,int location){
    if ( symTab.find(label) == symTab.end() ) {
        vector<string> tempo;
        tempo.push_back(to_string(location));
        symTab[label]=tempo;
    }else if(symTab[label].front()=="*"){
        //********************************************************************
        //code to be added here
        //code to iterate the vector and change the places left empty to the actual location
        //_____________________________________________________________________
        vector<string> tempo;
        tempo.push_back(to_string(location));
        symTab[label]=tempo;

    }else {
        errorFlag=1;
        //this means that you used the same label twice which is forbidden
    }
}

void memoryLocationAdder(string identifier,int location){
    if ( symTab.find(identifier) == symTab.end() || symTab[identifier].front()=="*") {
        current=current+"XXXXX";
        if ( symTab.find(identifier) == symTab.end()){
            vector<string> temp;
            temp.push_back("*");
            temp.push_back(to_string(location));
            symTab[identifier]=temp;
        }
        else{
            symTab[identifier].push_back(to_string(location));
        }

    } else {
        current=current+symTab[identifier].back();
    }
}

void updateObjectCode(string address, vector<string> appearences){
    list<string> textRecords;
    list<string>::iterator ilist;
    split(FileCode, "\n", &textRecords);
    int reqAddress, start, length, startAdd, endAdd, i;
    string before, after;
    for(string app : appearences){
        if(app == "*") continue;
        reqAddress = stoi(app, 0, 16);
        for(ilist = textRecords.begin(); ilist != textRecords.end(); ++ilist){
            if (ilist->at(0) == 'T') {
                start = stoi(ilist->substr(1, 0), 0, 16);
                length = stoi(ilist->substr(7, 2), 0, 16);
                if(reqAddress < (start + length)){
                    startAdd = (2 * (reqAddress - start)) + 9;
                    endAdd = startAdd + 4; // 5 digits address
                    i = address.length() - 1;
                    while(i >= 0){
                        ilist->at(endAdd) = address[i];
                        --i;
                        --endAdd;
                    }
                    while (endAdd >= startAdd){
                        ilist->at(endAdd) = '0';
                        --endAdd;
                    }
                    break;
                }
            }
        }
    }
    FileCode = "";
    for(ilist = textRecords.begin(); ilist != textRecords.end(); ++ilist){
        FileCode += *ilist;
    }
}

string ReadFile(string path)
{
    int PC=0;
    string first;
    ifstream CodeFile;
    string row;
    list<string> splitted;
    list<string>::iterator ilist;
    vector<string>container;
    CodeFile.open(path, ios::in);
    while (getline(CodeFile, row))
    {
        if (row.find('.')!=std::string::npos)
            continue;
        split(row, "\\s+", &splitted);
        for (ilist = splitted.begin(); ilist != splitted.end(); ++ilist)
            container.push_back(*ilist);
        if(OPTAB.find(container.at(0))!=OPTAB.end())
        {
            //Label,operand,1st,2nd,pc
            string first=container.at(1),second="";
            if(container.at(1).find(',')!=std::string::npos)
            {
                list<string>temp;
                split(container.at(1),",",&temp);
                ilist = temp.begin();
                first = *ilist;
                advance(ilist, 1);
                second = *ilist;
            }
            PC+=3;
            //myFunc("",container.at(0),first,second,PC);
        } //TODO warnings and errors
        else if(OPTAB.find(container.at(1))!=OPTAB.end())
        {
            string first=container.at(1),second="";
            if(container.at(2).find(',')!=std::string::npos)
            {
                list<string>temp;
                split(container.at(1),",",&temp);
                ilist = temp.begin();
                first = *ilist;
                advance(ilist, 1);
                second = *ilist;
            }
            PC+=3;
            //myFunc(container.at(0),container.at(1),first,second,PC);
        }
        else if(DIRECTIVES.find(container.at(1))!=DIRECTIVES.end())
        {
            string str = container.at(1);
            transform(str.begin(), str.end(), str.begin(), ::toupper);
            stringstream Str2Int(container.at(2));
            int x = 0;
            if((str.compare("WORD")) == 0){
                PC += 3;
            }else if((str.compare("RESW")) == 0){
                Str2Int >> x;
                PC += (3*x);
            }else if((str.compare("BYTE")) == 0){
                PC += 1;
            }else if((str.compare("RESBYTE")) == 0){
                Str2Int >> x;
                PC += x;
            }
        }
        splitted.clear();
        container.clear();
    }
    CodeFile.close();
}

void split(string str, string seperator, list<string> * strings){
    regex rgx(seperator);
    sregex_token_iterator iter(str.begin(),str.end(), rgx, -1);
    sregex_token_iterator end;
    for ( ; iter != end; ++iter)
        if(*iter!="")
            strings->push_back(*iter);
}


void setOPTAB() {
    string line, key, value;
    ifstream opFile;
    list<string> KVlist;
    list<string>::iterator ilist;
    opFile.open("OPTAB.txt", ios::in);
    while (getline(opFile, line)) {
        split(line, " ", &KVlist);
        ilist = KVlist.begin();
        key = *ilist;
        advance(ilist, 1);
        value = *ilist;
        KVlist.clear();
        OPTAB.insert(pair<string, string>(key, value));
    }
    opFile.close();
}

void setREGTAB() {
    string line, key, value;
    ifstream opFile;
    list<string> KVlist;
    list<string>::iterator ilist;
    opFile.open("REGTAB.txt", ios::in);
    while (getline(opFile, line)) {
        split(line, " ", &KVlist);
        ilist = KVlist.begin();
        key = *ilist;
        advance(ilist, 1);
        value = *ilist;
        KVlist.clear();
        REGTAB.insert(pair<string, string>(key, value));
    }
    opFile.close();
}

void setDirectives(){
    string dir;
    ifstream opFile;
    opFile.open("DIRECTIVES.txt", ios::in);
    while(getline(opFile, dir)){
        DIRECTIVES.insert(dir);
    }
    opFile.close();
}

string hex2bin(string hexa){
    string bin;
    for(char c : hexa){
        switch(c){
            case '0': bin += "0000"; break;
            case '1': bin += "0001"; break;
            case '2': bin += "0010"; break;
            case '3': bin += "0011"; break;
            case '4': bin += "0100"; break;
            case '5': bin += "0101"; break;
            case '6': bin += "0110"; break;
            case '7': bin += "0111"; break;
            case '8': bin += "1000"; break;
            case '9': bin += "1001"; break;
            case 'a':
            case 'A': bin += "1010"; break;
            case 'b':
            case 'B': bin += "1011"; break;
            case 'c':
            case 'C': bin += "1100"; break;
            case 'd':
            case 'D': bin += "1101"; break;
            case 'e':
            case 'E': bin += "1110"; break;
            case 'f':
            case 'F': bin += "1111"; break;
        }
    }
    return bin;
}

string bin2hex(string bin){
    string hexa, digit;
    int dec = 0;
    for (unsigned int i = 0; i < bin.length(); ++i){
        digit += bin[i];
        if((i + 1) % 4 == 0){
            dec = stoi(digit, 0, 2);
            if(dec < 10){
                // 0 -> 9
                hexa += (char)(dec + 48);
            }else{
                // A -> F
                hexa += (char)(dec + 55);
            }
            digit.clear();
        }
    }
    return hexa;
}

/*
 * operands : list of operands after being split over ","
 * if size == 1 : object code for this opcode is in 3 digits
 * if size == 2 : There is 2 cases:
 * 		1- index addressing
 * 		2- object code for this opcode is in 2 digits(size == 0 alse)
 */
string getObjectOpcode(string opcode, string operand1, string operand2){
    string ob;
    if(opcode[0] == '+'){
        pc = false;
        opcode.erase(0, 1);
        format = 4;
    }
    unordered_map<string, string>::iterator iop = OPTAB.find(opcode);
    if(iop != OPTAB.end()){ // valid opcode
        ob = iop->second;

        if(operand1.empty() && operand2.empty()){ // 2 digit opcode i.e. RSUB
            format = 1;
            return ob;
        }

        unordered_map<string, string>::iterator ireg = REGTAB.find(operand1);
        if (operand2.empty()){
            if (ireg == REGTAB.end()) {
                format = (format != 4) ? 3 : 4;
                ob = hex2bin(ob);
                ob = ob.substr(0, ob.length() - 2);
                ob += flags(operand1, "0");
                return bin2hex(ob);
            }else {
                format = (format != 4) ? 2 : 4;
                ob += ireg->second;
                ob += "0";
                return ob;
            }
        }else{
            if(ireg == REGTAB.end()){
                if (operand2 != "X"){
                    errorFlag = true;
                    return "";
                }else{
                    format = (format != 4) ? 3 : 4;
                    ob = hex2bin(ob);
                    ob = ob.substr(0, ob.length() - 2);
                    ob += flags(operand1, "1");
                    return bin2hex(ob);
                }
            }else{
                ob += ireg->second;
                ireg =  REGTAB.find(operand2);
                if(ireg == REGTAB.end()){
                    errorFlag = true;
                    return "";
                }else{
                    ob += ireg->second;
                    format = (format != 4) ? 2 : 4;
                    return ob;
                }
            }
        }
    }else{
        errorFlag = true;
        return "";
    }
}

string flags(string operand, const string& indexed){
    string nixbpe;

    // n i
    if(operand[0] == '@'){
        nixbpe += "10";
    }else if(operand[0] == '#'){
        nixbpe += "01";
    }else{
        nixbpe += "11";
    }
    // x
    nixbpe += indexed;

    if(pc){
        nixbpe += "010";
    }else{
        nixbpe += "001";
    }
    return nixbpe;
}
