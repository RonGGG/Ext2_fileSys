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
 */
extern struct ext2_super_block_memory superBlock_memory;
extern struct User * currentUser;
int main(int argc, const char * argv[]) {
    /*初始化模拟硬盘文件*/
    initBlocks();
    /*将硬盘中的文件系统装载到内存中,类似BOOT*/
    loadFileSysFromDisk();
    /*初始化用户*/
    initUser();//此处默认以Ron(普通用户)的身份登录
    /*初始化进程*/
    initTask();
    /*打印展示界面*/
    printf("==========================\n");
    printf("Block counts:%d\n",superBlock_memory.s_blocks_count);
    printf("Inodes counts:%d\n",superBlock_memory.s_inodes_count);
    printf("Block size: %d B\n\n",superBlock_memory.s_log_block_size);
    printf("Free blocks count:%d\n",superBlock_memory.s_free_blocks_count);
    printf("Free inodes count:%d\n\n",superBlock_memory.s_free_inodes_count);
    printf("Total size:%d B\n",superBlock_memory.s_blocks_count*superBlock_memory.s_log_block_size);
    printf("==========================\n");
    
    while (1) {
        char inputBuff[1024];   //用于接受用户输入
        printf("Mymac: %s$ ",currentUser->name);
        scanf("%s",inputBuff);
        checkInput(inputBuff);
    }
    return 0;
}
