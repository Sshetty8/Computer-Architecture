#include "project02.h"
#include <fstream>
#include <iterator>
#include <vector>
#include<iostream>
#include <array>

using namespace std;

execution_time=0;
request_done=0;
request_get=0;
line_num=1;
int inc,len;

struct instruction{
    int opcode;
    int dest;
    int left;
    int right;
}inst;

array<string,2> pipe;

string toHexa( unsigned i )
{ 
   const string DIGITS = "0123456789abcdef";
   return i?toHexa( i / 16 ) + DIGITS[i%16]:"0";
}

int todec(char c)
{
    switch(c)
    {
        case 'a': return 10;
        case 'b':return 11;
        case 'c':return 12;
        case 'd':return 13;
        case 'e':return 14;
        case 'f':return 15;
        default: return c-'0';
    }
}

void fetch(string filename)
{
    if(inc==0)
    {
        string s,result;
        string str(8,' ');
        char c;
        int counter = 0;
        int i=7;
        ifstream in( filename, ios::binary );
        while ( in.get( c ) )
        {
            s=toBase(c);
            if(s.length()<=2)
            {str[i]=s[s.length()-1];str[i-1]='0';i-=2;}
            else if(s.length()>2)
            {str[i-1]=s[s.length()-2];str[i]=s[s.length()-1];i-=2;}
            else
            {
            str[i-1]=s[0];
            str[i]=s[1];
            i-=2;
            }
            cout<<"[1] fetching: "<<str;
            counter++;
            if (counter==4 ) 
            {
                counter=0;i=7;
                result+=str;
                result+="@";
            }  
        }
    len=result.length();
    }
   
    pipe[0]=result.substr(inc,8);
}

void decode(const string& str,)
{
    if(str.compare("")!=0)
    {
        if(str.compare(0,1,"4")!=0||str.compare(0,1,"3")!=0)
        {
            cout<<"[2] decoding: stalling";
            return {};
        }
        cout<<"[2] decoding: "<<str;
        inst.opcode=todec(str[0]);
        inst.opcode+=(16*todec(str[1]);

        inst.dest=todec(str[3]);
        
        inst.left=todec(str[5]);
        inst.left+=(16*todec(str[4]));
        
        inst.right=todec(str[7]);
        inst.right+=(16*todec(str[6]));
        pipe[1]=pipe[0];
    }
}

void execute(const string& str, Simple_Pipe simple_pipe)
{
    if(str.compare("")!=0)
    {
        int cur_cycle=1;
        int cycle_need=1;
        cout<<"[3] executing: "<<str<<"current cycle: "<<cur_cycle<<"cycle needed: "<<cycle_need;
        swtich(inst.opcode)
        {
            case 0: simple_pipe.registers[inst.dest]=inst.left;
                    break;
            case 16:simple_pipe.registers[inst.dest]=simple_pipe.registers[inst.left]+simple_pipe.registers[inst.right];
                    break;
            case 17:simple_pipe.registers[inst.dest]=simple_pipe.registers[inst.left]+inst.right;
                    break;
            case 32:simple_pipe.registers[inst.dest]=simple_pipe.registers[inst.left]-simple_pipe.registers[inst.right];
                    break;
            case 33:simple_pipe.registers[inst.dest]=simple_pipe.registers[inst.left]-inst.right;
                    break;
            case 48:simple_pipe.registers[inst.dest]=simple_pipe.registers[inst.left]*simple_pipe.registers[inst.right];
                    break;
            case 49:simple_pipe.registers[inst.dest]=simple_pipe.registers[inst.left]*inst.right;
                    break;
            case 64:simple_pipe.registers[inst.dest]=simple_pipe.registers[inst.left]/simple_pipe.registers[inst.right];
                    break;
            case 65:simple_pipe.registers[inst.dest]=simple_pipe.registers[inst.left]/inst.right;    
                    break;
            default: break;
        }
        request_done++;
    }
    
}


int main(int argc, char *argv[])
{
    string file_name=argv[1]; 
    string str;
    char c;
    Simple_Pipe simple_pipe;
    for(int i = 0; i < REG_COUNT; i++){
        simple_pipe.registers[i] = 0;
    }
    
    while(inc<len)
    {
        request_get++;
        cout<<"execution time: "<<execution_time++;
        execution(pipe[1],&simple_pipe);
        decode(pipe[0]);
        fetch(file_name);
        inc+=9;
    }
    
    cout<<"Total instruction received: "<<request_get<<endl;
    cout<<"Total instruction handled: "<<request_done<<endl;
    simple_pipe.print_regs();
   cout << "Total execution cycles: " << execution_time <<endl;
   cout << "\nIPC: " << (request_done/(double)execution_time) <<endl <<endl;

    return 0;
}



void Simple_Pipe::print_regs(){
    printf("\nRegisters: \n");
   cout << "----------------------------------------" <<endl;
    for(int i = 0; i < REG_COUNT; i+=2){
       string regl("R");
        regl.append(std::to_string(i));
        regl.append(": ");

       string regr("R");
        regr.append(std::to_string(i+1));
        regr.append(": ");

        if(i < 15){
           cout << "  " <<setiosflags(std::ios::left) 
            <<setw(5) << regl  <<setw(10) << registers[i] << " |   " 
            <<setw(5) << regr <<setw(10) << registers[i+1] <<endl;
           cout << "----------------------------------------" <<endl;
        }else{
           cout << "  " <<setiosflags(std::ios::left) 
            <<setw(5) << regl <<setw(10) << registers[i] << " |   " <<endl;
           cout << "----------------------------------------" <<endl;
        }
    }  
    printf("\n");
}

