#include "../include/fs.h"
#include "../include/util.h"
#include "../include/screen.h"

#define FS_SECTOR 100
#define MAX_NAME_LEN 255
#define SECTOR_SIZE 512
#define MAX_SECTORS 16
#define BUFFER_SIZE (SECTOR_SIZE * MAX_SECTORS)

uint32 serialize_node(fs_node_t* node, uint8* buffer, uint32 offset, uint32 buffer_size) {
    if (!node || offset >= buffer_size) {
        print("Invalid node or offset exceeds buffer_size: ");
        print(int_to_string(offset));
        print("\n");
        return offset;
    }
    print("Serializing node: ");
    print(node->name);
    print(", is_directory=");
    print(int_to_string(node->is_directory));
    print(", num_children=");
    print(int_to_string(node->num_children));
    print(", offset=");
    print(int_to_string(offset));
    print("\n");

    if (offset + 1 > buffer_size) {
        print("Buffer too small for is_directory\n");
        return offset;
    }
    buffer[offset++] = node->is_directory;
    print("Wrote is_directory: ");
    print(int_to_string(node->is_directory));
    print("\n");

    uint32 name_len = strlen(node->name);
    if (name_len == 0 || name_len > MAX_NAME_LEN) {
        print("Invalid name length: ");
        print(int_to_string(name_len));
        print("\n");
        return offset;
    }
    if (offset + 1 + name_len > buffer_size) {
        print("Buffer too small for name\n");
        return offset;
    }
    buffer[offset++] = (uint8)name_len;
    memory_copy(node->name, (char*)(buffer + offset), name_len);
    offset += name_len;
    print("Wrote name: ");
    print(node->name);
    print(", length=");
    print(int_to_string(name_len));
    print("\n");

    if (!node->is_directory) {
        uint32 content_len = node->content ? strlen(node->content) : 0;
        if (offset + 4 + content_len > buffer_size) {
            print("Buffer too small for content\n");
            return offset;
        }
        *(uint32*)(buffer + offset) = content_len;
        offset += 4;
        if (content_len > 0) {
            memory_copy(node->content, (char*)(buffer + offset), content_len);
            offset += content_len;
        }
        print("Wrote content length: ");
        print(int_to_string(content_len));
        print("\n");
    } else {
        if (offset + 4 > buffer_size) {
            print("Buffer too small for num_children\n");
            return offset;
        }
        *(uint32*)(buffer + offset) = node->num_children;
        offset += 4;
        print("Wrote num_children: ");
        print(int_to_string(node->num_children));
        print("\n");
        for (uint32 i = 0; i < node->num_children; i++) {
            if (node->children[i]) {
                offset = serialize_node(node->children[i], buffer, offset, buffer_size);
                if (offset >= buffer_size) {
                    print("Serialization failed for child ");
                    print(int_to_string(i));
                    print("\n");
                    return offset;
                }
            } else {
                print("Skipping NULL child ");
                print(int_to_string(i));
                print("\n");
            }
        }
    }
    print("Serialized node, new offset=");
    print(int_to_string(offset));
    print("\n");
    return offset;
}

uint32 deserialize_node(fs_node_t** node, uint8* buffer, uint32 offset, uint32 buffer_size) {
    print("Deserializing node at offset: ");
    print(int_to_string(offset));
    print("\n");
    if (offset >= buffer_size) {
        print("Offset exceeds buffer_size\n");
        return offset;
    }

    *node = malloc(sizeof(fs_node_t));
    if (!*node) {
        print("Memory allocation failed for node\n");
        return offset;
    }
    (*node)->name = NULL;
    (*node)->content = NULL;
    (*node)->children = NULL;
    (*node)->num_children = 0;

    if (offset + 1 > buffer_size) {
        print("Buffer too small for is_directory\n");
        free(*node);
        *node = NULL;
        return offset;
    }
    (*node)->is_directory = buffer[offset++];
    print("Read is_directory: ");
    print(int_to_string((*node)->is_directory));
    print("\n");

    if (offset + 1 > buffer_size) {
        print("Buffer too small for name length\n");
        free(*node);
        *node = NULL;
        return offset;
    }
    uint32 name_len = buffer[offset++];
    if (name_len == 0 || name_len > MAX_NAME_LEN) {
        print("Invalid name length: ");
        print(int_to_string(name_len));
        print("\n");
        free(*node);
        *node = NULL;
        return offset;
    }
    if (offset + name_len > buffer_size) {
        print("Buffer too small for name\n");
        free(*node);
        *node = NULL;
        return offset;
    }
    (*node)->name = malloc(name_len + 1);
    if (!(*node)->name) {
        print("Memory allocation failed for name\n");
        free(*node);
        *node = NULL;
        return offset;
    }
    memory_copy((char*)(buffer + offset), (*node)->name, name_len);
    (*node)->name[name_len] = '\0';
    offset += name_len;
    print("Read node name: ");
    print((*node)->name);
    print("\n");

    if (!(*node)->is_directory) {
        if (offset + 4 > buffer_size) {
            print("Buffer too small for content length\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        uint32 content_len = *(uint32*)(buffer + offset);
        offset += 4;
        if (content_len > buffer_size - offset) {
            print("Invalid content length: ");
            print(int_to_string(content_len));
            print("\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        (*node)->content = malloc(content_len + 1);
        if (!(*node)->content) {
            print("Memory allocation failed for content\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        if (content_len > 0) {
            memory_copy((char*)(buffer + offset), (*node)->content, content_len);
        }
        (*node)->content[content_len] = '\0';
        offset += content_len;
        (*node)->children = NULL;
        (*node)->num_children = 0;
        print("Read content length: ");
        print(int_to_string(content_len));
        print("\n");
    } else {
        if (offset + 4 > buffer_size) {
            print("Buffer too small for num_children\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        uint32 num_children = *(uint32*)(buffer + offset);
        offset += 4;
        if (num_children > 1000) {
            print("Invalid num_children: ");
            print(int_to_string(num_children));
            print("\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        (*node)->num_children = num_children;
        print("Read num_children: ");
        print(int_to_string(num_children));
        print("\n");
        (*node)->children = malloc(num_children * sizeof(fs_node_t*));
        if (!(*node)->children) {
            print("Memory allocation failed for children\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        for (uint32 i = 0; i < num_children; i++) {
            (*node)->children[i] = NULL;
            offset = deserialize_node(&(*node)->children[i], buffer, offset, buffer_size);
            if ((*node)->children[i] == NULL) {
                print("Failed to deserialize child ");
                print(int_to_string(i));
                print("\n");
                for (uint32 j = 0; j < i; j++) {
                    if ((*node)->children[j]) {
                        free((*node)->children[j]->name);
                        if (!(*node)->children[j]->is_directory) {
                            free((*node)->children[j]->content);
                        }
                        free((*node)->children[j]);
                    }
                }
                free((*node)->children);
                free((*node)->name);
                free(*node);
                *node = NULL;
                return offset;
            }
        }
    }
    print("Deserialized node: ");
    print((*node)->name);
    print(", new offset=");
    print(int_to_string(offset));
    print("\n");
    return offset;
}

void save_fs_to_disk() {
#if !DISABLE_IDE
    print("Saving FS to disk, heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
    uint8* buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        print("Memory allocation failed for save buffer\n");
        return;
    }
    memory_set(buffer, 0, BUFFER_SIZE);
    uint32 offset = 0;

    // Сохраняем количество секторов (заглушка, обновим позже)
    offset += 4;

    if (offset + 4 > BUFFER_SIZE) {
        print("Buffer too small for root num_children\n");
        free(buffer);
        return;
    }
    *(uint32*)(buffer + offset) = root->num_children;
    offset += 4;
    print("Wrote root num_children: ");
    print(int_to_string(root->num_children));
    print("\n");

    for (uint32 i = 0; i < root->num_children; i++) {
        if (root->children[i]) {
            offset = serialize_node(root->children[i], buffer, offset, BUFFER_SIZE);
            if (offset >= BUFFER_SIZE) {
                print("Serialization exceeded buffer size at child ");
                print(int_to_string(i));
                print("\n");
                free(buffer);
                return;
            }
        }
    }

    uint32 num_sectors = (offset + SECTOR_SIZE - 1) / SECTOR_SIZE;
    if (num_sectors > MAX_SECTORS) {
        print("Too many sectors required: ");
        print(int_to_string(num_sectors));
        print("\n");
        free(buffer);
        return;
    }

    *(uint32*)buffer = num_sectors;
    print("Wrote num_sectors: ");
    print(int_to_string(num_sectors));
    print("\n");

    for (uint32 i = 0; i < num_sectors; i++) {
        uint8* sector_buffer = buffer + i * SECTOR_SIZE;
        write_sector(FS_SECTOR + i, sector_buffer);
        print("Wrote sector ");
        print(int_to_string(FS_SECTOR + i));
        print("\n");
    }

    free(buffer);
    print("FS saved to disk, heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
#else
    print("Saving FS to disk skipped (IDE disabled)\n");
#endif
}

void load_fs_from_disk(uint8* initial_buffer) {
#if !DISABLE_IDE
    print("Loading FS from disk, heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
    uint8* buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        print("Memory allocation failed for load buffer\n");
        root = malloc(sizeof(fs_node_t));
        if (root) {
            root->name = "/";
            root->is_directory = 1;
            root->children = malloc(0);
            root->num_children = 0;
        }
        return;
    }

    read_sector(FS_SECTOR, buffer);
    uint32 num_sectors = *(uint32*)buffer;
    if (num_sectors > MAX_SECTORS || num_sectors == 0) {
        print("Invalid number of sectors: ");
        print(int_to_string(num_sectors));
        print("\n");
        free(buffer);
        root = malloc(sizeof(fs_node_t));
        if (root) {
            root->name = "/";
            root->is_directory = 1;
            root->children = malloc(0);
            root->num_children = 0;
        }
        return;
    }
    print("Read num_sectors: ");
    print(int_to_string(num_sectors));
    print("\n");

    for (uint32 i = 0; i < num_sectors; i++) {
        read_sector(FS_SECTOR + i, buffer + i * SECTOR_SIZE);
        print("Read sector ");
        print(int_to_string(FS_SECTOR + i));
        print("\n");
    }

    uint32 offset = 4;
    if (offset + 4 > BUFFER_SIZE) {
        print("Buffer too small for num_nodes\n");
        free(buffer);
        root = malloc(sizeof(fs_node_t));
        if (root) {
            root->name = "/";
            root->is_directory = 1;
            root->children = malloc(0);
            root->num_children = 0;
        }
        return;
    }

    uint32 num_nodes = *(uint32*)(buffer + offset);
    offset += 4;
    print("Read root num_nodes: ");
    print(int_to_string(num_nodes));
    print("\n");

    if (num_nodes > 1000) {
        print("Invalid root num_nodes: ");
        print(int_to_string(num_nodes));
        print("\n");
        free(buffer);
        root = malloc(sizeof(fs_node_t));
        if (root) {
            root->name = "/";
            root->is_directory = 1;
            root->children = malloc(0);
            root->num_children = 0;
        }
        return;
    }

    root = malloc(sizeof(fs_node_t));
    if (!root) {
        print("Memory allocation failed for root\n");
        free(buffer);
        return;
    }
    root->name = "/";
    root->is_directory = 1;
    root->num_children = num_nodes;
    root->children = malloc(num_nodes * sizeof(fs_node_t*));
    if (!root->children) {
        print("Memory allocation failed for root children\n");
        free(root);
        root = NULL;
        free(buffer);
        return;
    }

    for (uint32 i = 0; i < num_nodes; i++) {
        root->children[i] = NULL;
        offset = deserialize_node(&root->children[i], buffer, offset, BUFFER_SIZE);
        if (root->children[i] == NULL) {
            print("Failed to deserialize child ");
            print(int_to_string(i));
            print("\n");
            for (uint32 j = 0; j < i; j++) {
                if (root->children[j]) {
                    free(root->children[j]->name);
                    if (!root->children[j]->is_directory) {
                        free(root->children[j]->content);
                    }
                    free(root->children[j]);
                }
            }
            free(root->children);
            free(root);
            root = NULL;
            free(buffer);
            return;
        }
    }

    free(buffer);
    print("FS loaded from disk, heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
#else
    print("Loading FS from disk skipped (IDE disabled)\n");
    root = malloc(sizeof(fs_node_t));
    if (root) {
        root->name = "/";
        root->is_directory = 1;
        root->children = malloc(0);
        root->num_children = 0;
    }
#endif
}