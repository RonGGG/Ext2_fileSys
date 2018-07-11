//
//  commands.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/11.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include "commands.h"
#include <string.h>
#include <stdlib.h>
#include "structs_def_file.h"
extern struct task_struct * currentTask;
/*
 读取inode文件数据，每次读1块
 */
void readfile_perpage(struct ext2_inode_memory * inode){
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "r"))==NULL) {
        printf("fopen异常\n");
    }
    if (ftell(file)!=0) {
        rewind(file);
    }
    char buff[BLOCK_SIZE];
    uint32_t blocks = inode->i_blocks;//共有多少块
    if (blocks<=7) {//一级索引
        for(int i=0;i<blocks;i++) {
            inode->i_block[0];
            memset(buff, '\0', BLOCK_SIZE);
            if (fread(&buff, BLOCK_SIZE, 1, file)<1) {
                printf("fread error\n");
            }
            
        }
    }else if(blocks<=BLOCK_SIZE/sizeof(inode->i_block[0])){
        
    }else{
        
    }
}
void commands_ls(char * command){
    if (strlen(command)<2) {
        
    }else{
        struct ext2_inode_memory * fs_pwd = currentTask->fs->pwd;
        readfile_perpage(fs_pwd);
    }
}
void commands_cd(char * command){
    if (strlen(command)<2) {
        
    }else{
        
    }
}
