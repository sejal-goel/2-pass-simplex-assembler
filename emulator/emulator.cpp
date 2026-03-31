/* 
Declaration of Authorship :
Name : Sejal Goel 
Roll no. : 2401CS17
*/
#include<bits/stdc++.h>
#include <limits>
#include <iomanip>
#include <fstream> 
#include <iostream> 
#include <vector> 
#include <map> 
#include <string> 
#include <sstream> 
#include <stdexcept>
using namespace std;

int a=0, b=0, sp=0 , pc=0 ;
int pctotal=0;
vector <uint32_t> memory(1 << 24,0);/*16MB memory (Address space up to 2^24)*/ 
int maxwritten=0; /* highest memory address written during execution*/
int minwritten=-1; /* lowest memory address written beyond code */ 

/*function to remove the whitespace at the start and end of a line */
string trim(const string &s ){
    size_t a= s.find_first_not_of(" \t\r\n");
    if(a==string::npos){
        return "";
    }
    size_t b=s.find_last_not_of("\t\n\r ");
    return s.substr(a,b-a+1);
}

/*converts to 8 digit hex string 8*/
string tohex(int n){
        stringstream ss ;
        ss << hex << setw(8) << setfill('0') << (static_cast <uint32_t>(n));
        string str=ss.str();
        for(int i=0;i<(int)str.size();i++){
                if(str[i]>='a' && str[i]<= 'f' ){
                        str[i]+='A'-'a';
                }
        }
        return str;
}

/* display current registers*/ 
void printreg(string mnm , string opr){
        cout << "A:" << tohex(a) <<"\t";
        cout <<"B:" << tohex(b) <<  "\t";
        cout  <<"SP:"<< tohex(sp) << "\t";
        cout  << "PC:" <<tohex(pc-1)  << endl; // we do pc-1 because the pc is pointing to the next instruction by now 
        cout << left << setw(5) << mnm << opr << endl;
}

/* print code region(loaded instructions) and data blocks(stacks and arrays)*/
void printdata(ostream &out){
        out << endl;
        out << "Code Region: addresses 0 to " << pctotal-1 << endl;
        out << "Address   || "<< endl ;

        for(int i=0; i<pctotal;){
                out << tohex(i) ;
                out << "  ||  " << tohex(memory[i]) ;
                if(i+1<pctotal){
                out << "  ||  " << tohex(memory[i+1]);                                                
                }
                if(i+2<pctotal){     
                out << "  ||  " << tohex(memory[i+2]);
                }     
                if(i+3<pctotal){     
                out << "  ||  " << tohex(memory[i+3]);
                }     
                out << endl;
                i=i+4;
        } 

        if(maxwritten <= pctotal - 1 || minwritten==-1){
        out << "No data written beyond code" << endl;
        return;
        }

        out << endl;
        out << "Data Region: "  << minwritten <<" to " << maxwritten   << endl;
        out << "Address   || Value     || (decimal) "<< endl ;

        int i=minwritten;
        while(i <= maxwritten){
                if(memory[i] ==0){/* count how many zeros in a row */
                        int zeroSt = i;
                        while(i <= maxwritten && memory[i] == 0){
                                i++;
                        }

                        int zeroCnt = i - zeroSt;
                        if(zeroCnt <=8){        /* small gap — print it so boundary zeros show */
                                for(int j = zeroSt; j < i; j++){
                                        out << tohex(j) << "  |  " ;
                                        out << tohex(0) << "  |  (0)" ;
                                        out << endl;
                                }
                        } else {        /* large gap — skip and show separator */
                                out << zeroCnt<< " zero addresses skipped: " << zeroSt << " to " << i-1 << endl;
                        }
                } else {
                        out << tohex(i) << "  |  "
                             << tohex((int)memory[i]) << "  |  ("
                             << (int)memory[i] << ")" << endl;
                        i++;
                }
        }

}

/* main emulator logic*/ 
bool run_emulator(bool trace , bool step , ofstream &error){
        a=0, b=0, sp=0, pc=0;
        bool halt=false;
        int ln=0; 
        maxwritten=0; minwritten=-1;
        while(pc < pctotal){
                ln++;
                if(ln > 1000000){
                        cout << "Error : Infinite loop detected (>10^6 instructions )" << endl;
                        error << "Runtime error: Infinite loop at PC=" << tohex(pc) << endl;
                        break;
                }
                /*fetch*/
                uint32_t instruction=memory[pc];
                int opcode= instruction & 0xFF; /*8 bit mask (last 8 bits remain same latter becomes 0)*/
                int operand= static_cast <int > (instruction) >>  8;
                /*sign-extention*/
                if(operand & 0x800000){
                        operand |= 0xFF000000;
                }

                int prevPC=pc;
                pc++;
                string mnm="";

                switch(opcode){
                        case 0:     
                                mnm="ldc";
                                b=a;
                                a=operand;
                                break;
                        case 1:    
                                mnm="adc"; 
                                if((a>0&& operand >INT_MAX-a) || (a<0 && operand<INT_MIN -a )){
                                        error <<"Error at PC  " << tohex(prevPC) << ": Integer overflow " << endl;
                                        return false;
                                }
                                a+=operand;
                                break;
                        case 2:    
                                mnm="ldl";
                                b=a;
                                if((sp+operand)<0 || (sp+operand)>=(int)memory.size()){
                                        cout<< "Error at PC  " << tohex(prevPC) << ": Index out of bounds " << endl;
                                        return false;
                                }
                                a=(int)memory[sp+operand];
                                break;
                        case 3: 
                                mnm="stl";
                                if((sp+operand)<0 || sp+operand>=(int)memory.size()){
                                        cout<< "Error at PC  " << tohex(prevPC) << ": Index out of bounds " << endl;
                                        return false;
                                }
                                memory[operand+sp]=a;
                                if(sp+operand>maxwritten ){
                                        maxwritten=sp+operand;
                                }
                                if(sp+operand >= pctotal && (minwritten==-1 || sp+operand< minwritten)){
                                        minwritten= sp+operand;
                                }
                                a=b;
                                break;
                        case 4 : 
                                mnm="ldnl";
                                if((a+operand <0) || (a+operand)>=(int)memory.size()){
                                        cout<< "Error at PC  " << tohex(prevPC) << ": Index out of bounds " << endl;
                                        return false;                                       
                                }
                                a=memory[operand+a];
                                break;
                        case 5 : 
                                mnm="stnl";
                                if((a+operand <0) || (a+operand)>=(int)memory.size()){
                                        cout<< "Error at PC  " << tohex(prevPC) << ": Index out of bounds " << endl;
                                        return false;                                       
                                }
                                if(a+operand>maxwritten ){
                                        maxwritten=a+operand;
                                }
                                if(a+operand >= pctotal && (minwritten==-1 || a+operand<minwritten)){
                                        minwritten= a+operand;
                                }                               
                                memory[operand+a]=b;
                                break;
                        case 6 : 
                                mnm="add";
                                if((a<0 && b>INT_MAX+a)||(a>0 && b<INT_MIN+a)){
                                        error<<"Error at PC=  "  << tohex(prevPC) << ": Integer overflow" <<endl;
                                        return false;
                                }
                                a=b+a;
                                break;
                        case 7 :
                                mnm="sub";
                                if((a<0 && b>INT_MAX+a)||(a>0 && b<INT_MIN+a)){
                                        error<<"Error at PC=  "  << tohex(prevPC) << ": Integer overflow" <<endl;
                                        return false;
                                }                               
                                a=b-a;
                                break;
                        case 8 : 
                                mnm="shl";
                                a=b<< a;
                                break;
                        case 9: 
                                mnm="shr"; 
                                a=b >> a;
                                break;
                        case 10: 
                                mnm="adj";
                                sp+=operand;
                                break;
                        case 11: 
                                mnm="a2sp";
                                sp=a;
                                a=b;
                                break;
                        case 12: 
                                mnm="sp2a";
                                b=a;
                                a=sp;
                                break;
                        case 13: 
                                mnm="call";
                                b=a;
                                a=pc;
                                pc=prevPC +1 +operand ;
                                break;
                        case 14 : 
                                mnm="return";
                                pc=a;
                                a=b;
                                break;
                        case 15: 
                                mnm="brz";

                                if(a==0){
                                        pc=prevPC+1+operand;
                                }
                                break;
                        case 16: 
                                mnm="brlz";
                                if(a<0){
                                        pc=prevPC +operand+1;
                                }
                                break;
                        case 17: 
                                mnm ="br";
                                pc =prevPC+operand+1;
                                break;
                        case 18: 
                                mnm="HALT";
                                halt=true;
                                break;
                        default: 
                                mnm="data";
                                break;

                }
                if(trace){
                        printreg(mnm,to_string(operand));
                        if(step ){
                                cout << "[Press Enter to Step , 'q' to MENU: ]";
                                string ch;
                                if(!(cin >> ws && getline(cin , ch))) return false;
                                if(ch=="q" || ch=="Q"){
                                        cout << "Returning to main menu" << endl;
                                        return false;
                                }
                        }
                }
                if(halt) break;


        }
        return halt;
        
}
int main(int argc , char* argv[]){

        string File;
        if(argc<2){
                cout << "Enter program file (.o extension)" << endl;
                getline(cin ,File);
                File=trim(File);
        }else{
                File=argv[1];
        }
        if(File.empty()){
                cout << "No file name given" << endl;
                return 1;
        }
        /*LOADING THE INSTRUCTIONS IN THE VECTOR AND GETTING THE PCTOTAL */
        ifstream input(File , ios :: binary);
        if(!input){
                cout << "Error: Could not open file " << File << endl;
                return 1;
        } 
        /* handle errors */
        ofstream errorfile;
        errorfile.open("errorlog.txt", ios :: app); /*append the errors in the same file where we wrote the errors for assembly*/
        errorfile << "Emulator starting for " << File << endl; /*indicate that these errors are for the emulator*/
        uint32_t instruction; /*4 byte block*/
        char* buffer=reinterpret_cast<char*> (&instruction); /* pointer to the instruction variable*/ 
        while(input.read(buffer, sizeof(instruction ))){
                if(pctotal<(int) memory.size()){
                        memory[pctotal]=instruction;
                        pctotal++;
                }

        }
        input.close(); 

        /* MENU */
        bool running=true;
        while(running){
                cout << "Emulator Menu " << endl;
                cout << "1. ISA " << endl;
                cout << "2. Trace " << endl;
                cout << "3. Run step" << endl;
                cout << "4. Before " << endl;
                cout << "5. After " << endl;
                cout << "6. After + Data (shows array /stack values ) " << endl;
                cout << "0. Exit " << endl;

                cout << "Enter your choice" << endl;
                int choice;
                cin >> choice ;

                switch(choice){
                        case 1 :
                                cout << "ISA (Instruction Set Architecture ): " << endl;
                                cout << "Opcode  ||    Mnemonic " << endl;
                                cout << "NA:     ||    data" << endl;
                                cout << "0:      ||    ldc" << endl;
                                cout << "1:      ||    adc" << endl;
                                cout << "2:      ||    ldl" << endl;
                                cout << "3:      ||    stl" << endl;
                                cout << "4:      ||    ldnl" << endl;
                                cout << "5:      ||    stnl" << endl;
                                cout << "6:      ||    add" << endl;
                                cout << "7:      ||    sub" << endl;
                                cout << "8:      ||    shl" << endl;
                                cout << "9:      ||    shr" << endl;
                                cout << "10:     ||    adj" << endl;
                                cout << "11:     ||    a2sp" << endl;
                                cout << "12:     ||    sp2a" << endl;
                                cout << "13:     ||    call" << endl;
                                cout << "14:     ||    return" << endl;
                                cout << "15:     ||    brz" << endl;
                                cout << "16:     ||    brlz" << endl;
                                cout << "17:     ||    br" << endl;
                                cout << "18:     ||    HALT" << endl;
                                cout << "NA:     ||    SET" << endl;
                                break;
                        case 2 : 
                                run_emulator(true , false , errorfile);
                                break;
                        case 3 :
                                run_emulator(true, true, errorfile);
                                break;
                        case 4 :
                                cout << "Initial Register State:" << endl;
                                cout << "A:" << a << "\nB:" << b << "\nSP:" << sp << "\nPC" << pc << endl; 
                                cout << "Memory (Before execution):" << endl;
                                cout << "Address   ||   Machine code " << endl;
                                for(int i=0;i< pctotal ;){
                                        cout << tohex(i) ;
                                        cout << "  ||  " << tohex(memory[i]) << "  ||  " << tohex(memory[i+1]) <<"  ||  " << tohex(memory[i+2]) << "  ||  " << tohex(memory[i+3]) <<"  ||  "<< endl;
                                        i=i+4;
                                }
                                cout << endl;
                                break;
                        case 5 :
                                run_emulator (false, false , errorfile);
                                cout << "Memory (After execution):" << endl;
                                cout << "Address   || "<< endl ;
                                for(int i=0; i<pctotal;){
                                        cout << tohex(i) ;
                                        cout << "  ||  " << tohex(memory[i]) ;
                                        if(i+1<pctotal){
                                        cout << "  ||  " << tohex(memory[i+1]);                                                
                                        }
                                        if(i+2<pctotal){     
                                        cout << "  ||  " << tohex(memory[i+2]);
                                        }     
                                        if(i+3<pctotal){     
                                        cout << "  ||  " << tohex(memory[i+3]);
                                        }     
                                        cout << endl;
                                        i=i+4;
                                }
                                cout << "Final Register State:" << endl;
                                printreg("HALT","");
                                break;
                        case 6:{
                                    /* base name*/
                                size_t lastdot=File.find_last_of('.');
                                string base;
                                if(lastdot==string::npos){
                                        base=File;
                                }else{
                                        base= File.substr(0,lastdot);
                                }
                                base+="_data.txt";
                                ofstream dataout(base.c_str());
                                if(!dataout.is_open()){
                                        cout << "Error: cannot create output file"<< endl;
                                        break;

                                }
                                run_emulator(false,false, errorfile);
                                printdata(dataout);
                                cout << "Final Register State:" << endl;
                                printreg("HALT","");
                                break;
                        }
                        case 0 :
                                cout << "Exiting emulator" << endl;
                                running=false;
                                break;

                        default:
                                cout << "INVALID CHOICE" << endl;

                }
        }
    
}