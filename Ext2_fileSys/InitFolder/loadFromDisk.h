//
//  loadFromDisk.h
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/10.
//  Copyright © 2018年  Ron. All rights reserved.
//

#ifndef loadFromDisk_h
#define loadFromDisk_h

#include <stdio.h>


#define INODE_TABLE_MAX 128  //内存中inodeTable数组限长
void loadFileSysFromDisk(void);/*加载硬盘中数据*/
#endif /* loadFromDisk_h */
