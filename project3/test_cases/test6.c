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
    int result;
};

   
struct Instruction inst_line[9];    //data structure to store PC and instructions in each stage

int ready[16]={0};      //records the state of 16 registers; 0 implies available for computation and 1 implies occupied or in process
int halt=0;
int branch1=0;          //stores 0 or 1, indicates where a branch statement is encountered or not
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
//	case 15:return "ret";
	default: return "ret";    
    }
}

//function to print instructions every stage
void print_status(const char* stage,int stagenum)
{
  
  char *str=inst((inst_line[stagenum].buf>>24)&0xff);
   
    if(str=="ret")
    printf("%s             :%.4d %s\n",stage,inst_line[stagenum].count,str);
    
    else if(str=="set"||((inst_line[stagenum].buf>>24)&0xff)/16==7||str=="ld"||str=="st")
    {
        if(str=="set"||((inst_line[stagenum].buf>>24)&0xff)/16==7||((inst_line[stagenum].buf>>24)&0xff)%16==0)
	printf("%s             :%.4d %s R%d, #%.4d\n",stage,inst_line[stagenum].count,str,(inst_line[stagenum].buf>>16)&0xff,inst_line[stagenum].buf&0xff);
        else
        printf("%s             :%.4d %s R%d, R%d\n",stage,inst_line[stagenum].count,str,(inst_line[stagenum].buf>>16)&0xff,inst_line[stagenum].buf&0xff);
    }
   
    else if(((inst_line[stagenum].buf>>24) & 0xff)%16==1)
    printf("%s             :%.4d %s R%d, R%d, #%.4d\n",stage,inst_line[stagenum].count, str, (inst_line[stagenum].buf>>16)&0xff, (inst_line[stagenum].buf>>8)&0xff,inst_line[stagenum].buf&0xff);
    else
    printf("%s             :%.4d %s R%d, R%d, R%d\n",stage,inst_line[stagenum].count, str, (inst_line[stagenum].buf>>16)&0xff, (inst_line[stagenum].buf>>8)&0xff,inst_line[stagenum].buf&0xff);
}


//function for write back
int wb()
{
        
if(inst_line[8].buf!=0x9000000)
    {
        if(inst_line[8].opcode==255)
        {
            print_status("WB",8);
            return 1;
        }
        
            regs[inst_line[8].dest]=inst_line[8].result;  //writing into the register
            ready[inst_line[8].dest]=0;      //updating the register the status
            print_status("WB  ",8);
//    printf("Result:%d dest:%d left:%d right:%d\n",inst_line[8].result,inst_line[8].dest,inst_line[8].rleft, inst_line[8].rright);

    }
    
return 0;
}


//function memory operation
void mem2(int file)
{
    if(inst_line[7].buf!=0x9000000)
    {
        if(inst_line[7].opcode/16==5)       //computing the load operation
        {
            if(inst_line[7].opcode%16==1)
            {
                inst_line[7].rleft=regs[inst_line[7].rright];
                ready[inst_line[7].dest]=1;
            }
            else
            {
                int i=lseek(file,inst_line[7].rright,SEEK_SET);
                i=read(file,&inst_line[7].rleft,2);
                ready[inst_line[7].dest]=1;
            }
        }
        if(inst_line[7].opcode/16==6)           //computing the store operation
        {
            if(inst_line[7].opcode%16==1)
            {
                int i=lseek(file,regs[inst_line[7].rright],SEEK_SET);   
                i=write(file,&inst_line[7].dest,2);
            }
            else
            {
                int i=lseek(file,inst_line[7].rright,SEEK_SET);
                i=write(file,&inst_line[7].dest,2);  
            }
        }

             //   printf("Result:%d dest:%d left:%d right:%d\n",inst_line[7].result,inst_line[7].dest,inst_line[7].rleft, inst_line[7].rright);

        print_status("Mem2",7);
    
    }
    inst_line[8]=inst_line[7];
}


//function memory operation
int mem1()
{
      int file;
    if(inst_line[6].buf!=0x9000000)
    {
        if(inst_line[6].opcode/16==5||inst_line->opcode/16==6)
       {
           if ((file = open("memory_map", O_RDWR)) < 0) 
            {
            printf ( "could not open file\n");
            return 2;
            }
       } 

            //    printf("Result:%d dest:%d left:%d right:%d\n",inst_line[6].result,inst_line[6].dest,inst_line[6].rleft, inst_line[6].rright);

        print_status("Mem1",6);  
    }
    inst_line[7]=inst_line[6];
    return file;
}


//function for multiplication and division
void ex2()
{
    if(inst_line[5].buf!=0x9000000)
    {
        if(inst_line[5].opcode/16==3)           //computaation for mul operation
        {
            if(inst_line[5].opcode%16==1)
            {
                inst_line[5].result=regs[inst_line[5].rleft]*inst_line[5].rright;
                ready[inst_line[5].dest]=1;
            }
            else
            {
                inst_line[5].result=regs[inst_line[5].rleft]*regs[inst_line[5].rright];
                ready[inst_line[5].dest]=1;
            }
        }
        if(inst_line[5].opcode/16==4)       //computation for div operation
        {
            if(inst_line[5].opcode%16==1)
            {
                inst_line[5].result=regs[inst_line[5].rleft]/inst_line[5].rright;
                ready[inst_line[5].dest]=1;
            }
            else
            {
                inst_line[5].result=regs[inst_line[5].rleft]/regs[inst_line[5].rright];
                ready[inst_line[5].dest]=1;
            }
        }
        
             //   printf("Result:%d dest:%d left:%d right:%d\n",inst_line[5].result,inst_line[5].dest,inst_line[5].rleft, inst_line[5].rright);

        print_status("Ex2 ",5);
    }
        inst_line[6]=inst_line[5];

}


//function for branching
int branch()
{
    int seek=-1;
    if(inst_line[4].buf!=0x9000000)
    {
        if(inst_line[4].opcode/16==7)
        {
            int rem=inst_line[4].opcode%16;
            switch (rem)
            {
            case 0: if(inst_line[4].dest==0)
                    seek=inst_line[4].rright;
                    printf(" branching :  %x\n",inst_line[4].rright);
                break;
            case 1: if(inst_line[4].dest>=0)
                    seek=inst_line[4].rright;
                    printf(" branching :  %x\n",inst_line[4].rright);
                break;
            case 2: if(inst_line[4].dest<=0)
                    seek=inst_line[4].rright;
                    printf(" branching :  %x\n",inst_line[4].rright);
                break;
            case 3: if(inst_line[4].dest>0)
                    seek=inst_line[4].rright;
                    printf(" branching :  %x\n",inst_line[4].rright);
                break;
            case 4: if(inst_line[4].dest<0)
                    seek=inst_line[4].rright;
                    printf(" branching :  %x\n",inst_line[4].rright);
                break;
            default:
                break;
            }    
        }
        
             //   printf("Result:%d dest:%d left:%d right:%d\n",inst_line[4].result,inst_line[4].dest,inst_line[4].rleft, inst_line[4].rright);

        print_status("Br  ",4);
    } 

   inst_line[5]=inst_line[4];
   branch1=0;
    return seek;
}


//function for addition and subtraction
void ex1()
{
    
	if(inst_line[3].buf!=0x9000000)
    {
        if(inst_line[3].opcode==0)              //performing set operation
        {
            inst_line[3].result=inst_line[3].rright;
            ready[inst_line[3].dest]=1;
        }
        else if(inst_line[3].opcode/16==1)      //compuation of add operation
        {
            if(inst_line[3].opcode%16==1)
            {
                inst_line[3].result=regs[inst_line[3].rleft]+inst_line[3].rright;
                ready[inst_line[3].dest]=1;
            }
            else
            {
                inst_line[3].result=regs[inst_line[3].rleft]+regs[inst_line[3].rright];
                ready[inst_line[3].dest]=1;
            }
        }
        else if(inst_line[3].opcode/16==2)          //computation for sub operation
        {
            if(inst_line[3].opcode%16==1)
            {
                inst_line[3].result=regs[inst_line[3].rleft]-inst_line[3].rright;
                ready[inst_line[3].dest]=1;
            }
            else
            {
                inst_line[3].result=regs[inst_line[3].rleft]-regs[inst_line[3].rright];
                ready[inst_line[3].dest]=1;
            }
        }
               // printf("Result:%d rright:%d buf:%x\n",inst_line[3].result,inst_line[3].rright, inst_line[3].buf);
print_status("Ex1 ",3);
    }
    inst_line[4]=inst_line[3];
}


//function for instruction analysis
int ia(int data_hazard_count)
{
        if(inst_line[2].buf!=0x9000000)
    {
	    if(ready[inst_line[2].dest]==0 && ready[inst_line[2].rleft]==0 && ready[inst_line[2].rright]==0)
	    {
		    halt=0;
		    branch1=0;
		}
	    else{
        if(inst_line[2].opcode/16==7)           
        {
            if(ready[inst_line[2].dest]==1)
            {printf("data hazard: %d\n",++data_hazard_count);branch1=1;halt=1;}
            else
            {printf("data hazard: %d\n",++data_hazard_count);branch1=1;}
        }
            
        else if(inst_line[2].opcode==0  && ready[inst_line[2].dest]==1)
        {printf("data hazard: %d\n",++data_hazard_count); halt=1;}

        else if(inst_line[2].opcode/16<5 && inst_line[2].opcode/16>0)
        {
            if(inst_line[2].opcode%16==0 && (ready[inst_line[2].dest]==1 || ready[inst_line[2].rleft]==1 || ready[inst_line[2].rright]==1))
	        { printf("data hazard: %d\n",++data_hazard_count); halt=1;}
           
            else if(inst_line[2].opcode%16==1 && (ready[inst_line[2].dest]==1 || ready[inst_line[2].rleft]==1 ))
	        {printf("data hazard: %d\n",++data_hazard_count); halt=1;}     
     	}
        else if((inst_line[2].opcode/16==5 || inst_line[2].opcode/16==6) && ready[inst_line[2].dest==1])
        {
            if(inst_line[2].opcode%16==1 && ready[inst_line[2].rright==1])
            {printf("data hazard: %d\n",++data_hazard_count); halt=1;}
            else
            {printf("data hazard: %d\n",++data_hazard_count); halt=1;}
        }
	    }
    
   // printf("halt:%d\n",halt);
    inst_line[3]=inst_line[2];   
	
    if(halt)
	inst_line[3].buf=0x9000000;
       // printf("ia: %x\t %d status:%d %d %d\n",inst_line[2].buf,data_hazard_count,ready[inst_line[2].dest],ready[inst_line[2].rleft],ready[inst_line[2].rright]);
        
        print_status("IA  ",2);
    }
    return data_hazard_count; 
}


//function to decode the instructions
void decode()
{
	if(inst_line[1].buf!=0x9000000)
    {
        inst_line[1].opcode=(inst_line[1].buf>>24)&0xff;
        inst_line[1].dest=(inst_line[1].buf>>16)&0xff;
        inst_line[1].rleft=(inst_line[1].buf>>8)&0xff;
        inst_line[1].rright=inst_line[1].buf&0xff;

	if(!halt)
        inst_line[2]=inst_line[1];
        print_status("ID  ",1);
    } 
}


//function to fetch the instructions
int fetch(int fd,int seek)
{
	int re=-1;
    if(seek>-1)
    {
        if(lseek(fd,seek,SEEK_SET) < 0) return 1;
        inst_line[0].count=lseek(fd,0,SEEK_CUR);
        re=read(fd,&inst_line[0].buf,4);
    	print_status("IF",0);
	inst_line[1]=inst_line[0];
    }
else if(((inst_line[0].buf>>24)&0xff)/16==7)
    {
        if(branch1==0)
       {
            branch1=1;
            re=0;
       // print_status("IF",0);
        inst_line[1] = inst_line[0];
       }
       else if(branch1==1)
       {
           re=0;
       }
    }
	
    else if(!branch1)
    {
        inst_line[0].count=lseek(fd,0,SEEK_CUR);
        re=read(fd,&inst_line[0].buf,4);
    	print_status("IF",0);
	inst_line[1]=inst_line[0];
    }
    
    return re;
}



int main(int argc, char *argv[])
{
    int data_hazard_count = 0;
    int control_hazard_count = 0;
    int execution_cycles = 0;
    int completed_instructions = 0;

	int fd,re,seek,file,end=0;

    for(int i=0;i<9;i++)
    inst_line[i].buf=0x9000000;

	if ((fd = open(argv[1], O_RDONLY)) < 0)
    {
       printf ( "could not open file\n");
       return 2;
    }

	while(1)
	{
        printf("================================\n");
        printf("Clock Cycle #: %d\n",++execution_cycles);
        printf("--------------------------------\n");
        
        end=wb();
        if(!end)
        completed_instructions++;
        mem2(file);
        file=mem1();
        ex2();
        seek=branch();
	 
        if(seek>0)
	++control_hazard_count;
        ex1();
        data_hazard_count = ia(data_hazard_count);
        decode();
	    re=fetch(fd,seek);
	    if(seek>-1)
		    seek=-1;

         printf("=============== STATE OF ARCHITECTURAL REGISTER FILE ==========\n\n");
        for(int i=0;i<16;i++)
        printf("|   REG[%2d]   |   Value=%d   |   Status=%s   |\n",i,regs[i],cur_status(ready[i]));
	 printf("re:%d   end:%d\n",re,end);
		if(re==-1||end==1)
		{
            printf("================================\n\n");
            break;
        }		}

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

