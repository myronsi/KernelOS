#ifndef FS_H
#define FS_H

#include "types.h"

typedef struct fs_node {
    string name;
    int is_directory;
    string content;
    struct fs_node** children;
    int num_children;
} fs_node_t;

extern fs_node_t* root;
extern fs_node_t* current_dir;

void init_fs();
void create_directory(fs_node_t* parent, string name);
void create_file(fs_node_t* parent, string name);
void list_directory(fs_node_t* dir);
fs_node_t* find_file(string path);
void write_file(fs_node_t* file, string new_content);
string read_file(fs_node_t* file);

#endif