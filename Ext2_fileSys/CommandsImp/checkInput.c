//
//  checkInput.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/11.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include "checkInput.h"
#include "commands.h"
#include <string.h>

extern struct task_struct * currentTask;/*当前进程*/
void checkInput(char * input){
    //这里暂时写2个命令,之后再加
    if (strncmp(input, "ls",2)==0) {//显示目录文件数据
        commands_ls(input);
    }else if (strncmp(input, "cd",2)==0){//当前目录的修改
        commands_cd(input);
    }else if (strncmp(input, "vim",3)==0){//集打开(显示)，创建，修改，写入 于一体（仅限Ordinary文件）
        commands_vim(input);
    }else if (strncmp(input, "mkdir",5)==0){//创建目录文件
        commands_mkdir(input);
    }else if (strncmp(input, "rmdir",5)==0){//删除目录文件
        commands_rmdir(input);
    }else if (strncmp(input, "rm",2)==0){//删除文件（Ordinary类型文件）
        commands_rm(input);
    }else{
        printf("Has not this command!\n");
    }
}
