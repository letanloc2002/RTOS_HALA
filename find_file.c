#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_PATH_LEN 512

// Hàm tìm file trong thư mục và thư mục con trên Windows
int find_file_recursive(const char *directory, const char *filename, char *result_path)
{
    char search_path[MAX_PATH_LEN];
    snprintf(search_path, MAX_PATH_LEN, "%s\\*", directory);

    WIN32_FIND_DATA find_data;
    HANDLE hFind = FindFirstFile(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE)
        return 0;

    do
    {
        // Bỏ qua "." và ".."
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
            continue;

        char full_path[MAX_PATH_LEN];
        snprintf(full_path, sizeof(full_path), "%s\\%s", directory, find_data.cFileName);

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Nếu là thư mục, tìm tiếp
        {
            if (find_file_recursive(full_path, filename, result_path))
            {
                FindClose(hFind);
                return 1;
            }
        }
        else if (strcmp(find_data.cFileName, filename) == 0) // Nếu tìm thấy file
        {
            strcpy(result_path, full_path);
            FindClose(hFind);
            return 1;
        }
    } while (FindNextFile(hFind, &find_data));

    FindClose(hFind);
    return 0;
}

int main()
{
    char file_path[MAX_PATH_LEN] = {0};
    const char *search_dir = "C:\\Users"; // Thư mục cần tìm kiếm
    const char *file_name = "hala_rtos.hex";

    if (find_file_recursive(search_dir, file_name, file_path))
    {
        printf("Tệp tìm thấy: %s\n", file_path);
    }
    else
    {
        printf("Không tìm thấy tệp %s trong %s\n", file_name, search_dir);
    }

    return 0;
}
