#include "defrag.h"
#include<string.h>
int block_size;
int data_offset;
SBK *spbk;
size_t bytes;
int main(int argc, char *argv[])
{
	FILE * fin, * fout;
	char * buffer;
	int i,j,k,l;
	spbk =  (SBK * )malloc(sizeof(SBK));


	//file in and file out;
	char *filein = argv[1];
	char *fileout = newFileName(filein);
	if((fin = fopen(filein, "r"))==NULL)
	{
		printf("不能打开文件%s\n", filein);
		exit(0);
	}
	if((fout = fopen(fileout, "wb"))==NULL)
	{
		printf("不能打开文件%s\n", fileout);
		exit(0);
	}

	//boot
	bootCopy(fin, fout);



	//superblock build
	superblock_build(fin, fout, spbk);


	//data offset in blocks;
	data_offset = spbk->data_offset;
	block_size = spbk->size;
	buffer = (char *)malloc(block_size);

	//printf("\n--------------------------------------\n");

	//inodes build
	int numOfInode = block_size*spbk->data_offset/100;
	IND **inode = (IND**)malloc(numOfInode*sizeof(IND*));
	for(i=0;i<numOfInode;i++)
		inode[i] = (IND*)malloc(sizeof(IND));
	buildIND(inode, numOfInode, fin, buffer);

	//write inodes to new file; 预写入，后面会重写的
	//fseek(fout, 1024+block_size*spbk->inode_offset, 0);

	for(i=0;i<numOfInode;i++)
	{
		fseek(fout, 1024 + (spbk->inode_offset)*block_size+i*sizeof(IND), 0);
		bytes =  fwrite(inode[i], sizeof(IND), 1, fout);
		error_checking(bytes);
	}

	//area Copy Inode To Data
	areaCopyInodeToData(fin, fout, spbk, buffer );


	//write data to a new file for each inode;
	int *iblock_p = (int *)malloc(block_size);
	int *i2block_p=(int *)malloc(block_size);
	int *i3block_p=(int *)malloc(block_size);

	for(i=0;i<numOfInode;i++)
	{

		//unused inode, just scape;
		if(inode[i]->nlink==0)
		{
            continue;
		}
		//for used inode
		int end=0;
		// for each dblocks
		for(j=0;j<N_DBLOCKS;j++)
		{
			if(inode[i]->dblocks[j]==0)
			{
				end = 1;
				break;
			}
			fseek(fin, 1024 + block_size*inode[i]->dblocks[j], 0);
			bytes = fread(buffer, block_size, 1, fin);
			error_checking(bytes);
			fseek(fout, 1024 + data_offset*block_size, 0);
			bytes = fwrite(buffer, block_size, 1, fout);
			error_checking(bytes);
			inode[i]->dblocks[j] = data_offset++;
		}


		//for each iblocks;
		if(end==0)
		{
			for(j=0;j<N_IBLOCKS;j++)
			{

				if(inode[i]->iblocks[j]==0)
				{
					end = 1;
					break;
				}
				//copy iblocks

				fseek(fin, 1024+block_size*inode[i]->iblocks[j], 0);
				bytes = fread(iblock_p, block_size, 1, fin);
				error_checking(bytes);
				for(k=0;k<block_size/4;k++)
				{
					if(iblock_p[k]==0)
					{
						end = 1;
						break;
					}
					fseek(fin, 1024+block_size*iblock_p[k], 0);
					bytes = fread(buffer, block_size, 1, fin);
					error_checking(bytes);
					fseek(fout, 1024 + data_offset*block_size, 0);
					bytes = fwrite(buffer, block_size, 1, fout);
					error_checking(bytes);
					iblock_p[k] = data_offset++;
				}
				fseek(fout, 1024 + data_offset*block_size, 0);
				bytes = fwrite(iblock_p, block_size, 1, fout);
				error_checking(bytes);
				inode[i]->iblocks[j] = data_offset++;
			}
		}
		//for each i2blocks
		if(end==0)
		{
			if(inode[i]->i2block==0)
			{
				end=1;
				continue;
			}
			fseek(fin, 1024+block_size*inode[i]->i2block, 0);
			bytes = fread(i2block_p, block_size, 1, fin);
			error_checking(bytes);

			for(j=0;j<block_size/4;j++)
			{
				if(i2block_p[j]==0)
				{
					end = 1;
					break;
				}
				//copy iblocks
				fseek(fin, 1024 + block_size*i2block_p[j], 0);
				bytes = fread(iblock_p, block_size, 1, fin);
				error_checking(bytes);

				for(k=0;k<block_size/4;k++)
				{
					if(iblock_p[k]==0)
					{
						end = 1;
						break;
					}
					fseek(fin, 1024 + block_size*iblock_p[k], 0);
					bytes = fread(buffer, block_size, 1, fin);
					error_checking(bytes);
					fseek(fout, 1024 + data_offset*block_size, 0);
					bytes = fwrite(buffer, block_size, 1, fout);
					error_checking(bytes);
					iblock_p[k] = data_offset++;
				}
				fseek(fout, 1024 + data_offset*block_size, 0);
				bytes = fwrite(iblock_p, block_size, 1, fout);
				error_checking(bytes);
				i2block_p[j] = data_offset++;

			}
			fseek(fout, 1024 + data_offset*512, 0);
			bytes = fwrite(i2block_p, block_size, 1, fout);
			error_checking(bytes);
			inode[i]->i2block = data_offset++;

		}
		//for each i3blocks
		if(end==0)
		{
			if(inode[i]->i3block==0)
			{
				end=1;
				continue;
			}

			fseek(fin, 1024+block_size*inode[i]->i3block, 0);
			bytes = fread(i3block_p, block_size, 1, fin);
			error_checking(bytes);
			for(j=0;j<block_size/4;j++)
			{
				if(i3block_p[j]==0)
				{
					end = 1;
					break;
				}

				fseek(fin, 1024+block_size*i3block_p[j], 0);
				bytes = fread(i2block_p, block_size, 1, fin);
				error_checking(bytes);

				for(k=0;k<block_size/4;k++)
				{
					if(i2block_p[k]==0)
					{
						end = 1;
						break;
					}
					//copy iblocks

					fseek(fin, 1024 + block_size*i2block_p[j], 0);
					bytes = fread(iblock_p, block_size, 1, fin);
					error_checking(bytes);

					for(l=0;l<block_size/4;l++)
					{
						if(iblock_p[l]==0)
						{
							end = 1;
							break;
						}
						fseek(fin, 1024 + block_size*iblock_p[l], 0);
						bytes = fread(buffer, block_size, 1, fin);
						error_checking(bytes);
						fseek(fout, 1024 + data_offset*block_size, 0);
						bytes = fwrite(buffer, block_size, 1, fout);
						error_checking(bytes);
						iblock_p[l] = data_offset++;
					}
					fseek(fout, 1024 + data_offset*block_size, 0);
					bytes = fwrite(iblock_p, block_size, 1, fout);
					error_checking(bytes);
					i2block_p[k] = data_offset++;

				}
				fseek(fout, 1024 + data_offset*block_size, 0);
				bytes = fwrite(i2block_p, block_size, 1, fout);
				error_checking(bytes);
				i3block_p[j] = data_offset++;

			}
			fseek(fout, 1024 + data_offset*block_size, 0);
			bytes = fwrite(i3block_p, block_size, 1, fout);
			error_checking(bytes);
			inode[i]->i3block = data_offset++;

		}
	}


	//rewrite superblock to new file
	spbk->free_iblock = data_offset;
	fseek(fout, 512, 0);
	bytes =  fwrite(spbk, sizeof(SBK), 1, fout);
	error_checking(bytes);

	//rewrite inodes to new file
	//fseek(fout, 1024+block_size*spbk->inode_offset, 0);

	for(i=0;i<numOfInode;i++)
	{
		fseek(fout, 1024 + (spbk->inode_offset)*block_size+i*sizeof(IND), 0);
		bytes =  fwrite(inode[i], sizeof(IND), 1, fout);
		error_checking(bytes);
	}

    //write free blocks
    fseek(fout, 1024+block_size*data_offset, 0);
    for(;data_offset<spbk->swap_offset-1;data_offset++)
    {
        int *next_free_block = (int *)buffer;
        *next_free_block = data_offset+1;
        bytes = fwrite(buffer, block_size, 1, fout);
		error_checking(bytes);
    }


    //write the last free blocks
    int *next_free_block = (int *)buffer;
    *next_free_block = -1;
    bytes = fwrite(buffer, block_size, 1, fout);
	error_checking(bytes);
    data_offset++;
    //printf("%d \n", data_offset);

	//write swap region
	fseek(fin, 1024+block_size*spbk->swap_offset, 0);
	//fseek(fout, 1024+block_size*spbk->swap_offset, 0);
	i=0;
	while(!feof(fin))
	{
		fseek(fin, 1024+block_size*spbk->swap_offset+i*block_size, 0);
		fseek(fout, 1024+block_size*spbk->swap_offset+(i++)*block_size, 0);
		bytes = fread(buffer, block_size, 1, fin);
		if(bytes==1)
			bytes = fwrite(buffer, block_size, 1, fout);
			error_checking(bytes);
	}

		free(iblock_p);
		free(i2block_p);
		free(i3block_p);
	free(spbk);
	free(fileout);
	free(buffer);
	for(i=0;i<numOfInode;i++)
		free(inode[i]);
	free(inode);
	fclose(fin);
	fclose(fout);
	return 0;
}



char *newFileName(char *filein)
{
    int i;
    char* fileout = (char*)malloc(sizeof(char)*(10+strlen(filein)+strlen("-defrag")));
    for(i = 0 ;i<strlen(filein); fileout[i]=filein[i],i++)
        if(filein[i]=='.')
        {fileout[i]=0;break;}
    if(i==strlen(filein)) fileout[i]=0;
    	strcat(fileout,"-defrag");
    if(i!=strlen(filein))
        strcat(fileout,filein+i);
    return fileout;
}
void bootCopy(FILE* fin, FILE* fout)
{
	char * buffer = (char *)malloc(512);
	fseek(fin, 0, 0);
	bytes = fread(buffer, 512, 1, fin);
	error_checking(bytes);
	fseek(fout, 0, 0);
	bytes = fwrite(buffer, 512, 1, fout);
	error_checking(bytes);
	free(buffer);
}

void superblock_build(FILE* fin,FILE* fout, SBK *spbk)
{
	char * bf = (char *)malloc(512);
	fseek(fin, 512, 0);
	bytes = fread(bf, 512, 1, fin);
	error_checking(bytes);
	bytes = fwrite(bf, 512, 1, fout);//预写入，后面会修改spbk部分
	error_checking(bytes);
	memcpy(spbk, bf, 6*4);
	free(bf);
}



void buildIND(IND** inode, int numOfInode, FILE *fin, char * buffer)//建立inode链表
{
	int i;
	for(i=0;i<numOfInode;i++)
	{
		fseek(fin, 1024+(spbk->inode_offset)*block_size+i*sizeof(IND), 0);
		bytes = fread(buffer, sizeof(IND), 1, fin);
		error_checking(bytes);
		memcpy(inode[i], buffer, sizeof(IND));
	}
}

void areaCopyInodeToData(FILE *fin, FILE *fout, SBK *spbk, char * buffer )
{
	int numOfInode = block_size*spbk->data_offset/100;
	int start = 1024 + numOfInode*100;
	int amount = block_size*spbk->data_offset%100;
	fseek(fin, start, 0);
	bytes = fread(buffer, amount, 1, fin);
	error_checking(bytes);
	fseek(fout, start, 0);
	bytes = fwrite(buffer, amount, 1, fout);
	error_checking(bytes);
}

void error_checking(int stat)
{
	if(stat!=1)
	{
		//printf("文件读写失败,%d\n", stat);
	}

}

