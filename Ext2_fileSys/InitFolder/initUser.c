//
//  initUser.c
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/11.
//  Copyright © 2018年  Ron. All rights reserved.
//

#include "initUser.h"
#include <string.h>
#include <stdlib.h>
#include "structs_def_file.h"
/*
 权限规则：超级用户级别 0  普通用户级别 4
 文件等级：(由高到低)
 Level 0: root  R
 Level 1: root  RW
 Level 2: Ron   R
 Level 3: Ron   RW
 计算访问权限方式： 用户级别+文件等级
 Result=0:root访问Level 0 文件 R
 Result=1:root访问Level 1 文件 RW
 Result=2:root访问Level 2 文件 RW
 Result=3:root访问Level 3 文件 RW
 Result=4:Ron 访问Level 0 文件 R
 Result=5:Ron 访问Level 1 文件 R
 Result=6:Ron 访问Level 2 文件 R
 Result=7:Ron 访问Level 3 文件 RW
 
 总结出：
 Result=1，2，3，7  RW
 Result=0，4，5，6  R
 */
struct User * currentUser;/*当前用户*/
struct User * userList[10];/*用户列表,暂定最大10个用户*/
void initUser(void){
    
    //1.初始化普通用户 Ron
    struct User * user = (struct User *)malloc(sizeof(struct User));
    char name[120] = "Ron";
    memcpy(user->name, name, 120);
    user->name_len = 3;
    user->priority = 4;   //普通用户权限
    user->uid = 123;
    userList[0] = user;
    //2.初始化超级用户 root_Ron
    struct User * root_user = (struct User *)malloc(sizeof(struct User));
    char root_name[120] = "root_Ron";
    memcpy(root_user->name, root_name, 120);
    root_user->name_len = 3;
    root_user->priority = 0;   //超级用户权限
    root_user->uid = 110;
    
    userList[1] = root_user;
    //3.设置当前用户 current=Ron (即以普通用户登录)
    currentUser = user;
}
