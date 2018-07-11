//
//  initTask.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/11.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include "initTask.h"
#include "structs_def_file.h"
#include <stdlib.h>

#define TASK_NUM_MAX 10
struct task_struct * currentTask;
struct task_struct * taskList[TASK_NUM_MAX];/*进程就绪队列*/
/*内部函数*/
struct ext2_inode_memory * getInodeByNum(uint32_t inode){
    extern struct ext2_inode_memory inodesTable_memory[];
    extern int p_inodeTable;
    for (int i=0; i<p_inodeTable; i++) {
        if (inodesTable_memory[i].i_number==inode) {
            return &inodesTable_memory[i];
        }
    }
    return NULL;
}
void initTask(void){
    //初始化用户进程
    struct task_struct * task = (struct task_struct *)malloc(sizeof(struct task_struct));
    task->pid = 2;
    extern struct User * currentUser;
    task->owner = currentUser;
    struct fs_struct * fs = (struct fs_struct*)malloc(sizeof(struct fs_struct));//创建fs
    fs->count = 1;
    fs->root = getInodeByNum(1);
    struct files_struct * files = (struct files_struct*)malloc(sizeof(struct files_struct));//创建file_struct
    
    taskList[0] = task;
    currentTask = task;
}
