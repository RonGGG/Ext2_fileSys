//
//  main.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/9.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include <stdio.h>
#include "initBlocks_disk.h"
#include "loadFromDisk.h"
#include "initUser.h"
#include "initTask.h"
#include "structs_def_file.h"
#include "checkInput.h"

#include "inode_operates.h"
#include <string.h>
/*全局可引用表
 struct ext2_super_block_memory superBlock_memory;    //定义全局superBlock结构体(内存中)
 struct ext2_inode_memory inodesTable_memory[128];     //定义全局inodeTable(内存中)
 int p_inodeTable = 0;               //永远指向inodeTable末尾
 char inodeMap_memory[16];                             //定义全局inode位图，因为最多有128个inode，128=16*8
 char blockMap_memory[64];                          //定义全局block位图，因为硬盘容量512块，一个位映射一个块，512=64*8
 
 struct User * currentUser;               //当前用户
 struct User * userList[10];              //用户列表,暂定最大10个用户
 
 struct task_struct * currentTask;       //当前进程
 struct task_struct * taskList[TASK_NUM_MAX];        //进程就绪队列
 
 struct file * file_table[NR_FILE_TABLE];//系统打开文件数组
 */
/*定义变量*/
char currentPwd[256];
/*引用变量*/
extern struct ext2_super_block_memory superBlock_memory;
extern struct User * currentUser;
extern struct User * userList[];/*用户列表,暂定最大10个用户*/
extern struct task_struct * currentTask;
/*函数*/
/*getchar的封装，可以接受空格，回车结束*/
void get_str(char * buff);

int main(int argc, const char * argv[]) {
    /*初始化模拟硬盘文件*/
    initBlocks();
    /*将硬盘中的文件系统装载到内存中,类似BOOT*/
    loadFileSysFromDisk();
    /*初始化用户*/
    initUser();//此处默认以Ron(普通用户)的身份登录
    //创建用户目录：
    struct ext2_inode_memory * inode;//inode即当前用户目录 /root/Ron/
    currentUser = userList[1];
    gzr_creat("/", "Ron", 3, Contents_File<<12, &inode);//这里需注意，Ron目录应该是root创建的，所以创建前需要切换用户
    currentUser = userList[0];
    currentUser->user_content = inode;//设定Ron用户目录
    find_inode(1, &userList[1]->user_content);//设定root用户目录
    /*初始化终端进程*/
    initTask(inode);
    /*打印展示界面*/
    printf("==========================\n");
    printf("Block counts:%d\n",superBlock_memory.s_blocks_count);
    printf("Inodes counts:%d\n",superBlock_memory.s_inodes_count);
    printf("Block size: %d B\n\n",superBlock_memory.s_log_block_size);
    printf("Free blocks count:%d\n",superBlock_memory.s_free_blocks_count);
    printf("Free inodes count:%d\n\n",superBlock_memory.s_free_inodes_count);
    printf("Total size:%d B\n",superBlock_memory.s_blocks_count*superBlock_memory.s_log_block_size);
    printf("==========================\n");
    
    char inputBuff[1024];   //用于接受用户输入
    while (1) {
        /**************/
//        extern struct ext2_inode_memory inodesTable_memory[];
//        extern int p_inodeTable;
//        extern char inodeMap_memory[];
//        extern char blockMap_memory[];
        
        /**************/
        if (currentPwd[0]=='\0') {
            printf("Mymac:~ %s$ ",currentUser->name);
        }else{
            printf("Mymac:%s %s$ ",currentPwd,currentUser->name);
        }
        memset(inputBuff, '\0', 1024);
        /*这里不能用scanf，因为空格不能被接受，所以重写函数封装getchar()*/
//        scanf("%s",inputBuff);
        get_str(inputBuff);
        checkInput(inputBuff);
    }
    return 0;
}
/*getchar的封装，可以接受空格，回车结束*/
void get_str(char * buff){
    char ch;
    int i=0;
    while ((ch = getchar())!=10) {
        buff[i] = ch;
        i+=1;
    }
}
