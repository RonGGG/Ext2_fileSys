//
//  initTask.h
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/11.
//  Copyright © 2018年  Ron. All rights reserved.
//

#ifndef initTask_h
#define initTask_h

#include <stdio.h>
#include "structs_def_file.h"
void initTask(struct ext2_inode_memory * inode);/*进程以及进程中的数据结构初始化*/

void initFs(void);
#endif /* initTask_h */
