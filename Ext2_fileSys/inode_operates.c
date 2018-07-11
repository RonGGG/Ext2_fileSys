//
//  inode_operates.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/12.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include "inode_operates.h"

#include <string.h>
#include <stdlib.h>


/*函数实现部分*/
void gzr_creat(char * dir,char * name,uint32_t name_len,uint16_t mode,struct ext2_inode_memory * inode){
    /*
     1.申请空闲inode号（通过inode位图实现，并且修改inode位图实现）
     2.在内存inode表中找文件所在的父目录文件inode中的最后一个数据块，再找到写入点
     (真实文件大小 % block大小:
     为0 则找个新块填入，修改块位图；
     否则在该位置填入目录项，不用修改块位图)
     3.修改硬盘父目录的inode信息
     4.数据写入硬盘(暂定为1K大小)
     5.修改硬盘superblock中部分信息
     6.在硬盘inode表中写入一个inode结构体，修改块位图（inode位图在上面已经修改了）
     7.加载到内存inode表中
     8.加权限
     */
    //1.得到空闲inode号
    uint32_t inodeNum;
    apply_free_inode(&inodeNum);//记得协会磁盘！！！！！
    
    struct ext2_dir_entry_2 dir_struct;
    dir_struct.inode = inodeNum;
    memset(&dir_struct.name, '\0', DIR_NAME_SIZE);
    if (name_len>DIR_NAME_SIZE) {
        memcpy(dir_struct.name, name, DIR_NAME_SIZE);
        dir_struct.name_len = DIR_NAME_SIZE;
    }else{
        memcpy(dir_struct.name, name, name_len);
        dir_struct.name_len = name_len;
    }
    dir_struct.file_type = mode>>12;
    if(dir_struct.file_type==Contents_File){
        dir_struct.rec_len = 2;
    }else{
        dir_struct.rec_len = 0;
    }
    //2.找到写入点并写入父目录中相应位置
    struct ext2_inode_memory * father = NULL;
    analyse_pathTo_inode(dir, &father);
    //打开流
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "w+"))==NULL) {
        printf("fopen异常\n");
    }
    if (ftell(file)!=0) {
        rewind(file);
    }
    uint32_t more = father->i_size%(father->i_blocks*BLOCK_SIZE);
    if (more==0) {//找个新的块,修改块位图
        
    }else{//不用修改块位图
        uint32_t more_count = more/DIR_ENTRY_SIZE;
        uint32_t write_addr = father->i_block[father->i_blocks-1]*BLOCK_SIZE+more_count*DIR_ENTRY_SIZE;//写入地址,基址为0
        fseek(file, write_addr, SEEK_SET);
        if (fwrite(&dir_struct, DIR_ENTRY_SIZE, 1, file)) {
            printf("fread异常\n");
        }
        fclose(file);
    }
}
/*申请inode节点*/
void apply_free_inode(uint32_t * inodeNum){
    //找到空的inode号
    get_free_inode_number(inodeNum);
    //修改位图相应处为1
    setImap(*inodeNum, 1);
}
/*通过位图找空闲inode号*/
void get_free_inode_number(uint32_t * inodeNum){
    uint8_t res;
    extern char inodeMap_memory[];
    uint8_t num;
    for (int i=0; i<16; i++) {
        num = 0x80;
        uint8_t tmp = (uint8_t)inodeMap_memory[i];
        if (tmp!=0) {
            if (tmp!=255) {//如果该字节8位全是1，则直接跳出
                while (1) {
                    if (tmp & num) {
                        num = num>>1;
                    }else{
                        res = 0x80/num;
                        res = res/2;//这里就是当前这个字节中的inode偏移
                        break;
                    }
                }
                res = i*8+res;//+1因为inode从1开始
                *inodeNum = res;
                return;
            }
        }else{
            res = i*8+1;//+1因为inode从1开始
            *inodeNum = res;
            return;
        }
    }
    *inodeNum = 0;
}
void get_free_block_number(uint32_t * blockNum){
    
}
/*修改inode位图*/
void setImap(uint32_t inode,uint8_t set_num){
    uint32_t res_byte = inode/8;
    uint32_t res_bit = inode%8;
    extern char inodeMap_memory[];
    inodeMap_memory[res_byte*8+res_bit] = set_num;
}
/*解析路径*/
void analyse_pathTo_inode(char * path,struct ext2_inode_memory ** inode){
    int p_begin = 0,p_end = 0,tmp = 0;
    if (path[0]=='/') {
        p_begin+=1;
        tmp+=1;
    }
    
    char name[DIR_NAME_SIZE];
    struct ext2_inode_memory * currentInode = NULL;
    find_inode(1, &currentInode);//通过inode号拿到curr
    uint32_t ret_inodeNum;
    while (path[tmp]!='\0') {//结束标志
        while (path[tmp]!='/'&&path[tmp]!='\0') {//
            tmp+=1;
        }
        p_end = tmp;
        memset(name, '\0', DIR_NAME_SIZE);//清空字符缓存区
        strncpy(name, path+p_begin, p_end-p_begin);
        //在当前目录中找name文件,并返回inode号
        get_inodeNum_from_content(name, currentInode, &ret_inodeNum);
        //再次改变currentinode
        find_inode(ret_inodeNum, &currentInode);
        if (path[tmp]=='\0') {//没有‘/’的情况
            if (tmp==strlen(path)) {//顶层目录项
                *inode = currentInode;
            }
        }else{
            if (tmp==strlen(path)-1) {//顶层目录项
                *inode = currentInode;
            }
        }
        p_begin = p_end+1;
        tmp = p_begin;
    }
}
/*通过inumber找inode结构体地址*/
void find_inode(uint32_t inodeNum,struct ext2_inode_memory ** inode){
    extern struct ext2_inode_memory inodesTable_memory[];
    extern int p_inodeTable;
    for(int i=0;i<p_inodeTable;i++) {
        if (inodesTable_memory[i].i_number==inodeNum) {
            *inode = &inodesTable_memory[i];
//            memcpy(inode, &inodesTable_memory[i], INODE_STRUCT_SIZE);
            return;
        }
    }
    inode = NULL;
}
/*从目录中找inodenum，通过文件名*/
void get_inodeNum_from_content(char * name , struct ext2_inode_memory * inode,uint32_t * ret_inode_num){
    //1.先拷贝一个block到缓冲，没找到再拷贝下一个
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "r"))==NULL) {
        printf("fopen异常\n");
    }
    struct ext2_dir_entry_2 dentry;
    char buff[BLOCK_SIZE];
    //    uint32_t more_size_256 = (inode->i_size%BLOCK_SIZE)/256;
    for (int i=0; i<inode->i_blocks; i++) {
        if (ftell(file)!=inode->i_block[i]*BLOCK_SIZE) {
            fseek(file, inode->i_block[i]*BLOCK_SIZE, SEEK_SET);
        }
        //每次读一个block大小
        if (fread(buff, BLOCK_SIZE, 1, file)<1) {
            printf("fread异常\n");
        }
        for (int j=0; j<BLOCK_SIZE/DIR_ENTRY_SIZE; j++) {
            memcpy(&dentry, buff+j*DIR_ENTRY_SIZE, DIR_ENTRY_SIZE);
            if (strcmp(dentry.name, name)==0) {
                if (dentry.inode==0) {
                    printf("该目录项已删除\n");
                    *ret_inode_num = 0;
                    fclose(file);
                    return;
                }
                *ret_inode_num = dentry.inode;
                fclose(file);
                return;
            }
        }
    }
    fclose(file);
}
