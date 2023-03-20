#include <stdio.h>
#include<fcntl.h>
#include <unistd.h>
#include<stdlib.h>

struct Instruction		         //stores data for each instruction
{   int buf;
    int count;
    int opcode;
    int dest;
    int rleft;
    int rright;
};

//STUDENT* students = malloc(numStudents * sizeof *students);
   
struct Instruction inst_line[8];      



//instruction aaaa = malloc(8*24); 
//instruction* inst_line = malloc(8 * sizeof *inst_line);

int ready[16]={0};      //records the state of 16 registers; 0 implies available for computation and 1 implies occupied or in process
int halt=0;
int branch1=0;
int regs[16];

//function to obtain the current status of the register
const char* cur_status(int status)
{
    if(status==0)
    return "valid";
    else if(status==1)
    return "writing";
}
//function that returns opcode as string
const char * inst(int opcode_inst)
{
    int num,rem;
    num=opcode_inst/16;
    rem=opcode_inst%16;
    switch(num)
    {
        case 0: return "set";
        case 1: return "add";
        case 2: return "sub";
        case 3: return "mul";
        case 4: return "div";
        case 5: return "ld";
        case 6: return "st";
        case 7: if(rem==0)return "bez"; else if(rem==1)return "bgez"; else if(rem==2)return "blez";else if(rem==3)return "bgtz"; else if(rem==4)return "bltz";
        default: return "ret";    
    }
}

//function to print instructions every stage
void print_status(const char* stage,int stagenum)
{
    char *str=inst(inst_line[stagenum].buf&0xff000000);
printf("%s\n",str);
    if(str=="set"||str=="ret"||str=="ld"||str=="st")
    {
		printf("%s",str);
    }
    if((inst_line[stagenum].buf&0xff000000)/16>=7)
    {
	printf("%s",str);
    }
    if((inst_line[stagenum].buf&0xff000000)%16==1)
    printf("%s             :%.4d %s R%d, R%d, #%d\n",stage,inst_line[stagenum].count, str, inst_line[stagenum].buf&0x00ff0000, inst_line[stagenum].buf&0x0000ff00,inst_line[stagenum].buf&0x000000ff);
    else
    printf("%s             :%.4d %s R%d, R%d, R%d\n",stage,inst_line[stagenum].count, str, inst_line[stagenum].buf&0x00ff0000, inst_line[stagenum].buf&0x0000ff00,inst_line[stagenum].buf&0x000000ff);
}

//function to fetch the instructions
int fetch(int fd,int seek)
{
	int re=-1;
    if(seek>-1)
    if(lseek(fd,seek,SEEK_SET) < 0) return 1;
	
    if(!branch1)
    {
        inst_line[0].count=lseek(fd,0,SEEK_CUR);
        re=read(fd,&inst_line[0].buf,4);
	printf("%x\n",inst_line[0].buf);
//	inst_line[stagenum].buf&0xff000000
    	print_status("IF",0);
    }
    return re;
}

//function to decode the instructions
void decode()
{
    inst_line[0].opcode=inst_line[0].buf&0xff000000;
    inst_line[0].dest=inst_line[0].buf&0x00ff0000;
    inst_line[0].rleft=inst_line[0].buf&0x0000ff00;
    inst_line[0].rright=inst_line[0].buf&0x000000ff;
    inst_line[1]=inst_line[0];
    print_status("ID",1);
}

//function for instruction analysis
int ia(int data_hazard_count)
{
    if(inst_line[1].opcode/16==7)
    {printf("data hazard: %d",data_hazard_count++);branch1=1;}
    
    else if(inst_line[1].opcode==0  && ready[inst_line[1].dest==1])
    {printf("data hazard: %d",data_hazard_count++); halt=1;}

    else if(inst_line[1].opcode/16<5 && inst_line[1].opcode%16==1 && (ready[inst_line[1].dest]==1 || ready[inst_line[1].rleft]==1))
    {printf("data hazard: %d",data_hazard_count++); halt=1;}

    else if(inst_line[1].opcode/16<5 && (ready[inst_line[1].dest]==1 || ready[inst_line[1].rleft]==1 || ready[inst_line[1].rright]==1))
    {printf("data hazard: %d",data_hazard_count++); halt=1;}

    else if((inst_line[1].opcode/16==5 || inst_line[1].opcode/16==6) && ready[inst_line[1].dest==1])
    {
        if(inst_line[1].opcode%16==1 && ready[inst_line[1].rright==1])
        {printf("data hazard: %d",data_hazard_count++); halt=1;}
        else
        {printf("data hazard: %d",data_hazard_count++); halt=1;}
    }

    if(!halt)
    inst_line[2]=inst_line[1];
    print_status("IA",2);
    return data_hazard_count; 
}

//function for addition and subtraction
void ex1()
{
    if(inst_line[2].opcode/16==1)
    {
        if(inst_line[2].opcode%16==1)
        {
            inst_line[2].rleft=regs[inst_line[2].rleft]+inst_line[2].rright;
            ready[inst_line[2].dest]=1;
        }
        else
        {
            inst_line[2].rleft=regs[inst_line[2].rleft]+regs[inst_line[2].rright];
            ready[inst_line[2].dest]=1;
        }
    }
    if(inst_line[2].opcode/16==2)
    {
        if(inst_line[2].opcode%16==1)
        {
            inst_line[2].rleft=regs[inst_line[2].rleft]-inst_line[2].rright;
            ready[inst_line[2].dest]=1;
        }
        else
        {
            inst_line[2].rleft=regs[inst_line[2].rleft]-regs[inst_line[2].rright];
            ready[inst_line[2].dest]=1;
        }
    }
    
    inst_line[3]=inst_line[2];
    print_status("Ex1",3);
}

//function for branching
int branch()
{
    int seek=-1;
    if(inst_line[3].opcode/16==7)
    {
        int rem=inst_line[3].opcode%16;
         switch (rem)
        {
        case 0: if(inst_line[3].dest==0)
                seek=inst_line[3].rright;
            break;
        case 1: if(inst_line[3].dest>=0)
                seek=inst_line[3].rright;
            break;
        case 2: if(inst_line[3].dest<=0)
                seek=inst_line[3].rright;
            break;
        case 3: if(inst_line[3].dest>0)
                seek=inst_line[3].rright;
            break;
        case 4: if(inst_line[3].dest<0)
                seek=inst_line[3].rright;
            break;
        default:
            break;
        }    
    }
    
    inst_line[4]=inst_line[3];
    print_status("Br",4);
  return seek;
}

//function for multiplication and division
void ex2()
{
    if(inst_line[4].opcode/16==3)
    {
        if(inst_line[4].opcode%16==1)
        {
            inst_line[4].rleft=regs[inst_line[4].rleft]*inst_line[4].rright;
            ready[inst_line[4].dest]=1;
        }
        else
        {
            inst_line[4].rleft=regs[inst_line[4].rleft]*regs[inst_line[4].rright];
            ready[inst_line[4].dest]=1;
        }
    }
    if(inst_line[4].opcode/16==4)
    {
        if(inst_line[4].opcode%16==1)
        {
            inst_line[4].rleft=regs[inst_line[4].rleft]/inst_line[4].rright;
            ready[inst_line[4].dest]=1;
        }
        else
        {
            inst_line[4].rleft=regs[inst_line[4].rleft]/regs[inst_line[4].rright];
            ready[inst_line[4].dest]=1;
        }
    }
    
    inst_line[5]=inst_line[4];
    print_status("Ex1",5);
  
}

//function memory operation
int mem1()
{
    int file;
    if ((file = open("memory_map", O_RDONLY)) < 0) 
    {
       printf ( "could not open file\n");
       return 2;
    }

    inst_line[6]=inst_line[5];
    print_status("Mem1",6);
    return file;
}

//function memory operation
void mem2(int file)
{
     if(inst_line[6].opcode/16==5)
    {
        if(inst_line[6].opcode%16==1)
        {
            inst_line[6].rleft=regs[inst_line[6].rright];
            ready[inst_line[6].dest]=1;
        }
        else
        {
            int i=lseek(file,inst_line[6].rright,SEEK_SET);
            i=read(file,&inst_line[6].rleft,2);
            ready[inst_line[6].dest]=1;
        }
    }
    if(inst_line[6].opcode/16==6)
    {
        if(inst_line[6].opcode%16==1)
        {
            inst_line[6].rright=regs[inst_line[6].dest];
            ready[inst_line[6].rright]=1;
        }
        else
        {
            int i=lseek(file,inst_line[6].rright,SEEK_SET);
            i=write(file,&inst_line[6].dest,2);  //no register update
        }
    }

    inst_line[7].buf=inst_line[6].buf;
    print_status("Mem2",7);
    
}

//function for write back
int wb()
{
    if(inst_line[7].opcode==255)
    {
        print_status("WB",8);
        return 1;
    }
    
        regs[inst_line[7].dest]=inst_line[7].rleft;
        ready[inst_line[7].dest]=0;    

    return 0;
}


int main(int argc, char *argv[]){
    int data_hazard_count = 0;
    int control_hazard_count = 0;
    int execution_cycles = 0;
    int completed_instructions = 0;
    

    int fd,end,end_r,seek=-1;
    int file;
    for(int i = 0; i < 16; i++){
        regs[i] = 0;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) 
    {
       printf ( "could not open file\n");
       return 2;
    }

    while(1)
    { 
        printf("================================\n");
        printf("Clock Cycle #: %d\n",execution_cycles++);
        printf("--------------------------------\n");
     /*   end_r=wb();
        if(!end_r)
        completed_instructions++;
        mem2(file);
        file=mem1();
        ex2();
        seek=branch();
        if(seek>-1)
        control_hazard_count++;
        ex1();
        data_hazard_count=ia(data_hazard_count);
	    decode();*/
        end=fetch(fd,seek);
        printf("=============== STATE OF ARCHITECTURAL REGISTER FILE ==========\n\n");
        for(int i=0;i<16;i++)
        printf("|   REG[%2d]   |   Value=%d   |   Status=%s   |\n",i,regs[i],cur_status(ready[i]));
        if(end==0||end_r==1)
         {
            printf("================================\n\n");
            break;
         }   
        

    }
    close (fd);


    printf("================================\n");
    printf("--------------------------------\n");
    for (int reg=0; reg<16; reg++) {
        printf("REG[%2d]   |   Value=%d  \n",reg, regs[reg]);
        printf("--------------------------------\n");
    }
    printf("================================\n\n");


    printf("Stalled cycles due to data hazard: %d \n", data_hazard_count);
    printf("Stalled cycles due to control hazard: %d \n", control_hazard_count);
    printf("\n");
    printf("Total stalls: %d \n", data_hazard_count+control_hazard_count);
    printf("Total execution cycles: %d\n", execution_cycles);
    printf("Total instruction simulated: %d\n", completed_instructions);
    printf("IPC: %f\n", ((double)completed_instructions/execution_cycles));

    return 0;
}
