//
//  inode_operates.h
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/12.
//  Copyright © 2018年  Ron. All rights reserved.
//

#ifndef inode_operates_h
#define inode_operates_h

#include <stdio.h>
#include "structs_def_file.h"
/*
 创建文件函数
 参数解释：(dir指定文件建立的目录位置，name为文件名，len为文件名长度，mode指定文件的类型和访问权限。参数res_inode返回新建inode的地址)
 */
void gzr_creat(char * dir,char * name,uint32_t name_len,uint16_t mode,struct ext2_inode_memory * inode);


/*
 得到空闲inode号(通过位图)
 参数：返回的inode号(如果有则设置成inode号，如果没有空闲inode则设置为0)
 */
void get_free_inode_number(uint32_t * inodeNum);


/*
 得到空闲block号(通过位图)
 参数：返回的block号(如果没有泽返回0)
 */
void get_free_block_number(uint32_t * blockNum);


/*
 申请空闲inode号(调用：修改inode)
 参数：预留地址
 */
void apply_free_inode(uint32_t * inodeNum);


/*
 修改inode位图
 参数：（inode号，修改为0/1）
 */
void setImap(uint32_t inode,uint8_t set_num);


/*
 解析路径
 参数：(char * 路径,struct ext2_inode_memory ** inode指针地址(因为是指针赋值))
 */
void analyse_pathTo_inode(char * path,struct ext2_inode_memory ** inode);

/*
 通过inode号找内存inode节点(没有就返回NULL)
 参数：（inodeNum，struct ext2_inode_memory ** inode指针赋值）
 */
void find_inode(uint32_t inodeNum,struct ext2_inode_memory ** inode);


/*
 在当前目录中找name文件，找到将inode号存在ret_inode_num中
 参数：（char * name,struct ext2_inode_memory * inode，int * ret_inode_num）
 */
void get_inodeNum_from_content(char * name , struct ext2_inode_memory * inode,uint32_t * ret_inode_num);

#endif /* inode_operates_h */
