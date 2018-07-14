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

/*所有外部函数*/

/*
 创建文件函数（通过父目录路径）
 参数解释：(dir指定文件建立的目录位置，name为文件名，len为文件名长度，mode指定文件的类型和访问权限。参数res_inode返回新建inode的指针地址)
 */
void gzr_creat(char * dir,char * name,uint32_t name_len,uint16_t mode,struct ext2_inode_memory ** inode);


/*
 创建文件函数（通过父节点）
 参数解释：(dir指定文件建立的目录inode，name为文件名，len为文件名长度，mode指定文件的类型和访问权限。参数res_inode返回新建inode的地址)
 */
void gzr_creat_with_fatherInode(struct ext2_inode_memory * father,char * name,uint32_t name_len,uint16_t mode,struct ext2_inode_memory ** inode);


/*
 删除普通文件
 参数：inode节点
 */
void gzr_rm_file_with_inode(struct ext2_inode_memory * inode);


/*
 删除目录文件
 参数：inode节点
 */
void gzr_rm_dir_with_inode(struct ext2_inode_memory * inode);


/*
 将硬盘，内存以及imap中的信息清空
 注意：这个方法适用需谨慎，因为直接无条件删除这些信息
 */
void clear_inode(struct ext2_inode_memory * inode);


/*
 直接删除inode所代表的文件数据函数(所有没有子文件的文件适用)
 已经修改位图
 */
void delete_file(struct ext2_inode_memory * inode);


/*
 权限检验
 参数：（操作用户，操作文件）
 返回值：1可以操作 0不可以操作
 */
int verify_priority(struct User * currentUsr,struct ext2_inode_memory * file_inode);


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
 修改inode位图(内存中)
 参数：（inode号，修改为0/1）
 */
void setImap(uint32_t inode,uint8_t set_num);


/*
 修改block位图(内存中)
 参数：（block号，修改为0/1）
 */
void setZmap(uint32_t block,uint8_t set_num);


/*
 解析路径（得到顶层目录项目）
 参数：(char * 路径,struct ext2_inode_memory ** inode指针地址(因为是指针赋值),顶层目录名)
 如果找不到inode就赋值NULL
 顶层目录名可以是NULL，就不赋值
 */
void analyse_pathTo_inode(char * path,struct ext2_inode_memory ** inode,char * ret_name);

/*
 通过inode号找内存inode节点(没有就返回NULL)
 参数：（inodeNum，struct ext2_inode_memory ** inode指针赋值）
 */
void find_inode(uint32_t inodeNum,struct ext2_inode_memory ** inode);



/*
 在当前目录中找name文件，找到将inode号存在ret_inode_num中
 参数：（char * name,struct ext2_inode_memory * inode，int * ret_inode_num）
 */
void get_inodeNum_from_content(char * name , struct ext2_inode_memory * inode,int * ret_inode_num);


/*
 初始化新的目录文件
 因为要写入两个指针目录项，所以用函数来生成
 参数：（buff是缓存，fatherNum是父目录项目数，inodeNums数组0处是自己的inode号1处是父节点inode号）
 */
void setEmpty_contentFile(char * buff,uint32_t fatherItemsNum,uint32_t inodeNums[]);


/*
 初始化新的inode结构体
 参数是基本值的设定
 */
void setEmpty_inode_struct(struct ext2_inode * inode,uint16_t imode,struct User * user,uint32_t time,uint32_t blockNum);




#endif /* inode_operates_h */
