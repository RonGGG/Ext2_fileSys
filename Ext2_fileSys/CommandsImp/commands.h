//
//  commands.h
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/11.
//  Copyright © 2018年  Ron. All rights reserved.
//

#ifndef commands_h
#define commands_h

#include <stdio.h>
#include "structs_def_file.h"
/*
 读取目录文件数据(每次读1块)
 现在只能一级索引，高级索引没有写
 */
void readfile_perpage(struct ext2_inode_memory * inode);


void commands_ls(char * command);/*ls command*/
void commands_cd(char * command);/*cd command*/
void commands_mkdir(char * command);/*mkdir command*/
void commands_rmdir(char * command);/*rmdir command*/
void commands_rm(char * command);/*rm command*/
void commands_vim(char * command);/*vim command*/
#endif /* commands_h */
