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

// Hàm đọc và phân tích file HEX
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
            printf("Dòng không hợp lệ: %s\n", line);
            continue;
        }

        // Đọc độ dài dữ liệu
        char data_length_str[3] = {line[1], line[2], '\0'};
        uint8_t data_length = (uint8_t)strtol(data_length_str, NULL, 16);

        // Đọc địa chỉ
        char address_str[5] = {line[3], line[4], line[5], line[6], '\0'};
        uint16_t address = (uint16_t)strtol(address_str, NULL, 16);

        // Đọc loại bản ghi
        char record_type_str[3] = {line[7], line[8], '\0'};
        uint8_t record_type = (uint8_t)strtol(record_type_str, NULL, 16);

        // Đọc dữ liệu
        char data[512] = {0};
        for (int i = 0; i < data_length * 2; i += 2)
        {
            data[i / 2] = (char)strtol((char[]){line[9 + i], line[10 + i], '\0'}, NULL, 16);
        }

        // Đọc checksum
        char checksum_str[3] = {line[9 + data_length * 2], line[10 + data_length * 2], '\0'};
        uint8_t checksum = (uint8_t)strtol(checksum_str, NULL, 16);

        // Tính toán checksum và kiểm tra
        uint8_t calculated_checksum = calculate_checksum(line);
        if (calculated_checksum != checksum)
        {
            printf("Checksum không hợp lệ trong dòng: %s\n", line);
        }

        // In thông tin bản ghi
        printf("Record Type: %02X, Address: %04X, Data Length: %02X, Data: ", record_type, address, data_length);
        for (int i = 0; i < data_length; i++)
        {
            printf("%02X ", (uint8_t)data[i]);
        }
        printf(", Checksum: %02X\n", checksum);
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Su dung: %s <file.hex>\n", argv[0]);
        return 1;
    }

    const char *file_path = argv[1];
    read_hex_file(file_path);

    return 0;
}