#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <tchar.h>

#define MAX_PATH_LEN 512
#define SOF 0xAA
#define EOF 0xBB

typedef enum
{
    Packet_type_CMD;
    Packet_type_DATA;
    Packet_type_HEADER;
    Packet_type_RESPONSE;
}
Packet_type;

// Hàm đếm số dòng trong file để cấp phát động
size_t
count_lines(const char *file_path)
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
uint8_t *read_hex_file(const char *file_path, size_t *data_size)
{
    *data_size = 0;
    size_t line_count = count_lines(file_path);

    if (line_count == 0)
    {
        printf("File rỗng hoặc không thể đọc.\n");
        return NULL;
    }

    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        perror("Không thể mở file");
        return NULL;
    }

    uint8_t *hex_data = NULL;
    size_t total_size = 0;

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), file))
    {
        buffer[strcspn(buffer, "\r\n")] = '\0'; // Loại bỏ ký tự xuống dòng

        if (buffer[0] != ':')
            continue; // Bỏ qua dòng không hợp lệ
        if (!verify_checksum(buffer))
            continue; // Bỏ qua dòng có checksum không hợp lệ

        // Đọc loại bản ghi
        char record_type_str[3] = {buffer[7], buffer[8], '\0'};
        uint8_t record_type = (uint8_t)strtol(record_type_str, NULL, 16);
        if (record_type != 0x00)
            continue; // Chỉ xử lý dữ liệu

        // Đọc độ dài dữ liệu
        char data_length_str[3] = {buffer[1], buffer[2], '\0'};
        uint8_t data_length = (uint8_t)strtol(data_length_str, NULL, 16);

        // Cấp phát động cho dữ liệu mới
        uint8_t *new_data = realloc(hex_data, total_size + data_length);
        if (!new_data)
        {
            perror("Lỗi cấp phát bộ nhớ");
            free(hex_data);
            fclose(file);
            return NULL;
        }
        hex_data = new_data;

        // Đọc dữ liệu từ dòng HEX
        for (int j = 0; j < data_length * 2; j += 2)
        {
            hex_data[total_size + (j / 2)] = (uint8_t)strtol((char[]){buffer[9 + j], buffer[10 + j], '\0'}, NULL, 16);
        }

        total_size += data_length;
    }
    fclose(file);
    *data_size = total_size;
    return hex_data;
}

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

void listAvailablePorts(void)
{
    printf("Available COM Ports:\n");
    for (int i = 1; i <= 255; i++)
    {
        char comPortName[10];
        sprintf(comPortName, "COM%d", i);

        HANDLE hPort = CreateFileA(
            comPortName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (hPort != INVALID_HANDLE_VALUE)
        {
            printf("%s is available\n", comPortName);
            CloseHandle(hPort);
        }
    }
}

// Ham mo cong COM
// Hàm mở cổng COM với các tham số UART
HANDLE OpenPort(int idx, DWORD baudRate, BYTE byteSize, BYTE parity, BYTE stopBits)
{
    HANDLE hComm;
    TCHAR comname[100];
    wsprintf(comname, TEXT("\\\\.\\COM%d"), idx);
    hComm = CreateFile(comname,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);

    if (hComm == INVALID_HANDLE_VALUE)
    {
        printf("Khong the mo cong COM%d\n", idx);
        return INVALID_HANDLE_VALUE;
    }

    // Cấu hình UART
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hComm, &dcbSerialParams))
    {
        printf("Khong the lay trang thai tu cong COM.\n");
        CloseHandle(hComm);
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = baudRate; // Baud rate (VD: 9600, 115200)
    dcbSerialParams.ByteSize = byteSize; // Số bit dữ liệu (5, 6, 7, 8)
    dcbSerialParams.Parity = parity;     // Parity (0=None, 1=Odd, 2=Even)
    dcbSerialParams.StopBits = stopBits; // Stop bit (0=1 bit, 1=1.5 bit, 2=2 bit)

    if (!SetCommState(hComm, &dcbSerialParams))
    {
        printf("Khong the thiet lap cau hinh UART.\n");
        CloseHandle(hComm);
        return INVALID_HANDLE_VALUE;
    }

    // Thiết lập timeout
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hComm, &timeouts))
    {
        printf("Khong the thiet lap thoi gian cho.\n");
        CloseHandle(hComm);
        return INVALID_HANDLE_VALUE;
    }

    printf("Da mo cong COM%d voi BaudRate=%d, ByteSize=%d, Parity=%d, StopBits=%d thanh cong.\n",
           idx, baudRate, byteSize, parity, stopBits);

    return hComm;
}

// Ham dong cong COM
void ClosePort(HANDLE com_port)
{
    if (com_port != INVALID_HANDLE_VALUE)
    {
        CloseHandle(com_port);
        printf("Da dong cong COM.\n");
    }
}

// Gui du lieu toi cong COM
int SendData(HANDLE com_port, const char *data)
{
    DWORD dNoOFBytestoWrite = strlen(data);
    DWORD dNoOfBytesWritten;
    if (!WriteFile(com_port, data, dNoOFBytestoWrite, &dNoOfBytesWritten, NULL))
    {
        printf("Loi khi gui du lieu.\n");
        return -1;
    }
    printf("Da gui %d byte du lieu.\n", dNoOfBytesWritten);
    return dNoOfBytesWritten;
}

// Nhan du lieu tu cong COM
int ReceiveData(HANDLE com_port, uint8_t *buffer, int len)
{
    DWORD dwEventMask;
    DWORD NoBytesRead;

    // Đợi sự kiện từ cổng COM
    if (!WaitCommEvent(com_port, &dwEventMask, NULL))
    {
        printf("Khong the cho su kien.\n");
        return -1;
    }

    // Đọc dữ liệu từ cổng COM
    if (!ReadFile(com_port, buffer, len, &NoBytesRead, NULL))
    {
        printf("Loi khi doc du lieu.\n");
        return -1;
    }

    // printf("Da nhan duoc %d byte: ", NoBytesRead);
    printf("\n");
    for (DWORD i = 0; i < NoBytesRead; i++)
    {
        printf("%c", buffer[i]); // Xuất dữ liệu dưới dạng hex
    }

    return NoBytesRead;
}
typedef struct OTA_Data_Format
{
    uint8_t Start_of_Frame;
    uint8_t Packet_Type;
    uint16_t Data_Lenght;
    uint32_t Data[4];
    uint32_t CRC32;
    uint8_t End_of_Frame;
} OTA_Data_Format;
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
