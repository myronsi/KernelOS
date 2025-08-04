#ifndef FS_H
#define FS_H

#include "types.h"

#define FS_SECTOR 100
#define MAX_NAME_LEN 255
#define IDE_PRIMARY_ERROR 0x1F1

typedef struct fs_node {
    char* name;
    uint8 is_directory;
    char* content;
    struct fs_node** children;
    uint32 num_children;
} fs_node_t;

extern fs_node_t* root;
extern fs_node_t* current_dir;

// fs.c
void init_fs();
void create_directory(fs_node_t* parent, char* name);
void create_file(fs_node_t* parent, char* name);
void list_directory(fs_node_t* dir);
fs_node_t* find_file(char* path);
void write_file(fs_node_t* file, char* new_content);
char* read_file(fs_node_t* file);

// fs_serialization.c
uint32 serialize_node(fs_node_t* node, uint8* buffer, uint32 offset, uint32 buffer_size);
uint32 deserialize_node(fs_node_t** node, uint8* buffer, uint32 offset, uint32 buffer_size);
void save_fs_to_disk();
void load_fs_from_disk(uint8* initial_buffer);

// ide.c
void read_sector(uint32 sector, uint8* buffer);
void write_sector(uint32 sector, uint8* buffer);
int ide_init();

#endif