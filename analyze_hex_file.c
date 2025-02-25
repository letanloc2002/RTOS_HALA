#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Hàm đếm số dòng trong file để cấp phát động
size_t count_lines(const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        perror("Không thể mở file");
        return 0;
    }

    size_t lines = 0;
    char temp[512];

    while (fgets(temp, sizeof(temp), file))
    {
        // Loại bỏ ký tự xuống dòng
        temp[strcspn(temp, "\r\n")] = '\0';

        // Chỉ đếm dòng bắt đầu bằng ':'
        if (temp[0] == ':')
        {
            lines++;
        }
    }

    fclose(file);
    return lines;
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

// Hàm đọc file HEX với cấp phát động và in ra kết quả theo yêu cầu
void read_hex_file(const char *file_path)
{
    size_t line_count = count_lines(file_path);
    if (line_count == 0)
    {
        printf("File rỗng hoặc không thể đọc.\n");
        return;
    }

    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        perror("Không thể mở file");
        return;
    }

    // Cấp phát động cho từng dòng HEX
    char **lines = (char **)malloc(line_count * sizeof(char *));
    if (!lines)
    {
        perror("Lỗi cấp phát bộ nhớ");
        fclose(file);
        return;
    }

    // Đọc từng dòng và lưu vào bộ nhớ động
    size_t index = 0;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), file) && index < line_count)
    {
        buffer[strcspn(buffer, "\r\n")] = '\0'; // Loại bỏ ký tự xuống dòng
        lines[index] = strdup(buffer);          // Sao chép dòng vào bộ nhớ động
        if (!lines[index])
        {
            perror("Lỗi cấp phát bộ nhớ cho dòng");
            fclose(file);
            return;
        }
        index++;
    }
    fclose(file);

    // Xử lý từng dòng HEX
    for (size_t i = 0; i < line_count; i++)
    {
        char *line = lines[i];

        if (line[0] != ':')
            continue; // Bỏ qua dòng không hợp lệ
        if (!verify_checksum(line))
            continue; // Bỏ qua dòng có checksum không hợp lệ

        // Đọc loại bản ghi
        char record_type_str[3] = {line[7], line[8], '\0'};
        uint8_t record_type = (uint8_t)strtol(record_type_str, NULL, 16);
        if (record_type != 0x00)
            continue; // Chỉ xử lý dữ liệu

        // Đọc độ dài dữ liệu
        char data_length_str[3] = {line[1], line[2], '\0'};
        uint8_t data_length = (uint8_t)strtol(data_length_str, NULL, 16);

        // Cấp phát động cho dữ liệu
        char *data = (char *)malloc(data_length);
        if (!data)
        {
            perror("Lỗi cấp phát bộ nhớ cho dữ liệu");
            continue;
        }

        // Đọc dữ liệu từ dòng HEX
        for (int j = 0; j < data_length * 2; j += 2)
        {
            data[j / 2] = (char)strtol((char[]){line[9 + j], line[10 + j], '\0'}, NULL, 16);
        }

        // In dữ liệu ra màn hình
        for (int j = 0; j < data_length; j++)
        {
            printf("%02X", (uint8_t)data[j]);
        }
        printf("\n"); // Xuống dòng sau mỗi đoạn dữ liệu

        free(data); // Giải phóng bộ nhớ của dữ liệu
    }

    // Giải phóng bộ nhớ của từng dòng
    for (size_t i = 0; i < line_count; i++)
    {
        free(lines[i]);
    }
    free(lines); // Giải phóng mảng con trỏ
}

int main()
{
    //
    const char *file_path = "C:/Users/Loc/OneDrive/Desktop/1.hex"; // Thay thế bằng đường dẫn thực tế
    read_hex_file(file_path);
    return 0;
}
