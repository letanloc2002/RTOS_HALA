#include <stdio.h>
#include "host_lib.h"
// Gui du lieu toi cong COM

int main()
{
    // const char *file_path = "C:/Users/Loc/OneDrive/Desktop/hala_rtos.hex"; // Thay thế bằng đường dẫn thực tế
    char file_path[MAX_PATH_LEN] = {0};
    const char *search_dir = "C:\\Users\\Loc\\OneDrive\\Desktop"; // Thư mục cần tìm kiếm
    const char *file_name = "hala_rtos.hex";

    if (find_file_recursive(search_dir, file_name, file_path))
    {
        printf("File found: %s\n", file_path);
    }
    else
    {
        printf("File not found %s in %s\n", file_name, search_dir);
    }
    size_t data_size = 0;
    uint8_t *hex_data = read_hex_file(file_path, &data_size);

    if (hex_data == NULL)
    {
        printf("Error read file HEX.\n");
        return 1;
    }

    printf("Data from HEX file (%zu bytes):\n", data_size);
    for (size_t i = 0; i < data_size; i++)
    {
        printf("%02X ", hex_data[i]); // In từng byte theo dạng HEX
        if ((i + 1) % 16 == 0)
            printf("\n"); // Xuống dòng sau mỗi 16 byte
    }
    printf("\n");
    // Truyen du lieu sang STM32
    /*char *command;
    do
    {
        printf("Flash Data(yes/no): ");
        scanf("%s", &command);

    } while (strcmp(command, "yes") != 0 || strcmp(command, "no") != 0);
    */
    listAvailablePorts();

    int com_index = 4; // Doi thanh cong COM cua ban
    HANDLE com_port = OpenPort(com_index, 9600, 8, 0, 0);
    if (com_port == INVALID_HANDLE_VALUE)
    {
        return 1;
    }
    // Tạo một khung dữ liệu
    Data_frame frame;
    frame.Start_Of_Frame = 0x7E;
    frame.Type = 0x01;
    frame.Data_Lenght = 16;
    for (int i = 0; i < 16; i++)
    {
        frame.Data[i] = 1;
    }

    frame.CRC32 = 0x12345678;
    frame.End_Of_Frame = 0x7E;
    // Gửi dữ liệu
    int re = SendData(com_port, &frame);

    free(hex_data); // Giải phóng bộ nhớ sau khi sử dụng
    return 0;
}