#include "defrag.h"
#include<string.h>
int main()
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
	if((fin = fopen(FILEIN, "r"))==NULL)
	{
		printf("不能打开文件in\n");
		exit(0);
	}
	if((fout = fopen(FILEOUT, "ab"))==NULL)
	{
		printf("不能打开文件out\n");
		exit(0);
	}
	//boot
	bytes = fread(buffer, 512, 1, fin);
		printf("%s\n\n\n", buffer);
	bytes = fwrite(buffer, 512, 1, fout);

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
		for(j=0;j<25;j++,index+=1)
			printf("%12d ", *(index));
	printf("\n--------------------------------------------------------------------------------------------\n");
	}
	
	//write data to a new file for each inode;
	for(i=0;i<20;i++)
	{
		//unused inode, just scape;
		if(inode[i]->nlink!=0) continue;
		//for used inode
		int end=0;
		fseek(fin,inode[i]->dblocks[0], 0);
		bytes = fread(buffer, 512, 1, fin);
		fseek(fout, 1024 + data_offset*512, 0);
		fwrite(buffer, 512, 1, fout);
		inode[i]->dblocks[0] = data_offset++;
		// for each dblocks
		for(j=0;j<N_DBLOCKS;j++)
		{
			if(inode[i]->dblocks[j]==0) 
			{
				end = 1;
				break;
			}			
			fseek(fin, inode[i]->dblocks[j], 0);
			bytes = fread(buffer, 512, 1, fin);
			fseek(fout, 1024 + data_offset*512, 0);
			fwrite(buffer, 512, 1, fout);
			inode[i]->dblocks[j] = data_offset++;
		}
		//for each iblocks;
		if(end==0)
		{
			for(j=0;j<N_IBLOCKS;j++)
			{
				if(inode[i]->dblocks[j]==0) 
				{
					end = 1;
					break;
				}
				//copy iblocks
				int iblock_p[512/4]={0};
				fseek(fin, 1024+512*inode[i]->iblocks[j], 0);
				bytes = fread(iblock_p, 512, 1, fin);

				for(k=0;k<512/4;k++)
				{
					if(iblock_p[k]==0)
					{
						end = 1;
						break;
					}
					fseek(fin, 1024+512*iblock_p[k], 0);
					bytes = fread(buffer, 512, 1, fin);
					fseek(fout, 1024 + data_offset*512, 0);
					fwrite(buffer, 512, 1, fout);
					iblock_p[k] = data_offset++;
				}
				fseek(fout, 1024 + data_offset*512, 0);
				fwrite(iblock_p, 512, 1, fout);
				inode[i]->iblocks[j] = data_offset++;
			}	
		}
		//for each i2blocks
		if(end==0)
		{
			int i2block_p[512/4]={0};
			fseek(fin, 1024+512*inode[i]->i2block, 0);
			bytes = fread(i2block_p, 512, 1, fin);
			
			for(j=0;j<512/4;j++)
			{
				if(i2block_p[j]==0) 
				{
					end = 1;
					break;
				}
				//copy iblocks
				int iblock_p[512/4]={0};
				fseek(fin, 1024 + 512*i2block_p[j], 0);
				bytes = fread(iblock_p, 512, 1, fin);
				
				for(k=0;k<512/4;k++)
				{
					if(iblock_p[k]==0)
					{
						end = 1;
						break;
					}
					fseek(fin, 1024 + 512*iblock_p[k], 0);
					bytes = fread(buffer, 512, 1, fin);
					fseek(fout, 1024 + data_offset*512, 0);
					fwrite(buffer, 512, 1, fout);
					iblock_p[k] = data_offset++;
				}
				fseek(fout, 1024 + data_offset*512, 0);
				fwrite(iblock_p, 512, 1, fout);
				i2block_p[j] = data_offset++;
			}
			fseek(fout, 1024 + data_offset*512, 0);
			fwrite(i2block_p, 512, 1, fout);
			inode[i]->i2block = data_offset++;
		}

		//for each i3blocks
		if(end==0)
		{
			int i3block_p[512/4]={0};
			fseek(fin, 1024+512*inode[i]->i3block, 0);
			bytes = fread(i3block_p, 512, 1, fin);
			for(j=0;j<512/4;j++)
			{
				if(i3block_p[j]==0) 
				{
					end = 1;
					break;
				}
				int i2block_p[512/4]={0};
				fseek(fin, 1024+512*i3block_p[j], 0);
				bytes = fread(i2block_p, 512, 1, fin);
			
				for(k=0;k<512/4;k++)
				{
					if(i2block_p[k]==0) 
					{
						end = 1;
						break;
					}
					//copy iblocks
					int iblock_p[512/4]={0};
					fseek(fin, 1024 + 512*i2block_p[j], 0);
					bytes = fread(iblock_p, 512, 1, fin);
				
					for(l=0;l<512/4;l++)
					{
						if(iblock_p[l]==0)
						{
							end = 1;
							break;
						}
						fseek(fin, 1024 + 512*iblock_p[l], 0);
						bytes = fread(buffer, 512, 1, fin);
						fseek(fout, 1024 + data_offset*512, 0);
						fwrite(buffer, 512, 1, fout);
						iblock_p[l] = data_offset++;
					}
					fseek(fout, 1024 + data_offset*512, 0);
					fwrite(iblock_p, 512, 1, fout);
					i2block_p[k] = data_offset++;
				}
				fseek(fout, 1024 + data_offset*512, 0);
				fwrite(i2block_p, 512, 1, fout);
				i3block_p[j] = data_offset++;
			}
			fseek(fout, 1024 + data_offset*512, 0);
			fwrite(i3block_p, 512, 1, fout);
			inode[i]->i3block = data_offset++;
		}		
	}
	
	//write superblock to new file
	spbk->free_iblock = data_offset;
	fseek(fout, 512, 0);
	bytes =  fwrite(spbk, 512, 1, fout);

	//write inodes to new file
	fseek(fout, 1024, 0);
	bytes =  fwrite(inode, 512, 1, fout);

	//write swap region
	fseek(fin, spbk->swap_offset, 0);
	fseek(fout, spbk->swap_offset, 0);
	while((bytes = fread(buffer, 512, 1, fin))==1)
	{
		bytes =  fwrite(inode, 512, 1, fout);
	}
	
	
	fclose(fin);
	fclose(fout);
	return 0;
}

