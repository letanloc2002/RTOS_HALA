#include <stdio.h>
#include "host_lib.h"

int main()
{
    // const char *file_path = "C:/Users/Loc/OneDrive/Desktop/hala_rtos.hex"; // Thay thế bằng đường dẫn thực tế
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
    size_t data_size = 0;
    uint8_t *hex_data = read_hex_file(file_path, &data_size);

    if (hex_data == NULL)
    {
        printf("Lỗi khi đọc file HEX.\n");
        return 1;
    }

    printf("Dữ liệu từ file HEX (%zu bytes):\n", data_size);
    for (size_t i = 0; i < data_size; i++)
    {
        printf("%02X ", hex_data[i]); // In từng byte theo dạng HEX
        if ((i + 1) % 16 == 0)
            printf("\n"); // Xuống dòng sau mỗi 16 byte
    }
    printf("\n");

    free(hex_data); // Giải phóng bộ nhớ sau khi sử dụng
    return 0;
}