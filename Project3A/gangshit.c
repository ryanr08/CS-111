//NAME: Anurag Sandireddy, Ryan Riahi EMAIL: anurag.sandireddy@gmail.com, ryanr08@gmail.com
//ID: 805114200, 105138860
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ext2_fs.h"
#include <math.h>
#include <time.h>

int fd;
int readSuperBlock();
void readDirectory(int inodesNum, struct ext2_inode inode);
struct tm* calculateTime(const time_t time);
struct ext2_super_block superBlock;
void readFreeBlocks(struct ext2_group_desc group, int blocksnum, int groupNum);
void readFreeInodes(struct ext2_group_desc group, int inodesNum, int groupsNum);
void readInode(struct ext2_group_desc, int inodeNum);
void readIndirect(int level, int inodeNum, struct ext2_inode inode, int blockID, int starting, char filetype);
void readDirectory_indirect(int inodeNum, unsigned int blockID);

void readGroup(int num_groups);
int main(int argc, char* argv[])
{
    if (argc > 2){
        fprintf(stderr, "Too many arguments provided\n");
        exit(1);
    }
    fd = open(argv[1], O_RDONLY);
    if (fd < 0){
        fprintf(stderr, "Error opening image\n");
        exit(1);
    }

    int groups = readSuperBlock();
    readGroup(groups);

    exit(0);
}


int readSuperBlock(){
    int charnum = pread(fd, &superBlock, 1024, 1024);
    if (charnum != 1024){
        fprintf(stderr, "Error in reading superblock\n");
        exit(1);
    }
    fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", 
    superBlock.s_blocks_count, superBlock.s_inodes_count,
    (EXT2_MIN_BLOCK_SIZE << superBlock.s_log_block_size), superBlock.s_inode_size,
    superBlock.s_blocks_per_group, superBlock.s_inodes_per_group,
    superBlock.s_first_ino);

    int groups = superBlock.s_blocks_count / superBlock.s_blocks_per_group;
    return groups;
}

void readGroup(int num_groups) {
    num_groups++;
    int i = 0;
    for (; i < num_groups; i++){
        struct ext2_group_desc group;
        int blocksnum;
        int inodesnum;
        int charnum = pread(fd, &group,
        sizeof(struct ext2_group_desc), 2048 + (i * sizeof(struct ext2_group_desc)));
        if (charnum < 0){
            fprintf(stderr, "Error reading from the image\n");
            exit(1);
        }
        if (num_groups == 1){
            fprintf(stdout, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", 
            0,superBlock.s_blocks_count,superBlock.s_inodes_count,
            group.bg_free_blocks_count, group.bg_free_inodes_count,
            group.bg_block_bitmap,group.bg_inode_bitmap,
            group.bg_inode_table);
            blocksnum = superBlock.s_blocks_count;
            inodesnum = superBlock.s_inodes_count;
        }
        else if (i == num_groups - 1){
            fprintf(stdout, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", 
            i,(superBlock.s_blocks_count % superBlock.s_blocks_per_group)
            ,(superBlock.s_inodes_count % superBlock.s_inodes_per_group),
            group.bg_free_blocks_count, group.bg_free_inodes_count,
            group.bg_block_bitmap,group.bg_inode_bitmap,
            group.bg_inode_table);
            blocksnum = (superBlock.s_blocks_count % superBlock.s_blocks_per_group);
            inodesnum = (superBlock.s_inodes_count % superBlock.s_inodes_per_group);
        }
        else {
            fprintf(stdout, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", 
            i,(superBlock.s_blocks_per_group),(superBlock.s_inodes_per_group),
            group.bg_free_blocks_count, group.bg_free_inodes_count,
            group.bg_block_bitmap,group.bg_inode_bitmap,
            group.bg_inode_table);
            blocksnum = superBlock.s_blocks_per_group;
            inodesnum = (superBlock.s_inodes_per_group);
        }
        readFreeBlocks(group, blocksnum, i);
        readFreeInodes(group, inodesnum, i);
    }
}

void readFreeBlocks(struct ext2_group_desc group, int blocksnum, int groupNum){
    char buffer[1024];
    int charnum = pread(fd, &buffer, 1024,
    group.bg_block_bitmap*1024);
    if (charnum != 1024){
        fprintf(stderr, "Error reading from the image\n");
        exit(1);
    }
    int numBytes = (blocksnum / 8);
    int i = 0;
    for (; i < numBytes; i++){
        char ch = buffer[i];
        int x = 0;
        while (x < 8){
            if (!(ch & (1 << x))){
                fprintf(stdout, "BFREE,%d\n",
                (superBlock.s_first_data_block) + (i * 8 + x) 
                + (groupNum * superBlock.s_blocks_per_group));
            }
            x++;
        }
    }
    int numBits = (blocksnum % 8);
    if (numBits != 0){
        char ch = buffer[i];
        int x = 0;
        while (x < numBits){
            if (!(ch & (1 << x))){
                fprintf(stdout, "BFREE,%d\n",
                (superBlock.s_first_data_block) + (i * 8 + x) 
                + (groupNum * superBlock.s_blocks_per_group));
            }
        }
    }
}

void readFreeInodes(struct ext2_group_desc group, int inodesNum, int groupsNum){
    char buffer[1024];
    int charnum = pread(fd, &buffer, 1024, 
    group.bg_inode_bitmap*1024);
    if (charnum != 1024){
        fprintf(stderr, "Error in reading from the image\n");
        exit(1);
    }
    int numBytes = (inodesNum / 8);
    int i = 0;
    for (; i < numBytes; i++){
        char ch = buffer[i];
        int x = 0;
        while (x < 8){
            if (!(ch & (1 << x))){
                fprintf(stdout, "IFREE,%d\n",
                (superBlock.s_first_data_block) + (i * 8 + x) 
                + (groupsNum * superBlock.s_inodes_per_group));
            }
            else {
                readInode(group, ((superBlock.s_first_data_block) + (i*8+x)
                + (groupsNum * superBlock.s_inodes_per_group)));
            }
            x++;
        }
    }
    int numBits = (inodesNum % 8);
    if (numBits != 0){
        char ch = buffer[i];
        int x = 0;
        while (x < numBits){
            if (!(ch & (1 << x))){
                fprintf(stdout, "IFREE,%d\n",
                (superBlock.s_first_data_block) + (i * 8 + x) 
                + (groupsNum * superBlock.s_inodes_per_group));
            }
        }
    }
}

void readInode(struct ext2_group_desc group, int inodeNum){
    struct ext2_inode inode;
    char filetype;
    int charnum = pread(fd, &inode, 128, 
    (group.bg_inode_table*1024) + (inodeNum * superBlock.s_inode_size));
    if (charnum < 128){
        fprintf(stderr, "Error in reading from the image\n");
        exit(1);
    }
    if (inode.i_mode == 0){
        return;
    }
    else if (inode.i_links_count == 0){
        return;
    }
    unsigned int m_mode = ((inode.i_mode >> 12) & (0x000F));
    if (!(m_mode ^ 0x000A)){
        filetype = 's';
    }
    else if (!(m_mode ^ 0x0008)){
        filetype = 'f';
    }
    else if (!(m_mode ^ 0x0004)){
        filetype = 'd';
        readDirectory(inodeNum, inode);
    }
    else {
        filetype = '?';
    }
    unsigned int octalMode = ((inode.i_mode) & (0x0FFF));

    fprintf(stdout, "INODE,%d,%c,%o,%d,%d,%d,",inodeNum + 1, filetype,
    octalMode, inode.i_uid, inode.i_gid,inode.i_links_count);

    const time_t ctime = inode.i_ctime;
    const time_t atime = inode.i_atime;
    const time_t mtime = inode.i_mtime;

    struct tm* iNodeChange = calculateTime(ctime);
    fprintf(stdout, "%02d/%02d/%02d %d:%d:%d,", 1+iNodeChange->tm_mon, 
    iNodeChange -> tm_mday, iNodeChange->tm_year % 100, iNodeChange->tm_hour,
    iNodeChange->tm_min, iNodeChange->tm_sec);

    struct tm* modificationTime = calculateTime(mtime);
    fprintf(stdout, "%02d/%02d/%02d %d:%d:%d,", 1+modificationTime->tm_mon, 
    modificationTime -> tm_mday, modificationTime->tm_year % 100, modificationTime->tm_hour,
    modificationTime->tm_min, modificationTime->tm_sec);

    struct tm* accessTime = calculateTime(atime);
    fprintf(stdout, "%02d/%02d/%02d %d:%d:%d,", 1+accessTime->tm_mon, 
    accessTime -> tm_mday, accessTime->tm_year % 100, accessTime->tm_hour,
    accessTime->tm_min, accessTime->tm_sec);
    fprintf(stdout, "%d,%d", inode.i_size, inode.i_blocks);


    if ((filetype == 'f') || (filetype == 'd')){
        fprintf(stdout, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        inode.i_block[0],inode.i_block[1],inode.i_block[2],inode.i_block[3],
        inode.i_block[4],inode.i_block[5],inode.i_block[6],inode.i_block[7],
        inode.i_block[8],inode.i_block[9],inode.i_block[10],inode.i_block[11],
        inode.i_block[12],inode.i_block[13],inode.i_block[14]);
        readIndirect(1, inodeNum, inode,inode.i_block[12], 0, filetype);
        readIndirect(2, inodeNum, inode,inode.i_block[13], 1, filetype);
        readIndirect(3, inodeNum, inode,inode.i_block[14], 2, filetype);
    }
    else if (filetype == 's'){
        if (inode.i_size >= 60){
        fprintf(stdout, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        inode.i_block[0],inode.i_block[1],inode.i_block[2],inode.i_block[3],
        inode.i_block[4],inode.i_block[5],inode.i_block[6],inode.i_block[7],
        inode.i_block[8],inode.i_block[9],inode.i_block[10],inode.i_block[11],
        inode.i_block[12],inode.i_block[13],inode.i_block[14]);
        }
        else {
            fprintf(stdout, "\n");
        }
    }
}

void readDirectory(int inodeNum, struct ext2_inode inode){
    struct ext2_dir_entry directory;
    int i = 0;
    int offset = 0;
    for (; i < 12; i++){
        if (inode.i_block[i] == 0){
            break;
        }
        offset = 0;
        while (offset < 1024){
            int charnum = pread(fd, &directory, sizeof(directory),
            (inode.i_block[i]*1024 + offset));
            if (charnum < (int)sizeof(directory)){
                fprintf(stderr, "Error in reading from image\n");
                exit(1);
            }
            if (directory.inode != 0){
                fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n", 
                inodeNum + 1, offset, directory.inode,directory.rec_len,
                directory.name_len, directory.name);
                offset += directory.rec_len;
            }
            else {
                break;
            }
        }
    }
}

void readIndirect(int level, int inodeNum, struct ext2_inode inode, int blockID, int starting, char filetype){
    int numBlocks = 256;
    if (level != 1){
        unsigned int *buffer;
        buffer = malloc(sizeof(unsigned int));
        int w = 0;
        for (; w < numBlocks; w++){
            int charnum = pread(fd, buffer, 4, (blockID * 1024) + (w * 4));
            if (charnum < 4){
                fprintf(stderr, "Error in reading from image\n");
                exit(1);
            }
            if (*buffer != 0){
                readIndirect(level - 1, inodeNum, inode, *buffer, starting, filetype);
                fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inodeNum + 1,
                level, (int)(12 + pow(256, starting)*(w + 1) + (starting - 1)*256), blockID, *buffer);
            }
        }
    }
    else {
        unsigned int *buffer;
        buffer = malloc(sizeof(unsigned int));
        int i = 0;
        for (; i < numBlocks; i++){
            int charnum = pread(fd, buffer, 4, (blockID * 1024) + (i * 4));
            if (charnum < 4){
                fprintf(stderr, "Error in reading from image\n");
                exit(1);
            }
            if (*buffer != 0){
                int power;
                if (starting == 0){
                    power = 0;
                }
                else {
                    power = pow(256, starting) + (starting - 1)*256;
                }
                fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n",
                inodeNum + 1, level, (int)(12 + power + i), blockID, *buffer);
                if (filetype == 'd')
                    readDirectory_indirect(inodeNum, *buffer);
            }
        }
    }
}

void readDirectory_indirect(int inodeNum, unsigned int blockID)
{
    struct ext2_dir_entry directory;
    int offset = 0;
    while (offset < 1024){
        int charnum = pread(fd, &directory, sizeof(directory),
        (blockID*1024 + offset));
        if (charnum < (int)sizeof(directory)){
            fprintf(stderr, "Error in reading from image\n");
            exit(1);
        }
        if (directory.inode != 0){
            fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n", 
            inodeNum + 1, offset, directory.inode,directory.rec_len,
            directory.name_len, directory.name);
            offset += directory.rec_len;
        }
        else {
            break;
        }
    }
}

struct tm * calculateTime(const time_t time){
    return gmtime(&time);
}
