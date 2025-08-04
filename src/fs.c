#include "../include/fs.h"
#include "../include/util.h"
#include "../include/screen.h"
#include "../include/system.h"

fs_node_t* root;
fs_node_t* current_dir;

void init_fs() {
    print("Initializing FS, heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
#if !DISABLE_IDE
    asm("cli");
    if (ide_init() < 0) {
        print("IDE initialization failed, creating new FS\n");
        root = malloc(sizeof(fs_node_t));
        if (root) {
            root->name = "/";
            root->is_directory = 1;
            root->children = malloc(0);
            root->num_children = 0;
        }
    } else {
        print("IDE initialized, checking FS\n");
        uint8 buffer[4096]; // Увеличиваем буфер
        print("Before read_sector\n");
        read_sector(FS_SECTOR, buffer);
        print("After read_sector\n");
        uint8 error = inportb(IDE_PRIMARY_ERROR);
        print("IDE error register: ");
        print(int_to_string(error));
        print("\n");
        print("Buffer [0-15]: ");
        for (int i = 0; i < 16; i++) {
            print(int_to_string(buffer[i]));
            print(" ");
        }
        print("\n");
        if (error || *(uint32*)buffer == 0) {
            print("Creating new FS\n");
            root = malloc(sizeof(fs_node_t));
            if (root) {
                root->name = "/";
                root->is_directory = 1;
                root->children = malloc(0);
                root->num_children = 0;
            }
            save_fs_to_disk();
        } else {
            print("Loading existing FS\n");
            load_fs_from_disk(buffer);
        }
    }
    asm("sti");
#else
    print("IDE initialization skipped, creating new FS\n");
    root = malloc(sizeof(fs_node_t));
    if (root) {
        root->name = "/";
        root->is_directory = 1;
        root->children = malloc(0);
        root->num_children = 0;
    }
#endif
    current_dir = root;
    print("FS initialization complete, heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
}

void create_directory(fs_node_t* parent, char* name) {
    if (!parent || !parent->is_directory) {
        print("Invalid parent directory\n");
        return;
    }
    if (strlen(name) == 0 || strEql(name, "/")) {
        print("Invalid directory name\n");
        return;
    }
    uint32 name_len = strlen(name);
    if (name_len > MAX_NAME_LEN) {
        print("Directory name too long: ");
        print(int_to_string(name_len));
        print("\n");
        return;
    }
    print("Creating directory: ");
    print(name);
    print(", parent=");
    print(parent->name);
    print(", heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
    fs_node_t* new_dir = malloc(sizeof(fs_node_t));
    if (!new_dir) {
        print("Memory allocation failed for new_dir\n");
        return;
    }
    new_dir->name = malloc(name_len + 1);
    if (!new_dir->name) {
        print("Memory allocation failed for new_dir name\n");
        free(new_dir);
        return;
    }
    memory_copy(name, new_dir->name, name_len);
    new_dir->name[name_len] = '\0';
    print("Copied name: ");
    print(new_dir->name);
    print("\n");
    new_dir->is_directory = 1;
    new_dir->children = malloc(0);
    if (!new_dir->children) {
        print("Memory allocation failed for new_dir children\n");
        free(new_dir->name);
        free(new_dir);
        return;
    }
    new_dir->num_children = 0;
    void* old_children = parent->children;
    parent->children = realloc(parent->children, (parent->num_children + 1) * sizeof(fs_node_t*));
    if (!parent->children) {
        print("Memory allocation failed for parent children\n");
        parent->children = old_children;
        free(new_dir->name);
        free(new_dir->children);
        free(new_dir);
        return;
    }
    parent->children[parent->num_children] = new_dir;
    parent->num_children++;
    save_fs_to_disk();
    print("Directory created, heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
}

void create_file(fs_node_t* parent, char* name) {
    if (!parent || !parent->is_directory) {
        print("Invalid parent directory\n");
        return;
    }
    if (strlen(name) == 0 || strEql(name, "/")) {
        print("Invalid file name\n");
        return;
    }
    uint32 name_len = strlen(name);
    if (name_len > MAX_NAME_LEN) {
        print("File name too long: ");
        print(int_to_string(name_len));
        print("\n");
        return;
    }
    print("Creating file: ");
    print(name);
    print(", parent=");
    print(parent->name);
    print(", heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
    fs_node_t* new_file = malloc(sizeof(fs_node_t));
    if (!new_file) {
        print("Memory allocation failed for new_file\n");
        return;
    }
    new_file->name = malloc(name_len + 1);
    if (!new_file->name) {
        print("Memory allocation failed for new_file name\n");
        free(new_file);
        return;
    }
    memory_copy(name, new_file->name, name_len);
    new_file->name[name_len] = '\0';
    print("Copied name: ");
    print(new_file->name);
    print("\n");
    new_file->is_directory = 0;
    new_file->content = malloc(1);
    if (!new_file->content) {
        print("Memory allocation failed for new_file content\n");
        free(new_file->name);
        free(new_file);
        return;
    }
    new_file->content[0] = '\0';
    void* old_children = parent->children;
    parent->children = realloc(parent->children, (parent->num_children + 1) * sizeof(fs_node_t*));
    if (!parent->children) {
        print("Memory allocation failed for parent children\n");
        parent->children = old_children;
        free(new_file->name);
        free(new_file->content);
        free(new_file);
        return;
    }
    parent->children[parent->num_children] = new_file;
    parent->num_children++;
    save_fs_to_disk();
    print("File created, heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
}

void list_directory(fs_node_t* dir) {
    if (!dir || !dir->is_directory) {
        print("Invalid directory\n");
        return;
    }
    print("Listing directory: ");
    print(dir->name);
    print(", num_children=");
    print(int_to_string(dir->num_children));
    print("\n");
    uint32 max_children = dir->num_children < 1000 ? dir->num_children : 1000;
    for (uint32 i = 0; i < max_children; i++) {
        if (dir->children[i] == NULL || dir->children[i]->name == NULL || dir->children[i]->name[0] == '\0' || strEql(dir->children[i]->name, "/")) {
            print("Skipping invalid child ");
            print(int_to_string(i));
            print("\n");
            continue;
        }
        print("Child ");
        print(int_to_string(i));
        print(": ");
        print(dir->children[i]->name);
        if (dir->children[i]->is_directory) {
            print("/");
        }
        print("\n");
    }
    if (dir->num_children > max_children) {
        print("Warning: num_children too large (");
        print(int_to_string(dir->num_children));
        print("), limited to ");
        print(int_to_string(max_children));
        print("\n");
    }
}

fs_node_t* find_file(char* path) {
    if (!path || path[0] != '/') {
        print("Invalid path\n");
        return NULL;
    }
    fs_node_t* current = root;
    int i = 1;
    char* name = malloc(256);
    if (!name) {
        print("Memory allocation failed for path name\n");
        return NULL;
    }
    int j = 0;
    while (path[i]) {
        j = 0;
        while (path[i] && path[i] != '/' && j < 255) {
            name[j++] = path[i++];
        }
        name[j] = '\0';
        if (path[i] == '/') i++;
        if (name[0] == '\0') continue;
        int found = 0;
        for (uint32 k = 0; k < current->num_children; k++) {
            if (current->children[k] == NULL || current->children[k]->name == NULL) continue;
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

void write_file(fs_node_t* file, char* new_content) {
    if (!file || file->is_directory) {
        print("Invalid file\n");
        return;
    }
    free(file->content);
    uint32 content_len = strlen(new_content);
    file->content = malloc(content_len + 1);
    if (!file->content) {
        print("Memory allocation failed for file content\n");
        return;
    }
    memory_copy(new_content, file->content, content_len + 1);
    save_fs_to_disk();
}

char* read_file(fs_node_t* file) {
    if (!file || file->is_directory) {
        print("Invalid file\n");
        return "";
    }
    return file->content;
}