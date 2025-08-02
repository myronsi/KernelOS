#include "../include/fs.h"
#include "../include/util.h"
#include "../include/screen.h"
#include "../include/system.h"

#define IDE_PRIMARY_DATA        0x1F0
#define IDE_PRIMARY_ERROR       0x1F1
#define IDE_PRIMARY_SECTORCOUNT 0x1F2
#define IDE_PRIMARY_LBA_LOW     0x1F3
#define IDE_PRIMARY_LBA_MID     0x1F4
#define IDE_PRIMARY_LBA_HIGH    0x1F5
#define IDE_PRIMARY_DEVICE      0x1F6
#define IDE_PRIMARY_STATUS      0x1F7
#define IDE_PRIMARY_COMMAND     0x1F7
#define IDE_CONTROL            0x3F6
#define IDE_ALT_STATUS         0x3F6

#define IDE_CMD_READ_SECTORS    0x20
#define IDE_CMD_WRITE_SECTORS   0x30
#define IDE_CMD_IDENTIFY        0xEC
#define IDE_CMD_SOFT_RESET      0x04

#define DISABLE_IDE 0

#if !DISABLE_IDE
static void ide_delay() {
    inportb(IDE_ALT_STATUS);
    inportb(IDE_ALT_STATUS);
    inportb(IDE_ALT_STATUS);
    inportb(IDE_ALT_STATUS);
}

static int ide_wait_status(uint8 mask, uint8 expected, int timeout) {
    print("Waiting for IDE status\n");
    while (timeout--) {
        uint8 status = inportb(IDE_PRIMARY_STATUS);
        uint8 alt_status = inportb(IDE_ALT_STATUS);
        print("IDE status (primary): ");
        print(int_to_string(status));
        print(" | Alt status: ");
        print(int_to_string(alt_status));
        print("\n");
        if ((status & mask) == expected) {
            print("IDE status condition met\n");
            return 0;
        }
        if (status & 0x01) {
            uint8 error = inportb(IDE_PRIMARY_ERROR);
            print("IDE error: ");
            print(int_to_string(error));
            print("\n");
            return -1;
        }
        ide_delay();
    }
    print("IDE status timeout\n");
    return -1;
}

static int ide_check_device() {
    print("Checking IDE device presence\n");
    for (int attempt = 0; attempt < 3; attempt++) {
        print("Attempt ");
        print(int_to_string(attempt + 1));
        print("\n");
        uint8 device_codes[] = {0xE0, 0xA0};
        for (int i = 0; i < 2; i++) {
            print("Trying device code: ");
            print(int_to_string(device_codes[i]));
            print("\n");
            outportb(IDE_PRIMARY_DEVICE, device_codes[i]);
            ide_delay();
            outportb(IDE_PRIMARY_SECTORCOUNT, 0);
            outportb(IDE_PRIMARY_LBA_LOW, 0);
            outportb(IDE_PRIMARY_LBA_MID, 0);
            outportb(IDE_PRIMARY_LBA_HIGH, 0);
            outportb(IDE_PRIMARY_COMMAND, IDE_CMD_IDENTIFY);
            ide_delay();
            uint8 status = inportb(IDE_PRIMARY_STATUS);
            print("Initial IDE status: ");
            print(int_to_string(status));
            print("\n");
            if (status != 0xFF && status != 0x00) {
                if (ide_wait_status(0x88, 0x08, 1000000) < 0) {
                    print("IDE identify wait failed\n");
                    continue;
                }
                return 0;
            }
            print("No IDE device detected with code ");
            print(int_to_string(device_codes[i]));
            print("\n");
            ide_delay();
        }
    }
    print("All IDE detection attempts failed\n");
    return -1;
}

static void ide_reset() {
    print("Resetting IDE controller\n");
    outportb(IDE_CONTROL, IDE_CMD_SOFT_RESET);
    ide_delay();
    outportb(IDE_CONTROL, 0x00);
    ide_delay();
    ide_wait_status(0x80, 0x00, 1000000);
}

static int ide_init() {
    print("Initializing IDE controller\n");
    ide_reset();
    if (ide_check_device() < 0) {
        print("IDE device not found, skipping initialization\n");
        return -1;
    }
    outportb(IDE_PRIMARY_DEVICE, 0xE0);
    ide_delay();
    outportb(IDE_PRIMARY_SECTORCOUNT, 0);
    outportb(IDE_PRIMARY_LBA_LOW, 0);
    outportb(IDE_PRIMARY_LBA_MID, 0);
    outportb(IDE_PRIMARY_LBA_HIGH, 0);
    outportb(IDE_PRIMARY_COMMAND, IDE_CMD_IDENTIFY);
    ide_delay();
    if (ide_wait_status(0x88, 0x08, 1000000) < 0) {
        print("IDE identify failed\n");
        return -1;
    }
    uint8 buffer[512];
    print("Reading identify buffer\n");
    for (int i = 0; i < 256; i++) {
        uint16 data = inw(IDE_PRIMARY_DATA);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
    }
    print("IDE identify complete\n");
    print("Identify buffer [0-3]: ");
    print(int_to_string(buffer[0]));
    print(" ");
    print(int_to_string(buffer[1]));
    print(" ");
    print(int_to_string(buffer[2]));
    print(" ");
    print(int_to_string(buffer[3]));
    print("\n");
    print("Assuming IDE device is a hard disk\n");
    return 0;
}

void read_sector(uint32 sector, uint8* buffer) {
    print("Reading sector ");
    print(int_to_string(sector));
    print("\n");
    
    asm("cli");
    if (ide_wait_status(0x80, 0x00, 1000000) < 0) {
        print("IDE wait failed before read\n");
        asm("sti");
        return;
    }
    outportb(IDE_PRIMARY_DEVICE, 0xE0 | ((sector >> 24) & 0x0F));
    ide_delay();
    outportb(IDE_PRIMARY_SECTORCOUNT, 1);
    ide_delay();
    outportb(IDE_PRIMARY_LBA_LOW, sector & 0xFF);
    ide_delay();
    outportb(IDE_PRIMARY_LBA_MID, (sector >> 8) & 0xFF);
    ide_delay();
    outportb(IDE_PRIMARY_LBA_HIGH, (sector >> 16) & 0xFF);
    ide_delay();
    outportb(IDE_PRIMARY_COMMAND, IDE_CMD_READ_SECTORS);
    ide_delay();
    
    if (ide_wait_status(0x88, 0x08, 1000000) < 0) {
        print("IDE wait failed after read command\n");
        asm("sti");
        return;
    }
    uint8 error = inportb(IDE_PRIMARY_ERROR);
    if (error) {
        print("IDE read error: ");
        print(int_to_string(error));
        print("\n");
        asm("sti");
        return;
    }
    
    print("Starting data read\n");
    for (int i = 0; i < 256; i++) {
        uint16 data = inw(IDE_PRIMARY_DATA);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
        if (i < 2) {
            print("Data [");
            print(int_to_string(i * 2));
            print("-");
            print(int_to_string(i * 2 + 1));
            print("]: ");
            print(int_to_string(buffer[i * 2]));
            print(" ");
            print(int_to_string(buffer[i * 2 + 1]));
            print("\n");
        }
    }
    print("Sector read complete\n");
    asm("sti");
}

void write_sector(uint32 sector, uint8* buffer) {
    print("Writing sector ");
    print(int_to_string(sector));
    print("\n");
    
    asm("cli");
    if (ide_wait_status(0x80, 0x00, 1000000) < 0) {
        print("IDE wait failed before write\n");
        asm("sti");
        return;
    }
    outportb(IDE_PRIMARY_DEVICE, 0xE0 | ((sector >> 24) & 0x0F));
    ide_delay();
    outportb(IDE_PRIMARY_SECTORCOUNT, 1);
    ide_delay();
    outportb(IDE_PRIMARY_LBA_LOW, sector & 0xFF);
    ide_delay();
    outportb(IDE_PRIMARY_LBA_MID, (sector >> 8) & 0xFF);
    ide_delay();
    outportb(IDE_PRIMARY_LBA_HIGH, (sector >> 16) & 0xFF);
    ide_delay();
    outportb(IDE_PRIMARY_COMMAND, IDE_CMD_WRITE_SECTORS);
    ide_delay();
    
    if (ide_wait_status(0x88, 0x08, 1000000) < 0) {
        print("IDE wait failed after write command\n");
        asm("sti");
        return;
    }
    uint8 error = inportb(IDE_PRIMARY_ERROR);
    if (error) {
        print("IDE write error: ");
        print(int_to_string(error));
        print("\n");
        asm("sti");
        return;
    }
    
    print("Starting data write\n");
    for (int i = 0; i < 256; i++) {
        uint16 data = buffer[i * 2] | (buffer[i * 2 + 1] << 8);
        outw(IDE_PRIMARY_DATA, data);
        if (i < 2) {
            print("Data [");
            print(int_to_string(i * 2));
            print("-");
            print(int_to_string(i * 2 + 1));
            print("]: ");
            print(int_to_string(buffer[i * 2]));
            print(" ");
            print(int_to_string(buffer[i * 2 + 1]));
            print("\n");
        }
    }
    if (ide_wait_status(0x80, 0x00, 1000000) < 0) {
        print("IDE wait failed after write\n");
        asm("sti");
        return;
    }
    print("Sector write complete\n");
    asm("sti");
}
#endif

fs_node_t* root;
fs_node_t* current_dir;

#define FS_SECTOR 100
#define MAX_NAME_LEN 255
#define MAX_SECTORS 1

uint32 serialize_node(fs_node_t* node, uint8* buffer, uint32 offset, uint32 buffer_size) {
    print("Serializing node: ");
    print(node->name);
    print(", offset=");
    print(int_to_string(offset));
    print("\n");
    if (offset >= buffer_size) {
        print("Offset exceeds buffer size\n");
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
    print("Name length: ");
    print(int_to_string(name_len));
    print("\n");
    if (offset + 1 + name_len > buffer_size) {
        print("Name length exceeds buffer size\n");
        return offset;
    }
    buffer[offset++] = (uint8)name_len;
    memory_copy(node->name, buffer + offset, name_len);
    offset += name_len;
    print("Wrote name: ");
    print(node->name);
    print("\n");

    if (!node->is_directory) {
        uint32 content_len = strlen(node->content);
        print("Content length: ");
        print(int_to_string(content_len));
        print("\n");
        if (offset + 4 + content_len > buffer_size) {
            print("Content exceeds buffer size\n");
            return offset;
        }
        *(uint32*)(buffer + offset) = content_len;
        offset += 4;
        memory_copy(node->content, buffer + offset, content_len);
        offset += content_len;
        print("Wrote content length: ");
        print(int_to_string(content_len));
        print("\n");
    } else {
        if (offset + 4 > buffer_size) {
            print("Num children exceeds buffer size\n");
            return offset;
        }
        *(uint32*)(buffer + offset) = node->num_children;
        print("Wrote num_children: ");
        print(int_to_string(node->num_children));
        print("\n");
        offset += 4;
        print("Buffer [offset-4:offset-1]: ");
        for (int i = offset - 4; i < offset && i < buffer_size; i++) {
            print(int_to_string(buffer[i]));
            print(" ");
        }
        print("\n");
        for (int i = 0; i < node->num_children; i++) {
            offset = serialize_node(node->children[i], buffer, offset, buffer_size);
        }
    }
    print("Serialized node, offset: ");
    print(int_to_string(offset));
    print("\n");
    return offset;
}

uint32 deserialize_node(fs_node_t** node, uint8* buffer, uint32 offset, uint32 buffer_size) {
    print("Deserializing node at offset: ");
    print(int_to_string(offset));
    print("\n");
    if (offset >= buffer_size) {
        print("Offset exceeds buffer size\n");
        return offset;
    }
    *node = malloc(sizeof(fs_node_t));
    if (!*node) {
        print("Memory allocation failed for node\n");
        return offset;
    }
    (*node)->is_directory = buffer[offset++];
    print("Read is_directory: ");
    print(int_to_string((*node)->is_directory));
    print("\n");

    if (offset >= buffer_size) {
        print("Offset exceeds buffer size after is_directory\n");
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
    print("Read name length: ");
    print(int_to_string(name_len));
    print("\n");

    if (offset + name_len > buffer_size) {
        print("Name length exceeds buffer size\n");
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
    memory_copy(buffer + offset, (*node)->name, name_len);
    (*node)->name[name_len] = '\0';
    print("Read node name: ");
    print((*node)->name);
    print("\n");
    offset += name_len;

    if (!(*node)->is_directory) {
        if (offset + 4 > buffer_size) {
            print("Content length exceeds buffer size\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        print("Buffer [offset:offset+3]: ");
        for (int i = offset; i < offset + 4 && i < buffer_size; i++) {
            print(int_to_string(buffer[i]));
            print(" ");
        }
        print("\n");
        uint32 content_len = *(uint32*)(buffer + offset);
        print("Read content length: ");
        print(int_to_string(content_len));
        print("\n");
        if (content_len > buffer_size - offset - 4) {
            print("Invalid content length: ");
            print(int_to_string(content_len));
            print("\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        offset += 4;
        (*node)->content = malloc(content_len + 1);
        if (!(*node)->content) {
            print("Memory allocation failed for content\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        memory_copy(buffer + offset, (*node)->content, content_len);
        (*node)->content[content_len] = '\0';
        print("Read node content: ");
        print((*node)->content);
        print("\n");
        offset += content_len;
        (*node)->children = NULL;
        (*node)->num_children = 0;
    } else {
        if (offset + 4 > buffer_size) {
            print("Num children exceeds buffer size\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        print("Buffer [offset:offset+3]: ");
        for (int i = offset; i < offset + 4 && i < buffer_size; i++) {
            print(int_to_string(buffer[i]));
            print(" ");
        }
        print("\n");
        uint32 num_children = *(uint32*)(buffer + offset);
        print("Read num_children: ");
        print(int_to_string(num_children));
        print("\n");
        if (num_children < 0 || num_children > 1000) {
            print("Invalid num_children: ");
            print(int_to_string(num_children));
            print("\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        (*node)->num_children = num_children;
        offset += 4;
        (*node)->children = malloc(num_children * sizeof(fs_node_t*));
        if (!(*node)->children) {
            print("Memory allocation failed for children\n");
            free((*node)->name);
            free(*node);
            *node = NULL;
            return offset;
        }
        for (int i = 0; i < num_children; i++) {
            (*node)->children[i] = NULL;
            offset = deserialize_node(&(*node)->children[i], buffer, offset, buffer_size);
            if ((*node)->children[i] == NULL) {
                print("Failed to deserialize child ");
                print(int_to_string(i));
                print("\n");
                for (int j = 0; j < i; j++) {
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
    print("Deserialized node, offset: ");
    print(int_to_string(offset));
    print("\n");
    return offset;
}

void save_fs_to_disk() {
#if !DISABLE_IDE
    print("Saving FS to disk, heap_pos=");
    print(int_to_string(get_heap_pos()));
    print("\n");
    uint8* buffer = malloc(512);
    if (!buffer) {
        print("Memory allocation failed for save buffer\n");
        return;
    }
    uint32 buffer_size = 512;
    memory_set(buffer, 0, buffer_size);
    uint32 offset = 0;

    if (offset + 4 > buffer_size) {
        print("Buffer too small for root num_children\n");
        free(buffer);
        return;
    }
    *(uint32*)buffer = root->num_children;
    print("Wrote root num_children: ");
    print(int_to_string(root->num_children));
    print("\n");
    print("Buffer [0:3]: ");
    for (int i = 0; i < 4; i++) {
        print(int_to_string(buffer[i]));
        print(" ");
    }
    print("\n");
    offset += 4;

    for (int i = 0; i < root->num_children; i++) {
        offset = serialize_node(root->children[i], buffer, offset, buffer_size);
        if (offset > buffer_size) {
            print("Serialization exceeded buffer size at child ");
            print(int_to_string(i));
            print("\n");
            free(buffer);
            return;
        }
    }

    print("Buffer [0-15] before write: ");
    for (int i = 0; i < 16 && i < buffer_size; i++) {
        print(int_to_string(buffer[i]));
        print(" ");
    }
    print("\n");

    write_sector(FS_SECTOR, buffer);
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
    uint8* buffer = initial_buffer ? initial_buffer : malloc(512);
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
    uint32 buffer_size = 512;

    if (!initial_buffer) {
        read_sector(FS_SECTOR, buffer);
        uint8 error = inportb(IDE_PRIMARY_ERROR);
        if (error) {
            print("Failed to load FS, creating new FS\n");
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
    }

    print("Buffer [0-15] after read: ");
    for (int i = 0; i < 16 && i < buffer_size; i++) {
        print(int_to_string(buffer[i]));
        print(" ");
    }
    print("\n");

    uint32 offset = 0;
    if (offset + 4 > buffer_size) {
        print("Buffer too small for num_nodes\n");
        if (!initial_buffer) free(buffer);
        root = malloc(sizeof(fs_node_t));
        if (root) {
            root->name = "/";
            root->is_directory = 1;
            root->children = malloc(0);
            root->num_children = 0;
        }
        return;
    }
    uint32 num_nodes = *(uint32*)buffer;
    print("Read root num_nodes: ");
    print(int_to_string(num_nodes));
    print("\n");
    print("Buffer [0:3]: ");
    for (int i = 0; i < 4; i++) {
        print(int_to_string(buffer[i]));
        print(" ");
    }
    print("\n");
    if (num_nodes > 1000 || num_nodes < 0) {
        print("Invalid num_nodes: ");
        print(int_to_string(num_nodes));
        print("\n");
        if (!initial_buffer) free(buffer);
        root = malloc(sizeof(fs_node_t));
        if (root) {
            root->name = "/";
            root->is_directory = 1;
            root->children = malloc(0);
            root->num_children = 0;
        }
        return;
    }
    offset += 4;

    root = malloc(sizeof(fs_node_t));
    if (!root) {
        print("Memory allocation failed for root\n");
        if (!initial_buffer) free(buffer);
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
        if (!initial_buffer) free(buffer);
        return;
    }

    for (int i = 0; i < num_nodes; i++) {
        root->children[i] = NULL;
        offset = deserialize_node(&root->children[i], buffer, offset, buffer_size);
        if (root->children[i] == NULL) {
            print("Failed to deserialize child ");
            print(int_to_string(i));
            print("\n");
            for (int j = 0; j < i; j++) {
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
            if (!initial_buffer) free(buffer);
            return;
        }
    }
    if (!initial_buffer) {
        free(buffer);
    }
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
        uint8 buffer[512];
        print("Before read_sector\n");
        read_sector(FS_SECTOR, buffer);
        print("After read_sector\n");
        uint8 error = inportb(IDE_PRIMARY_ERROR);
        print("IDE error register: ");
        print(int_to_string(error));
        print("\n");
        print("Buffer [0-3]: ");
        print(int_to_string(buffer[0]));
        print(" ");
        print(int_to_string(buffer[1]));
        print(" ");
        print(int_to_string(buffer[2]));
        print(" ");
        print(int_to_string(buffer[3]));
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

void create_directory(fs_node_t* parent, string name) {
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

void create_file(fs_node_t* parent, string name) {
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
    print("Listing directory, num_children: ");
    print(int_to_string(dir->num_children));
    print("\n");
    for (int i = 0; i < dir->num_children; i++) {
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
}

fs_node_t* find_file(string path) {
    if (path[0] != '/') return NULL;
    fs_node_t* current = root;
    int i = 1;
    string name = malloc(256);
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
        int found = 0;
        for (int k = 0; k < current->num_children; k++) {
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

void write_file(fs_node_t* file, string new_content) {
    if (file->is_directory) return;
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

string read_file(fs_node_t* file) {
    if (file->is_directory) return "";
    return file->content;
}