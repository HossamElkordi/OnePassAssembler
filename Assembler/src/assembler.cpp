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
bool forwardRef = false;
bool extended = false;
map<string, vector<string>> symTab;
bool errorFlag=false;
string TextRecord;
int format, startAdd;
bool exists, exp = false, NoPC=true;;
int PC=0, LengthIndex=6,LengthOfTextRecord=0;
bool lcCounter = false;
int lctr,line=1;
string ErrorLines;

//============== End Of Global Variable ==================================

//============== Functions declarations ==================================

string decToHexa(int n);
void split(string str, string seperator, list<string> * strings);
string getObjectOpcode(string opcode, string operand1, string operand2);
string getObjectCode(string opcode, string op1, string op2, int pc);
void updateObjectCode(string address, vector<string> appearences);
string flags(string operand, const string& indexed);
string hex2bin(string hexa);
string bin2hex(string bin);
string decToBinary(int n);
string twosComplement(string bin);
void setOPTAB();
void setREGTAB();
void setDirectives();
string memoryLocationAdder(string identifier,int location);
void labelAdder(string label,int location);
string ReadFile(string path);
int displacementCalculator(int address, int PC);
void equHandle(string name, string value);
void orgHandle(string operand);
bool checkNumericString(string s);
bool isExpression(string operand);
string expressionCalc(string operand);
void WriteFile(string objectFile);
//============== End Of Functions declarations ===========================

int main() {
    setOPTAB();
    setREGTAB();
	setDirectives();
	string path;
	cout<<"Enter the absolute path of the assembly file"<<endl;
    getline(cin,path);
    string obj=ReadFile(path);
    WriteFile(obj);
    return 0;
}
string twosComplement(string bin){
    int n = bin.length();
    int i;

    string ones, twos;
    ones = twos = "";

    for (i = 0; i < n; i++)
        ones += (bin[i] == '0')? '1': '0';

    twos = ones;
    for (i = n - 1; i >= 0; i--){
        if (ones[i] == '1')
            twos[i] = '0';
        else{
            twos[i] = '1';
            break;
        }
    }
    if (i == -1)
        twos = '1' + twos;
    return twos;
}
string decToBinary(int n){
    string binaryNum;

    int i = 0;
    while (n > 0) {
        binaryNum = to_string(n % 2) + binaryNum;
        n = n / 2;
        i++;
    }
    int size = binaryNum.length();
    while(size < 12){
        binaryNum = "0" + binaryNum;
        ++size;
    }
    return binaryNum;
}

string decToHexa(int n){
    string hexa;
    if(n < 0){
        hexa = bin2hex(twosComplement(decToBinary(abs(n))));
    }else {
        while (n != 0) {
            int temp = 0;
            temp = n % 16;
            if (temp < 10) {
                hexa = to_string(temp) + hexa;
            } else {
                hexa = char(temp + 55) + hexa;
            }
            n = n / 16;
        }
    }
    int size = hexa.length();
    while(size < 5){
        hexa = "0" + hexa;
        ++size;
    }
    return hexa;
}

int displacementCalculator(int address, int PC){
    if(abs(address-(PC + 3))>2048){
        errorFlag= true;
        ErrorLines+="Error line: "+to_string(line)+" Pc displacement out of range\n";
        return 0;

    } else{
        pc=true;
        return address-(PC + 3);
    }
}

void labelAdder(string label,int location){
    exists=true;
    if ( symTab.find(label) == symTab.end() ) {
        vector<string> tempo;
        tempo.push_back(to_string(location));
        symTab[label]=tempo;
    }else if(symTab[label].front()=="*"){
        TextRecord+="\n";
        TextRecord.insert(LengthIndex,decToHexa(LengthOfTextRecord/2).substr(3, 2));
        LengthOfTextRecord=0;
        updateObjectCode(decToHexa(location), symTab[label]);
        NoPC=true;
        vector<string> tempo;
        tempo.push_back(to_string(location));
        symTab[label]=tempo;
        exists=false;
    }else {
        errorFlag=1;
        ErrorLines+="Error line: "+to_string(line)+" same label identified twice\n";
        //this means that you used the same label twice which is forbidden
    }
}

string memoryLocationAdder(string identifier,int location){
    if ( symTab.find(identifier) == symTab.end() || symTab[identifier].front()=="*") {
        if ( symTab.find(identifier) == symTab.end()){
            vector<string> temp;
            temp.push_back("*");
            temp.push_back(decToHexa(location));
            symTab[identifier]=temp;
        }
        else{
            symTab[identifier].push_back(decToHexa(location));
        }
        forwardRef = true;
        return "XXXXX";
    } else {
        return symTab[identifier].back();
    }
}

void updateObjectCode(string address, vector<string> appearences){
    list<string> textRecords;
    list<string>::iterator ilist;
    split(TextRecord, "\n", &textRecords);
    int reqAddress, start, length, startAdd, endAdd, i;
    string before, after;
    for(string app : appearences){
        if(app == "*") continue;
        reqAddress = stoi(app, 0, 16);
        for(ilist = textRecords.begin(); ilist != textRecords.end(); ++ilist){
            if (ilist->at(0) == 'T') {
                start = stoi(ilist->substr(1, 6), 0, 16);
                length = stoi(ilist->substr(7, 2), 0, 16);
                if(reqAddress < (start + length)){
                    startAdd = (2 * (reqAddress - start)) + 10;
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
    TextRecord = "";
    for(ilist = textRecords.begin(); ilist != textRecords.end(); ++ilist){
        TextRecord += *ilist+"\n";
    }
    //TextRecord=TextRecord.substr(0,TextRecord.length()-1);
}

string ReadFile(string path)
{
    int size;
    string start="H", sAdd;
    string first,FirstExecutable;
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
        if(container.size()==0)
        {
            ErrorLines+="Error line: "+to_string(line)+" empty line\n";
            ++line;
            continue;
        }
        for(auto & i : container)
            transform(i.begin(), i.end(), i.begin(), ::toupper);
        if(OPTAB.find(container.at(0))!=OPTAB.end())
        {
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
            LOOP:if(NoPC)
            {
                sAdd = decToHexa(PC);
                size = sAdd.length();
                while(size < 6){
                    sAdd = "0" + sAdd;
                    ++size;
                }
                LengthOfTextRecord=0;
                TextRecord+=("T"+sAdd);
                LengthIndex=TextRecord.length();
                if(FirstExecutable.empty())
                    FirstExecutable=sAdd;
                NoPC=false;
            }
            if (isExpression(container.at(1)))
            {
                string temp=expressionCalc(container.at(1));
                if(!temp.empty()) {
                    first="#"+temp;
                    exp = true;
                }
            }
            string TempObj=getObjectCode(container.at(0),first,second,PC);
            if(LengthOfTextRecord+TempObj.length()<=60)
            {
                TextRecord+=TempObj;
                LengthOfTextRecord+=TempObj.length();
            }
            else
            {
                NoPC= true;
                TextRecord.insert(LengthIndex,decToHexa(LengthOfTextRecord/2).substr(3,2));
                LengthOfTextRecord=0;
                TextRecord+="\n";
                goto LOOP;
            }
            PC+=format;
        }
        else if(OPTAB.find(container.at(1))!=OPTAB.end())
        {
            string first=container.at(2),second="";
            if(container.at(2).find(',')!=std::string::npos)
            {
                list<string>temp;
                split(container.at(2),",",&temp);
                ilist = temp.begin();
                first = *ilist;
                advance(ilist, 1);
                second = *ilist;
            }
            labelAdder(container.at(0),PC);
            LOOP2:if(!exists)
            {
                NoPC=false;
                TextRecord+="T";
//                TextRecord.insert(LengthIndex,decToHexa(PC - OldPC));
                exists=true;
            }
            if(NoPC)
            {
                sAdd = decToHexa(PC);
                size = sAdd.length();
                while(size < 6){
                    sAdd = "0" + sAdd;
                    ++size;
                }
                TextRecord+=("T"+sAdd);
                LengthOfTextRecord=0;
                if(FirstExecutable.empty())
                    FirstExecutable=sAdd;
                LengthIndex=TextRecord.length();
                NoPC=false;
            }
            if (isExpression(container.at(2)))
            {
                string temp=expressionCalc(container.at(2));
                if(!temp.empty()) {
                    first="#"+temp;
                    exp = true;
                }
            }
            string TempObj=getObjectCode(container.at(1),first,second,PC);
            if(LengthOfTextRecord+TempObj.length()<=60)
            {
                TextRecord+=TempObj;
                LengthOfTextRecord+=TempObj.length();
            }
            else
            {
                NoPC=true;
                TextRecord+="\n";
                TextRecord.insert(LengthIndex,decToHexa(LengthOfTextRecord/2).substr(3,2));
                LengthOfTextRecord=0;
                goto LOOP2;
            }
            PC+=format;
        }
        else if(DIRECTIVES.find(container.at(1))!=DIRECTIVES.end()&&container.at(1)!="START")
        {
            string str = container.at(1);
            int x = 0;
            labelAdder(container.at(0), (lcCounter) ? lctr : PC);
            /*if(!exists)
            {
                NoPC=false;
//                TextRecord+="\nT";
//                TextRecord.insert(LengthIndex,decToHexa(PC - OldPC));
                OldPC=PC;
            }*/
            if((str.compare("WORD")) == 0){
                if (isExpression(container.at(2)))
                    container.at(2)=expressionCalc(container.at(2));
                if (lcCounter) lctr += 3; else PC += 3;
            }else if((str.compare("RESW")) == 0){
                if (isExpression(container.at(2)))
                    container.at(2)=expressionCalc(container.at(2));
                stringstream Str2Int(container.at(2));
                Str2Int >> x;
                if (lcCounter) lctr += (3*x); else PC += (3*x);
            }else if((str.compare("BYTE")) == 0){
                if (isExpression(container.at(2)))
                    container.at(2)=expressionCalc(container.at(2));
                else if(container.at(2).at(0)=='C')
                {
                    LOOP3:if(NoPC)
                    {
                        sAdd = decToHexa(PC);
                        size = sAdd.length();
                        while(size < 6){
                            sAdd = "0" + sAdd;
                            ++size;
                        }
                        TextRecord+=("T"+sAdd);
                        LengthOfTextRecord=0;
                        if(FirstExecutable.empty())
                            FirstExecutable=sAdd;
                        LengthIndex=TextRecord.length();
                        NoPC=false;
                    }
                    string temp;
                    for (char c:container.at(2).substr(2,container.at(2).size()-3))
                        temp+=decToHexa(c).substr(3,6);
                    if(LengthOfTextRecord+temp.length()<=60)
                    {
                        TextRecord+=temp;
                        LengthOfTextRecord+=temp.length();
                    }
                    else
                    {
                        NoPC=true;
                        TextRecord+="\n";
                        TextRecord.insert(LengthIndex,decToHexa(LengthOfTextRecord/2).substr(3,2));
                        LengthOfTextRecord=0;
                        goto LOOP3;
                    }
                }
                else if(container.at(2).at(0)=='X')
                {
                    LOOP4:if(NoPC)
                {
                    sAdd = decToHexa(PC);
                    size = sAdd.length();
                    while(size < 6){
                        sAdd = "0" + sAdd;
                        ++size;
                    }
                    TextRecord+=("T"+sAdd);
                    LengthOfTextRecord=0;
                    if(FirstExecutable.empty())
                        FirstExecutable=sAdd;
                    LengthIndex=TextRecord.length();
                    NoPC=false;
                }
                    string temp=container.at(2).substr(2,container.at(2).size()-3);
                    if(LengthOfTextRecord+temp.length()<=60)
                    {
                        TextRecord+=temp;
                        LengthOfTextRecord+=temp.length();
                    }
                    else
                    {
                        NoPC=true;
                        TextRecord+="\n";
                        TextRecord.insert(LengthIndex,decToHexa(LengthOfTextRecord/2).substr(3,2));
                        LengthOfTextRecord=0;
                        goto LOOP4;
                    }
                }
                if (lcCounter) lctr += 1; else PC += 1;
            }else if((str.compare("RESBYTE")) == 0) {
                if (isExpression(container.at(2)))
                    container.at(2)=expressionCalc(container.at(2));
                stringstream Str2Int(container.at(2));
                Str2Int >> x;
                if (lcCounter) lctr += x; else PC += x;
            }else if(container.at(1)=="EQU"){
                if (isExpression(container.at(2)))
                    container.at(2)=expressionCalc(container.at(2));
                equHandle(container.at(0), container.at(2));
            }
        }
        else if(DIRECTIVES.find(container.at(0))!=DIRECTIVES.end()||DIRECTIVES.find(container.at(1))!=DIRECTIVES.end())
        {
            if(container.at(1)=="START")
            {
                PC=stoi(container.at(2), 0, 16);
                sAdd = container.at(2);
                size = sAdd.length();
                while(size < 6){
                    sAdd = "0" + sAdd;
                    ++size;
                }
                start+=sAdd;
                startAdd = PC;
            }
            else if(container.at(0)=="END")
            {
                TextRecord.insert(LengthIndex,decToHexa(LengthOfTextRecord/2).substr(3, 2));
                sAdd = decToHexa(PC - startAdd);
                size = sAdd.length();
                while(size < 6){
                    sAdd = "0" + sAdd;
                    ++size;
                }
                start+=sAdd;
                sAdd = container.at(1);
                size = sAdd.length();
                while(size < 6){
                    sAdd += " " ;
                    ++size;
                }
                start.insert(1,sAdd);
            }else if(container.at(0) == "ORG"){
                orgHandle((container.size() == 1) ? "" : container.at(1));
            }
        }
        else{
            ErrorLines+="Error line: "+to_string(line)+" wrong syntax\n";
        }
        splitted.clear();
        container.clear();
        ++line;
    }
    CodeFile.close();
    return start+"\n"+TextRecord+"\nE"+FirstExecutable;
}

void WriteFile(string objcode)
{
    ofstream myfile;
    myfile.open ("Object Code.txt");
    myfile << objcode;
    myfile.close();
    myfile.open ("Error report.txt");
    (ErrorLines.empty())?myfile << "No Errors":myfile << ErrorLines;
    myfile.close();
}

string expressionCalc(string operand){
    list<string> operands;
    list<string>::iterator ilist;
    string a, b;
    if(operand.find('+') != string::npos){
        split(operand, "+", &operands);
        ilist = operands.begin();
        a = *ilist;
        advance(ilist, 1);
        b = *ilist;
        if(checkNumericString(a) && checkNumericString(b)){
            return to_string(stoi(a) + stoi(b));
        }else if(checkNumericString(a) && !checkNumericString(b)){
            if(symTab.find(b) != symTab.end()){
                if(symTab[b].front() == "*"){
                    errorFlag = true;
                    ErrorLines+="Error line: "+to_string(line)+" Forward reference in expression not allowed\n";
                    return "";
                }
                return to_string(stoi(a) + stoi(symTab[b].front()));
            }else{
                errorFlag = true;
                ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
                return "";
            }
        }else if(!checkNumericString(a) && checkNumericString(b)){
            if(symTab.find(a) != symTab.end()){
                if(symTab[a].front() == "*"){
                    errorFlag = true;
                    ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
                    return "";
                }
                return to_string(stoi(b) + stoi(symTab[a].front()));
            }else{
                errorFlag = true;
                ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
                return "";
            }
        }else{
            errorFlag = true;
            ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
            return "";
        }
    }else if(operand.find('-') != string::npos){
        split(operand, "-", &operands);
        ilist = operands.begin();
        a = *ilist;
        advance(ilist, 1);
        b = *ilist;
        if(checkNumericString(a) && checkNumericString(b)){
            return to_string(stoi(a) - stoi(b));
        }else if(checkNumericString(a) && !checkNumericString(b)){
            if(symTab.find(b) != symTab.end()){
                if(symTab[b].front() == "*"){
                    errorFlag = true;
                    ErrorLines+="Error line: "+to_string(line)+" forward reference in expressions not allowed\n";
                    return "";
                }
                return to_string(stoi(a) - stoi(symTab[b].front()));
            }else{
                errorFlag = true;
                ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
                return "";
            }
        }else if(!checkNumericString(a) && checkNumericString(b)){
            if(symTab.find(a) != symTab.end()){
                if(symTab[a].front() == "*"){
                    errorFlag = true;
                    ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
                    return "";
                }
                return to_string(stoi(b) - stoi(symTab[a].front()));
            }else{
                errorFlag = true;
                ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
                return "";
            }
        }else{
            if(symTab.find(a) == symTab.end() || symTab.find(b) == symTab.end() ||
            symTab[a].front() == "*" || symTab[b].front() == "*"){
                errorFlag = true;
                ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
                return "";
            }
            return to_string(stoi(symTab[a].front()) - stoi(symTab[b].front()));
        }
    }else if(operand.find('*') != string::npos){
        split(operand, "*", &operands);
        ilist = operands.begin();
        a = *ilist;
        advance(ilist, 1);
        b = *ilist;
        if(checkNumericString(a) && checkNumericString(b)) return to_string(stoi(a) * stoi(b));
        errorFlag = true;
        ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
        return "";
    }else if(operand.find('/') != string::npos){
        split(operand, "/", &operands);
        ilist = operands.begin();
        a = *ilist;
        advance(ilist, 1);
        b = *ilist;
        if(checkNumericString(a) && checkNumericString(b)){
            if(stoi(b) == 0){
                errorFlag = true;
                ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
                return "";
            }
            return to_string(stoi(a) / stoi(b));
        }
        errorFlag = true;
        ErrorLines+="Error line: "+to_string(line)+" wrong expression format\n";
        return "";
    }
}

bool isExpression(string operand){
    if(operand.find('/') != string::npos || operand.find('*') != string::npos
    || operand.find('+') != string::npos || operand.find('-') != string::npos){
        return true;
    }
    return false;
}

bool checkNumericString(string s){
    for(int i = 0; i < s.length(); i++) {
        if (!isdigit(s.at(i))) {
            return false;
        }
    }
    return true;
}

void orgHandle(string operand){
    if(operand == ""){
        lcCounter = false;
    }else{
        map<string, vector<string>>::iterator imap = symTab.find(operand);
        if(imap == symTab.end() || symTab[operand].front() == "*"){
            errorFlag = true;
            ErrorLines+="Error line: "+to_string(line)+" ORG with forward reference\n";
            return;
        }
        lcCounter = true;
        lctr = stoi(symTab[operand].front());
    }
}

void equHandle(string name, string value){
    map<string, vector<string>>::iterator imap = symTab.find(value);
    if(imap != symTab.end()){
        if(symTab[value].front() == "*"){
            errorFlag = true;
            ErrorLines+="Error line: "+to_string(line)+" EQU with forward reference\n";
        }else {
            symTab[name] = symTab[value];
        }
    }else{
        errorFlag = false;
        for(int i = 0; i < value.length(); i++) {
            if (!isdigit(value.at(i))) {
                errorFlag = true;
                ErrorLines+="Error line: "+to_string(line)+" EQU with no digit as opperand\n";
                break;
            }
        }
        if(!errorFlag){
            symTab[name] = symTab[value];
        }
    }
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
        extended = true;
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
                    ErrorLines+="Error line: "+to_string(line)+" wrong register symbol\n";
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
                    ErrorLines+="Error line: "+to_string(line)+" wrong register symbol\n";
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
        ErrorLines+="Error line: "+to_string(line)+" error getting opcode\n";
        return "";
    }
}

string flags(string operand, const string& indexed){
    string nixbpe;

    // n i
    if(operand[0] == '@'){
        nixbpe += "10";
    }else if(operand[0] == '#'){
        if(operand[1] >= '0' && operand[1] <= '9'){
            pc = false;
        }
        if(exp) nixbpe += "11"; else nixbpe += "01";
        exp = false;
    }else{
        nixbpe += "11";
    }
    // x
    nixbpe += indexed;

    if(pc){
        nixbpe += "01";
    }else{
        nixbpe += "00";
    }
    if(extended){
        nixbpe += "1";
    }else{
        nixbpe += "0";
    }
    return nixbpe;
}

string getObjectCode(string opcode, string op1, string op2, int PC) {
    format = 0;
    extended = false;
    pc = true;
    forwardRef = false;
    string opobcode = getObjectOpcode(opcode, op1, op2);
    if (errorFlag) {
        ErrorLines+="Error line: "+to_string(line)+" Error can't get opcode\n";
        return "";
    }
    if (format == 2) {
        return opobcode;
    }
    if (format == 1) {
        format = 3;
        return opobcode + "0000";
    }
    string ob;
    if (format == 3 || format == 4) {
        if(op1[0] == '@'){
            op1.erase(0, 1);
        }
        if(op1[0] == '#'){
            op1.erase(0, 1);
            if(op1[0] >= '0' && op1[0] <= '9'){
                ob = decToHexa(stoi(op1));
                return (format == 3) ? opobcode + ob.substr(2, 3) : opobcode + ob;
            }else {
                ob = memoryLocationAdder(op1, PC + 1);
            }
        }else{
            ob = memoryLocationAdder(op1, PC + 1);
        }
        if (forwardRef || format == 4) {
            if (format == 3) {
                string op = hex2bin(opobcode);
                op.at(10) = '0';
                op.at(11) = '1';
                opobcode = bin2hex(op);
            }
            format = 4;
            int size = ob.length();
            while(size < 5){
                ob = "0" + ob;
                ++size;
            }
            return opobcode + ob;
        } else {
            ob = decToHexa(displacementCalculator(stoi(ob), PC));
            ob = ob.substr(2, 3);
            return opobcode + ob;
        }
    }
}