#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <stdint.h>

// Hàm liệt kê các cổng COM có sẵn
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

int main()
{

    listAvailablePorts();

    int com_index = 4; // Doi thanh cong COM cua ban
    HANDLE com_port = OpenPort(com_index, 9600, 8, 0, 0);
    if (com_port == INVALID_HANDLE_VALUE)
    {
        return 1;
    }

    // Gui du lieu toi thiet bi
    const char *data_to_send = "Hello, STM32!\r\n";
    if (SendData(com_port, data_to_send) == -1)
    {
        ClosePort(com_port);
        return 1;
    }

    // // Nhan du lieu tu thiet bi
    // char read_buffer[100];
    // while (1) {
    //     if (ReceiveData(com_port, read_buffer, sizeof(read_buffer) - 1) == -1) {
    //         ClosePort(com_port);
    //         return 1;
    //     }
    // }

    // Dong cong COM
    ClosePort(com_port);
    return 0;
}

// list cong com (com1 com2 com3)
// open com 1 cau hinh
// usb to uart pc-stm