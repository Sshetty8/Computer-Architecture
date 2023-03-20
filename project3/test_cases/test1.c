#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include<stdio.h>
int buffer[3];
int count=8;
int fetch(int file)
{
	        if(lseek(file,count,SEEK_SET) < 0) return 1;
count+=4;
	int c=lseek(file,0,SEEK_CUR);
        int re=read(file,&buffer[0],4);// != 4)  return 1;
       // int c =lseek(file,0,SEEK_CUR);
	printf("%d\t",c);
	printf("%x\t%d\n",buffer[0],re);

}
int main()
{
        int file=0;
        if((file=open("test01",O_RDONLY)) < -1)
                return 1;
 
      //  int buffer;
       // if(read(file,buffer,4) != 4)  return 1;
       // printf("%x\n",buffer);
 /*
        if(lseek(file,28,SEEK_SET) < 0) return 1;
 
        if(read(file,&buffer,4) != 4)  return 1;
        printf("%x\n",buffer);
 */int i;
	while(i<5)
	{
		fetch(file);
		i++;
	}
        return 0;
}
