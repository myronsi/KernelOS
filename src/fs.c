#include "../include/fs.h"
#include "../include/util.h"
#include "../include/screen.h"

fs_node_t* root;
fs_node_t* current_dir;

void init_fs() {
    root = malloc(sizeof(fs_node_t));
    root->name = "/";
    root->is_directory = 1;
    root->children = malloc(0);
    root->num_children = 0;
    current_dir = root;
}

void create_directory(fs_node_t* parent, string name) {
    fs_node_t* new_dir = malloc(sizeof(fs_node_t));
    new_dir->name = name;
    new_dir->is_directory = 1;
    new_dir->children = malloc(0);
    new_dir->num_children = 0;
    parent->children = realloc(parent->children, (parent->num_children + 1) * sizeof(fs_node_t*));
    parent->children[parent->num_children] = new_dir;
    parent->num_children++;
}

void create_file(fs_node_t* parent, string name) {
    fs_node_t* new_file = malloc(sizeof(fs_node_t));
    new_file->name = name;
    new_file->is_directory = 0;
    new_file->content = malloc(1);
    new_file->content[0] = '\0';
    parent->children = realloc(parent->children, (parent->num_children + 1) * sizeof(fs_node_t*));
    parent->children[parent->num_children] = new_file;
    parent->num_children++;
}

void list_directory(fs_node_t* dir) {
    for (int i = 0; i < dir->num_children; i++) {
        print(dir->children[i]->name);
        if (dir->children[i]->is_directory) {
            print("/");
        }
        print(" ");
    }
    print("\n");
}