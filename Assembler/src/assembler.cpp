#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <regex>

using namespace std;

//============== Global Variable =========================================

map<string, string> OPTAB;
bool extended = 0;
bool pc = 1;
bool b = 0;
map<string, vector<string>> symTab;
string current="";
bool errorFlag=0;

//============== End Of Global Variable ==================================

//============== Functions declarations ==================================

void split(string str, string seperator, list<string> * strings);
//string getObjectOpcode(string opcode, list<string> * operands);
string hex2bin(string hexa);
string bin2hex(string bin);
void setOPTAB();
void memoryLocationAdder(string identifier,int location);
void labelAdder(string label,int location);

//============== End Of Functions declarations ===========================

int main() {
	setOPTAB();
	return 0;
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

void split(string str, string seperator, list<string> * strings){
	regex rgx(seperator);
	sregex_token_iterator iter(str.begin(),str.end(), rgx, -1);
	sregex_token_iterator end;
	for ( ; iter != end; ++iter)
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
//string getObjectOpcode(string opcode, list<string> * operands){
//	string ob;
//	if(opcode[0] == '+'){
//		extended = 1;
//		b = pc = 0;
//		opcode.erase(0, 1);
//	}
//	map<string, string>::iterator imap = OPTAB.find(opcode);
//	if(imap != OPTAB.end()){ // valid opcode
//		ob = imap->second;
//
//		if(operands->size() == 0){ // object code for this opcode is in 2 digits
//			return ob;
//		}
//
//		if(operands->size() == 2){
//			list<string>::iterator ilist = operands->begin();
//			advance(ilist, 1);
//			string second = *ilist;
//			if(second != "X"){ // object code for this opcode is in 2 digits
//				return ob;
//			}
//		}
//
//		// object code for this opcode is in 3 digits
//		string bin = hex2bin(ob);
//		bin = bin.substr(0, bin.length() - 2);
//		list<string>::iterator ilist = operands->begin();
//		string first = *ilist;
//		// set ni of nixbpe
//		if(first[0] == '@'){
//			bin += "10";
//		}else if(first[0] == '#'){
//			bin += "01";
//		}else{
//			bin += "11";
//		}
//		// set x of nixbpe
//		if(operands->size() == 2){
//			bin += "1";
//		}else{
//			bin += "0";
//		}
//		// set b of nixbpe
//		if(b){
//			bin += "1";
//		}else{
//			bin += "0";
//		}
//		// set p of nixbpe
//		if(pc){
//			bin += "1";
//		}else{
//			bin += "0";
//		}
//		// set e of nixbpe
//		if(extended){
//			bin += "1";
//		}else{
//			bin += "0";
//		}
//
//		ob = bin2hex(bin);
//
//	}else{
//		return "Wrong OPCODE";
//	}
//	return ob;
//}
