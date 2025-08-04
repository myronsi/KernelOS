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

int ide_init() {
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