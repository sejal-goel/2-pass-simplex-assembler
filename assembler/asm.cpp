/* 
Declaration of Authorship :
Name : Sejal Goel 
Roll no. : 2401CS17
*/

#include <bits/stdc++.h>
#include <iomanip>
#include <algorithm>
#include <fstream> 
#include <iostream> 
#include <vector> 
#include <cctype >
#include <map> 
#include <string> 
#include <sstream> 
#include <stdexcept>
using namespace std;

void pass1(std :: string file);
void pass2();

// lookup table for operations 
map <string ,pair <string , int > > optable ; 
void opcodetable(){
    /* (mnemonic = opcode  || operand )
    operand:
    0 -> nothing required 
    1 -> value required 
    2 -> offset required 
    */
    optable["ldc"]={"00",1};
    optable["adc"]={"01",1};
    optable["ldl"]={"02",2};
    optable["stl"]={"03",2};
    optable["ldnl"]={"04",2};
    optable["stnl"]={"05",2};
    optable["add"]={"06",0};
    optable["sub"]={"07",0};
    optable["shl"]={"08",0};
    optable["shr"]={"09",0};
    optable["adj"]={"0A",1};
    optable["a2sp"]={"0B",0};
    optable["sp2a"]={"0C",0};
    optable["call"]={"0D",2};
    optable["return"]={"0E",0};
    optable["brz"]={"0F",2};
    optable["brlz"]={"10",2};
    optable["br"]={"11",2};
    optable["HALT"]={"12",0};
    optable["SET"]={"",1};    
    optable["data"]={"",1};
}

/*errors and warnings collected during both passes*/
vector <string > error_list;
#define el error_list
#define pb push_back
vector <string> warn_list;

struct Line{
    int pc;/* program counter (-1 for no instruction)*/
    string label ;
    string mnm; /*mnemonic */
    string opr ; /* operand */
    int lc; /* line number (1 based )*/
};

vector <Line> records; /* all lines with their details in order*/
vector <string > mcodes; /* machine codes in hexadecimal */

/*symbol table*/  
map <string , int > symtable;

/*function to remove the whitespace at the start and end of a line */
string trim(const string &s ){
    size_t a= s.find_first_not_of(" \t\r\n");
    if(a==string::npos){
        return "";
    }
    size_t b=s.find_last_not_of("\t\n\r ");
    return s.substr(a,b-a+1);
}

string tohex8(unsigned int n){
    static const char digits[]="0123456789ABCDEF";
    char buff[9];
    buff[8]='\0';
    for(int i=7 ;i>=0;i--){
        buff[i]=digits[n & 0xF];
        n = n >> 4 ;

    }
    return string(buff);
}

// function hex to 32bit 
unsigned int  hexto32bit(const string &s){
    unsigned int val =0;
    for(size_t i=0;i<s.size();i++){
        char c=s[i];
        val*=16;
        if(isdigit(c)){
            val+=c-'0';
        }else if(c>='A' && c<='F'){
            val+=c-'A'+10;
            
        }else if(c>='a' && c<='f'){
            val+=c-'a'+10;
        }
    }
    return val;

}

// function to check if the extension is .asm 
bool valid_filename(const string &File){
    if(File.size()<5) return false;
    string ext= File.substr(File.size()-4);
    return (ext[0]=='.' && ext[1]=='a' && ext[2]=='s' && ext[3]=='m');
}
bool splitline(string &line ,string &label , string &mnm, string &opr ){
    vector <string > parts;
    stringstream ss(line);
    string part;
    while(ss>> part){
        parts.pb(part);
    }
    if(parts.empty()){
        return true;
    }
    size_t idx =0;
    string s=parts[idx];
    size_t colonPos = s.find(':');
    if (colonPos != string::npos) {
        label = s.substr(0, colonPos);
        string remainder = s.substr(colonPos + 1);
        idx++;
    if (!remainder.empty()) {
        parts.insert(parts.begin() + idx, remainder);
    }
}
    if(idx < parts.size()){
        mnm=parts[idx];
        idx++;
    }
    if(idx < parts.size()){
        opr=parts[idx];
        idx++;
    }
    if(idx < parts.size()){
        return false;
    }
    return true;

}

bool validlabel(const string & label){
    if(label.empty()) return false;
    if(!isalpha((unsigned char)label[0])){
        return false;/*first char of label should be an alphabet */
    }
    for(size_t i=1;i<label.size();i++){
        unsigned char c =label[i];
        if(!isalpha(c) && !isdigit(c) && c!='_'){
            return false; /*other letters can only be 0-9 ,alphabets, underscore */
        }
    }
    return true;
}
bool isDecimal(const string &s ){
    if(s.empty()) {
        return false;
    }
    size_t i=0;
    if(s[i]=='+' || s[i] =='-'){ // sign of decimal
        i++;
    }
    if(i==s.size()) return false;
    for(;i< s.size();i++){
        if(!isdigit((unsigned char)s[i])){
            return false;
        }
    }
    return true;

}

bool ishex(const string &s ){
    if(s.size()<3) {
        return false;
    }
    if(s[0]!='0' ||  (s[1]!='x' && s[1]!='X')){
        return false;
    }
    for(size_t i=2; i<s.size();i++){
        if(!isxdigit((unsigned char)s[i])){
            return false;
        }
    }
    return true;

}

bool isOctal(const string &s ){
    if(s.empty()) {
        return false;
    }
    if(s[0]!='0'){
        return false;
    }
    for(size_t i=1; i<s.size();i++){
        if(s[i]<'0' || s[i]>'7'){
            return false;
        }
    }
    return true;
}

bool isnum(const string &s){
    return isDecimal(s) || ishex(s)  || isOctal(s);
}

// function takes a number string and converts it to plain C++ int 
bool strToInt(const string &line , int &res ){
    string s ;
    s=trim(line); // clean the input 
    if(s.empty()){
        return false;
    }
    bool neg=false;
    size_t st=0; /* start*/
    if(s[0]=='+' || s[0]=='-'){
        if(s[0]=='-'){
            neg=1;
        }
        st=1;
    }
    string body=s.substr(st);
    if(body.empty()){
        return false;
    }
    long long val=0;
    if( ishex(body)){
        for(size_t i=2 ;i<body.size();i++){
            val=val*16;
            char c=body[i];
            if(isdigit(c)){
                val+=c-'0';
            }else if(c>='a' && c<='f'){
                val+=c-'a' +10;
            }else if(c>='A' && c<='F'){
                val+=c-'A' +10;
            }else{
                return false;
            }
        }
    }else if(body.size()>1 && body[0]=='0'){
        for(size_t i=0;i<body.size();i++){
            if(body[i]<'0' || body[i] >'7'){
                return false;
            }
            val=val*8 +(body[i]-'0');
        }
    }else{
        for(size_t i=0;i<body.size();i++){
            if(!isdigit((unsigned char) body[i])){
                return false;
            }
            val=val*10 +(body[i]-'0');
        }
    }
    if(neg){
        val=val*(-1);
    }
    res=(int)val;
    return true;
}


void pass1 (string File){

    ifstream file(File); // open file 
    int pc=0 ;  // program counter 
    int lc =0 ; // line counter 

    if(!file.is_open()){ // file does not exist 
        cout << "Error: Trouble opening the file." << "'"<< File << "'"<<  endl;
        return;
    }

    string line; // current line
    while(getline(file, line)){ // get the line from file 
        lc++;
        size_t semipos =line.find(';'); // position of ; 
        if(semipos != string::npos){
            line =line.substr(0,semipos); // removed comments 
        }
        if(line.empty()){
            continue;
        }
        line=trim(line);
        if(line.empty()){
            continue;
        }
        string label="", mnm="", opr="" ; /* label , mnemonic , operand */
        bool valid = splitline(line, label , mnm, opr);
        if(!valid){
            error_list.push_back("Line " + to_string(lc) + ": Extra text at the end of the line");
        }

        /* Label handling */
        if(!label.empty()){
            if(!validlabel(label)){
                error_list.push_back("Line " + to_string(lc) +": Invalid label name "+ "'" + label + "'" );
                label="";
            }else if(symtable.count(label)){
                error_list.pb("Line " + to_string(lc) + ": Duplicate label '" + label + "'" );
                label="";
            }else{
                symtable[label]=pc; /* may be overwritten by SET */
            }
        }

        /* Mnemonic handling */
        if(mnm.empty()){  /*label only line*/
           Line r ;
           r.pc=-1;
           r.label=label;
           r.mnm="";
           r.opr="";
           r.lc=lc;
           records.pb(r);continue;

        }

        /* check mnemonic exists */
        if(!optable.count(mnm)){
            el.pb("Line" + to_string(lc) + ": UNknown mnemonic '"+mnm +"'" );
            Line r ;
            r.pc=-1;
            r.label=label;
            r.mnm=mnm;
            r.opr=opr;
            r.lc=lc;
            records.pb(r);
            continue;
        }

        if(mnm=="SET"){
            if(label.empty()){
                el.pb("Line "+ to_string(lc) +": SET requires a label ");
            }else if(opr.empty()){
                el.pb("Line " + to_string(lc) +": SET requires a numeric operand ");
            }else {
                int val=0;
                if(!strToInt(opr,val)){
                    el.pb("Line "+to_string(lc)+ ": SET operand '"+opr+ "' is not a valid number");
                }else{
                    symtable[label]=val; /*overwrite  with constant*/
                }
            }
            Line r ;
            r.pc=-1;
            r.label=label;
            r.mnm=mnm;
            r.opr=opr;
            r.lc=lc;
            records.pb(r);
            continue;
        }
        
        int optype=optable[mnm].second;
        switch(optype){
            case 0:
                if(!opr.empty()){
                    el.pb("Line" +to_string(lc)+": '"+ mnm +"' takes no operand");

                }
                break;
            default:
                if(opr.empty()){
                    el.pb("Line " +to_string(lc)+ ": '" + mnm +"' requires an operand ");

                }

        }


        Line r ;
        r.pc=pc;
        r.label=label;
        r.mnm=mnm;
        r.opr=opr;
        r.lc=lc;
        records.pb(r);
        pc++;
    }
    file.close();
}

void pass2(){
    map<string ,bool> labelUsed; /*  track if the label is used or not(warn about unused labels)*/
    for(auto &it : symtable){
        labelUsed[it.first]=false;

    }
    for(auto &r : records){
        if(r.mnm.empty() || r.pc<0){
            continue;
        }
        if(!optable.count(r.mnm)){
            continue;
        }
        int optype=optable[r.mnm].second;
        string opcHex=optable[r.mnm].first; /* opcode in hex */
        int lc=r.lc ;
        unsigned int word=0;
        bool skip =0; /* flag to skip mcodes.pb() at the bottom */

        switch(optype){
            case 0:{ /* operand=0 */
                unsigned int opc = hexto32bit(opcHex);
                word=opc & 0xFF;
                break;
            }
            case 1:{
                int val=0;
                if(r.opr.empty()){
                    mcodes.pb("00000000"); /*Already reported in pass1*/
                    skip=1;
                    break;
                }
                if(validlabel(r.opr) && !isnum(r.opr)){ /* operand is a label name */
                    if(!symtable.count(r.opr)){
                        el.pb("Line " +to_string(lc) +": Undefined label '"+ r.opr +"'");
                        mcodes.pb("00000000");
                        skip=1;
                        break;
                    }
                
                labelUsed[r.opr]=1;
                int addr=symtable[r.opr];
                
                val=addr; // different in case 1 and case 2 
                }else{
                    if(!strToInt (r.opr,val)){
                        el.pb("Line " +to_string(lc)+ ": Invalid operand ' " +r.opr +"'");
                        mcodes.pb("00000000");
                        skip=1;
                        break;
                    }

                }
                unsigned int opc=0;
                if(r.mnm!="data"){
                    opc=hexto32bit(opcHex);
                }
                word=((unsigned int)val << 8 ) |(opc &0xFF);
                break;
            }
            case 2:{
                int val=0;
                if(r.opr.empty()){
                    mcodes.pb("00000000"); /*Already reported in pass1*/
                    skip=1;
                    break;
                }
                if(validlabel(r.opr) && !isnum(r.opr)){ /* operand is a label name */
                    if(!symtable.count(r.opr)){
                        el.pb("Line " +to_string(lc) +": Undefined label '"+ r.opr +"'");
                        mcodes.pb("00000000");
                        skip=1;
                        break;
                    }
                
                labelUsed[r.opr]=1;
                int addr=symtable[r.opr];

                val= addr-(r.pc+1);  // different in case 1 and case 2 

                }else{
                    if(!strToInt (r.opr,val)){
                        el.pb("Line " +to_string(lc)+ ": Invalid operand ' " +r.opr +"'");
                        mcodes.pb("00000000");
                        skip=1;
                        break;
                    }
                }   
                unsigned int opc=0;
                if(r.mnm!="data"){
                    opc=hexto32bit(opcHex);
                }
                word=((unsigned int)val << 8 ) |(opc & 0xFF);
                break;
            }
            default:
                break;
        }
        if(!skip){
            mcodes.pb(tohex8(word));
        }
    }

    /* warning about unused labels */
    for(auto &it :labelUsed){
        if(!it.second){
            int loc=-1; /* Line where the label is declared */
            for(auto &r :  records){
                if(r.label==it.first){
                    loc=r.lc;
                    break;
                }
            }
            string location="";
            if(loc>0){
                location="Line"+to_string(loc) +": ";

            }
            warn_list.pb(location+"Warning: Label '"+it.first +"' declared but never used");

        }
    }


}

void writeLog(const string &base)
{
    string logname = base + ".log";
    ofstream log(logname.c_str());
    if (!log.is_open()) {
        cerr << "Error: cannot create log file '" << logname << "'" << endl;
        return;
    }

    log << "Assembler Log : " << base << " " << endl;
    log << endl;

    if (error_list.empty()) {
        log << "No errors." << endl;
    } else {
        log << "ERRORS (" << error_list.size() << "):" << endl;
        for (size_t i = 0; i < error_list.size(); i++)
            log << "  " << error_list[i] << endl;
    }

    log << endl;

    if (warn_list.empty()) {
        log << "No warnings." << endl;
    } else {
        log << "WARNINGS (" << warn_list.size() << "):" << endl;
        for (size_t i = 0; i < warn_list.size(); i++)
            log << "  " << warn_list[i] << endl;
    }

    log.close();
    cout << "Log file written : " << logname << endl;
}

void writeListing(const string &base)
{
    string lstname = base + ".lst";
    ofstream lst(lstname.c_str());
    if (!lst.is_open()) {
        cerr << "Error: cannot create listing file '" << lstname << "'" << endl;
        return;
    }

    lst << "; Listing file : " << base << ".asm" << endl;
    lst << "; " << setw(8) << left << "PC"
        << "  " << setw(8) << left << "MachCode"
        << "  Source" << endl;
    lst << "; " << string(60, '-') << endl;

    /* we need to iterate records in original order and pick up mcodes */
    size_t mcIdx = 0; /* index into mcodes[] */

    for (size_t i = 0; i < records.size(); i++) {
        const Line &r = records[i];

        /* determine the display strings */
        string pcStr   = "        "; /* 8 spaces */
        string mcStr   = "        ";

        bool hasCode = (r.pc >= 0 && !r.mnm.empty() &&
                        optable.count(r.mnm) &&
                        r.mnm != "SET");

        if (hasCode) {
            pcStr = tohex8((unsigned int)r.pc);
            if (mcIdx < mcodes.size()) {
                mcStr = mcodes[mcIdx];
                mcIdx++;
            }
        }

        /* rebuild a readable source representation */
        string srcDisplay = "";
        if (!r.label.empty())    srcDisplay += r.label + ":  ";
        if (!r.mnm.empty()) srcDisplay += r.mnm;
        if (!r.opr.empty())  srcDisplay += "  " + r.opr;

        lst << "  " << pcStr << "  " << mcStr << "  " << srcDisplay << endl;
    }

    lst.close();
    cout << "Listing file written : " << lstname << endl;
}

void writeObject(const string &base)
{
    if (!error_list.empty()) {
        cout << "Object file NOT written due to errors." << endl;
        return;
    }

    string oname = base + ".o";
    ofstream obj(oname.c_str(), ios::binary);
    if (!obj.is_open()) {
        cerr << "Error: cannot create object file '" << oname << "'" << endl;
        return;
    }

    for (size_t i = 0; i < mcodes.size(); i++) {
        /* convert 8-digit hex string to a 32-bit unsigned int */
        unsigned int word = 0;
        for (int j=0;j<8;j++) {
            word <<= 4;
            char c= mcodes[i][j];
            if (isdigit(c)){
                word |= (unsigned int)(c-'0');
            }
            else if (c >= 'A' && c <= 'F'){
                word=word |(unsigned int)(c-'A'+10);
            }
            else if (c >= 'a' && c <= 'f'){
                word=word | (unsigned int)(c-'a'+10);
            } 
        }
        
        /* write 4 bytes, little-endian (byte 0 = least significant) */
        unsigned char bytes[4];
        bytes[0]= (unsigned char)( word & 0xFF);
        bytes[1]= (unsigned char)((word >>  8) & 0xFF);
        bytes[2]= (unsigned char)((word >> 16) & 0xFF);
        bytes[3]= (unsigned char)((word >> 24) & 0xFF);
        obj.write((const char *)bytes, 4);
    }

    obj.close();
    cout << "Object file written : " << oname << endl;
}


int main(int argc , char* argv[]){ // reads from the outside files (count , arrays of the strings typed )

    /* input file */
    string File;
    if(argc < 2 ){
        cout << "Enter file name (.asm extention )" << endl;
        getline(cin ,File);
        File=trim(File);
    }else {
        File=argv[1] ;
    }

    if(File.empty()){
        cout << "No file name given " << endl;
        return 1;
    }

    if(!valid_filename(File)){
        cout << "Error: The file entered is not a .asm file" << endl;
        return 1;
    }

    /* base name for test.o , test.log , test.lst */
    size_t lastdot=File.find_last_of('.');
    string base;
    if(lastdot==string::npos){
        base=File;
    }else{
        base= File.substr(0,lastdot);
    }

    
    /* initialise the opcode table */ 
    opcodetable();

    /* pass 1 */
    cout << "Pass 1:" << endl;
    pass1(File);
    /* pass 2 */
    cout << "Pass 2 : " << endl;
    pass2();

    writeLog(base);
    writeListing(base);
    if(!error_list.empty()){
        cout << error_list.size() << "Error(s) found."<<endl;
        cout << "Errors logged in " << base << ".log" << endl;
        return 1 ;
    }

    if(!warn_list.empty()){
        cout << warn_list.size() << " " << "warning(s)" << endl;
        cout << "Warnings logged in " << base << ".log" << endl;
    }
    writeObject(base);
    cout << "Assembly successful" << endl;
    cout << mcodes.size() << "words processed " << endl;
    return 0;
}

