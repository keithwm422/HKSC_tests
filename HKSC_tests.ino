// try out multiple I2C twowire libs
#include "driverlib/uart.h"
#include "inc/hw_nvic.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inc/hw_i2c.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_gpio.h>
#include <inc/hw_ints.h>
#include <inc/hw_pwm.h>
#include <driverlib/i2c.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include <driverlib/interrupt.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/udma.h>
#include <driverlib/pwm.h>
#include <driverlib/ssi.h>
#include <driverlib/systick.h>
#include <driverlib/adc.h>
#include <string.h>
#include "Wire.h"
#define  WIRE_INTERFACES_COUNT 4

// declare 2 two wire objs but YOU ONLY NEED ONE! CHECK WHICH ONE TO USE THOROUGHLY.

/* How to Use

    Specify the temperature and the GPIO addresses

    For the GPIO:
    Enter "O" if you want the GPIO to be pulled high
    Enter "F" if you want the GPIO to be pulled low

    For the Temperature Sensor:
    Enter "R" for remote sensor
    Enter "L" for local sensor
*/


#define I2C_ADDRESS_GPIO 76 // Specify the GPIO address 
#define I2C_ADDRESS_TEMP 63 // Specify the Temp Sensor address

// COOL LED BRO
#define LED RED_LED
bool is_high = true;
//int LED=38;
unsigned long LED_time = 0;
#define LED_PERIOD 1000

/*****************/
//GPIO (and 1wire bridge) for RF ON which is I2C bus #2
#define RF_ON_GPIO_ADDR0 0x20
#define RF_ON_GPIO_ADDR1 0x21
#define RF_ON_GPIO_ADDR2 0x22
#define RF_ON_GPIO_ADDR3 0x23
#define RF_ON_1Wire_ADDR 0x18  //0 0 0 1 , 1 A2 A1 A0 -> all 0's so 0x18

unsigned long  i2c_bus_RF_ON = 2;
TwoWire *wire_RF_ON = new TwoWire(i2c_bus_RF_ON); // i2C object for the i2c port on the launchpad
uint8_t command_byte_write = 0x03;
uint8_t ports_to_write = 0xFF; //all high
/*****************/

/*****************/
//ADC for PWR CTRL which is I2C bus #1
#define PWR_CTRL_GPIO_ADDR0 0x20
#define PWR_CTRL_GPIO_ADDR1 0x21
#define PWR_CTRL_GPIO_ADDR2 0x22
#define PWR_CTRL_GPIO_ADDR3 0x23
unsigned long  i2c_bus_PWR_CTRL = 1;
TwoWire *wire_PWR_CTRL = new TwoWire(i2c_bus_PWR_CTRL); // i2C object for the i2c port on the launchpad
//uint8_t command_byte_write=0x03;
//uint8_t ports_to_write=0xFF;//all high
// need a struct per Patrick Allison
/*****************/

/*****************/
//GPIO for PWR ANA which is I2C bus #0
#define PWR_ANA_ADC_ADDR 0x14 // 0001,0100
#define PWR_ANA_DAC_ADDR 0x4C // 0100,1000 // AHHHH // change to floating now its 0 1001 100
#define PWR_ANA_MUX_ADDR_TOP 0x4A  // 0100,1010
#define PWR_ANA_MUX_ADDR0 0x48  // schematic says address 0x90, but that is 8 bit and we need 7bit addr: 1 0 0 1 0 A1 A0, so 1001000 is smallest addr -> 0x48
#define PWR_ANA_MUX_ADDR1 0x49  // 0100,1001 
#define PWR_ANA_MUX_ADDR2 0x4B  // 0100,1011
unsigned long  i2c_bus_PWR_ANA = 0;
uint8_t PWR_ANA_MUX[3]={PWR_ANA_MUX_ADDR0,PWR_ANA_MUX_ADDR1,PWR_ANA_MUX_ADDR2};
TwoWire *wire_PWR_ANA = new TwoWire(i2c_bus_PWR_ANA); // i2C object for the i2c port on the launchpad
uint16_t PWR_ANA_READS[25]={0}; // one extra because the last one is always the most recent read
uint16_t PWR_ANA_READS_0[25]={0}; // one extra because the last one is always the most recent read
uint8_t PWR_DAC_write[3]={0x2F,0xFF,0x00}; // after I2C address, need 3 bytes, where 1st byte is Command/access byte, and then the next two bytes are the data bytes
// upper nibble doesn't matter: X X X X , EN C2 C1 C0 -> EN is 0 means all off (won't matter what C2-C0 are). EN is 1, then S0-S7 corresponds to the bits of C2-C0 as 000->S0, 001->S1... 111->S7
//uint8_t command_byte_write=0x03;
//uint8_t ports_to_write=0xFF;//all high
#define ADC_CONST_B 32768
#define ADC_CONST_DX 1.25
#define ADC_CONST_DY ADC_CONST_B
float adc_const_m=ADC_CONST_DY/ADC_CONST_DX;

// need a struct per Patrick Allison
/*****************/

/*****************/
// EXT_ANA space is I2C for extra analog and some GPIO pins from launchpad
// PD2 (pin 25) is EXT ANA RESET pin on launchpad (for I2C bus switch)
#define EXT_ANA_I2C_MUX_ADDR 0x70 // 0 1 1 1 , 0 0 0(A1) 0(A0) A1=0, A0=0 // EXT_ANA goes to I2C switch, and I2C0 of that switch goes to ADC_3 (pin 25-48) of schema, I2C1 goes to ADC_2 (pin 1-24)
#define EXT_ANA_ADC_ADDR 0x14 // 0001,0100
#define EXT_ANA_MUX_ADDR_TOP 0x4A  // 0100,1010
#define EXT_ANA_MUX_ADDR0 0x48  // schematic says address 0x90, but that is 8 bit and we need 7bit addr: 1 0 0 1 0 A1 A0, so 1001000 is smallest addr -> 0x48
#define EXT_ANA_MUX_ADDR1 0x49  // 0100,1001 
#define EXT_ANA_MUX_ADDR2 0x4B  // 0100,1011
// analog pin (In) at connector to mux channel number (Jout) should follow: Jout=7-((In+8)%8); 
uint8_t EXT_ANA_MUX[3]={EXT_ANA_MUX_ADDR0,EXT_ANA_MUX_ADDR1,EXT_ANA_MUX_ADDR2};
unsigned long  i2c_bus_EXT_ANA = 3; // PD0 and PD1
TwoWire *wire_EXT_ANA = new TwoWire(i2c_bus_EXT_ANA); // i2C object for the i2c port on the launchpad
int EXT_ANA_RST_pin=25;  // really this is a NOT RST pin.
uint16_t EXT_ANA_READS[25]={0}; // one extra because the last one is always the most recent read
uint16_t EXT_ANA_READS_0[25]={0}; // one extra because the last one is always the most recent read
uint16_t EXT_ANA_READS_1[25]={0}; // one extra because the last one is always the most recent read

/*****************/

/*
   1. sends a command byte that is stored in the control register
   Two bits for state a read or write operation and internal register that is affected (input, output, pol inversion, or configuration)
   command byte is only sent during a write transmission
   2. Once a command byte has beent sent, the register that was addressed continues to be accerss by reads until new command byte has been sent
*/

/*****************/
//Serial7 is the CC_UART line


char one_byte;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Hello, starting...");
  Serial.print("\n");
  //wire_1->begin();
  wire_RF_ON->begin();
  wire_PWR_CTRL->begin();
  wire_PWR_ANA->begin();
  wire_EXT_ANA->begin();
  //LED blinky start
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  LED_time = millis();
  pinMode(EXT_ANA_RST_pin, OUTPUT);
  digitalWrite(EXT_ANA_RST_pin, HIGH);  // this is really NOT RST, so leave high to not RST....
  Serial2.begin(115200); // TURF UART
  
  Serial7.begin(115200); // Charge Controller UART
}

void loop() {
  while (Serial.available() > 0) {
    one_byte = Serial.read();
    Serial.print("\n");
    if (one_byte == 49) Serial.print("1\n");
    else if (one_byte == 50) { //2
      // put some code here to execute this command like: your_writefunction_name(0xFF);
      Serial.print("BAUD");
      Serial.print("2\n");
    }

    else if (one_byte == 51) { //3 --> Configure ports to be outputs
      //Serial.print(gpio_write(0x03, 0x00), DEC);
      //Serial.print(gpio_write(0x01, 0xFF), DEC);
      Serial.print(RF_ON_write(RF_ON_GPIO_ADDR0,0x03, 0x00), DEC);
      Serial.print(RF_ON_write(RF_ON_GPIO_ADDR1,0x03, 0x00), DEC);
      Serial.print(RF_ON_write(RF_ON_GPIO_ADDR2,0x03, 0x00), DEC);
      Serial.print(RF_ON_write(RF_ON_GPIO_ADDR3,0x03, 0x00), DEC);
      Serial.print(PWR_CTRL_write(PWR_CTRL_GPIO_ADDR0,0x03, 0x00), DEC);
      Serial.print(PWR_CTRL_write(PWR_CTRL_GPIO_ADDR1,0x03, 0x00), DEC);
      Serial.print(PWR_CTRL_write(PWR_CTRL_GPIO_ADDR2,0x03, 0x00), DEC);
      Serial.print(PWR_CTRL_write(PWR_CTRL_GPIO_ADDR3,0x03, 0x00), DEC);
      Serial.print("GPIO --> Configured\n");
    }

    else if (one_byte == 70) { //F --> All output ports set to high
      Serial.print(RF_ON_write(RF_ON_GPIO_ADDR0,0x01, 0xFF), DEC);
      Serial.print(RF_ON_write(RF_ON_GPIO_ADDR1,0x01, 0xFF), DEC);
      Serial.print(RF_ON_write(RF_ON_GPIO_ADDR2,0x01, 0xFF), DEC);
      Serial.print(RF_ON_write(RF_ON_GPIO_ADDR3,0x01, 0xFF), DEC);
      Serial.print(PWR_CTRL_write(PWR_CTRL_GPIO_ADDR0,0x01, 0xFF), DEC);
      Serial.print(PWR_CTRL_write(PWR_CTRL_GPIO_ADDR1,0x01, 0xFF), DEC);
      Serial.print(PWR_CTRL_write(PWR_CTRL_GPIO_ADDR2,0x01, 0xFF), DEC);
      Serial.print(PWR_CTRL_write(PWR_CTRL_GPIO_ADDR3,0x01, 0xFF), DEC);
      Serial.print("GPIO --> ON\n");
    }

    else if (one_byte == 82) { //R --> Alll output ports set to low
      RF_ON_write(RF_ON_GPIO_ADDR0,0x01, 0x00);
      RF_ON_write(RF_ON_GPIO_ADDR1,0x01, 0x00);
      RF_ON_write(RF_ON_GPIO_ADDR2,0x01, 0x00);
      RF_ON_write(RF_ON_GPIO_ADDR3,0x01, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR0,0x01, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR1,0x01, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR2,0x01, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR3,0x01, 0x00);
      Serial.print("GPIO --> Off\n");
    }
    
    else if (one_byte == 90) { //Z --> Cool blinkies
      //uint8_t addrs[4]={RF_ON_GPIO_ADDR0,RF_ON_GPIO_ADDR1,RF_ON_GPIO_ADDR2,RF_ON_GPIO_ADDR3};
      uint8_t addrs[4]={PWR_CTRL_GPIO_ADDR0,PWR_CTRL_GPIO_ADDR1,PWR_CTRL_GPIO_ADDR2,PWR_CTRL_GPIO_ADDR3};
      uint8_t writer=0x00;
      int i=0;
      while(i<4){
        PWR_CTRL_write(addrs[i],0x01, writer);
        //RF_ON_write(addrs[i],0x01, writer);
        writer+=1;
        delay(100);
        if(writer==0){
          i+=1;
        }
      }
      Serial.print("GPIO --> Fun done\n");
    }
    else if (one_byte == 72) { //H --> PWR ANA - ctrl_top_mux ch0 select
      Serial.println(PWR_ANA_mux_ctrl(0, PWR_ANA_MUX_ADDR_TOP), DEC); // this is to point to remote high byte
    }
    else if (one_byte == 73) { //I --> PWR ANA - ctrl_top_mux ch3 select
      Serial.println(PWR_ANA_mux_ctrl(3, PWR_ANA_MUX_ADDR_TOP), DEC); // this is to point to remote high byte
    }
    else if (one_byte == 74) { //I --> PWR ANA - ctrl_top_mux ch4 select
      Serial.println(PWR_ANA_mux_ctrl(4, PWR_ANA_MUX_ADDR_TOP), DEC); // this is to point to remote high byte
    }

    else if (one_byte == 71) { //G --> PWR ANA - just read value
      bool try_read=PWR_ANA_read(2,PWR_ANA_ADC_ADDR);
      if(try_read){
        Serial.println("no more loops");
      }
      else{
        Serial.println("loops");
      }
      Serial.print("\n");
    }

    else if (one_byte == 53) { //5 --> EXT ANA - ctrl I2C switch I2C0
      Serial.println(EXT_ANA_I2C_MUX_write(0x01), DEC); // this control register // X X NINT1 NINT0 X X I2C1 I2C0 so 0x01 enables I2C0 with I2C1 disabled.
      Serial.println(EXT_ANA_mux_ctrl(3, EXT_ANA_MUX_ADDR_TOP), BIN); // this is to point to remote high byte
      bool try_conv=EXT_ANA_read(0,EXT_ANA_ADC_ADDR);
      if(try_conv) Serial.println("read start");
      bool try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
      if(try_read){
        Serial.println("no more loops");
        Serial.println(EXT_ANA_READS[24],DEC);
      }
      else{
        Serial.println("loops");
      }
      Serial.print("\n");
    }

    else if (one_byte == 54) { //6 --> EXT ANA - ctrl I2C switch I2C1
      Serial.println(EXT_ANA_I2C_MUX_write(0x02), DEC); // this control register // X X NINT1 NINT0 X X I2C1 I2C0 so 0x02 enables I2C1 with I2C0 disabled.
      Serial.println(EXT_ANA_mux_ctrl(4, EXT_ANA_MUX_ADDR_TOP), DEC); // this is to point to remote high byte
      bool try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
      if(try_read){
        Serial.println("no more loops");
      }
      else{
        Serial.println("loops");
      }
      Serial.print("\n");
    }
    /*else if (one_byte == 57) { //9 --> EXT ANA - ctrl I2C switch I2C0 and read every channel on a MUX_x
      Serial.println(EXT_ANA_I2C_MUX_write(0x01), DEC); // this control register // X X NINT1 NINT0 X X I2C1 I2C0 so 0x01 enables I2C0 with I2C1 disabled.
      Serial.println(EXT_ANA_mux_ctrl(5, EXT_ANA_MUX_ADDR_TOP), BIN); // this sets the input of the mux (channel 7 is MUX_1 0 through 7, channel 6 is MUX_2 0 through 7, channel 5 is MUX_3 0 through 7). 
      // now set MUX_x to a channel...
      Serial.println(EXT_ANA_mux_ctrl(3, EXT_ANA_MUX_ADDR2), DEC);
      // now do an i2c read of ADC with no bytes requested, so that new conversion starts
      bool try_conv=EXT_ANA_read(0,EXT_ANA_ADC_ADDR);
      if(try_conv) Serial.println("read start");
      bool try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
      if(try_read){
        Serial.println("no more loops");
      }
      else{
        Serial.println("loops");
      }
      Serial.print("\n");
    }*/
    else if (one_byte == 56) { //8 --> EXT ANA - ctrl I2C switch I2C0 and read every channel on a MUX_x
      EXT_ANA_I2C_MUX_write(0x02); // this control register // X X NINT1 NINT0 X X I2C1 I2C0 so 0x01 enables I2C0 with I2C1 disabled. IC20-> pin 25-48, I2C1-> pin 1-24
      for(int ana=0;ana<=23;ana++){
        int chip_num=mux_IC_num(ana);
        int arg_chip_num=calc_mux_channel(chip_num);
        int mux_channel=calc_mux_channel(ana);
        EXT_ANA_mux_ctrl(chip_num, EXT_ANA_MUX_ADDR_TOP);
        EXT_ANA_mux_ctrl(mux_channel, EXT_ANA_MUX[arg_chip_num]);
        delay(20);
        bool try_conv=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
        //EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
        delay(20);
        bool try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);        
        //delay(20);
        //bool try_read2=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);        
        Serial.print(arg_chip_num,DEC);
        Serial.print("|");
        Serial.print(chip_num,DEC);
        Serial.print("|");
        Serial.print(ana,DEC);
        Serial.print("|");
        Serial.print(mux_channel,DEC);
        Serial.print("|");
        if(try_read){
          EXT_ANA_READS[ana]=EXT_ANA_READS[24];// last one is most recent?
          Serial.println(EXT_ANA_READS[ana]-32768,DEC);
        }
        else{
          // reads sucked so try again with same ana?
          ana--;
          Serial.println("-1");
          //try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
          //EXT_ANA_READS[ana]=EXT_ANA_READS[24];// last one is most recent?
          //Serial.println(EXT_ANA_READS[ana],DEC);
        }
      }
    }
    else if (one_byte == 57) { //9 --> EXT ANA - ctrl I2C switch I2C0 and read every channel then I2C1 and read every channel
      EXT_ANA_I2C_MUX_write(0x01); // this control register // X X NINT1 NINT0 X X I2C1 I2C0 so 0x01 enables I2C0 with I2C1 disabled. IC20-> pin 25-48, I2C1-> pin 1-24
      for(int ana=0;ana<=23;ana++){
        int chip_num=mux_IC_num(ana);
        int arg_chip_num=calc_mux_channel(chip_num);
        int mux_channel=calc_mux_channel(ana);
        EXT_ANA_mux_ctrl(chip_num, EXT_ANA_MUX_ADDR_TOP);
        EXT_ANA_mux_ctrl(mux_channel, EXT_ANA_MUX[arg_chip_num]);
        delay(20);
        bool try_conv=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
        //EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
        delay(20);
        bool try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);        
        if(try_read){
          EXT_ANA_READS[ana]=EXT_ANA_READS[24];// last one is most recent?
          //Serial.println(EXT_ANA_READS[ana]-32768,DEC);
        }
        else{
          // reads sucked so try again with same ana?
          ana--;
          Serial.println("-1");
          //try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
          //EXT_ANA_READS[ana]=EXT_ANA_READS[24];// last one is most recent?
          //Serial.println(EXT_ANA_READS[ana],DEC);
        }
      }
      // copy EXT_ANA_READS to appr storage
      for(int i=0; i<24; i++) {
        EXT_ANA_READS_0[i]=EXT_ANA_READS[i];
      }
      // now repeat for I2C1
      EXT_ANA_I2C_MUX_write(0x02); // this control register // X X NINT1 NINT0 X X I2C1 I2C0 so 0x01 enables I2C0 with I2C1 disabled. IC20-> pin 25-48, I2C1-> pin 1-24
      for(int ana=0;ana<=23;ana++){
        int chip_num=mux_IC_num(ana);
        int arg_chip_num=calc_mux_channel(chip_num);
        int mux_channel=calc_mux_channel(ana);
        EXT_ANA_mux_ctrl(chip_num, EXT_ANA_MUX_ADDR_TOP);
        EXT_ANA_mux_ctrl(mux_channel, EXT_ANA_MUX[arg_chip_num]);
        delay(20);
        bool try_conv=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
        //EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
        delay(20);
        bool try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);        
        if(try_read){
          EXT_ANA_READS[ana]=EXT_ANA_READS[24];// last one is most recent?
          //Serial.println(EXT_ANA_READS[ana]-32768,DEC);
        }
        else{
          // reads sucked so try again with same ana?
          ana--;
          Serial.println("-1");
          //try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
          //EXT_ANA_READS[ana]=EXT_ANA_READS[24];// last one is most recent?
          //Serial.println(EXT_ANA_READS[ana],DEC);
        }
      }
      // copy EXT_ANA_READS to appr storage
      for(int i=0; i<24; i++) {
        EXT_ANA_READS_1[i]=EXT_ANA_READS[i];
        Serial.print(EXT_ANA_READS_1[i]-32768,DEC);
        Serial.print(",");
      }
      for(int i=0; i<24; i++) {
        Serial.print(EXT_ANA_READS_0[i]-32768,DEC);
        Serial.print(",");
      }
      Serial.print("\n");
    }
    else if (one_byte == 65) { //A --> PWR ANA - read every channel on a MUX_x
      for(int ana=0;ana<=23;ana++){
        int chip_num=mux_IC_num(ana);
        int arg_chip_num=calc_mux_channel(chip_num);
        int mux_channel=calc_mux_channel(ana);
        PWR_ANA_mux_ctrl(chip_num, PWR_ANA_MUX_ADDR_TOP);
        PWR_ANA_mux_ctrl(mux_channel, PWR_ANA_MUX[arg_chip_num]);
        delay(20);
        bool try_conv=PWR_ANA_read(2,PWR_ANA_ADC_ADDR);
        //EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
        delay(20);
        bool try_read=PWR_ANA_read(2,PWR_ANA_ADC_ADDR);        
        //delay(20);
        //Serial.print(arg_chip_num,DEC);
        //Serial.print("|");
        //Serial.print(chip_num,DEC);
        //Serial.print("|");
        //Serial.print(ana+25,DEC);
        //Serial.print("|");
        //Serial.print(mux_channel,DEC);
        //Serial.print("|");
        if(try_read){
          PWR_ANA_READS[ana]=PWR_ANA_READS[24];// last one is most recent?
          //Serial.println(PWR_ANA_READS[ana]-32768,DEC);
        }
        else{
          // reads sucked so try again with same ana?
          ana--;
          Serial.println("-1");
          //try_read=EXT_ANA_read(2,EXT_ANA_ADC_ADDR);
          //EXT_ANA_READS[ana]=EXT_ANA_READS[24];// last one is most recent?
          //Serial.println(EXT_ANA_READS[ana],DEC);
        }
      }
      // copy EXT_ANA_READS to appr storage
      for(int i=0; i<24; i++) {
        PWR_ANA_READS_0[i]=PWR_ANA_READS[i];
        Serial.print(PWR_ANA_READS_0[i]-32768,DEC);
        Serial.print(",");
      }
    }
    else {
      Serial.print("Not a valid request\n");
    }
  }// end of commanding
  //delay(3000);
  if ((long) (millis() - LED_time) > 0) {
    LED_time = millis() + LED_PERIOD;
    switch_LED();
    Serial7.print("A\r");
    Serial2.print("A\r");
    
  }
  int ser7_val=Serial7.read();
  if(ser7_val != -1){
    Serial.print((char)ser7_val);
  }
  int ser2_val=Serial2.read();
  if(ser2_val != -1){
    Serial.print((char)ser2_val);
  }


}



uint8_t RF_ON_write(uint8_t addr_I, uint8_t to_write, uint8_t to_write_2) {
  wire_RF_ON->beginTransmission(addr_I);
  wire_RF_ON->write(to_write); // write to point at register
  wire_RF_ON->write(to_write_2); // write to point at register
  //wire_1->write(to_write); // then write 0xFF
  return wire_RF_ON->endTransmission();
}

uint8_t PWR_CTRL_write(uint8_t addr_I,uint8_t to_write, uint8_t to_write_2) {
  wire_PWR_CTRL->beginTransmission(addr_I);
  wire_PWR_CTRL->write(to_write); // write to point at register
  wire_PWR_CTRL->write(to_write_2); // write to point at register
  //wire_1->write(to_write); // then write 0xFF
  return wire_PWR_CTRL->endTransmission();
}

uint8_t PWR_ANA_mux_ctrl(int channel, uint8_t addr_mux){
  wire_PWR_ANA->beginTransmission(addr_mux);
  uint8_t to_write=muxconvert(channel);
  wire_PWR_ANA->write(to_write); // write to point at register
  return wire_PWR_ANA->endTransmission();
}

//change type so we can return when we are done at any point in this FN
bool PWR_ANA_read(uint8_t num_bytes, uint8_t temp_addr){
  bool buffer_clear=clear_buffer_with_timeout();
  // now do a request if buffer was clear  but the problem is if we requested 2 bytes, then we can't just wait for nonzero available, need to wait for available to be 2!
  if(buffer_clear){
    wire_PWR_ANA->requestFrom(temp_addr, num_bytes); // request 2 bytes from tmp441
    bool reads_avail=check_reads_avail_with_timeout(num_bytes);
    if(reads_avail){ // yes, we have right bytes to read so read them
      bool real_read_good=do_real_reads();
      if(real_read_good){
        //Serial.println("GOODREADS");
        return true;
      }
      else{
        Serial.println("loopy reads");
        return false;
      }
    }
    else{
      Serial.println("reads weren't available");
      return false;
    }
  }
  else {
    Serial.print("Buffer UNCLEAR");
    return false;
  }
}

bool do_real_reads(){
  int bytes_ready=(int) wire_PWR_ANA->available();
  int check_loop=0;
  uint8_t all_reads[2]={0};
  for(int read_iter=0;read_iter<bytes_ready;read_iter++){  
    uint8_t read_val = wire_PWR_ANA->read();
    all_reads[read_iter]=read_val;
    //Serial.print((uint8_t) read_val, HEX); // okay if this actually can print out as decimal we're good 
    //Serial.println((uint8_t) read_val, DEC);
    check_loop++;
    if(check_loop>100) return false;
  }
  float volts=ADC_converter(all_reads[0],all_reads[1]);
  PWR_ANA_READS[24]=((uint16_t) (all_reads[0]) << 8) | (uint16_t) (all_reads[1]);
  //Serial.println(volts,6);
  return true;
}

bool check_reads_avail_with_timeout(uint8_t bytes_total){
  int timeout_max = 10000;
  int iter_time=0;
  if (wire_PWR_ANA->available()== (int) (bytes_total)) return true; // if there the bytes are ready, just return true.
  else{
    while(iter_time < timeout_max) {
      if(wire_PWR_ANA->available()!= (int)(bytes_total)){ // check available again and if there is still more to wait for, then wait longer.
        iter_time++;
      }
      else{ // we got here because after waiting and checking, we found that avail had the right number of bytes waiting.
        iter_time=timeout_max+1; // kill the timeout in a funny way but really just return out of function at next line.
        return true;
      }
    }
    //if we got here, this should return false, because there was somehow a timeout after checking avail alot of times but the correct number of bytes wasn't ready
    return false;
  }
}

bool clear_buffer_with_timeout(){
  int timeout_max = 10000;
  int iter_time=0;
  if (wire_PWR_ANA->available()!=0){ // if there is something to read that wasn't requested, do it. This is called clearing the buffer
    while (iter_time < timeout_max) {
      wire_PWR_ANA->read();  // do a read which should reduce available's return value
      if (wire_PWR_ANA->available()!=0){ // check available again and if there is still more to read, do it.
        iter_time++;
      }
      else{ // we got here because after reading, the available was 0, so nothing left to read
        iter_time=timeout_max+1; // kill the timeout in a funny way but really just return out of function at next line.
        return true;
      }
    }
    //if we got here, this should return false, because there was somehow a timeout after reading with alot of availables?
    return false;
  }
  else return true;  // really this is if nothing was available as soon as going into this function, so buffer was clear.
}

//#define CONST_M CONST_DY/CONST_DX
float ADC_converter(uint8_t adc_high, uint8_t adc_low){
  uint16_t byte_conv=((uint16_t) (adc_high) << 8) | (uint16_t) (adc_low);
  float conv = (float)(byte_conv-ADC_CONST_B)/adc_const_m;
  return conv;
}


uint8_t muxconvert(int channel){
  uint8_t mux_byte = 0x08; // EN is high already
  if(channel<=7) return (mux_byte | (uint8_t) (channel));
  else return 0x00;
}

int calc_mux_channel(int In){
      int Jout=-1;
      if(In <=23 && In >=0){
       Jout=7-((In+8)%8);
       return Jout;
      }
      else return Jout;
}
int mux_IC_num(int In){
      int Label=-1;
      if(In <=7 && In >=0) Label=7;
      else if(In <=15 && In >=8) Label=6;
      else if(In <=23 && In >=16) Label=5;
      return Label;
}

uint8_t EXT_ANA_I2C_MUX_write(uint8_t to_write) {
  wire_EXT_ANA->beginTransmission(EXT_ANA_I2C_MUX_ADDR);
  wire_EXT_ANA->write(to_write); // write to point at register
  //wire_1->write(to_write); // then write 0xFF
  return wire_EXT_ANA->endTransmission();
}

uint8_t EXT_ANA_mux_ctrl(int channel, uint8_t addr_mux){
  wire_EXT_ANA->beginTransmission(addr_mux);
  uint8_t to_write=muxconvert(channel);
  //Serial.println(to_write,HEX);
  wire_EXT_ANA->write(to_write); // write to point at register
  return wire_EXT_ANA->endTransmission();
}

//change type so we can return when we are done at any point in this FN
bool EXT_ANA_read(uint8_t num_bytes, uint8_t temp_addr){
  bool buffer_clear=EXT_clear_buffer_with_timeout();
  // now do a request if buffer was clear  but the problem is if we requested 2 bytes, then we can't just wait for nonzero available, need to wait for available to be 2!
  if(buffer_clear){
    wire_EXT_ANA->requestFrom(temp_addr, num_bytes); // request 2 bytes from tmp441
    bool reads_avail=EXT_check_reads_avail_with_timeout(num_bytes);
    if(reads_avail){ // yes, we have right bytes to read so read them
      bool real_read_good=EXT_do_real_reads();
      if(real_read_good){
        //Serial.println("GOODREADS");
        return true;
      }
      else{
        Serial.println("loopy reads");
        return false;
      }
    }
    else{
      Serial.println("reads weren't available");
      return false;
    }
  }
  else {
    Serial.print("Buffer UNCLEAR");
    return false;
  }
}
bool EXT_do_real_reads(){
  int bytes_ready=(int) wire_EXT_ANA->available();
  int check_loop=0;
  uint8_t all_reads[2]={0};
  for(int read_iter=0;read_iter<bytes_ready;read_iter++){  
    uint8_t read_val = wire_EXT_ANA->read();
    all_reads[read_iter]=read_val;
    //Serial.print((uint8_t) read_val, HEX); // okay if this actually can print out as decimal we're good 
    //Serial.println((uint8_t) read_val, DEC);
    check_loop++;
    if(check_loop>100) return false;
  }
  float volts=ADC_converter(all_reads[0],all_reads[1]);
  EXT_ANA_READS[24]=((uint16_t) (all_reads[0]) << 8) | (uint16_t) (all_reads[1]);
  //Serial.println(volts,6);
  return true;
}

bool EXT_check_reads_avail_with_timeout(uint8_t bytes_total){
  int timeout_max = 1000000;
  int iter_time=0;
  if (wire_EXT_ANA->available()== (int) (bytes_total)) return true; // if there the bytes are ready, just return true.
  else{
    while(iter_time < timeout_max) {
      if(wire_EXT_ANA->available()!= (int)(bytes_total)){ // check available again and if there is still more to wait for, then wait longer.
        iter_time++;
      }
      else{ // we got here because after waiting and checking, we found that avail had the right number of bytes waiting.
        iter_time=timeout_max+1; // kill the timeout in a funny way but really just return out of function at next line.
        return true;
      }
    }
    //if we got here, this should return false, because there was somehow a timeout after checking avail alot of times but the correct number of bytes wasn't ready
    return false;
  }
}

bool EXT_clear_buffer_with_timeout(){
  int timeout_max = 100000;
  int iter_time=0;
  if (wire_EXT_ANA->available()!=0){ // if there is something to read that wasn't requested, do it. This is called clearing the buffer
    while (iter_time < timeout_max) {
      wire_EXT_ANA->read();  // do a read which should reduce available's return value
      if (wire_EXT_ANA->available()!=0){ // check available again and if there is still more to read, do it.
        iter_time++;
      }
      else{ // we got here because after reading, the available was 0, so nothing left to read
        iter_time=timeout_max+1; // kill the timeout in a funny way but really just return out of function at next line.
        return true;
      }
    }
    //if we got here, this should return false, because there was somehow a timeout after reading with alot of availables?
    return false;
  }
  else return true;  // really this is if nothing was available as soon as going into this function, so buffer was clear.
}
//for led blink
void switch_LED() {
  if (is_high) {
    is_high = false;
    digitalWrite(LED, LOW);
  }
  else {
    is_high = true;
    digitalWrite(LED, HIGH);
  }
}
