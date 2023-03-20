#include <stdio.h>
#include<fcntl.h>
#include <unistd.h>

int fetch(int fd)
{
	int re;
	int buf=0;
	re=read(fd,&buf,4);
	printf("%x\n",buf);
	return re;
}


int main(int argc, char const *argv[]){
    int data_hazard_count = 0;
    int control_hazard_count = 0;
    int execution_cycles = 0;
    int completed_instructions = 0;
    int regs[16];
    int fd,eof;
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
	printf("fetching\n");	  
	eof=fetch(fd);
	if(eof==0)
	 break;
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
