//
// Created by ironmanvim on 24/6/19.
//

#ifndef FILE_SYSTEM_DISKIO_H
#define FILE_SYSTEM_DISKIO_H

struct Metadata {
    int magic_number;
    int no_of_files;
    struct FileTable {
        char filename[20];
        unsigned int size;
        unsigned int start_block;
        unsigned int no_of_blocks;
    } fileTable[30];
};

extern void init(char *hard_disk_name);

extern void read_block(void *buffer, unsigned int block_no);

extern void write_block(void *buffer, unsigned int block_no);

extern void format();

#endif //FILE_SYSTEM_DISKIO_H
