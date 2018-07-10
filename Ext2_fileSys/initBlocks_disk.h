//
//  initBlocks_disk.h
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/9.
//  Copyright © 2018年  Ron. All rights reserved.
//

#ifndef initBlocks_disk_h
#define initBlocks_disk_h

#include <stdio.h>
#include <string.h>
#include "structs_def_file.h"
void initBlocks(void);//初始化各个block
void setMySuperblock(FILE * file);//初始化superBlock和调用初始化root目录函数
void setMyBlockBit(FILE * file);//初始化block位图
void setMyInodeBit(FILE * file);//初始化inode位图
void setMyInodeTable(FILE * file,int num[]);//初始化inodeTble
//void setMyRoot(FILE * file,struct ext2_super_block * superblock);//初始化root目录函数
void setOriginalData(FILE * file);
#endif /* initBlocks_disk_h */
