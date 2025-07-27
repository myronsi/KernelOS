#include "../include/fs.h"
#include "../include/util.h"
#include "../include/screen.h"

void read_sector(uint32 sector, uint8* buffer) {
    // TODO: Реализуйте чтение сектора с диска
    print("read_sector not implemented\n");
}

void write_sector(uint32 sector, uint8* buffer) {
    // TODO: Реализуйте запись сектора на диск
    print("write_sector not implemented\n");
}

fs_node_t* root;
fs_node_t* current_dir;

#define FS_SECTOR 100
#define MAX_NAME_LEN 32

uint32 serialize_node(fs_node_t* node, uint8* buffer, uint32 offset) {
    buffer[offset++] = node->is_directory;

    uint32 name_len = strlen(node->name);
    buffer[offset++] = name_len;
    memory_copy(node->name, buffer + offset, name_len);
    offset += name_len;

    if (!node->is_directory) {
        uint32 content_len = strlen(node->content);
        *(uint32*)(buffer + offset) = content_len;
        offset += 4;
        memory_copy(node->content, buffer + offset, content_len);
        offset += content_len;
    } else {
        *(uint32*)(buffer + offset) = node->num_children;
        offset += 4;
        for (int i = 0; i < node->num_children; i++) {
            offset = serialize_node(node->children[i], buffer, offset);
        }
    }
    return offset;
}

uint32 deserialize_node(fs_node_t** node, uint8* buffer, uint32 offset) {
    *node = malloc(sizeof(fs_node_t));
    (*node)->is_directory = buffer[offset++];

    uint32 name_len = buffer[offset++];
    (*node)->name = malloc(name_len + 1);
    memory_copy(buffer + offset, (*node)->name, name_len);
    (*node)->name[name_len] = '\0';
    offset += name_len;

    if (!(*node)->is_directory) {
        uint32 content_len = *(uint32*)(buffer + offset);
        offset += 4;
        (*node)->content = malloc(content_len + 1);
        memory_copy(buffer + offset, (*node)->content, content_len);
        (*node)->content[content_len] = '\0';
        offset += content_len;
        (*node)->children = NULL;
        (*node)->num_children = 0;
    } else {
        (*node)->num_children = *(uint32*)(buffer + offset);
        offset += 4;
        (*node)->children = malloc((*node)->num_children * sizeof(fs_node_t*));
        for (int i = 0; i < (*node)->num_children; i++) {
            offset = deserialize_node(&(*node)->children[i], buffer, offset);
        }
    }
    return offset;
}

void save_fs_to_disk() {
    uint8* buffer = malloc(512 * 10);
    uint32 offset = 0;

    *(uint32*)buffer = root->num_children;
    offset += 4;

    offset = serialize_node(root, buffer, offset);

    for (int i = 0; i < (offset + 511) / 512; i++) {
        write_sector(FS_SECTOR + i, buffer + i * 512);
    }
    free(buffer);
}

void load_fs_from_disk() {
    uint8* buffer = malloc(512 * 10);
    read_sector(FS_SECTOR, buffer);

    uint32 offset = 0;
    uint32 num_nodes = *(uint32*)buffer;
    offset += 4;

    root = malloc(sizeof(fs_node_t));
    root->name = "/";
    root->is_directory = 1;
    root->num_children = num_nodes;
    root->children = malloc(num_nodes * sizeof(fs_node_t*));

    for (int i = 0; i < num_nodes; i++) {
        offset = deserialize_node(&root->children[i], buffer, offset);
    }
    free(buffer);
}

void init_fs() {
    uint8 buffer[4];
    read_sector(FS_SECTOR, buffer);
    if (*(uint32*)buffer == 0) {
        root = malloc(sizeof(fs_node_t));
        root->name = "/";
        root->is_directory = 1;
        root->children = malloc(0);
        root->num_children = 0;
    } else {
        load_fs_from_disk();
    }
    current_dir = root;
}

void create_directory(fs_node_t* parent, string name) {
    fs_node_t* new_dir = malloc(sizeof(fs_node_t));
    new_dir->name = malloc(strlen(name) + 1);
    memory_copy(name, new_dir->name, strlen(name) + 1);
    new_dir->is_directory = 1;
    new_dir->children = malloc(0);
    new_dir->num_children = 0;
    parent->children = realloc(parent->children, (parent->num_children + 1) * sizeof(fs_node_t*));
    parent->children[parent->num_children] = new_dir;
    parent->num_children++;
    save_fs_to_disk();
}

void create_file(fs_node_t* parent, string name) {
    fs_node_t* new_file = malloc(sizeof(fs_node_t));
    new_file->name = malloc(strlen(name) + 1);
    memory_copy(name, new_file->name, strlen(name) + 1);
    new_file->is_directory = 0;
    new_file->content = malloc(1);
    new_file->content[0] = '\0';
    parent->children = realloc(parent->children, (parent->num_children + 1) * sizeof(fs_node_t*));
    parent->children[parent->num_children] = new_file;
    parent->num_children++;
    save_fs_to_disk();
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

fs_node_t* find_file(string path) {
    if (path[0] != '/') return NULL;
    fs_node_t* current = root;
    int i = 1;
    string name = malloc(256);
    int j = 0;
    while (path[i]) {
        j = 0;
        while (path[i] && path[i] != '/' && j < 255) {
            name[j++] = path[i++];
        }
        name[j] = '\0';
        if (path[i] == '/') i++;
        int found = 0;
        for (int k = 0; k < current->num_children; k++) {
            if (strEql(current->children[k]->name, name)) {
                current = current->children[k];
                found = 1;
                break;
            }
        }
        if (!found) {
            free(name);
            return NULL;
        }
    }
    free(name);
    return current;
}

void write_file(fs_node_t* file, string new_content) {
    if (file->is_directory) return;
    free(file->content);
    file->content = malloc(strlen(new_content) + 1);
    memory_copy(new_content, file->content, strlen(new_content) + 1);
    save_fs_to_disk();
}

string read_file(fs_node_t* file) {
    if (file->is_directory) return "";
    return file->content;
}