//
//  loadFromDisk.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/10.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include "loadFromDisk.h"
#include <stdlib.h>
#include "structs_def_file.h"
/*结构体列表*/
struct ext2_super_block_memory superBlock_memory;/*定义全局superBlock结构体(内存中)*/
struct ext2_inode_memory inodesTable_memory[128];/*定义全局inodeTable(内存中)*/
int p_inodeTable = 1;   /*永远指向inodeTable末尾,0处不填数据,为了保证和硬盘inodetable对齐*/
char inodeMap_memory[16];/*定义全局inode位图，因为最多有128个inode，128=16*8 */
char blockMap_memory[64];/*定义全局block位图，因为硬盘容量512块，一个位映射一个块，512=64*8 */
/*函数列表*/
void loadSuperBlock(FILE * file);/*加载superBlock*/
void loadInodeTable(FILE * file);/*加载inodeTable*/
void loadInode_bit(FILE * file);/*加载inode位图*/
void loadBlock_bit(FILE * file);/*加载块位图*/
/*
 从硬盘中引导文件系统
 （模拟BOOT的过程）
 */
void loadFileSysFromDisk(void){
    /*
     1.加载superblock
     2.加载blockmap
     3.加载inodemap
     4.加载inodetable
     5.加载其他（根目录文件结构等）
     */
    
    //打开文件：
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "r+"))==NULL) {
        printf("fopen异常\n");
    }
    //1.加载superblock
    loadSuperBlock(file);
    //2.加载inodeTable
    loadInodeTable(file);
    //3.加载imap
    loadInode_bit(file);
    //4.加载block位图
    loadBlock_bit(file);
    
    fclose(file);
}
/*
 加载superblock函数
 注意要添加一些变量在结尾，所以使用memory族的结构体
 */
void loadSuperBlock(FILE * file){
    printf("开始加载superBlock\n");
    
    if (ftell(file)!=0) {
        rewind(file);
    }
    if (fread(&superBlock_memory, SUPER_STRUCT_SIZE, 1, file)<1) {
        printf("fread error\n");
    }
    //写入部分数据
    superBlock_memory.dev_num = 4396;   /*设备号 4396*/
    superBlock_memory.access_count = 0;   /*访问进程数 暂时0*/
    superBlock_memory.mute_lock = 0;   /*互斥锁 0:可以访问 1:不可以访问*/
    printf("superBlock加载完毕\n");
}
/*
 加载inodeTable
 */
void loadInodeTable(FILE * file){
    printf("开始加载inodeTable\n");
    //通过superBlock拿到inodeTable首地址:(以字节为单位)
    uint32_t inodeTable_addr_disk = (superBlock_memory.s_imap_first_block+1)*superBlock_memory.s_log_block_size+superBlock_memory.s_inode_size;
    //inodeSize 以字节为单位
    uint32_t inodeSize = superBlock_memory.s_inode_size;
    //已使用的inode数量
    uint32_t count = superBlock_memory.s_inodes_count-superBlock_memory.s_free_inodes_count;
    //跳转到inodeTable开始处
    if (ftell(file)!=inodeTable_addr_disk) {
        fseek(file, inodeTable_addr_disk, SEEK_SET);
    }
    for (int i=0; i<count; i++) {
        struct ext2_inode_memory inode_tmp;
        if (fread(&inode_tmp, inodeSize, 1, file)<1) {
            printf("fread error\n");
        }
        inode_tmp.i_number = i+1;   /*inode号*/
        inode_tmp.i_count = 0;   /*访问此节点的进程数，暂时为0*/
        inode_tmp.i_flag = 0;   /*标志位，暂时用于互斥锁 0：即可以访问 1:不能访问*/
        inode_tmp.i_dev = superBlock_memory.dev_num;   /*设备*/
        
        inodesTable_memory[p_inodeTable] = inode_tmp;
        p_inodeTable+=1;
    }
    printf("inodeTable加载完毕\n");
}
/*
 加载inode位图
 */
void loadInode_bit(FILE * file){
    printf("开始加载inode位图\n");
    uint32_t imap_addr = superBlock_memory.s_imap_first_block*superBlock_memory.s_log_block_size;
    //跳转到imap开始处
    if (ftell(file)!=imap_addr) {
        fseek(file, imap_addr, SEEK_SET);
    }
    //这里size只需要16字节大小
    if (fread(&inodeMap_memory, sizeof(inodeMap_memory), 1, file)<1) {
        printf("fread error\n");
    }
    printf("inode位图加载完成\n");
}
/*
 加载block位图
 */
void loadBlock_bit(FILE * file){
    printf("开始加载block位图");
    uint32_t blockMap_addr = superBlock_memory.s_zmap_first_block*superBlock_memory.s_log_block_size;
    //跳转到zmap开始处
    if (ftell(file)!=blockMap_addr) {
        fseek(file, blockMap_addr, SEEK_SET);
    }
    //这里size只需要64字节大小
    if (fread(&blockMap_memory, sizeof(blockMap_memory), 1, file)<1) {
        printf("fread error\n");
    }
    printf("block位图加载完成\n");
}
