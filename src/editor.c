#include "../include/editor.h"
#include "../include/screen.h"
#include "../include/kb.h"
#include "../include/util.h"

#define MAX_LINES 100
#define MAX_LINE_LEN 80
#define STATUS_LINES 2

void edit_file(string path) {
    fs_node_t* file = find_file(path);
    if (!file || file->is_directory) {
        print("File not found or is a directory\n");
        return;
    }
    string content = read_file(file);
    string lines[MAX_LINES];
    int num_lines = 0;
    if (!*content) {
        lines[0] = malloc(MAX_LINE_LEN);
        lines[0][0] = '\0';
        num_lines = 1;
    } else {
        string line = content;
        while (*line && num_lines < MAX_LINES) {
            lines[num_lines] = malloc(MAX_LINE_LEN);
            int j = 0;
            while (*line && *line != '\n' && j < MAX_LINE_LEN - 1) {
                lines[num_lines][j++] = *line++;
            }
            lines[num_lines][j] = '\0';
            if (*line == '\n') line++;
            num_lines++;
        }
    }
    cursorX = 0;
    cursorY = 0;
    int view_offset = 0;
    int running = 1;

    clearScreen();

    while (running) {
        int max_lines = sh - STATUS_LINES;
        for (int i = view_offset; i < num_lines && i < view_offset + max_lines; i++) {
            cursorX = 0;
            cursorY = i - view_offset;
            clearLine(cursorY, cursorY);
            print(lines[i]);
            print("\n");
        }

        cursorY = sh - STATUS_LINES;
        cursorX = 0;
        clearLine(sh - STATUS_LINES, sh - 1);
        print("--- NIDOS Editor --- Lines: ");
        string line_count = int_to_string(num_lines);
        print(line_count);
        free(line_count);
        print("\n");
        print("Ctrl+S: Save | Ctrl+X: Save & Exit");

        cursorY = cursorY - view_offset;
        if (cursorY < 0) cursorY = 0;
        if (cursorY >= max_lines) cursorY = max_lines - 1;
        updateCursor();

        uint8 key = 0;
        while (!(inportb(0x64) & 0x1));
        key = inportb(0x60);
        switch (key) {
            case 75:
                if (cursorX > 0) {
                    cursorX--;
                } else if (cursorY > 0) {
                    cursorY--;
                    cursorX = strlen(lines[cursorY + view_offset]);
                }
                break;
            case 77:
                if (cursorX < strlen(lines[cursorY + view_offset])) {
                    cursorX++;
                } else if (cursorY + view_offset < num_lines - 1) {
                    cursorY++;
                    cursorX = 0;
                }
                break;
            case 72:
                if (cursorY > 0) {
                    cursorY--;
                    if (cursorX > strlen(lines[cursorY + view_offset])) {
                        cursorX = strlen(lines[cursorY + view_offset]);
                    }
                } else if (view_offset > 0) {
                    view_offset--;
                }
                break;
            case 80:
                if (cursorY + view_offset < num_lines - 1 && cursorY < max_lines - 1) {
                    cursorY++;
                    if (cursorX > strlen(lines[cursorY + view_offset])) {
                        cursorX = strlen(lines[cursorY + view_offset]);
                    }
                } else if (cursorY + view_offset < num_lines - 1) {
                    view_offset++;
                }
                break;
            case 14:
                if (cursorX > 0) {
                    for (int i = cursorX - 1; i < strlen(lines[cursorY + view_offset]); i++) {
                        lines[cursorY + view_offset][i] = lines[cursorY + view_offset][i + 1];
                    }
                    cursorX--;
                } else if (cursorY + view_offset > 0) {
                    cursorY--;
                    cursorX = strlen(lines[cursorY + view_offset]);
                    if (num_lines > 1) {
                        memory_copy(lines[cursorY + view_offset + 1], lines[cursorY + view_offset] + cursorX, strlen(lines[cursorY + view_offset + 1]) + 1);
                        free(lines[cursorY + view_offset + 1]);
                        for (int i = cursorY + view_offset + 1; i < num_lines - 1; i++) {
                            lines[i] = lines[i + 1];
                        }
                        num_lines--;
                        if (cursorY >= max_lines) {
                            view_offset--;
                        }
                    }
                }
                break;
            case 28:
                if (num_lines < MAX_LINES) {
                    string new_line = malloc(MAX_LINE_LEN);
                    memory_set(new_line, 0, MAX_LINE_LEN);
                    for (int i = num_lines; i > cursorY + view_offset + 1; i--) {
                        lines[i] = lines[i - 1];
                    }
                    lines[cursorY + view_offset + 1] = new_line;
                    if (cursorX < strlen(lines[cursorY + view_offset])) {
                        memory_copy(lines[cursorY + view_offset] + cursorX, new_line, strlen(lines[cursorY + view_offset]) - cursorX + 1);
                        lines[cursorY + view_offset][cursorX] = '\0';
                    }
                    num_lines++;
                    cursorY++;
                    cursorX = 0;
                    if (cursorY >= max_lines) {
                        view_offset++;
                    }
                }
                break;
            default: {
                string input = readStr();
                if (strEql(input, "")) {
                    free(input);
                    continue;
                }
                if (strEql(input, "\x01")) {
                    string new_content = malloc(1);
                    new_content[0] = '\0';
                    for (int i = 0; i < num_lines; i++) {
                        string temp = malloc(strlen(new_content) + strlen(lines[i]) + 2);
                        memory_copy(new_content, temp, strlen(new_content));
                        memory_copy(lines[i], temp + strlen(new_content), strlen(lines[i]));
                        temp[strlen(new_content) + strlen(lines[i])] = '\n';
                        temp[strlen(new_content) + strlen(lines[i]) + 1] = '\0';
                        free(new_content);
                        new_content = temp;
                    }
                    write_file(file, new_content);
                    free(new_content);
                    cursorY = sh - STATUS_LINES;
                    cursorX = 0;
                    print("\nFile saved");
                    free(input);
                    continue;
                } else if (strEql(input, "\x02")) {
                    string new_content = malloc(1);
                    new_content[0] = '\0';
                    for (int i = 0; i < num_lines; i++) {
                        string temp = malloc(strlen(new_content) + strlen(lines[i]) + 2);
                        memory_copy(new_content, temp, strlen(new_content));
                        memory_copy(lines[i], temp + strlen(new_content), strlen(lines[i]));
                        temp[strlen(new_content) + strlen(lines[i])] = '\n';
                        temp[strlen(new_content) + strlen(lines[i]) + 1] = '\0';
                        free(new_content);
                        new_content = temp;
                    }
                    write_file(file, new_content);
                    free(new_content);
                    running = 0;
                    free(input);
                    continue;
                } else if (strEql(input, "\n")) {
                    if (num_lines < MAX_LINES) {
                        string new_line = malloc(MAX_LINE_LEN);
                        memory_set(new_line, 0, MAX_LINE_LEN);
                        for (int i = num_lines; i > cursorY + view_offset + 1; i--) {
                            lines[i] = lines[i - 1];
                        }
                        lines[cursorY + view_offset + 1] = new_line;
                        if (cursorX < strlen(lines[cursorY + view_offset])) {
                            memory_copy(lines[cursorY + view_offset] + cursorX, new_line, strlen(lines[cursorY + view_offset]) - cursorX + 1);
                            lines[cursorY + view_offset][cursorX] = '\0';
                        }
                        num_lines++;
                        cursorY++;
                        cursorX = 0;
                        if (cursorY >= max_lines) {
                            view_offset++;
                        }
                    }
                } else if (strEql(input, "\b")) {
                    if (cursorX > 0) {
                        for (int i = cursorX - 1; i < strlen(lines[cursorY + view_offset]); i++) {
                            lines[cursorY + view_offset][i] = lines[cursorY + view_offset][i + 1];
                        }
                        cursorX--;
                    } else if (cursorY + view_offset > 0) {
                        cursorY--;
                        cursorX = strlen(lines[cursorY + view_offset]);
                        if (num_lines > 1) {
                            memory_copy(lines[cursorY + view_offset + 1], lines[cursorY + view_offset] + cursorX, strlen(lines[cursorY + view_offset + 1]) + 1);
                            free(lines[cursorY + view_offset + 1]);
                            for (int i = cursorY + view_offset + 1; i < num_lines - 1; i++) {
                                lines[i] = lines[i + 1];
                            }
                            num_lines--;
                            if (cursorY >= max_lines) {
                                view_offset--;
                            }
                        }
                    }
                } else {
                    for (int i = 0; input[i] && cursorX < MAX_LINE_LEN - 1; i++) {
                        char c = input[i];
                        if ((c >= ' ' && c <= '~') || c == '\t') {
                            for (int j = strlen(lines[cursorY + view_offset]); j > cursorX; j--) {
                                lines[cursorY + view_offset][j] = lines[cursorY + view_offset][j - 1];
                            }
                            lines[cursorY + view_offset][cursorX] = c;
                            lines[cursorY + view_offset][cursorX + 1] = '\0';
                            cursorX++;
                        }
                    }
                }
                free(input);
                break;
            }
        }

        for (int i = 0; i < num_lines; i++) {
            if (strlen(lines[i]) >= MAX_LINE_LEN) {
                lines[i][MAX_LINE_LEN - 1] = '\0';
            }
        }
    }

    for (int i = 0; i < num_lines; i++) {
        free(lines[i]);
    }
}