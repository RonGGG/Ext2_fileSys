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
//extern struct ext2_super_block superBlock_memory;
int main(int argc, const char * argv[]) {
    /*初始化模拟硬盘文件*/
    initBlocks();
    /*将硬盘中的文件系统装载到内存中,类似BOOT*/
    loadFileSysFromDisk();
    /*初始化用户进程*/
    return 0;
}
