#include "project01.h"
#include<string.h>
#include<stdio.h>

char line[12][25];


void scaling(char* str)
{
    if(!strcmp(str,"end"))
	    return;
    if(strcmp(str,"stall"))
    printf("[1] scaling %s\n",str);  
}

void mixing(char* str)
{
    if(bakery_time>=1 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[2] mixing %s\n",str);
}

void fermentation(char* str)
{
    if(bakery_time>=2 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[3] fermentation %s\n",str);
}

void folding(char* str)
{
    if(bakery_time>=3 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[4] folding %s\n",str);
}

void dividing(char* str)
{
    if(bakery_time>=4 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[5] dividing %s\n",str);
}

void rounding(char* str)
{
     if(bakery_time>=5 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[6] rounding %s\n",str);
}

void resting(char* str) 
{
    if(bakery_time>=6 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[7] resting %s\n",str);
}

void shaping(char* str)
{
    if(bakery_time>=7 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[8] shaping %s\n",str);
}

void proofing(char* str)
{
    if(bakery_time>=8 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[9] proofing %s\n",str);
}

void baking(char* str)
{
    if(bakery_time>=9 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    {
        if(strstr(str,"Baguette"))
        {
            printf("[10] baking %s\n",str);
            printf("[10] baking stall\n");
        }
        else
        printf("[10] baking %s\n",str);
    }
}

void cooling(char * str) 
{
    if(bakery_time>=10 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[11] cooling %s\n",str);
}

void stalking(char* str)
{
    if(bakery_time>=11 ^ !strcmp(str,"stall")^!strcmp(str,"end"))
    printf("[12] stalking %s\n",str);
}


void bake()
{
    printf("\ncurrent bakery time: %d\n",bakery_time);

    stalking(line[11]);

    cooling(line[10]);
    strcpy(line[11],line[10]); 
            
    if(baking_count%10==0)
    {
        baking(line[9]);
        strcpy(line[10],"stall");
    } 
    else
    {
        baking(line[9]);
        strcpy(line[10],line[9]); 
    }
            
    proofing(line[8]);
    strcpy(line[9],line[8]); 
            
    shaping(line[7]);
    strcpy(line[8],line[7]); 
            
    resting(line[6]);
    strcpy(line[7],line[6]); 
            
    rounding(line[5]);
    strcpy(line[6],line[5]); 
            
    dividing(line[4]);
    strcpy(line[5],line[4]); 
            
    folding(line[3]);
    strcpy(line[4],line[3]); 
            
    fermentation(line[2]);
    strcpy(line[3],line[2]);
            
    mixing(line[1]);
    strcpy(line[2],line[1]); 
            
    if(baking_count%1000==0)
    {
        for(int i=0;i<10;i++)
        {
            printf("[1] scaling stall\n"); 
            strcpy(line[1],"stall");
        } 

    }
    else if(!strcmp(line[0],"No_Request"))
    {
        scaling(line[0]);
        strcpy(line[1],"stall");
    }
    else 
    {
        scaling(line[0]);
        strcpy(line[1],line[0]);
    }

    bakery_time++;
 }

    


int main(int argc, char *argv[])  {

    FILE *fp;
    char * str;
    char *l=NULL;
    size_t len=0;
    
    fp=fopen(argv[1],"r");
    baking_count=0;
    bakery_time=0;

    while(getline(&l,&len,fp)!=-1)
    {
        if(strstr(l,"Bagel"))
        { 
            baking_count++;
            bagel_baked++;
            str="Bake_Bagel";
        }
        else if(strstr(l,"Baguette"))
        {
            baking_count++;
            baguette_baked++;
            str="Bake_Baguette";
        }
        else if(strstr(l,"No-Request"))
        {
            no_request++;
            str="No_Request";
        } 
        
        strcpy(line[0],str);
        bake();  
    }
    
    fclose(fp);

        while(strcmp(line[11],"end"))
        {
            bake();
            strcpy(line[0],"end");
        }

    
    
    performance=(float)baking_count/bakery_time;
    //output formats
    printf("\nBaking count: %d\n", baking_count);
    printf(" -Bagel baked: %d\n", bagel_baked);
    printf(" -Baguette baked: %d\n", baguette_baked);
    printf("No request: %d\n", no_request);

    printf("\nHow many minutes to bake: %d\n", bakery_time);

    printf("\nPerformance (bakes/minutes): %.2f\n", performance);

    return 0;
}
