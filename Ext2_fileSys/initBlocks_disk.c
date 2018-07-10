//
//  initBlocks_disk.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/9.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include "initBlocks_disk.h"
#include <sys/time.h>
void initBlocks(void){
    /*
     文件流写操作
     No.0 写入超级块 0-1023 B   共1K
     No.1 写入block位图 1024 - 2047 B   共1K
     No.2 写入inode位图 2048 - 3071 B   共1K
     No.3 写入inode表  3072 - 19455 B   共16K
     No.4-No.19 保留数据区 19456 - 35839 B   共16K
     No.20-No.511 非保留数据区 35840 - 524288 B 共477K
     */
    //打开文件流：
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "w+"))==NULL) {
        printf("fopen异常\n");
    }
    //No.0 写入超级块
    setMySuperblock(file);
    //No.1 写入block位图
    setMyBlockBit(file);
    //No.2 写入inode位图
    setMyInodeBit(file);
    //设置初始数据
    //并调用：No.3 写入inode表函数
    setOriginalData(file);
    
//    fseek(file, 0, SEEK_END);
//    printf("length :%ld\n",ftell(file));
    //关闭文件流：
    fclose(file);
}
/*
 写入superblock初始内容到模拟硬盘的文件中（占用1K）
 */
void setMySuperblock(FILE * file){
    struct timeval time;
    gettimeofday(&time, 0);   //获取当前时间,存入time中
    printf("初始化superlock:\n");
    struct ext2_super_block superBlock;
    superBlock.s_inode_size = 128;   //inode节点大小 128B
    superBlock.s_inodes_count = 128;   //inode节点数量 = inodetable块大小/inode大小 = 1024*16/128 = 128
    superBlock.s_blocks_count = 512;   //block的数量 512个
    superBlock.s_log_block_size = 1024;   //block的大小 1024B
    superBlock.s_r_blocks_count = 16;   //为超级用户保留的块数 16个
    
    superBlock.s_free_blocks_count = 477-3;  //空闲块数初始化为非保留数据区块数(这里还要减去初始化root文件那部分，共需3个block)
    superBlock.s_free_inodes_count = 128-3;   //空闲inode数 初始化为128-3 因为有初始化的3个文件
    superBlock.s_first_data_block = 35;   //第一个数据块号(非保留数据块) 除去前面的4个功能块和16个块的保留区
    superBlock.s_lastcheck = (uint32_t)time.tv_sec;   //最近一次检查ext_fs状态时间
//    superBlock.s_checkinterval = 0;   //最近两次检查最大允许间隔时间
    superBlock.s_def_resuid = 0;   //0为最高等级用户:root 表示只允许root操作保留块
    superBlock.s_wtime = (uint32_t)time.tv_sec;   //最后一次对superblock写的时间
    superBlock.s_state = EXT2_VALID_FS;   //文件系统状态： 正常
    superBlock.s_first_ino = 1;   //第一个非保留的inode号，默认从1开始
    superBlock.s_imap_blocks = 1;   //inode位图所占块数
    superBlock.s_imap_first_block = 2;  //第一个inode位图块块号
    superBlock.s_zmap_blocks = 1;   //block位图所占块数
    superBlock.s_zmap_first_block = 1;  //第一个block位图块块号
    
    /*
     通过char来转换成buf缓冲，并在结尾填个Q来验证（记得把Q删掉）
     这样做的目的是可以让superblock占用一整个块，虽然后面会有空白
     */
    char buf[BLOCK_SIZE] = "";
    memcpy(buf, &superBlock, sizeof(superBlock));
    //写superblock到模拟硬盘文件中
    if (ftell(file)!=0) {
        rewind(file);//这里保证每次superblock都在开头
    }
    if (fwrite(&buf, BLOCK_SIZE, 1, file)<=0) {
        printf("fwrite异常！\n");
    }
    printf("superblock硬盘中初始化成功\n");
}
/*
 将block位图的初始化内容写入模拟硬盘文件中（共1K）
 (容易计算出 只需要将前19位设置成1，后面都是0即可)
 */
void setMyBlockBit(FILE * file){
    printf("初始化blockBit:\n");
    if (ftell(file)!=BLOCK_SIZE) {
        fseek(file, BLOCK_SIZE, SEEK_SET);//移动指针到1k处，写入数据
    }
    char buff[BLOCK_SIZE];
    memset(buff, 0, BLOCK_SIZE);   //全部设置为0
    //设置前19位为1
    buff[0] = 0xff;
    buff[1] = 0xff;
    buff[2] = 0xe0;
    
    buff[35/8] = 0x1c;   //35-37 位需要置1，因为需要3块block
    if (fwrite(&buff, BLOCK_SIZE, 1, file)<=0) {
        printf("fwrite异常！\n");
    }
    printf("blockBit硬盘中初始化成功\n");
}
/*
 将inode位图初始化内容写入模拟硬盘文件中（共1K）
 因为最多128个inode，所以数组大小16
 */
void setMyInodeBit(FILE * file){
    printf("初始化inodeBit:\n");
    if (ftell(file)!=BLOCK_SIZE*2) {
        fseek(file, 2*BLOCK_SIZE, SEEK_SET);//移动指针到2k处，写入数据
    }
    //置空该区域
    char buff[BLOCK_SIZE];
    memset(buff, 0, sizeof(buff));
    //设置 前3个已使用
    buff[0] = 0xe0;
    //写入
    if (fwrite(&buff, BLOCK_SIZE, 1, file)<=0) {
        printf("fwrite异常！\n");
    }
    printf("inodeBit硬盘中初始化成功\n");
}
/*
 初始化inode表：(共16K)
 (一个inode节点大小固定为128B，共有9个zone，倒数第二个是二级索引，最后一个是三级索引)
 inodeTable部分可以同时存在128个inode节点
 */
void setMyInodeTable(FILE * file,int num[]){
    printf("初始化inodeTable区域:\n");
    if (ftell(file)!=BLOCK_SIZE*3) {
        fseek(file, 3*BLOCK_SIZE, SEEK_SET);//移动指针到3k处，写入数据
    }
    //置空该区域
    char buff[BLOCK_SIZE*16];
//    buff[BLOCK_SIZE*16-1] = 'E';
    memset(buff, 0, sizeof(buff));
    /*需要初始化三个inode*/
    //1.root目录文件
    struct timeval time;
    gettimeofday(&time, 0);
    struct ext2_inode inode;
    inode.i_mode = Contents_File << 12;  //高4位表示文件类型，所以左移12位
    inode.i_uid = 1;   //类型1表示普通用户拥有
    inode.i_size = num[0];   //注意：以字节记！！！！
    inode.i_atime = (uint32_t)time.tv_sec;   //以下这三个时间都暂时默认为现在
    inode.i_ctime = (uint32_t)time.tv_sec;
    inode.i_mtime = (uint32_t)time.tv_sec;
    inode.i_dtime = 0;   //文件删除时间，这个暂时不赋值
    inode.i_gid = 1;   //用户组标识，暂时填用户标识
    inode.i_links_count = 2;   //硬连接数，目录文件 >2
    inode.i_blocks = 2;   //文件所占block数量
    inode.i_flags = 0;   //暂时不赋值
    inode.i_block[0] = 35;   //root块号暂定为35，即非保留区的第1个数据块
    inode.i_block[1] = 36;
    char buff_inode[128] = "";
    memcpy(buff_inode, &inode, sizeof(inode));
    memcpy(buff, buff_inode, 128);
    
    //2.bin目录文件
    struct ext2_inode inode1;
    inode1.i_mode = Contents_File << 12;  //高4位表示文件类型，所以左移12位
    inode1.i_uid = 1;   //类型1表示普通用户拥有
    inode1.i_size = num[1];   //注意：以字节记！！！！
    inode1.i_atime = (uint32_t)time.tv_sec;   //以下这三个时间都暂时默认为现在
    inode1.i_ctime = (uint32_t)time.tv_sec;
    inode1.i_mtime = (uint32_t)time.tv_sec;
    inode1.i_dtime = 0;   //文件删除时间，这个暂时不赋值
    inode1.i_gid = 1;   //用户组标识，暂时填用户标识
    inode1.i_links_count = 2;   //硬连接数，目录文件 >2
    inode1.i_blocks = 1;   //文件所占block数量
    inode1.i_flags = 0;   //暂时不赋值
    inode1.i_block[0] = 37;   //bin块号暂定为36，即非保留区的第2个数据块
    char buff_inode1[128] = "";
    memcpy(buff_inode1, &inode1, sizeof(inode1));
    memcpy(buff+128, buff_inode1, 128);
    //3.hello.c 普通文件
    struct ext2_inode inode2;
    inode2.i_mode = Ordinary_File << 12;  //高4位表示文件类型，所以左移12位
    inode2.i_uid = 1;   //类型1表示普通用户拥有
    inode2.i_size = num[2];   //注意：以字节记！！！！
    inode2.i_atime = (uint32_t)time.tv_sec;   //以下这三个时间都暂时默认为现在
    inode2.i_ctime = (uint32_t)time.tv_sec;
    inode2.i_mtime = (uint32_t)time.tv_sec;
    inode2.i_dtime = 0;   //文件删除时间，这个暂时不赋值
    inode2.i_gid = 1;   //用户组标识，暂时填用户标识
    inode2.i_links_count = 2;   //硬连接数，目录文件 >2
    inode2.i_blocks = 1;   //文件所占block数量
    inode2.i_flags = 0;   //暂时不赋值
    inode2.i_block[0] = 38;   //bin块号暂定为37，即非保留区的第3个数据块
    char buff_inode2[128] = "";
    memcpy(buff_inode2, &inode2, sizeof(inode2));
    memcpy(buff+256, buff_inode2, 128);
    
    if (fwrite(&buff, BLOCK_SIZE*16, 1, file)<=0) {
        printf("fwrite异常！\n");
    }
    printf("inodeTable区域硬盘中初始化成功\n");
}
/*
 设置初始数据函数（root文件，bin文件，hello.c文件）
 */
void setOriginalData(FILE * file){
//    printf("%ld\n",ftell(file));
    if (ftell(file)!=BLOCK_SIZE*35) {
        fseek(file, 35*BLOCK_SIZE, SEEK_SET);//移动指针到35k处，写入数据
    }
    //1.root目录数据结构
//    struct ext2_dir_entry_2 dentry_root;
//    dentry_root.inode = 1;   //inode号
//    char name[255] = "root";
//    memcpy(dentry_root.name, name, 255);   //文件名字
//    dentry_root.name_len = 4;   //文件名字长度
//    dentry_root.file_type = (uint8_t)Contents_File;   //文件类型
//    dentry_root.rec_len = 4;   //目录数
    
    /*
     root文件内容设置
     */
    char buff[BLOCK_SIZE*3];  //root目录文件总大小 1K
    //1.写入"." ".."目录项
    struct ext2_dir_entry_2 dentry_this;
    dentry_this.inode = 1;   //inode号 root文件inode 即自己
    char this[247] = ".";
    memcpy(dentry_this.name, this, 247);   //文件名字
    dentry_this.name_len = 1;   //文件名字长度
    dentry_this.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_this.rec_len = 4;   //目录数 因为指向自己
    memcpy(buff, &dentry_this, DIR_ENTRY_SIZE);//写入buff
    
    struct ext2_dir_entry_2 dentry_father;
    dentry_father.inode = 1;   //inode号 root文件inode 即自己
    char father[247] = "..";
    memcpy(dentry_father.name, father, 247);   //文件名字
    dentry_father.name_len = 2;   //文件名字长度
    dentry_father.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_father.rec_len = 4;   //目录数 因为是根目录，也指向自己
    memcpy(buff+DIR_ENTRY_SIZE, &dentry_father, DIR_ENTRY_SIZE);//写入buff
    
    //2.写入bin目录项
    struct ext2_dir_entry_2 dentry_bin;
    dentry_bin.inode = 2;   //inode号
    char bin[247] = "bin";
    memcpy(dentry_bin.name, bin, 247);   //文件名字
    dentry_bin.name_len = 3;   //文件名字长度
    dentry_bin.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_bin.rec_len = 2;   //目录数
    memcpy(buff+DIR_ENTRY_SIZE*2, &dentry_bin, DIR_ENTRY_SIZE);//写入buff
    
    //3.hello文件目录项
    struct ext2_dir_entry_2 dentry_hello;
    dentry_hello.inode = 3;   //inode号
    char hello[247] = "hello.c";
    memcpy(dentry_hello.name, hello, 247);   //文件名字
    dentry_hello.name_len = 7;   //文件名字长度
    dentry_hello.file_type = (uint8_t)Ordinary_File;   //文件类型
    dentry_hello.rec_len = 0;   //目录数
    memcpy(buff+DIR_ENTRY_SIZE*3, &dentry_hello, DIR_ENTRY_SIZE);//写入buff
    
    /*
     bin 内容设置
     */
    char buff_bin[BLOCK_SIZE];
    //1.写入"." ".."目录项
    struct ext2_dir_entry_2 dentry_bin_this;
    dentry_bin_this.inode = 2;   //inode号
    char bin_this[247] = ".";
    memcpy(dentry_bin_this.name, bin_this, 247);   //文件名字
    dentry_bin_this.name_len = 1;   //文件名字长度
    dentry_bin_this.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_bin_this.rec_len = 2;   //目录数 因为指向自己
    memcpy(buff_bin, &dentry_bin_this, DIR_ENTRY_SIZE);//写入buff_bin
    
    struct ext2_dir_entry_2 dentry_bin_father;
    dentry_bin_father.inode = 1;   //inode号
    char bin_father[247] = "..";
    memcpy(dentry_bin_father.name, bin_father, 247);   //文件名字
    dentry_bin_father.name_len = 2;   //文件名字长度
    dentry_bin_father.file_type = (uint8_t)Contents_File;   //文件类型
    dentry_bin_father.rec_len = 4;   //目录数
    memcpy(buff_bin+DIR_ENTRY_SIZE, &dentry_bin_father, DIR_ENTRY_SIZE);//写入buff_bin
    
    memcpy(buff+BLOCK_SIZE, buff_bin, BLOCK_SIZE);//buff_bin写入buff
    /*
     hello.c 内容设置
     */
    char buff_hello[BLOCK_SIZE];
    char helloFile[512] = "hello,world!";
    helloFile[511] = 'H';   //用来debug
    memcpy(buff_hello, helloFile, 512);
    memcpy(buff+BLOCK_SIZE*2, buff_hello, BLOCK_SIZE);//buff_hello写入buff
    
    //4.写入硬盘数据区
    if (fwrite(&buff, BLOCK_SIZE*3, 1, file)<=0) {
        printf("fwrite异常！\n");
    }
    
    /*调用写入inodeTable函数*/
    int num[3] = {BLOCK_SIZE,512,512};
    setMyInodeTable(file,num);
}
