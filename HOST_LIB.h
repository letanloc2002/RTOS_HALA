#ifndef HOST_LIB_H
#define HOST_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <stdbool.h>

#define MAX_PATH_LEN 512

// Hàm đếm số dòng trong file để cấp phát động
size_t count_lines(const char *file_path);
// Hàm kiểm tra checksum
bool verify_checksum(const char *line);
// Hàm đọc file HEX với cấp phát động và in ra kết quả theo yêu cầu
uint8_t *read_hex_file(const char *file_path, size_t *data_size);
// Hàm tìm file trong thư mục và thư mục con trên Windows
int find_file_recursive(const char *directory, const char *filename, char *result_path);
// Hàm liệt kê các cổng COM có sẵn
void listAvailablePorts(void);
// Hàm mở cổng COM với các tham số UART
HANDLE OpenPort(int idx, DWORD baudRate, BYTE byteSize, BYTE parity, BYTE stopBits);
// Ham dong cong COM
void ClosePort(HANDLE com_port);
// Gui du lieu toi cong COM
int SendData(HANDLE com_port, const char *data);
// Nhan du lieu tu cong COM
int ReceiveData(HANDLE com_port, uint8_t *buffer, int len);

#endif