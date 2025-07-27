#include "../include/shell.h"
#include "../include/fs.h"
#include "../include/editor.h"

void launch_shell(int n)
{
    string ch = (string) malloc(200);
    string temp;
    do
    {
        print("KernelOS (");
        temp = int_to_string(n);
        print(temp);
        free(temp);
        print(")> ");
        ch = readStr();
        if (strEql(ch, "cmd"))
        {
            print("\nYou are already in cmd. A new recursive shell is opened\n");
            launch_shell(n + 1);
        }
        else if (strEql(ch, "clear"))
        {
            clearScreen();
        }
        else if (strEql(ch, "sum"))
        {
            sum();
        }
        else if (strEql(ch, "echo"))
        {
            echo();
        }
        else if (strEql(ch, "sort"))
        {
            sort();
        }
        else if (strEql(ch, "fibonacci"))
        {
            fibonacci();
        }
        else if (strEql(ch, "gcd"))
        {
            gcd();
        }
        else if (strEql(ch, "help"))
        {
            help();
        }
        else if (strEql(ch, "color"))
        {
            set_background_color();
        }
        else if (strEql(ch, "multiply"))
        {
            multiply();
        }
        else if (strEql(ch, "mkdir"))
        {
            print("\nDirectory name: ");
            string dir_name = readStr();
            create_directory(current_dir, dir_name);
            free(dir_name);
            print("\n");
        }
        else if (strEql(ch, "mkf"))
        {
            print("\nFile name: ");
            string file_name = readStr();
            create_file(current_dir, file_name);
            free(file_name);
            print("\n");
        }
        else if (strEql(ch, "ls"))
        {
            print("\n");
            list_directory(current_dir);
        }
        else if (strEql(ch, "edit"))
        {
            print("\nFile path: ");
            string path = readStr();
            edit_file(path);
            free(path);
            print("\n");
        }
        else if (strEql(ch, "cd"))
        {
            print("\nDirectory path: ");
            string path = readStr();
            if (strEql(path, "/"))
            {
                current_dir = root;
                print("\nChanged to root directory\n");
            }
            else
            {
                fs_node_t* dir = find_file(path);
                if (dir && dir->is_directory)
                {
                    current_dir = dir;
                    print("\nDirectory changed\n");
                }
                else
                {
                    print("\nDirectory not found or not a directory\n");
                }
            }
            free(path);
        }
        else
        {
            print("\nCommand not found\n");
            print("KernelOS> ");
        }
        free(ch);
    } while (!strEql(ch, "exit"));
}

void sum()
{
    print("\nHow many numbers: ");
    string input = readStr();
    int n = str_to_int(input);
    free(input);
    print("\n");
    int arr[n];
    fill_array(arr, n);
    int s = sum_array(arr, n);
    print("Result: ");
    string result = int_to_string(s);
    print(result);
    free(result);
    print("\n");
}

void echo()
{
    print("\n");
    string str = readStr();
    print("\n");
    print(str);
    print("\n");
    free(str);
}

void sort()
{
    int arr[100];
    print("\nArray size: ");
    string input = readStr();
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
        string index = int_to_string(i);
        print(index);
        free(index);
        print("]: ");
        string input = readStr();
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
        string value = int_to_string(arr[i]);
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
    string input = readStr();
    int n = str_to_int(input);
    free(input);
    print("\n");
    int i = 0;
    for (i = 0; i < n; i++)
    {
        print("Fibo ");
        string index = int_to_string(i);
        print(index);
        free(index);
        print(" : ");
        string value = int_to_string(fibo(i));
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
    string input = readStr();
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
    string result = int_to_string(matrix[n - 1][0]);
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
            string value = int_to_string(matrix[i][j]);
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
    string text_input = readStr();
    int text_color = str_to_int(text_input);
    free(text_input);
    print("\n\nBackground color ? : ");
    string bg_input = readStr();
    int bg_color = str_to_int(bg_input);
    free(bg_input);
    set_screen_color(text_color, bg_color);
    clearScreen();
}

void multiply()
{
    print("\nNum 1 :");
    string input1 = readStr();
    int num1 = str_to_int(input1);
    free(input1);
    print("\nNum 2 :");
    string input2 = readStr();
    int num2 = str_to_int(input2);
    free(input2);
    print("\nResult : ");
    string result = int_to_string(num1 * num2);
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
    print("\nmkdir     : Creates a new directory");
    print("\nmkf       : Creates a new empty file");
    print("\nls        : Lists the contents of the current directory");
    print("\nedit      : Opens a text editor for a file");
    print("\ncd        : Changes the current directory");
    print("\n\n");
}
