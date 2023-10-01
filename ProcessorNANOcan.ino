#include <SPI.h>
#include <mcp_can.h>
#include <avr/io.h>
#include <EEPROM.h>
#include <TaskScheduler.h>
#include <Wire.h>
#include <iarduino_I2C_connect.h>

#define CAN_BUFFER_SIZE 4
MCP_CAN CAN(10);  // Создаем объект CAN и задаем CS пин как 53

Scheduler ts;

iarduino_I2C_connect I2C2;

const int TOTAL_ANALOG_PINS = 6;

int analogPins[TOTAL_ANALOG_PINS];

volatile uint16_t adcBuff[TOTAL_ANALOG_PINS];

int16_t adc0, adc1, adc2, adc3, adc4, adc5;

byte           i2cdata[12];              // объявляем массив, данные которого будут доступны для чтения/записи по шине I2C


void initializeADCs() {
   
    adc0 = (adcBuff[0] > 0) ? adcBuff[0] : 0;
    adc1 = (adcBuff[1] > 0) ? adcBuff[1] : 0;
    adc2 = (adcBuff[2] > 0) ? adcBuff[2] : 0;
    adc3 = (adcBuff[3] > 0) ? adcBuff[3] : 0;
    adc4 = (adcBuff[6]> 0) ? adcBuff[6]: 0;
    adc5 = (adcBuff[7]> 0) ? adcBuff[7]: 0;

}

void updateadcBuffAndSave() {
  
  for (int i = 0; i < TOTAL_ANALOG_PINS; i++) {
    adcBuff[i] = analogPins[i];
 
  }
  //saveAnalogStatesToEEPROM();
}

struct canMsg {
    long unsigned int can_id;  // CAN id
    unsigned char can_dlc;     // Data length
    unsigned char data[8];     // Data
} canMsg1, canMsg2;
byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

struct CanBuffer {
    struct canMsg messages[CAN_BUFFER_SIZE];
    uint8_t head = 0;  // указатель на первое свободное место
    uint8_t tail = 0;  // указатель на первое сообщение для отправки
} canBuffer;

bool addToCanBuffer(const struct canMsg& message) {
    uint8_t nextHead = (canBuffer.head + 1) % CAN_BUFFER_SIZE;
    if (nextHead == canBuffer.tail) {
        // Буфер переполнен, сообщение не добавлено
        return false;
    }
    canBuffer.messages[canBuffer.head] = message;
    canBuffer.head = nextHead;
    return true;
}

bool removeFromCanBuffer(struct canMsg& message) {
    if (canBuffer.tail == canBuffer.head) {
        // Буфер пуст, нет сообщений для извлечения
        return false;
    }
    message = canBuffer.messages[canBuffer.tail];
    canBuffer.tail = (canBuffer.tail + 1) % CAN_BUFFER_SIZE;
    return true;
}

void sendBufferedCanMessages() {
    struct canMsg messages;
    while (removeFromCanBuffer(messages)) {
        CAN.sendMsgBuf(messages.can_id, 0, messages.can_dlc, messages.data);  // Изменили на новую функцию
    }
}

void ReadAnalogStatuses() {
  for (int i = 0; i < TOTAL_ANALOG_PINS; i++) {
    analogPins[i] = analogRead(i);   
  }  
 
updateadcBuffAndSave();   

initializeADCs(); // Здесь вызываем функцию инициализации переменных adcX  
  
}

void sendCanMessagesTask() {
  
    canMsg1.can_dlc = 8;
    canMsg1.can_id = 0x690;
    canMsg1.data[0] = adc0 >> 8;
    canMsg1.data[1] = adc0 & 0xFF;
    canMsg1.data[2] = adc1 >> 8;
    canMsg1.data[3] = adc1 & 0xFF;
    canMsg1.data[4] = adc2 >> 8;
    canMsg1.data[5] = adc2 & 0xFF;
    canMsg1.data[6] = adc3 >> 8;
    canMsg1.data[7] = adc3 & 0xFF;
    
    addToCanBuffer(canMsg1);

// Заполняем данные для CAN сообщения 2

    canMsg2.can_dlc = 8;
    canMsg2.can_id = 0x691;
    canMsg2.data[0] = adc4 >> 8;
    canMsg2.data[1] = adc4 & 0xFF;
    canMsg2.data[2] = adc5 >> 8;
    canMsg2.data[3] = adc5 & 0xFF;
    canMsg2.data[4] = 0x00;
    canMsg2.data[5] = 0x00;
    canMsg2.data[6] = 0x00;
    canMsg2.data[7] = 0x00;

    addToCanBuffer(canMsg2); 
  
    sendBufferedCanMessages();

}

void adcsendTask() { 
  
  i2cdata[0]    = adc0>>8;      // Сохраняем старший байт значения adc0 в 1 byte i2cdata[12];
  i2cdata[1]    = adc0;         // Сохраняем младший байт значения adc0 в 2 byte i2cdata[12]; 
  i2cdata[2]    = adc1>>8;      // Сохраняем старший байт значения adc1 в 3 byte i2cdata[12];
  i2cdata[3]    = adc1;         // Сохраняем младший байт значения adc1 в 4 byte i2cdata[12];
  i2cdata[4]    = adc2>>8;      // Сохраняем старший байт значения adc2 в 5 byte i2cdata[12];
  i2cdata[5]    = adc2;         // Сохраняем младший байт значения adc2 в 6 byte i2cdata[12];
  i2cdata[6]    = adc3>>8;      // Сохраняем старший байт значения adc3 в 7 byte i2cdata[12];
  i2cdata[7]    = adc3;         // Сохраняем младший байт значения adc3 в 8 byte i2cdata[12]; 
  i2cdata[8]    = adc4>>8;      // Сохраняем старший байт значения adc4 в 9 byte i2cdata[12];
  i2cdata[9]    = adc4;         // Сохраняем младший байт значения adc4 в 10 byte i2cdata[12];
  i2cdata[10]   = adc5>>8;      // Сохраняем старший байт значения adc5 в 11 byte i2cdata[12];
  i2cdata[11]   = adc5;         // Сохраняем младший байт значения adc5 в 12 byte i2cdata[12];

}

Task t1(10, TASK_FOREVER, &ReadAnalogStatuses, &ts, true);
Task t2(10, TASK_FOREVER, &sendCanMessagesTask, &ts, true);
Task t3(10, TASK_FOREVER, &adcsendTask, &ts, true);

void setup() {

 // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK);
  CAN.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
  
  SPI.begin(); 
  Wire.begin(0x02);                          // инициируем подключение к шине I2C в качестве ведомого (slave) устройства, с указанием своего адреса на шине.
  I2C2.begin(i2cdata);                       // инициируем возможность чтения/записи данных по шине I2C, из/в указываемый массив
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);

}

void loop() {

ts.execute();  

}
