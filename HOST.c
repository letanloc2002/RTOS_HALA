#include "HOST_  LIB.h"
#define MAX_PATH_LEN 512
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