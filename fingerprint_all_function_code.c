#include "stm32f4xx_hal.h"

// Enums for command and acknowledgment
typedef enum {
    SUCCESS = 0x00,
    FAIL = 0x01,
    FULL = 0x04,
    NOUSER = 0x05,
    USER_EXIST = 0x07,
    TIMEOUT = 0x08,
    WRONG_FORMAT = 0x09,
    BREAK = 0x18,
    INVALID_PARAMETER = 0xB0,
    FINGER_IS_NOT_PRESSED = 0xB1,
    COMMAND_NO_SUPPORT = 0xB4,
    ENROLL_OVEREXPOSURE = 0xB5,
    ENROLL_DUPLICATE = 0xB8,
    FINGER_PRESS_NOT_FULL = 0xB9,
    ENROLL_POOR_QUALITY = 0xBA,
} ACK;

typedef enum {
    OPEN = 0xA0,
    CLOSE = 0xA1,
    LED_CONTROL = 0xB4,
    IS_FINGER_PRESSED = 0xB5,
    ENROLL = 0x01,
    DELETE_ID = 0x04,
    DELETE_All = 0x05,
    GEt_USER_COUNT = 0x09,
    Identify = 0x0C,
    GET_ENTRY_ID = 0x0D,
} CMD;

typedef struct {
    uint8_t start_byte;
    CMD command;
    uint8_t parameter[4];
    uint8_t checksum;
    uint8_t end_byte;
} Packet;

// UART handle definitions
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);

void send_command(UART_HandleTypeDef *huart, packet *pkt);
uint8_t calculate_checksum(packet *pkt);

void Open_Fingerprint_Module(void);
void Close_Fingerprint_Module(void);
void LED_Control_Fingerprint_Module(uint8_t state);
uint8_t Is_Finger_Pressed(void);
void Enroll_Fingerprint(uint8_t ID);
void Delete_Fingerprint_ID(uint8_t ID);
void Delete_All_Fingerprints(void);
uint8_t Get_User_Count(void);
uint8_t Identify_Fingerprint(void);
uint8_t Get_Entry_ID(void);
void Enroll_Cancel(void);

void send_command(UART_HandleTypeDef *huart, Packet *pkt) {
    pkt->checksum = calculate_checksum(pkt);
    HAL_UART_Transmit(huart, (uint8_t*)pkt, sizeof(Packet), HAL_MAX_DELAY);
}

uint8_t calculate_checksum(Packet *pkt) {
    uint16_t sum = pkt->command;
    for (int i = 0; i < 4; i++) {
        sum += pkt->parameter[i];
    }
    return (uint8_t)sum;
}

void Open_Fingerprint_Module(void) {
    packet open_cmd = {0xF5, OPEN, {0, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &open_cmd);
}

void Close_Fingerprint_Module(void) {
    Packet close_cmd = {0xF5, CLOSE, {0, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &close_cmd);
}

void LED_Control_Fingerprint_Module(uint8_t state) {
    Packet led_cmd = {0xF5, LED_CONTROL, {state, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &led_cmd);
}

uint8_t Is_Finger_Pressed(void) {
    Packet is_press_cmd = {0xF5, IS_FINGER_PRESSED, {0, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &is_press_cmd);
    uint8_t response[8];
    HAL_UART_Receive(&huart5, response, 8, HAL_MAX_DELAY);
    return response[4];
}

void Enroll_Fingerprint(uint8_t ID) {
    Packet enroll_cmd = {0xF5, ENROLL, {ID, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &enroll_cmd);
}

void Delete_Fingerprint_ID(uint8_t ID) {
    Packet delete_cmd = {0xF5, DELETE_ID, {ID, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &delete_cmd);
}

void Delete_All_Fingerprints(void) {
    Packet delete_all_cmd = {0xF5, DELETE_All, {0, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &delete_all_cmd);
}

uint8_t Get_User_Count(void) {
    Packet user_count_cmd = {0xF5, GET_USER_COUNT, {0, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &user_count_cmd);
    uint8_t response[8];
    HAL_UART_Receive(&huart5, response, 8, HAL_MAX_DELAY);
    return response[4];
}

uint8_t Identify_Fingerprint(void) {
    Packet identify_cmd = {0xF5, IDENTIFY, {0, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &identify_cmd);
    uint8_t response[8];
    HAL_UART_Receive(&huart5, response, 8, HAL_MAX_DELAY);
    return response[4];
}

uint8_t Get_Entry_ID(void) {
    Packet get_entry_id_cmd = {0xF5, GET_ENTRY_ID, {0, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &get_entry_id_cmd);
    uint8_t response[8];
    HAL_UART_Receive(&huart5, response, 8, HAL_MAX_DELAY);
    return response[4];
}

void Enroll_Cancel(void) {
    Packet enroll_cancel_cmd = {0xF5, ENROLL_CANCEL, {0, 0, 0, 0}, 0, 0xF5};
    send_command(&huart5, &enroll_cancel_cmd);
}
