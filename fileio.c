//
// Created by ironmanvim on 25/6/19.
//

#include "fileio.h"
#include "diskio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * SEARCH FILE IN METADATA AND GET FILE INDEX
 */
static int search_file(struct Metadata *metadata, char *filename) {
    for (int i = 0; i < metadata->no_of_files; i++) {
        if (!strcmp(metadata->fileTable[i].filename, filename)) {
            return i; // return the file index in metadata
        }
    }
    return -1; // return -1 if no such file exists
}

/*
 * GET THE FIRST FREE BLOCK AND MARKS IT AS USED
 */
static int get_free_block(struct Metadata *metadata) {
    for (int i = 0; i < NO_OF_BLOCKS; i++) {
        if (metadata->block_vector[i] == 0) {
            metadata->block_vector[i] = 1; // mark it as used
            return i; // returns the free block index
        }
    }
    return -1; // returns -1 if no block is free
}

/*
 * UPDATE METADATA OF THE FILE
 */
static void update_metadata(struct Metadata *metadata, char *alias_name, unsigned int file_size, int no_of_blocks,
                     int first_free_block) {
    metadata->no_of_files++;
    int file_index = metadata->no_of_files - 1;
    strcpy(metadata->fileTable[file_index].filename, alias_name);
    metadata->fileTable[file_index].no_of_blocks = no_of_blocks;
    metadata->fileTable[file_index].size = file_size;
    metadata->fileTable[file_index].start_block = first_free_block;
    put_metadata(metadata);
}

/*
 * An Update for write_block
 * IT WRITE FOR A SPECIFIC SIZE
 */
static void write_block_with_size(void *item, unsigned int block_no, unsigned int size_required) {
    void *buffer = malloc(BLOCK_SIZE);
    memset(buffer, 0, BLOCK_SIZE);
    memcpy(buffer, item, size_required);
    write_block(buffer, block_no);
    free(buffer);
}

/*
 * An Update for read_block
 * IT READ FOR A SPECIFIC SIZE
 */
static void read_block_with_size(void *item, unsigned int block_no, unsigned int size_required) {
    void *buffer = malloc(BLOCK_SIZE);
    read_block(buffer, block_no);
    memcpy(item, buffer, size_required);
    free(buffer);
}

/*
 * WRITE FILE TO THE PLACABLE BLOCKS
 */
static void write_file_to_blocks(FILE *file, int file_size, int *placable_blocks) {
    rewind(file);
    int temp_file_size = file_size;
    void *buffer = malloc(BLOCK_SIZE);
    int i = 0;
    while (temp_file_size > BLOCK_SIZE) {
        int status = fread(buffer, BLOCK_SIZE, 1, file);
        if (status != 1) {
            printf("FREAD FAILED IN COPY TO DISK FUNCTION. (1)\n");
            return;
        }
        write_block(buffer, placable_blocks[i]);
        temp_file_size -= BLOCK_SIZE;
        i++;
    }
    memset(buffer, 0, BLOCK_SIZE);
    int status = fread(buffer, temp_file_size, 1, file);
    if (status != 1) {
        printf("FREAD FAILED IN COPY TO DISK FUNCTION. (2)\n");
        return;
    }
    write_block(buffer, placable_blocks[i]);
    free(buffer);
}

/*
 * GET FILE FROM PLACABLE BLOCKS
 */
void get_file_from_blocks(FILE *file, int file_size, int *placable_blocks) {
    rewind(file);
    void *buffer = malloc(BLOCK_SIZE);
    int temp_file_size = file_size;
    int i = 0;
    while (temp_file_size > BLOCK_SIZE) {
        read_block(buffer, placable_blocks[i]);
        int status = fwrite(buffer, BLOCK_SIZE, 1, file);
        if (status != 1) {
            printf("WRITE BLOCK UNSUCCESSFUL.\n");
            return;
        }
        temp_file_size -= BLOCK_SIZE;
        i++;
    }
    read_block(buffer, placable_blocks[i]);
    int status = fwrite(buffer, temp_file_size, 1, file);
    if (status != 1) {
        printf("WRITE BLOCK UNSUCCESSFUL.\n");
        return;
    }
    free(buffer);
}

/*
 * GET BLOCKS TO PLACE A FILE
 */
int *get_placable_blocks(struct Metadata *metadata, int no_of_blocks) {
    int *placable_blocks = malloc(sizeof(int) * no_of_blocks);
    for (int i = 0; i < no_of_blocks; i++) {
        placable_blocks[i] = get_free_block(metadata); // get free block
        if (placable_blocks[i] == -1) {
            return NULL; // return null if no free block available
        }
    }
    return placable_blocks; // return placable blocks
}

/*
 * GET FILE SIZE
 */
unsigned int get_file_size(FILE *file) {
    int status = fseek(file, 0, SEEK_END);
    if (status != 0) {
        printf("ERROR IN FINDING FILE SIZE.\n");
        return 0;
    }
    return ftell(file);
}

/*
 * COPY THE FILE TO MY HARD DISK
 */
int copy_to_disk(char *filename, char *alias_name) {
    FILE *file = fopen(filename, "rb"); // open file in read in binary mode
    if (file == NULL) {
        printf("ERROR WHILE FILE OPENING.\n");
        return 0;
    }

    unsigned int file_size = get_file_size(file); // get file size
    int no_of_blocks = file_size / BLOCK_SIZE + 1; // get no_of_blocks that a file can be placed
    struct Metadata *metadata = get_metadata(); // get metadata

    int first_free_block = get_free_block(metadata); // get the first free block to place the free blocks array
    if (first_free_block == -1) {
        printf("NO FREE BLOCK.\n");
        return 0;
    }

    int *placable_blocks = get_placable_blocks(metadata, no_of_blocks); // get the placable blocks
    if (placable_blocks == NULL) {
        printf("NO FREE BLOCK.\n");
        return 0;
    }
    write_block_with_size(placable_blocks, first_free_block,
                          sizeof(int) * no_of_blocks); // put placable blocks in the first block
    write_file_to_blocks(file, file_size, placable_blocks); // put file in the placable blocks

    update_metadata(metadata, alias_name, file_size, no_of_blocks, first_free_block); // update metadata
    return 1;
}

/*
 * COPY A FILE FROM MY HARD DISK
 */
int copy_from_disk(char *filename, char *alias_name) {
    struct Metadata *metadata = get_metadata(); // get metadata
    int file_index = search_file(metadata, filename); // search for file
    if (file_index == -1) {
        printf("FILE DOESN'T EXISTS.\n");
        return 0;
    }

    // metadata extraction
    int first_block = metadata->fileTable[file_index].start_block;
    int no_of_blocks = metadata->fileTable[file_index].no_of_blocks;
    unsigned int file_size = metadata->fileTable[file_index].size;

    int *placable_blocks = malloc(sizeof(int) * no_of_blocks);
    read_block_with_size(placable_blocks, first_block,
                         sizeof(int) * no_of_blocks); // get_placable blocks which are put in the first block
    FILE *file = fopen(alias_name, "wb"); // open a file in write in binary mode
    if (file == NULL) {
        printf("CANNOT OPEN FILE TO WRITE.\n");
        return 0;
    }

    get_file_from_blocks(file, file_size, placable_blocks); // write placable blocks to file
    return 1;
}

