//
// Created by ironmanvim on 25/6/19.
//

#include "fileio.h"
#include "diskio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_free_block(struct Metadata *metadata) {
    for (int i = 0; i < NO_OF_BLOCKS; i++) {
        if (metadata->block_vector[i] == 0) {
            return i;
        }
    }
    return -1;
}

unsigned int get_file_size(FILE *file) {
    int status = fseek(file, 0, SEEK_END);
    if (status != 0) {
        printf("ERROR IN FINDING FILE SIZE.\n");
        return 0;
    }
    return ftell(file);
}

void copy_to_disk(char *filename, char *alias_name) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("ERROR WHILE FILE OPENING.\n");
    }
    unsigned int file_size = get_file_size(file);
    int no_of_blocks = file_size / BLOCK_SIZE + 1;
    struct Metadata *metadata = get_metadata();
    metadata->no_of_files++;
    int first_free_block = get_free_block(metadata);
    if (first_free_block == -1) {
        printf("NO FREE BLOCK.\n");
        return;
    }
    metadata->block_vector[first_free_block] = 1;
    int *placable_blocks = malloc(sizeof(int) * no_of_blocks);
    for (int i = 0; i < no_of_blocks; i++) {
        placable_blocks[i] = get_free_block(metadata);
        if (placable_blocks[i] == -1) {
            printf("NO FREE BLOCK.\n");
            return;
        }
        metadata->block_vector[placable_blocks[i]] = 1;
    }
    void *buffer = malloc(BLOCK_SIZE);
    memset(buffer, 0, BLOCK_SIZE);
    memcpy(buffer, placable_blocks, no_of_blocks);
    write_block(buffer, first_free_block);
    int temp_file_size = file_size;
    rewind(file);
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
    int file_index = metadata->no_of_files - 1;
    strcpy(metadata->fileTable[file_index].filename, alias_name);
    metadata->fileTable[file_index].no_of_blocks = no_of_blocks;
    metadata->fileTable[file_index].size = file_size;
    metadata->fileTable[file_index].start_block = first_free_block;
    put_metadata(metadata);
}

void copy_from_disk(char *filenaame, char *alias_name) {

}

