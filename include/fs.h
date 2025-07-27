#ifndef FS_H
#define FS_H

#include "types.h"

typedef struct fs_node {
    string name;
    int is_directory;
    string content; // для файлов
    struct fs_node** children; // для директорий
    int num_children;
} fs_node_t;

extern fs_node_t* root;
extern fs_node_t* current_dir;

void init_fs();
void create_directory(fs_node_t* parent, string name);
void create_file(fs_node_t* parent, string name);
void list_directory(fs_node_t* dir);

#endif