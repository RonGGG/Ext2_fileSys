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

#include "inode_operates.h"
/*引用变量*/
extern struct task_struct * currentTask;/*当前进程*/
extern struct User * currentUser;/*当前用户*/
extern char currentPwd[];
/*
 读取目录文件数据，每次读1块
 */
void readfile_perpage(struct ext2_inode_memory * inode){
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "r"))==NULL) {
        printf("fopen异常\n");
    }
    char buff[BLOCK_SIZE];
    uint32_t blocks = inode->i_blocks;//共有多少块
    uint32_t more = inode->i_size%BLOCK_SIZE/DIR_ENTRY_SIZE;//多余的结构体数 0则不多余
    if (blocks<=7) {//一级索引
        for(int i=0;i<blocks;i++) {
            uint32_t addr = inode->i_block[i]*BLOCK_SIZE;//打开块的地址
            memset(buff, '\0', BLOCK_SIZE);
            fseek(file, addr, SEEK_SET);
            if (fread(&buff, BLOCK_SIZE, 1, file)<1) {
                printf("fread error\n");
            }
            struct ext2_dir_entry_2 tmp;
            if (i==blocks-1) {//最后一个数据块
                for (int j=0; j<more; j++) {
                    memcpy(&tmp, buff+j*DIR_ENTRY_SIZE, DIR_ENTRY_SIZE);
                    printf("%s      ",tmp.name);
                }
            }else{
                for (int j=0; j<BLOCK_SIZE/DIR_ENTRY_SIZE; j++) {
                    memcpy(&tmp, buff+j*DIR_ENTRY_SIZE, DIR_ENTRY_SIZE);
                    printf("%s      ",tmp.name);
                }
                
                printf("已显示1个block,回车继续显示:");
                char ch=0;
                while (ch!='q') {
                    ch = getchar();
                    if (ch==10) {//回车
                        break;
                    }
                }
                if (ch=='q') break;
            }
        }
    }else if(blocks<=BLOCK_SIZE/sizeof(inode->i_block[0])){
        
    }else{
        
    }
    printf("\n");
    fclose(file);
}

void commands_ls(char * command){
    if (strlen(command)>2) {//有参数
        struct ext2_inode_memory * open_inode = NULL;
        char * curStr = command+3;
        char ch[256];
        analyse_pathTo_inode(curStr, &open_inode,ch);
        if ((open_inode->i_mode>>12)==Contents_File) {
            if (verify_priority(currentUser,open_inode)==DISS_OP) {
                printf("该用户权限不够\n");
                return;
            }
            readfile_perpage(open_inode);
        }else{
            printf("不能执行此命令，该文件不是目录文件\n");
        }
    }else{//无参数
        if ((currentTask->fs->pwd->i_mode>>12)==Contents_File) {//需判断当前目录是否是目录文件
            if (verify_priority(currentUser,currentTask->fs->pwd)==DISS_OP) {
                printf("该用户权限不够\n");
                return;
            }
            readfile_perpage(currentTask->fs->pwd);
        }else{
            printf("不能执行此命令，该文件不是目录文件\n");
        }
    }
}
void commands_cd(char * command){
    if (strlen(command)>2) {//有参数
        struct ext2_inode_memory * open_inode = NULL;
        char * curStr = command+3;
        analyse_pathTo_inode(curStr, &open_inode,currentPwd);//得到顶层文件
        if (open_inode->i_number==currentUser->user_content->i_number) {
            memset(currentPwd, '\0', 256);
            currentTask->fs->pwd = currentUser->user_content;
            return;
        }
        if ((open_inode->i_mode>>12)==Contents_File) {
            if (verify_priority(currentUser,open_inode)==DISS_OP) {
                printf("该用户权限不够\n");
                return;
            }
            //修改fs中pwd路径
            currentTask->fs->pwd = open_inode;
        }else{
            printf("不能执行此命令，该文件不是目录文件\n");
        }
    }else{
        //修改fs中pwd路径
        currentTask->fs->pwd = currentUser->user_content;
        //置空显示当前路径的数组：
        memset(currentPwd, '\0', 256);
        return;
    }
}
void commands_mkdir(char * command){
    
}
void commands_rmdir(char * command){
}
void commands_rm(char * command){
}
void commands_vim(char * command){
}
/*检查用户权限*/
int verify_priority(struct User * currentUsr,struct ext2_inode_memory * file_inode){
    /*这里知识简单的做一个权限处理，只用了priority+fileLevel*/
    int res = currentUsr->priority+(file_inode->i_mode & 0x0fff);
    if (res==1||res==2||res==3||res==7) {//WR
        return READ_AND_WRITE;
    }else if(res==0||res==4||res==5||res==6) {//R
        return READ;
    }else return DISS_OP;
    
}
