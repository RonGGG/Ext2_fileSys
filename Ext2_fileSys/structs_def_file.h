//
//  structs_def_file.h
//  Ext2_fileSys
//
//  Created by  Ron on 2018/7/9.
//  Copyright © 2018年  Ron. All rights reserved.
//

#ifndef structs_def_file_h
#define structs_def_file_h

#define BLOCK_SIZE 1024  //数据块大小常量
#define INODE_TABLE_ADDR 1024*3  //inodeTable位置(硬盘)
#define IMAP_ADDR 1024*2
#define ZMAP_ADDR 1024

#define EXT2_VALID_FS    0x0001 //文件系统没有出错
#define EXT2_ERROR_FS   0x0002  //内核检测到错误
enum FileType{
    Ordinary_File = 0,   //普通文件
    Contents_File = 1,   //目录文件
    Block_File = 2,   //块设备
    ASCII_File = 3,   //字符设备
    Socket_File = 4,   //套接口
    Symbol_File = 5,   //符号连接
    FIFO_File = 6   //FIFO/管道文件
};
/*超级块数据结构（硬盘上）*/
#define SUPER_STRUCT_SIZE 1024
struct ext2_super_block
{
    uint32_t    s_inodes_count;    /* 文件系统中索引节点总数 */
    uint32_t    s_blocks_count;    /*文件系统中总块数 */
    uint32_t    s_r_blocks_count;      /* 为超级用户保留的块数 */
    uint32_t    s_free_blocks_count;    /*文件系统中空闲块总数 */
    uint32_t    s_free_inodes_count;    /*文件系统中空闲索引节点总数*/
    uint32_t    s_first_data_block;    /* 文件系统中第一个数据块 */
    uint32_t    s_log_block_size;     /* 用于计算逻辑块大小 */
//    int32_t    s_log_frag_size;     /* 用于计算片大小 */
//    uint32_t    s_blocks_per_group;    /* 每组中块数 */
//    uint32_t    s_frags_per_group;  /* 每组中片数 */
//    uint32_t    s_inodes_per_group;    /* 每组中索引节点数 */
//    uint32_t    s_mtime;   /*最后一次安装操作的时间 */
    uint32_t    s_wtime;  /*最后一次对该超级块进行写操作的时间 */
//    uint16_t    s_mnt_count;    /* 安装计数 */
//    int16_t    s_max_mnt_count;     /* 最大可安装计数 */
//    uint16_t    s_magic;  /* 用于确定文件系统版本的标志 */
    uint16_t    s_state;  /* 文件系统的状态*/
    uint16_t    s_errors;  /* 当检测到有错误时如何处理 */
//    uint16_t    s_minor_rev_level;     /* 次版本号 */
    uint32_t    s_lastcheck;    /* 最后一次检测文件系统状态的时间 */
    uint32_t    s_checkinterval; /* 两次对文件系统状态进行检测的间隔时间 */
//    uint32_t    s_rev_level;    /* 版本号 */
    uint16_t    s_def_resuid;     /* 保留块的默认用户标识号 */
//    uint16_t    s_def_resgid;  /* 保留块的默认用户组标识号*/
    uint32_t   s_first_ino;  /* 第一个非保留的索引节点 */
    uint16_t   s_inode_size;  /* 索引节点的大小 */
//    uint16_t   s_block_group_nr;  /* 该超级块的块组号 */
    uint32_t   s_feature_compat;  /* 兼容特点的位图*/
    uint32_t   s_feature_incompat;  /* 非兼容特点的位图 */
    uint32_t   s_feature_ro_compat;  /* 只读兼容特点的位图*/
    uint8_t    s_uuid[16];  /* 128位的文件系统标识号*/
    char    s_volume_name[16];  /* 卷名 */
    char    s_last_mounted[64];  /* 最后一个安装点的路径名 */
    uint32_t   s_algorithm_usage_bitmap;  /* 用于压缩*/
    uint8_t    s_prealloc_blocks;  /* 预分配的块数*/
    uint8_t    s_prealloc_dir_blocks;  /* 给目录预分配的块数 */
    uint16_t   s_padding1;  /* 填充 */
    /*后来加的变量*/
    uint8_t s_imap_blocks;  /*inode位图所占块数*/
    uint8_t s_imap_first_block;  /*第一个inode位图块号*/
    uint8_t s_zmap_blocks;  /*块位图所占块数*/
    uint8_t s_zmap_first_block;  /*第一个块位图块号*/
    
    
    uint32_t   s_reserved[204+8];  /* 用null填充块的末尾 */
};
/*superblock（内存上）*/
struct ext2_super_block_memory
{
    uint32_t    s_inodes_count;    /* 文件系统中索引节点总数 */
    uint32_t    s_blocks_count;    /*文件系统中总块数 */
    uint32_t    s_r_blocks_count;      /* 为超级用户保留的块数 */
    uint32_t    s_free_blocks_count;    /*文件系统中空闲块总数 */
    uint32_t    s_free_inodes_count;    /*文件系统中空闲索引节点总数*/
    uint32_t    s_first_data_block;    /* 文件系统中第一个数据块 */
    uint32_t    s_log_block_size;     /* 用于计算逻辑块大小 */
    uint32_t    s_wtime;  /*最后一次对该超级块进行写操作的时间 */
    uint16_t    s_state;  /* 文件系统的状态*/
    uint16_t    s_errors;  /* 当检测到有错误时如何处理 */
    uint32_t    s_lastcheck;    /* 最后一次检测文件系统状态的时间 */
    uint32_t    s_checkinterval; /* 两次对文件系统状态进行检测的间隔时间 */
    uint16_t    s_def_resuid;     /* 保留块的默认用户标识号 */
    uint32_t   s_first_ino;  /* 第一个非保留的索引节点 */
    uint16_t   s_inode_size;  /* 索引节点的大小 */
    uint32_t   s_feature_compat;  /* 兼容特点的位图*/
    uint32_t   s_feature_incompat;  /* 非兼容特点的位图 */
    uint32_t   s_feature_ro_compat;  /* 只读兼容特点的位图*/
    uint8_t    s_uuid[16];  /* 128位的文件系统标识号*/
    char    s_volume_name[16];  /* 卷名 */
    char    s_last_mounted[64];  /* 最后一个安装点的路径名 */
    uint32_t   s_algorithm_usage_bitmap;  /* 用于压缩*/
    uint8_t    s_prealloc_blocks;  /* 预分配的块数*/
    uint8_t    s_prealloc_dir_blocks;  /* 给目录预分配的块数 */
    uint16_t   s_padding1;  /* 填充 */
    /*后来加的变量*/
    uint8_t s_imap_blocks;  /*inode位图所占块数*/
    uint8_t s_imap_first_block;  /*第一个inode位图块号*/
    uint8_t s_zmap_blocks;  /*块位图所占块数*/
    uint8_t s_zmap_first_block;  /*第一个块位图块号*/
    
    uint32_t   s_reserved[211];  /* 用null填充块的末尾 */
    uint16_t dev_num;    /*设备号*/
    uint8_t access_count;    /*访问计数*/
    uint8_t mute_lock;    /*互斥锁*/
};
#define Ext2_N_BLOCKS 9
#define INODE_STRUCT_SIZE 128

/*inode数据结构：(硬盘上)*/
struct ext2_inode {
    uint16_t    i_mode; /* 文件类型和访问权限 高4位表示类型，低12位表示权限：0，1，2，3级别*/
    uint16_t    i_uid;    /* 文件拥有者标识号*/
    uint32_t    i_size; /* 以字节计的文件大小 */
    uint32_t    i_atime; /* 文件的最后一次访问时间 */
    uint32_t    i_ctime; /* 该节点最后被修改时间 */
    uint32_t    i_mtime; /* 文件内容的最后修改时间 */
    uint32_t    i_dtime; /* 文件删除时间 */
    uint16_t    i_gid;    /* 文件的用户组标志符 */
    uint16_t    i_links_count; /* 文件的硬链接计数 */
    uint32_t    i_blocks; /* 文件所占块数（每块以1024字节计）*/
    uint32_t    i_flags; /* 打开文件的方式 */
    uint32_t    i_block[Ext2_N_BLOCKS];  /* 指向数据块的指针数组 */
    /*填充部分*/
    uint32_t i_reserved[14];   /*因为差56B就128B了*/
};
/*inode数据结构：（内存中）*/
struct ext2_inode_memory {
    uint16_t    i_mode; /* 文件类型和访问权限 */
    uint16_t    i_uid;    /* 文件拥有者标识号*/
    
    uint32_t    i_size; /* 以字节计的文件大小 */
    uint32_t    i_atime; /* 文件的最后一次访问时间 */
    uint32_t    i_ctime; /* 该节点最后被修改时间 */
    uint32_t    i_mtime; /* 文件内容的最后修改时间 */
    uint32_t    i_dtime; /* 文件删除时间 */
    uint16_t    i_gid;    /* 文件的用户组标志符 */
    uint16_t    i_links_count; /* 文件的硬链接计数 */
    uint32_t    i_blocks; /* 文件所占块数（每块以1024字节计）*/
    uint32_t    i_flags; /* 打开文件的方式 */
    uint32_t    i_block[Ext2_N_BLOCKS];  /* 指向数据块的指针数组 */
    /*填充部分*/
    uint32_t i_reserved[9];   /*因为差56B就128B了*/
    /*以下数据在硬盘中没有，仅在内存中存在*/
    uint32_t i_number;   /*内存索引节点编号，作为内存索引节点标识符(定义为数组下标)*/
    uint32_t i_count;   /*记录当前有几个进程正在访问此i结点，每当有进程访问此i结点时，对i_count+1，退出-1*/
    uint16_t i_flag;   /*内存索引结点是否已上锁、是否有进程等待此i结点解锁、i结点是否被修改、是否有最近被访问等标志*/
    uint16_t i_dev;   /*设备号*/
    //因为使用数组，因此就不写前后指针了
    uint32_t i_forw;   /*Hash前向指针*/
    uint32_t i_back;   /*Hash后向指针*/
};
/*目录项数据结构：*/
#define DIR_ENTRY_SIZE 256
#define DIR_NAME_SIZE 247
struct ext2_dir_entry_2 {
    int32_t inode; // 文件入口的inode号，0表示该项未使用
    int16_t rec_len; // 目录项长度
    uint8_t name_len; // 文件名包含的字符数
    uint8_t file_type; // 文件类型
    char name[DIR_NAME_SIZE]; // 文件名
};
/*用户结构体*/
struct User{
    uint32_t uid;
    uint16_t priority;
    uint16_t name_len;
    char name[120];
    struct ext2_inode_memory * user_content;
};
/*
 fs_struct记录跟目录inode地址和当前目录inode地址
 */
struct fs_struct{
    int count;    /* 共享此结构的计数值 */
    uint16_t umask;  /* 文件掩码 */
    struct ext2_inode_memory * root, * pwd;  /* 根目录和当前目录inode指针 */
};
/*
 file系统打开文件结构体
 */
#define NR_FILE_TABLE 128
struct file{
    uint16_t f_mode;  /* 文件的打开模式 */
    uint16_t f_flags; /* 文件操作标志 */
    uint16_t f_count; /* 共享该结构体的计数值 */
    uint16_t f_pos;  /* 文件的当前读写位置 以字节记 */
    struct ext2_inode_memory * f_inode;  /* 指向文件对应的inode(内存中) */
};
/*
 files_struct 用户打开文件结构体
 */
#define NR_OPEN 10
struct files_struct{
    int count;    /* 共享该结构的计数值 */
//    fd_set close_on_exec;//没用
//    fd_set open_fds;//没用
    int used;/*永远指向最后一个已使用的描述符后,如果used之前的描述符被关闭，则务必设置NULL*/
    struct file * fd[NR_OPEN];//文件描述符表
};
/*
 任务结构体
 */
struct task_struct{
    pid_t pid;   /*进程标识符*/
    struct fs_struct * fs;   /*fs_struct*/
    struct files_struct * files;   /*files_struct*/
    struct User * owner;   /*标示该进程属于哪个用户*/
};
/*操作权限常量*/
#define READ_AND_WRITE 0
#define READ 1
#define DISS_OP 2
#endif /* structs_def_file_h */
