#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Hàm tính checksum của một dòng HEX
uint8_t calculate_checksum(const char *line)
{
    uint8_t checksum = 0;
    int len = strlen(line);
    for (int i = 1; i < len; i += 2)
    {
        char byte_str[3] = {line[i], line[i + 1], '\0'};
        uint8_t byte = (uint8_t)strtol(byte_str, NULL, 16);
        checksum += byte;
    }
    return (uint8_t)(~checksum + 1);
}

// Hàm kiểm tra checksum
int verify_checksum(const char *line)
{
    uint8_t sum = 0;
    int len = strlen(line);
    for (int i = 1; i < len; i += 2)
    {
        char byte_str[3] = {line[i], line[i + 1], '\0'};
        uint8_t byte = (uint8_t)strtol(byte_str, NULL, 16);
        sum += byte;
    }
    return (sum == 0); // Tổng các byte (bao gồm checksum) phải bằng 0
}

// Hàm đọc và in ra từng dòng dữ liệu trong file HEX
void read_hex_file(const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        perror("Không thể mở file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        // Loại bỏ ký tự xuống dòng (nếu có)
        line[strcspn(line, "\r\n")] = '\0';

        // Kiểm tra xem dòng có bắt đầu bằng ':' không
        if (line[0] != ':')
        {
            continue; // Bỏ qua dòng không hợp lệ
        }

        // Kiểm tra checksum
        if (!verify_checksum(line))
        {
            printf("Checksum không hợp lệ trong dòng: %s\n", line);
            continue;
        }

        // Đọc loại bản ghi
        char record_type_str[3] = {line[7], line[8], '\0'};
        uint8_t record_type = (uint8_t)strtol(record_type_str, NULL, 16);

        // Chỉ xử lý các bản ghi dữ liệu (Record Type = 00)
        if (record_type != 0x00)
        {
            continue;
        }

        // Đọc độ dài dữ liệu
        char data_length_str[3] = {line[1], line[2], '\0'};
        uint8_t data_length = (uint8_t)strtol(data_length_str, NULL, 16);

        // Đọc địa chỉ
        char address_str[5] = {line[3], line[4], line[5], line[6], '\0'};
        uint16_t address = (uint16_t)strtol(address_str, NULL, 16);

        // Đọc dữ liệu
        char data[512] = {0};
        for (int i = 0; i < data_length * 2; i += 2)
        {
            data[i / 2] = (char)strtol((char[]){line[9 + i], line[10 + i], '\0'}, NULL, 16);
        }

        // In ra dữ liệu
        printf("Address: %04X, Data: ", address);
        for (int i = 0; i < data_length; i++)
        {
            printf("%02X ", (uint8_t)data[i]);
        }
        printf("\n");
    }

    fclose(file);
}

int main()
{
    // Đường dẫn đến file 1.hex trên desktop
    const char *file_path = "C:/Users/Loc/OneDrive/Desktop/1.hex"; // Thay thế bằng đường dẫn thực tế

    // Gọi hàm đọc file HEX
    read_hex_file(file_path);

    return 0;
}