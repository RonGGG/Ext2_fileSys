//
//  check_disk.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/14.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include "check_disk.h"
#include "structs_def_file.h"
extern struct ext2_super_block_memory superBlock_memory;    //定义全局superBlock结构体(内存中)
extern struct ext2_inode_memory inodesTable_memory[];     //定义全局inodeTable(内存中)
extern int p_inodeTable;               //永远指向inodeTable末尾
extern char inodeMap_memory[];                             //定义全局inode位图，因为最多有128个inode，128=16*8
extern char blockMap_memory[];                          //定义全局block位图，因为硬盘容量512块，一个位映射一个块，512=64*8

extern struct User * currentUser;               //当前用户
extern struct User * userList[];              //用户列表,暂定最大10个用户

extern struct task_struct * currentTask;       //当前进程
extern struct task_struct * taskList[];        //进程就绪队列
void checkDisk(void){
    
}
