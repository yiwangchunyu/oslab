#include "defrag.h"
#include<string.h>
int main(int argc, char * argv[])
{
	FILE * fin, * fout; 
	char * buffer; 
	int * index;
	int i,j,k,l;
	size_t bytes;
	//data offset in blocks;
	int data_offset = 4; 
	SBK *spbk =  (SBK * )malloc(sizeof(SBK));
	IND *inode[20];
	buffer = malloc(512); 
	//file in and file out;
	if((fin = fopen(argv[1], "r"))==NULL)
	{
		printf("不能打开文件in\n");
		exit(0);
	}

	//boot
	bytes = fread(buffer, 512, 1, fin);
		printf("%s\n\n\n", buffer);

	//superblock
	bytes = fread(buffer, 512, 1, fin);
	memcpy(spbk, buffer, 6*4);
	index = (int *)buffer;
	for(i=0;i<128;i++,index+=1)
	printf("%d ", *(index));

	
	printf("\n--------------------------------------\n");
	//inodes
	//malloc for inode[20];
	for(i=0;i<20;i++)
	{
		inode[i] = (IND *)malloc(sizeof(IND));		
	}	
	//memory copy to inode[20];
	for(i=0;i<20;i++)
	{
		printf("--%d--\n",i);
		bytes = fread(buffer, 25*4, 1, fin);
		memcpy(inode[i], buffer, 100);
		index = (int *)buffer;
		for(j=0;j<9;j++,index+=1)
			printf("%12d", *(index));
			putchar(10);
		for(j=9;j<19;j++,index+=1)
			printf("%12d", *(index));
			putchar(10);
		for(j=19;j<25;j++,index+=1)
			printf("%12d", *(index));
		
	printf("\n--------------------------------------------------------------------------------------------\n");
	}

	fclose(fin);
	return 0;
}
