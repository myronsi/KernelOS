#include "../include/shell.h"
#include "../include/fs.h"
#include "../include/editor.h"

int strncmp(const char* s1, const char* s2, uint32 n) {
    for (uint32 i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
            return s1[i] - s2[i];
        }
    }
    return 0;
}

char* strrchr(const char* s, int c) {
    char* last = NULL;
    while (*s) {
        if (*s == c) {
            last = (char*)s;
        }
        s++;
    }
    if (c == '\0') {
        return (char*)s;
    }
    return last;
}

char* strncpy(char* dest, const char* src, uint32 n) {
    uint32 i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

void parse_command(char* input, char** command, char** args) {
    *command = input;
    *args = NULL;
    int i = 0;
    while (input[i] && input[i] != ' ') i++;
    if (input[i] == ' ') {
        input[i] = '\0';
        *args = input + i + 1;
        while (**args == ' ') (*args)++;
        if (**args == '\0') *args = NULL;
    }
}

fs_node_t* resolve_path(char* path, fs_node_t* current_dir, fs_node_t* root) {
    if (!path || path[0] == '\0') return NULL;

    fs_node_t* base = (path[0] == '/') ? root : current_dir;
    if (path[0] == '/') path++;
    else if (strncmp(path, "./", 2) == 0) path += 2;

    if (path[0] == '\0') return base;

    fs_node_t* current = base;
    char* name = malloc(256);
    if (!name) {
        print("Memory allocation failed for path name\n");
        return NULL;
    }
    int i = 0, j = 0;

    while (path[i]) {
        j = 0;
        while (path[i] && path[i] != '/' && j < 255) {
            name[j++] = path[i++];
        }
        name[j] = '\0';
        if (path[i] == '/') i++;

        if (strEql(name, "..")) {
            if (current == root) continue;
            for (int k = 0; k < root->num_children; k++) {
                if (root->children[k] == current) {
                    current = root;
                    break;
                }
            }
            for (int k = 0; k < current->num_children; k++) {
                if (current->children[k] == current) {
                    current = root;
                    break;
                }
            }
            continue;
        }

        if (name[0] == '\0') continue;

        int found = 0;
        for (uint32 k = 0; k < current->num_children; k++) {
            if (current->children[k] && strEql(current->children[k]->name, name)) {
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

void launch_shell(int n)
{
    char* ch = (char*) malloc(200);
    char* command;
    char* args;
    do
    {
        print("KernelOS (");
        char* temp = int_to_string(n);
        print(temp);
        free(temp);
        print(")> ");
        ch = readStr();
        parse_command(ch, &command, &args);

        if (strEql(command, "cmd"))
        {
            print("\nYou are already in cmd. A new recursive shell is opened\n");
            launch_shell(n + 1);
        }
        else if (strEql(command, "clear"))
        {
            clearScreen();
        }
        else if (strEql(command, "sum"))
        {
            sum();
        }
        else if (strEql(command, "echo"))
        {
            echo();
        }
        else if (strEql(command, "sort"))
        {
            sort();
        }
        else if (strEql(command, "fibonacci"))
        {
            fibonacci();
        }
        else if (strEql(command, "gcd"))
        {
            gcd();
        }
        else if (strEql(command, "help"))
        {
            help();
        }
        else if (strEql(command, "color"))
        {
            set_background_color();
        }
        else if (strEql(command, "multiply"))
        {
            multiply();
        }
        else if (strEql(command, "mkdir"))
        {
            if (!args) {
                print("\nDirectory name: ");
                char* dir_name = readStr();
                create_directory(current_dir, dir_name);
                free(dir_name);
                print("\n");
            } else {
                char* path = args;
                char* last_slash = strrchr(path, '/');
                if (last_slash && last_slash != path) {
                    char* parent_path = malloc(last_slash - path + 1);
                    if (!parent_path) {
                        print("Memory allocation failed\n");
                        print("\n");
                        continue;
                    }
                    strncpy(parent_path, path, last_slash - path);
                    parent_path[last_slash - path] = '\0';
                    fs_node_t* parent = resolve_path(parent_path, current_dir, root);
                    free(parent_path);
                    if (!parent || !parent->is_directory) {
                        print("\nParent directory not found or not a directory\n");
                        print("\n");
                        continue;
                    }
                    create_directory(parent, last_slash + 1);
                } else {
                    fs_node_t* parent = (path[0] == '/') ? root : current_dir;
                    if (path[0] == '/') path++;
                    create_directory(parent, path);
                }
                print("\n");
            }
        }
        else if (strEql(command, "mkf"))
        {
            if (!args) {
                print("\nFile name: ");
                char* file_name = readStr();
                create_file(current_dir, file_name);
                free(file_name);
                print("\n");
            } else {
                char* path = args;
                char* last_slash = strrchr(path, '/');
                if (last_slash && last_slash != path) {
                    char* parent_path = malloc(last_slash - path + 1);
                    if (!parent_path) {
                        print("Memory allocation failed\n");
                        print("\n");
                        continue;
                    }
                    strncpy(parent_path, path, last_slash - path);
                    parent_path[last_slash - path] = '\0';
                    fs_node_t* parent = resolve_path(parent_path, current_dir, root);
                    free(parent_path);
                    if (!parent || !parent->is_directory) {
                        print("\nParent directory not found or not a directory\n");
                        print("\n");
                        continue;
                    }
                    create_file(parent, last_slash + 1);
                } else {
                    fs_node_t* parent = (path[0] == '/') ? root : current_dir;
                    if (path[0] == '/') path++;
                    create_file(parent, path);
                }
                print("\n");
            }
        }
        else if (strEql(command, "ls"))
        {
            fs_node_t* dir = current_dir;
            if (args) {
                dir = resolve_path(args, current_dir, root);
                if (!dir || !dir->is_directory) {
                    print("\nDirectory not found or not a directory\n");
                    print("\n");
                    continue;
                }
            }
            print("\n");
            list_directory(dir);
            print("\n");
        }
        else if (strEql(command, "edit"))
        {
            if (!args) {
                print("\nFile path: ");
                char* path = readStr();
                edit_file(path);
                free(path);
                print("\n");
            } else {
                edit_file(args);
                print("\n");
            }
        }
        else if (strEql(command, "cd"))
        {
            if (!args) {
                print("\nDirectory path: ");
                char* path = readStr();
                if (strEql(path, "/")) {
                    current_dir = root;
                    print("\nChanged to root directory\n");
                } else {
                    fs_node_t* dir = resolve_path(path, current_dir, root);
                    if (dir && dir->is_directory) {
                        current_dir = dir;
                        print("\nDirectory changed\n");
                    } else {
                        print("\nDirectory not found or not a directory\n");
                    }
                }
                free(path);
                print("\n");
            } else {
                if (strEql(args, "/")) {
                    current_dir = root;
                    print("\nChanged to root directory\n");
                } else {
                    fs_node_t* dir = resolve_path(args, current_dir, root);
                    if (dir && dir->is_directory) {
                        current_dir = dir;
                        print("\nDirectory changed\n");
                    } else {
                        print("\nDirectory not found or not a directory\n");
                    }
                }
                print("\n");
            }
        }
        else
        {
            print("\nCommand not found\n");
        }
        free(ch);
    } while (!strEql(command, "exit"));
}

void sum()
{
    print("\nHow many numbers: ");
    char* input = readStr();
    int n = str_to_int(input);
    free(input);
    print("\n");
    int arr[n];
    fill_array(arr, n);
    int s = sum_array(arr, n);
    print("Result: ");
    char* result = int_to_string(s);
    print(result);
    free(result);
    print("\n");
}

void echo()
{
    print("\n");
    char* str = readStr();
    print("\n");
    print(str);
    print("\n");
    free(str);
}

void sort()
{
    int arr[100];
    print("\nArray size: ");
    char* input = readStr();
    int n = str_to_int(input);
    free(input);
    print("\n");
    fill_array(arr, n);
    print("Before sorting:\n");
    print_array(arr, n);
    print("\nOrder: (1 for increasing/ 0 for decreasing): ");
    input = readStr();
    int ordre = str_to_int(input);
    free(input);
    insertion_sort(arr, n, ordre);
    print("\nAfter sorting:\n");
    print_array(arr, n);
}

void fill_array(int arr[], int n)
{
    int i = 0;
    for (i = 0; i < n; i++)
    {
        print("ARR[");
        char* index = int_to_string(i);
        print(index);
        free(index);
        print("]: ");
        char* input = readStr();
        arr[i] = str_to_int(input);
        free(input);
        print("\n");
    }
}

void print_array(int arr[], int n)
{
    int i = 0;
    for (i = 0; i < n; i++)
    {
        char* value = int_to_string(arr[i]);
        print(value);
        free(value);
        print("   ");
    }
    print("\n");
}

void insertion_sort(int arr[], int n, int ordre)
{
    int i = 0;
    for (i = 1; i < n; i++)
    {
        int aux = arr[i];
        int j = i;
        while ((j > 0) && ((aux < arr[j - 1]) && ordre))
        {
            arr[j] = arr[j - 1];
            j = j - 1;
        }
        arr[j] = aux;
    }
}

int sum_array(int arr[], int n)
{
    int i = 0;
    int s = 0;
    for (i = 0; i < n; i++)
    {
        s += arr[i];
    }
    return s;
}

void fibonacci()
{
    print("\nHow many Elements: ");
    char* input = readStr();
    int n = str_to_int(input);
    free(input);
    print("\n");
    int i = 0;
    for (i = 0; i < n; i++)
    {
        print("Fibo ");
        char* index = int_to_string(i);
        print(index);
        free(index);
        print(" : ");
        char* value = int_to_string(fibo(i));
        print(value);
        free(value);
        print("\n");
    }
}

int fibo(int n)
{
    if (n < 2)
        return 1;
    else
        return fibo(n - 1) + fibo(n - 2);
}

int gcd_couple(int a, int b)
{
    if (b == 0)
        return a;
    if (a % b == 0)
        return b;
    else
        return gcd_couple(b, a % b);
}

void gcd()
{
    print("\nHow many numbers: ");
    char* input = readStr();
    int n = str_to_int(input);
    free(input);
    print("\n");
    int arr[n];
    int matrix[n][100];
    fill_array(arr, n);
    for (int i = 0; i < n; i++)
    {
        matrix[0][i] = arr[i];
    }
    for (int i = 1; i < n; i++)
    {
        for (int j = 0; j < n - 1; j++)
        {
            matrix[i][j] = gcd_couple(matrix[i - 1][j], matrix[i - 1][j + 1]);
        }
    }
    print("Result: ");
    char* result = int_to_string(matrix[n - 1][0]);
    print(result);
    free(result);
    print("\n");
}

void print_matrix(int matrix[][100], int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            char* value = int_to_string(matrix[i][j]);
            print(value);
            free(value);
            print("   ");
        }
        print("\n");
    }
}

void set_background_color()
{
    print("\nColor codes : ");
    print("\n0 : black");
    print_colored("\n1 : blue", 1, 0);
    print_colored("\n2 : green", 2, 0);
    print_colored("\n3 : cyan", 3, 0);
    print_colored("\n4 : red", 4, 0);
    print_colored("\n5 : purple", 5, 0);
    print_colored("\n6 : orange", 6, 0);
    print_colored("\n7 : grey", 7, 0);
    print_colored("\n8 : dark grey", 8, 0);
    print_colored("\n9 : blue light", 9, 0);
    print_colored("\n10 : green light", 10, 0);
    print_colored("\n11 : blue lighter", 11, 0);
    print_colored("\n12 : red light", 12, 0);
    print_colored("\n13 : rose", 13, 0);
    print_colored("\n14 : yellow", 14, 0);
    print_colored("\n15 : white", 15, 0);

    print("\n\nText color ? : ");
    char* text_input = readStr();
    int text_color = str_to_int(text_input);
    free(text_input);
    print("\n\nBackground color ? : ");
    char* bg_input = readStr();
    int bg_color = str_to_int(bg_input);
    free(bg_input);
    set_screen_color(text_color, bg_color);
    clearScreen();
}

void multiply()
{
    print("\nNum 1 :");
    char* input1 = readStr();
    int num1 = str_to_int(input1);
    free(input1);
    print("\nNum 2 :");
    char* input2 = readStr();
    int num2 = str_to_int(input2);
    free(input2);
    print("\nResult : ");
    char* result = int_to_string(num1 * num2);
    print(result);
    free(result);
    print("\n");
}

void help()
{
    print("\ncmd       : Launch a new recursive Shell");
    print("\nclear     : Clears the screen");
    print("\nsum       : Computes the sum of n numbers");
    print("\necho      : Reprint a given text");
    print("\nsort      : Sorts a given n numbers");
    print("\nfibonacci : Prints the first n numbers of fibonacci");
    print("\ngcd       : Computes the greatest common divisor of n given numbers");
    print("\nexit      : Quits the current shell");
    print("\ncolor     : Changes the colors of the terminal");
    print("\nmultiply  : Multiplies two numbers");
    print("\nmkdir     : Creates a new directory (e.g., mkdir test or mkdir /test/test1)");
    print("\nmkf       : Creates a new empty file (e.g., mkf test or mkf /test/file1)");
    print("\nls        : Lists the contents of a directory (e.g., ls or ls /test/test1)");
    print("\nedit      : Opens a text editor for a file (e.g., edit /test/file1)");
    print("\ncd        : Changes the current directory (e.g., cd test/test1 or cd ..)");
    print("\n\n");
}
