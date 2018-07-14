//
//  commands.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/11.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include "commands.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "inode_operates.h"
/*引用变量*/
extern struct task_struct * currentTask;/*当前进程*/
extern struct User * currentUser;/*当前用户*/
extern char currentPwd[];
/*函数*/
/*getchar的封装，可以接受空格，回车结束*/
void get_str_op(char * buff);
/*vim创建子函数：*/
void vim_op_with_inode(char * name,struct ext2_inode_memory * father);//通过inode节点
void vim_op_with_path(char * name,char * fatherPath);//通过path
/*vim查看修改子函数*/
void vim_exist(struct ext2_inode_memory * inode);
/*
 读取目录文件数据，每次读1块
 */
void readfile_perpage(struct ext2_inode_memory * inode){
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "r"))==NULL) {
        printf("fopen异常\n");
    }
    char buff[BLOCK_SIZE];
    uint32_t blocks = inode->i_blocks;//共有多少块
    uint32_t more = inode->i_size%BLOCK_SIZE/DIR_ENTRY_SIZE;//多余的结构体数 0则不多余
    if (blocks<=7) {//一级索引
        for(int i=0;i<blocks;i++) {
            uint32_t addr = inode->i_block[i]*BLOCK_SIZE;//打开块的地址
            memset(buff, '\0', BLOCK_SIZE);
            fseek(file, addr, SEEK_SET);
            if (fread(&buff, BLOCK_SIZE, 1, file)<1) {
                printf("fread error\n");
            }
            struct ext2_dir_entry_2 tmp;
            if (i==blocks-1) {//最后一个数据块
                if (more==0) {
                    for (int j=0; j<BLOCK_SIZE/DIR_ENTRY_SIZE; j++) {
                        memcpy(&tmp, buff+j*DIR_ENTRY_SIZE, DIR_ENTRY_SIZE);
                        if (tmp.name[0]!='\0') {
                            printf("%s      ",tmp.name);
                        }
                        
                    }
                }else{
                    for (int j=0; j<more; j++) {
                        memcpy(&tmp, buff+j*DIR_ENTRY_SIZE, DIR_ENTRY_SIZE);
                        if (tmp.name[0]!='\0') {
                            printf("%s      ",tmp.name);
                        }
                    }
                }
            }else{
                for (int j=0; j<BLOCK_SIZE/DIR_ENTRY_SIZE; j++) {
                    memcpy(&tmp, buff+j*DIR_ENTRY_SIZE, DIR_ENTRY_SIZE);
                    if (tmp.name[0]!='\0') {
                        printf("%s      ",tmp.name);
                    }
                }
                
//                printf("已显示1个block,回车继续显示:");
//                char ch=0;
//                while (ch!='q') {
//                    ch = getchar();
//                    if (ch==10) {//回车
//                        break;
//                    }
//                }
//                if (ch=='q') break;
            }
            printf("\n");
        }
    }else if(blocks<=BLOCK_SIZE/sizeof(inode->i_block[0])){
        
    }else{
        
    }
//    printf("\n");
    fclose(file);
}

void commands_ls(char * command){
    if (strlen(command)>2) {//有参数
        struct ext2_inode_memory * open_inode = NULL;
        char * curStr = command+3;
        char ch[256];
        analyse_pathTo_inode(curStr, &open_inode,ch);
        if ((open_inode->i_mode>>12)==Contents_File) {
            if (verify_priority(currentUser,open_inode)==DISS_OP) {
                printf("该用户权限不够\n");
                return;
            }
            readfile_perpage(open_inode);
        }else{
            printf("不能执行此命令，该文件不是目录文件\n");
        }
    }else{//无参数
        if ((currentTask->fs->pwd->i_mode>>12)==Contents_File) {//需判断当前目录是否是目录文件
            if (verify_priority(currentUser,currentTask->fs->pwd)==DISS_OP) {
                printf("该用户权限不够\n");
                return;
            }
            readfile_perpage(currentTask->fs->pwd);
        }else{
            printf("不能执行此命令，该文件不是目录文件\n");
        }
    }
}
void commands_cd(char * command){
    if (strlen(command)>2) {//有参数
        struct ext2_inode_memory * open_inode = NULL;
        char * curStr = command+3;
        analyse_pathTo_inode(curStr, &open_inode,currentPwd);//得到顶层文件
        if (open_inode->i_number==currentUser->user_content->i_number) {
            memset(currentPwd, '\0', 256);
            currentTask->fs->pwd = currentUser->user_content;
            return;
        }
        if ((open_inode->i_mode>>12)==Contents_File) {
            if (verify_priority(currentUser,open_inode)==DISS_OP) {
                printf("该用户权限不够\n");
                return;
            }
            //修改fs中pwd路径
            currentTask->fs->pwd = open_inode;
        }else{
            printf("不能执行此命令，该文件不是目录文件\n");
        }
    }else{
        //修改fs中pwd路径
        currentTask->fs->pwd = currentUser->user_content;
        //置空显示当前路径的数组：
        memset(currentPwd, '\0', 256);
        return;
    }
}
void commands_mkdir(char * command){
    if (strlen(command)>5) {//有参数
        struct ext2_inode_memory * open_inode = NULL;
        char * curStr = command+6;
        char name[256] = "";
        analyse_pathTo_inode(curStr, &open_inode,name);//得到顶层文件
        if (open_inode==NULL) {//说明该名字的文件不存在，可以创建
            //创建目录：
            struct ext2_inode_memory * content_inode;
            if (strcmp(name, curStr)==0) {//在当前用户目录创建
                gzr_creat_with_fatherInode(currentTask->fs->pwd,name, (uint32_t)strlen(name), Contents_File<<12, &content_inode);
            }else{
                gzr_creat(curStr, name, (uint32_t)strlen(name), Contents_File<<12, &content_inode);
            }
        }else{//说明该名字文件存在，不允许创建
            printf("该文件已存在\n");
        }
    }else{
        printf("请输入参数\n");
    }
}
void commands_rmdir(char * command){
    if (strlen(command)>5) {//有参数
        struct ext2_inode_memory * open_inode = NULL;
        char * curStr = command+6;
        char name[256] = "";
        analyse_pathTo_inode(curStr, &open_inode,name);//得到顶层文件
        if (open_inode!=NULL) {//说明该名字的文件存在，可以删除
            gzr_rm_dir_with_inode(open_inode);
        }else{//说明该名字文件不存在，不允许删除
            printf("该文件不存在\n");
        }
    }else{
        printf("请输入参数\n");
    }
}
//删除普通文件
void commands_rm(char * command){
    if (strlen(command)>2) {//有参数
        struct ext2_inode_memory * open_inode = NULL;
        char * curStr = command+3;
        char name[256] = "";
        analyse_pathTo_inode(curStr, &open_inode,name);//得到顶层文件
        if (open_inode!=NULL) {//说明该名字的文件存在，可以删除
            gzr_rm_file_with_inode(open_inode);
        }else{//说明该名字文件不存在，不允许删除
            printf("该文件不存在\n");
        }
    }else{
        printf("请输入参数\n");
    }
}
void commands_vim(char * command){
    if (strlen(command)>3) {//有参数
        struct ext2_inode_memory * open_inode = NULL;
        char * curStr = command+4;
        char name[256] = "";
        analyse_pathTo_inode(curStr, &open_inode,name);//得到顶层文件
        if (open_inode==NULL) {//说明该名字的文件不存在，可以创建
            printf("Please Input:\n");
            if (strcmp(name, curStr)==0) {//在当前用户目录创建
                vim_op_with_inode(name, currentTask->fs->pwd);
            }else{
                vim_op_with_path(name, curStr);
            }
        }else{//说明该名字文件存在，不允许创建,可以修改和查看(修改注意权限问题)
//            printf("该文件已存在\n");
            vim_exist(open_inode);
        }
    }else{
        printf("vim 请输入参数\n");
    }
}
void vim_op_with_inode(char * name,struct ext2_inode_memory * father){//这里的inode是父目录inode
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "r+"))==NULL) {
        printf("fopen异常\n");
    }
    //1.编辑写入缓存
    char buff[BLOCK_SIZE*512] = "";
    get_str_op(buff);
    //2.计算写入缓存
    uint32_t lens = (uint32_t)strlen(buff);
    
    uint32_t blocks = lens/BLOCK_SIZE+1;
//    uint32_t more = lens%BLOCK_SIZE;
    //3.创建普通文件
    struct ext2_inode_memory * content_inode;//这个是创建的文件inode
    gzr_creat_with_fatherInode(father,name, (uint32_t)strlen(name), Ordinary_File<<12, &content_inode);
    //4.修改inode信息（内存）
    content_inode->i_size = lens;//设置真实大小
    content_inode->i_blocks = blocks;//设置块数量
    //5.写入文件
    char write_buff_block[BLOCK_SIZE];
    for (int i=0; i<blocks; i++) {
        memset(write_buff_block, '\0', BLOCK_SIZE);//清空
        memcpy(write_buff_block, buff+i*BLOCK_SIZE, BLOCK_SIZE);//将1块写入buff
        if (i==0) {//第一个block已经创建
            fseek(file, content_inode->i_block[i]*BLOCK_SIZE, SEEK_SET);
        }else{//需要申请一个新块的内存
            //得到空闲块号
            uint32_t blockNum;
            get_free_block_number(&blockNum);
            //设置该文件inode中block数组那一项块号
            content_inode->i_block[i] = blockNum;
            //设置块位图
            setZmap(blockNum, 1);
        }
        if (fwrite(write_buff_block, BLOCK_SIZE, 1, file)<1) {
            printf("fwrite异常！\n");
        }
    }
    //块位图写回硬盘
    extern char blockMap_memory[];
    fseek(file, BLOCK_SIZE, SEEK_SET);
    if (fwrite(blockMap_memory, BLOCK_SIZE, 1, file)<1) {
        printf("fwrite异常\n");
    }
    fclose(file);
}
void vim_op_with_path(char * name,char * fatherPath){
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "r+"))==NULL) {
        printf("fopen异常\n");
    }
    //1.编辑写入缓存
    char buff[BLOCK_SIZE*512] = "";
    get_str_op(buff);
    //2.计算写入缓存
    uint32_t lens = (uint32_t)strlen(buff);
    
    uint32_t blocks = lens/BLOCK_SIZE;
//    uint32_t more = lens%BLOCK_SIZE;
    //3.创建普通文件
    struct ext2_inode_memory * content_inode;
    gzr_creat(fatherPath, name, (uint32_t)strlen(name), Ordinary_File<<12, &content_inode);
    //4.修改inode信息（内存）
    content_inode->i_size = lens;//设置真实大小
    content_inode->i_blocks = blocks;//设置块数量
    //5.写入文件
    char write_buff_block[BLOCK_SIZE];
    for (int i=0; i<blocks; i++) {
        memset(write_buff_block, '\0', BLOCK_SIZE);//清空
        memcpy(write_buff_block, buff+i*BLOCK_SIZE, BLOCK_SIZE);//将1块写入buff
        if (i==0) {//第一个block已经创建
            fseek(file, content_inode->i_block[i]*BLOCK_SIZE, SEEK_SET);
        }else{//需要申请一个新块的内存
            //得到空闲块号
            uint32_t blockNum;
            get_free_block_number(&blockNum);
            //设置该文件inode中block数组那一项块号
            content_inode->i_block[i] = blockNum;
            //设置块位图
            setZmap(blockNum, 1);
        }
        if (fwrite(write_buff_block, BLOCK_SIZE, 1, file)<1) {
            printf("fwrite异常！\n");
        }
    }
    //块位图写回硬盘
    extern char blockMap_memory[];
    fseek(file, BLOCK_SIZE, SEEK_SET);
    if (fwrite(blockMap_memory, BLOCK_SIZE, 1, file)<1) {
        printf("fwrite异常\n");
    }
    fclose(file);
}
/*查看并修改vim的程序*/
void vim_exist(struct ext2_inode_memory * inode){
    FILE * file = NULL;
    if ((file = fopen("/Users/ron/Downloads/ext_file", "r+"))==NULL) {
        printf("fopen异常\n");
    }
    //1.一次显示一个块
    char buff[BLOCK_SIZE];
    for (int i=0; i<inode->i_blocks; i++) {
        memset(buff, '\0', BLOCK_SIZE);//清空
        fseek(file, inode->i_block[i]*BLOCK_SIZE, SEEK_SET);//找到显示的块
        if (fread(buff, BLOCK_SIZE, 1, file)<1) {
            printf("fread error\n");
        }
        printf("%s\n",buff);
//        //把当前缓存数据写到标准输入流缓存中
//        if (write(0, buff, BLOCK_SIZE)==-1) {
//            printf("write error\n");
//        }
        while (1) {
            printf("回车继续显示，q退出:\n");
            char ch = getchar();
            if (ch==10) {//回车
                break;
            }else if (ch=='q'){
                return;
            }else;
        }
    }
    fclose(file);
}
/*getchar的封装，可以接受空格，q结束*/
void get_str_op(char * buff){
    char ch;
    int i=0;
    while ((ch = getchar())!=10) {
        buff[i] = ch;
        i+=1;
    }
}

