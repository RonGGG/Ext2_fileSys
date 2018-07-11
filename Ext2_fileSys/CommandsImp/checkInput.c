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
void checkInput(char * input){
    //这里暂时写2个命令,之后再加
    if (strncmp(input, "ls",2)==0) {
        commands_ls(input);
    }else if (strncmp(input, "cd",2)==0){
        commands_cd(input);
    }else{
        printf("Has not this command!\n");
    }
}
