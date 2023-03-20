#include<stdio.h>
#include<fcntl.h>
#include <unistd.h>
int r;
int fetch(int fd)
{
	int re;
	int buf[3]={0};

// if(lseek(fd,10,SEEK_SET) > 4) return 0;	
 	re=read(fd,&buf[0],4);
	r+=re;
	printf("%.8x\t%d\n",buf[0],r);
	return re;
}
int main( int argc, char *argv[])
{
    int fd;
    int retval;
    int each = 0;
    unsigned int buf = 0;
    unsigned char opcode[4] = {0};
    unsigned char arg1[4] = {0};
    unsigned char arg2[4] = {0};
    unsigned char arg3[4] = {0};

    if ( argc < 2) {//was filename part of command
        printf ( "run as\n\tprogram filename\n");
        return 1;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        printf ( "could not open file\n");
        return 2;
    }

 /*   while ( ( retval = read ( fd, &buf, 4)) > 0) {//read until end of file
        if ( retval == 4) {//read four words
         //  for ( each = 0; each < 4; each++) {
           //     opcode[each] = buf[each];
             //   arg1[each] = buf[each + 4];
               // arg2[each] = buf[each + 8];
               // arg3[each] = buf[each + 12];
          //  }
	  
   	printf("%x\t%x\n",(buf & 0x000000ff),buf);//,arg1[i],arg2[i],arg3[i]); //do something with opcode and arg...
        }
    }*/
int i=0;
    while(1)
    { 
printf("fetching\n");	  
i=fetch(fd);
if(i==0)
	break;
    }
    close (fd);
    return 0;
}
