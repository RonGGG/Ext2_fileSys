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
#include <sys/time.h>

/*函数实现部分*/
void gzr_creat(char * dir,char * name,uint32_t name_len,uint16_t mode,struct ext2_inode_memory * inode){
    /*
     1.申请空闲inode号（通过inode位图实现，并且修改inode位图实现）
     2.在内存inode表中找文件所在的父目录文件inode中的最后一个数据块，再找到写入点
     (真实文件大小 % block大小:
     为0 则找个新块填入，修改块位图；
     否则在该位置填入目录项，不用修改块位图)
     3.修改硬盘父目录的inode信息(inodesize，如果加入新块需要修改iblock数组和iblocks，时间信息)
     4.数据写入硬盘(暂定为1K大小) 修改块位图
     5.在硬盘inode表中写入一个inode结构体，不用修改块位图（inode位图在上面已经修改了）
     记得加权限！
     6.修改硬盘superblock中部分信息(mem->disk)(时间信息，空闲inode信息，空闲block信息)
     7.修改硬盘inode表，修改内存inode表，内存中位图写入硬盘中，superblock写入硬盘中，
     */
    //此过程要占用的块数
    uint32_t free_blocks = 0;
    //1.得到空闲inode号
    uint32_t inodeNum;
    apply_free_inode(&inodeNum);//结尾写入硬盘
    
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
    char ch[256] = "";
    analyse_pathTo_inode(dir, &father,ch);
    
    //打开流
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "r+"))==NULL) {
        printf("fopen异常\n");
    }
    
    uint32_t more = father->i_size%(father->i_blocks*BLOCK_SIZE);
    if (more==0) {//找个新的块,修改块位图
        //得到空闲blocknum(算法同得到空闲inode算法)
        uint32_t free_blockNum;
        get_free_block_number(&free_blockNum);
        if (free_blockNum<35) {
            printf("没有权限写入此block处\n");
            fclose(file);
            return;
        }
        //找到写入block,写入数据
        fseek(file, free_blockNum*BLOCK_SIZE, SEEK_SET);
        //修改block位图,记得写入硬盘
        setZmap(free_blockNum, 1);
        free_blocks+=1;
        //修改父目录inode信息(iblocks和iblock[])
        father->i_block[father->i_blocks] = free_blockNum;
        father->i_blocks = father->i_blocks+1;
    }else{//不用修改块位图
        uint32_t more_count = more/DIR_ENTRY_SIZE;
        uint32_t write_addr = father->i_block[father->i_blocks-1]*BLOCK_SIZE+more_count*DIR_ENTRY_SIZE;//写入地址,基址为0
        fseek(file, write_addr, SEEK_SET);
    }
    if (fwrite(&dir_struct, DIR_ENTRY_SIZE, 1, file)<1) {
        printf("fwrite异常\n");
    }
    
    //修改父inode信息（内存中）：inodesize，时间信息（如果修改block信息，上面已经修改了）写入了硬盘
    father->i_size = father->i_size+DIR_ENTRY_SIZE;
    struct timeval time;
    gettimeofday(&time, 0);
    father->i_atime = (uint32_t)time.tv_sec;
    father->i_ctime = (uint32_t)time.tv_sec;
    father->i_mtime = (uint32_t)time.tv_sec;
    char inodeOnedisk[128];
    memcpy(&inodeOnedisk, father, INODE_STRUCT_SIZE);
    memset(inodeOnedisk+108, '\0', 20);
    struct ext2_inode in;
    memcpy(&in, inodeOnedisk, INODE_STRUCT_SIZE);
    fseek(file, 3*BLOCK_SIZE+INODE_STRUCT_SIZE*father->i_number, SEEK_SET);
    if (fwrite(&in, INODE_STRUCT_SIZE, 1, file)<1) {
        printf("fwrite异常\n");
    }
    //真实数据写入硬盘
    char buff[BLOCK_SIZE];//这里初始化一个块大小
//    buff[0] = 'B';
//    buff[BLOCK_SIZE-1] = 'E';
    if(dir_struct.file_type==Contents_File){//目录文件情况,add'.' '..'
        uint32_t nums[2];
        nums[0] = inodeNum;
        nums[1] = father->i_number;
        setEmpty_contentFile(buff, father->i_size/DIR_ENTRY_SIZE, nums);//写入两个目录项'.'和'..'
    }
    uint32_t freeBlockNum_data;
    get_free_block_number(&freeBlockNum_data);//再新块存数据获取
    if (freeBlockNum_data<35) {
        printf("没有权限写入此block处\n");
        fclose(file);
        return;
    }
    fseek(file, freeBlockNum_data*BLOCK_SIZE, SEEK_SET);
    if (fwrite(&buff, BLOCK_SIZE, 1, file)<1) {
        printf("fwrite异常\n");
    }
    //修改block位图,写入硬盘
    setZmap(freeBlockNum_data, 1);
    free_blocks+=1;
    //5.在硬盘inode表中写入一个inode结构体，不用修改块位图（inode位图在上面已经修改了）
    //记得加权限！
    struct ext2_inode inode_disk;
    extern struct User * currentUser;
    setEmpty_inode_struct(&inode_disk, mode, currentUser, (uint32_t)time.tv_sec, freeBlockNum_data);//初始化inode
    fseek(file, 3*BLOCK_SIZE+inodeNum*INODE_STRUCT_SIZE, SEEK_SET);
    if (fwrite(&inode_disk, INODE_STRUCT_SIZE, 1, file)<1) {
        printf("fwrite异常\n");
    }
    //同时写入内存
    extern struct ext2_super_block_memory superBlock_memory;
    extern struct ext2_inode_memory inodesTable_memory[];
    struct ext2_inode_memory inode_mem;
    memcpy(&inode_mem, &inode_disk, INODE_STRUCT_SIZE);
    inode_mem.i_number = inodeNum;
    inode_mem.i_count = 0;
    inode_mem.i_flag = 0;   /*标志位，暂时用于互斥锁 0：即可以访问 1:不能访问*/
    inode_mem.i_dev = superBlock_memory.dev_num;   /*设备*/
    memcpy(&inodesTable_memory[inodeNum], &inode_mem, INODE_STRUCT_SIZE);
    //修改super信息，并写入硬盘 (时间信息，空闲inode信息，空闲block信息)
    superBlock_memory.s_free_inodes_count-=1;
    superBlock_memory.s_free_blocks_count-=free_blocks;
    superBlock_memory.s_wtime = (uint32_t)time.tv_sec;
    //super->disk
    fseek(file, 0, SEEK_SET);
    if (fwrite(&superBlock_memory, SUPER_STRUCT_SIZE, 1, file)<1) {
        printf("fwrote异常\n");
    }
    //imap->disk
    fseek(file, 2*BLOCK_SIZE, SEEK_SET);
    extern char inodeMap_memory[];
    if (fwrite(&inodeMap_memory, BLOCK_SIZE, 1, file)<1) {
        printf("fwrite异常\n");
    }
    //zmap->disk
    extern char blockMap_memory[];
    fseek(file, BLOCK_SIZE, SEEK_SET);
    if (fwrite(&blockMap_memory, BLOCK_SIZE, 1, file)) {
        printf("fread异常\n");
    }
    *inode = inode_mem;
    fclose(file);
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
                int count = 1;
                
                while (1) {
                    if (tmp & num) {
                        num = num>>1;
                        count+=1;
                    }else{
//                        res = 0x80/num;
//                        res = res/2;//这里就是当前这个字节中的inode偏移
                        break;
                    }
                }
                res = i*8+count;//+1因为inode从1开始
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
/*通过位图找空闲block号*/
void get_free_block_number(uint32_t * blockNum){
    uint8_t res;
    extern char blockMap_memory[];
    uint8_t num;
    for (int i=0; i<64; i++) {
        num = 0x80;
        uint8_t tmp = (uint8_t)blockMap_memory[i];
        if (tmp!=0) {
            if (tmp!=255) {//如果该字节8位全是1，则直接跳出
                int count = 0;
                while (1) {
                    if (tmp & num) {
                        num = num>>1;
                        count+=1;
                    }else{
//                        res = 0x80/num;
//                        res = res/2;//这里就是当前这个字节中的inode偏移
                        break;
                    }
                }
                res = i*8+count;
                *blockNum = res;
                return;
            }
        }else{
            res = i*8;
            *blockNum = res;
            return;
        }
    }
    *blockNum = 0;
}
/*修改inode位图*/
void setImap(uint32_t inode,uint8_t set_num){
    uint32_t res_byte = inode/8;
    uint32_t res_bit = inode%8;
    extern char inodeMap_memory[];
    inodeMap_memory[res_byte] = inodeMap_memory[res_byte]+(0x80>>res_bit<<1);
}
/*修改block位图*/
void setZmap(uint32_t block,uint8_t set_num){
    uint32_t res_byte = block/8;
    uint32_t res_bit = block%8;
    extern char blockMap_memory[];
    blockMap_memory[res_byte] = blockMap_memory[res_byte] + (0x80>>res_bit);
}
/*解析路径(获得顶层目录项)*/
void analyse_pathTo_inode(char * path,struct ext2_inode_memory ** inode,char * ret_name){
    if (strcmp(path, "/")==0 || strcmp(path, "/root/")==0 || strcmp(path, "/root")==0) {
        struct ext2_inode_memory * inode_root = NULL;
        find_inode(1, &inode_root);//通过inode号拿到curr
        *inode = inode_root;
        strncpy(ret_name, "/",1);
        return;
    }
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
        strncpy(ret_name, path+p_begin,p_end-p_begin);
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
/*初始化新生成的目录文件*/
void setEmpty_contentFile(char * buff,uint32_t fatherItemsNum,uint32_t inodeNums[]){
    //1.写入"."
    struct ext2_dir_entry_2 dentry_this;
    dentry_this.inode = inodeNums[0];   //inode号 root文件inode 即自己
    char this[247] = ".";
    memcpy(dentry_this.name, this, 247);   //文件名字
    dentry_this.name_len = 1;   //文件名字长度
    dentry_this.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_this.rec_len = 2;   //目录数 因为指向自己
    memcpy(buff, &dentry_this, DIR_ENTRY_SIZE);//写入buff
    //".."目录项
    struct ext2_dir_entry_2 dentry_father;
    dentry_father.inode = inodeNums[1];   //inode号
    char father[247] = "..";
    memcpy(dentry_father.name, father, 247);   //文件名字
    dentry_father.name_len = 2;   //文件名字长度
    dentry_father.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_father.rec_len = fatherItemsNum;   //目录数
    memcpy(buff+DIR_ENTRY_SIZE, &dentry_father, DIR_ENTRY_SIZE);//写入buff
}
void setEmpty_inode_struct(struct ext2_inode * inode,uint16_t imode,struct User * user,uint32_t time,uint32_t blockNum){
    if (user->priority==0) {//root
        inode->i_mode = user->priority+1;  //高4位表示文件类型,权限为当前用户级别可读写，低级别只可读
    }else{//Ron
        inode->i_mode = user->priority-1;  //高4位表示文件类型,权限为当前用户级别可读写，低级别只可读
    }
    inode->i_uid = user->uid;
    if ((imode>>12)==Contents_File) {
        inode->i_size = DIR_ENTRY_SIZE*2;
    }else{
        inode->i_size = 0;   //注意：以字节记！！！！
    }
    inode->i_atime = time;   //以下这三个时间都暂时默认为现在
    inode->i_ctime = time;
    inode->i_mtime = time;
    inode->i_dtime = 0;   //文件删除时间，这个暂时不赋值
    inode->i_gid = user->uid;   //用户组标识，暂时填用户标识
    if ((imode>>12)==Contents_File) {//目录文件
        inode->i_links_count = 2;   //硬连接数
    }else{
        inode->i_links_count = 1;   //硬连接数
    }
    inode->i_blocks = 1;   //文件所占block数量
    inode->i_flags = 0;   //暂时不赋值
    inode->i_block[0] = blockNum;
}
