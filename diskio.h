//
// Created by ironmanvim on 24/6/19.
//

#ifndef FILE_SYSTEM_DISKIO_H
#define FILE_SYSTEM_DISKIO_H

#define HARD_DISK_SIZE (100 * 1024 * 1024)
#define BLOCK_SIZE (16 * 1024)
#define NO_OF_BLOCKS ( HARD_DISK_SIZE / BLOCK_SIZE )


struct Metadata {
    unsigned int magic_number;
    unsigned int no_of_files;
    unsigned char block_vector[NO_OF_BLOCKS];
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

extern struct Metadata *get_metadata();

extern void put_metadata(struct Metadata *metadata);

#endif //FILE_SYSTEM_DISKIO_H
