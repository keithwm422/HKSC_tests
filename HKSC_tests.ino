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
#include "inc/hw_flash.h"
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

#include "src/HSK_lib/Core_protocol.h"
#include "src/HSK_lib/PacketSerial.h"
#include "src/HSK_lib/HSK_protocol.h"
using namespace HSK_cmd;

// SFC stuff
#define DOWNBAUD 1152000 // Baudrate to the SFC
#define FIRST_LOCAL_COMMAND 2 // value of hdr->cmd that is the first command local to the board
#define NUM_LOCAL_CONTROLS 8 // how many commands total are local to the board
/* Declare instances of PacketSerial to set up the serial lines */
PacketSerial downStream1; // SFC
/* Outgoing buffer, for up or downstream. Only gets used once a complete packet
 * is received -- a command or forward is executed before anything else happens,
 * so there shouldn't be any over-writing here. */
uint8_t outgoingPacket [MAX_PACKET_LENGTH] ={0}; 
/* Use pointers for all device's housekeeping headers and the autopriorityperiods*/
housekeeping_hdr_t * hdr_in{nullptr};     housekeeping_hdr_t * hdr_out{nullptr};
housekeeping_err_t * hdr_err{nullptr};
uint8_t numDevices = 0;           // Keep track of how many devices are upstream
uint8_t commandPriority[NUM_LOCAL_CONTROLS] = {0};     // Each command's priority takes up one byte
PacketSerial *serialDevices = &downStream1;
uint8_t addressList = 0; // List of all downstream devices
uint8_t seq_number_in;
/* Utility variables for internal use */
size_t hdr_size = sizeof(housekeeping_hdr_t)/sizeof(hdr_out->cmd); // size of the header
uint8_t numSends = 0; // Used to keep track of number of priority commands executed
uint16_t currentPacketCount=0;
static_assert(sizeof(float) == 4);


//Big test packet for SFC comms dev
#define PACKET_UPDATE_PERIOD 6000
unsigned long PacketUpdateTime=0; // unprompted packet timer
uint8_t packet_fake[100]={0}; // array for using existing functions to implement command and send packet. This is called 'fake' header because it is forcing this hsk board to run code AS IF it received a packet with the contents of "a fake header" 
uint8_t Bigpacket[220]={0};
int clear_buffers_with_this=0;

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
uint16_t PWR_ANA_READS[30]={0}; // one extra because the last one is always the most recent read
uint16_t PWR_ANA_READS_0[25]={0}; // one extra because the last one is always the most recent read
uint8_t PWR_DAC_write[3]={0x2F,0xFF,0x00}; // after I2C address, need 3 bytes, where 1st byte is Command/access byte, and then the next two bytes are the data bytes
// upper nibble doesn't matter: X X X X , EN C2 C1 C0 -> EN is 0 means all off (won't matter what C2-C0 are). EN is 1, then S0-S7 corresponds to the bits of C2-C0 as 000->S0, 001->S1... 111->S7
//uint8_t command_byte_write=0x03;
//uint8_t ports_to_write=0xFF;//all high
#define ADC_CONST_B 32768
#define ADC_CONST_DX 1.25
#define ADC_CONST_DY ADC_CONST_B
float adc_const_m=ADC_CONST_DY/ADC_CONST_DX;
unsigned long PWR_ANA_time = 0;
#define PWR_ANA_PERIOD 1000
unsigned long PWR_ANA_reread_time=0;
#define PWR_ANA_REREAD_PERIOD 20
uint8_t PWR_ANA_read_debug=0x00;
bool PWR_ANA_read_real=false;

int PWR_ANA_chip_num=0; // mux_IC_num(ana);
int PWR_ANA_arg_chip_num=0; // calc_mux_channel(chip_num);
int PWR_ANA_mux_channel=0; // calc_mux_channel(ana);
int ana=0;
int ana_prev=0;
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

// variables for packet construction
uint32_t TempRead=0;  // internal uC temperature

char one_byte;

// time zeroed for reading things.
unsigned long time_store=0;
void setup() {
  // Serial port for downstream to SFC
  Serial.begin(DOWNBAUD);
  clear_buffers_with_this=0;
  while(clear_buffers_with_this!=-1){
    clear_buffers_with_this=Serial.read();
  }
  downStream1.setStream(&Serial);
  downStream1.setPacketHandler(&checkHdr);
  // Point to data in a way that it can be read as a header
  hdr_out = (housekeeping_hdr_t *) outgoingPacket;
  hdr_err = (housekeeping_err_t *) (outgoingPacket + hdr_size);
  currentPacketCount=0;
  // Test Packet setup 
  PacketUpdateTime= millis() + PACKET_UPDATE_PERIOD;
  for(int i=0;i<220;i++){
    Bigpacket[i]=(uint8_t) i;
  }
  
  // Setup I2C stuff
  //wire_1->begin();
  wire_RF_ON->begin();
  wire_PWR_CTRL->begin();
  wire_PWR_ANA->begin();
  wire_EXT_ANA->begin();

  pinMode(EXT_ANA_RST_pin, OUTPUT);
  digitalWrite(EXT_ANA_RST_pin, HIGH);  // this is really NOT RST, so leave high to not RST....
  Serial2.begin(115200); // TURF UART  
  Serial7.begin(9600); // Charge Controller UART

  //LED blinky start
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  LED_time = millis();
}

void loop() {
  if((long) (millis() - LED_time) > 0){
    LED_time+= LED_PERIOD;
    switch_LED();
    TempRead=analogRead(TEMPSENSOR);
  }
  // example send packet unprompted every PACKET_PERIOD
  if((long)(millis() -PacketUpdateTime) > 0){
    PacketUpdateTime+= PACKET_UPDATE_PERIOD;
    housekeeping_hdr_t *packet_fake_hdr = (housekeeping_hdr_t *) packet_fake; // fakehdr is best way to send a packet
    hdr_out = (housekeeping_hdr_t *) outgoingPacket;
    packet_fake_hdr->len=0; // this should always be 0, especially because the array is just enough to hold the header.
    packet_fake_hdr->cmd=161;  // which command you want on the timer goes here.
    // to construct a packet, pass it a fake header
    handleLocalCommand(packet_fake_hdr, (uint8_t *) packet_fake_hdr + hdr_size, (uint8_t *) outgoingPacket);
  }
  /* Continuously read in one byte at a time until a packet is received */
  if (downStream1.update() != 0) badPacketReceived(&downStream1);
  int ser7_val=Serial7.read();
  if(ser7_val != -1){
    Serial.print((char)ser7_val);
  }
  int ser2_val=Serial2.read();
  if(ser2_val != -1){
    Serial.print((char)ser2_val);
  }
  // Read PWR_ANA ADC wherever its pointing?
  if((long) (millis() - PWR_ANA_time) > 0){
    // iterate the position of the read, or something? because the next read completed is the previous read that was done ? interesting, read twice and store first read, and newest read?
    // so we need to iterate the indexers and keep the previous?
    PWR_ANA_chip_num=mux_IC_num(ana);  // goes to top mux
    PWR_ANA_arg_chip_num=calc_mux_channel(PWR_ANA_chip_num); // goes to lower level mux
    PWR_ANA_mux_channel=calc_mux_channel(ana); // goes to lower level mux channel
    PWR_ANA_mux_ctrl(PWR_ANA_chip_num, PWR_ANA_MUX_ADDR_TOP);
    if(ana<24) PWR_ANA_mux_ctrl(PWR_ANA_mux_channel, PWR_ANA_MUX[PWR_ANA_arg_chip_num]);
    PWR_ANA_time= millis() + PWR_ANA_PERIOD;
    PWR_ANA_read_debug=PWR_ANA_read(2,PWR_ANA_ADC_ADDR);
    // now we can do a real read for updated value...
    //delay(20);
    if(PWR_ANA_read_debug==0x00){
      if(ana==0) ana_prev=28;
      else ana_prev=ana-1;
      PWR_ANA_READS[ana_prev] = PWR_ANA_READS[29];
      PWR_ANA_reread_time=millis()+PWR_ANA_REREAD_PERIOD;
      PWR_ANA_read_real=true;
    }
    else PWR_ANA_read_real=false;
  }
  if(PWR_ANA_read_real && (long) (millis() - PWR_ANA_reread_time) > 0){
    PWR_ANA_read_debug=PWR_ANA_read(2,PWR_ANA_ADC_ADDR);
    PWR_ANA_read_real=false;
    if(PWR_ANA_read_debug==0x00){
      // this was a good read so store it in appropriate place
      PWR_ANA_READS[ana]= PWR_ANA_READS[29]; // most recent read is just ana
      ana++;
      if(ana>28) ana=0; // restart the mux scanning
    }

  }
}
/*******************************************************************************
 * Packet handling functions
 *******************************************************************************/
void checkHdr(const void *sender, const uint8_t *buffer, size_t len) {
  // Default header & error data values
  hdr_out = (housekeeping_hdr_t *) outgoingPacket;
  hdr_in = (housekeeping_hdr_t *)buffer;
  // check the seq number which is the very last byte of buffer and there's two ways to do it at least, either memcpy or pointing correctly
  seq_number_in=*((uint8_t *) &buffer+hdr_in->len + hdr_size);
  //memcpy((uint8_t *)&seq_number_in,(uint8_t *)&buffer+hdr_out->len+hdr_size,sizeof(seq_number_in)); 
  //handlePriority(hdr_in->cmd, (uint8_t *) outgoingPacket); // for doing a send of priority type.
  handleLocalCommand(hdr_in, (uint8_t *) hdr_in + hdr_size, (uint8_t *) outgoingPacket); // this constructs the outgoingpacket when its a localcommand and sends the packet. 
  // If the message wasn't meant for this device pass it along (up is away from SFC and down and is to SFC? this shouldn't happen?
}

/* Function flow:
 * --Find the device address that produced the error
 * --Execute the bad length function & send the error to the SFC
 * Function params:
 * sender:    PacketSerial instance which triggered the error protocol
 * Send an error if a packet is unreadable in some way */
void badPacketReceived(PacketSerial * sender){
  buildError(hdr_err, hdr_out, hdr_in, EBADARGS);
  memcpy((uint8_t *)&outgoingPacket+hdr_out->len+hdr_size,(uint8_t *) &seq_number_in, sizeof(seq_number_in));
  downStream1.send(outgoingPacket, hdr_size + hdr_out->len + 1); // add one for the sequence number
  currentPacketCount++;
}

// Function for building the error packets to send back when an error is found (see the Core_Protocol.h for the defs of the errors and the error typdefs).
void buildError(housekeeping_err_t *err, housekeeping_hdr_t *respHdr, housekeeping_hdr_t * hdr, int error){
  respHdr->magic=MAGIC_BYTE_VALUE;
  respHdr->cmd = eError;
  respHdr->len = 4;
  err->cmd = hdr->cmd;
  err->error = error;
}
/***********************
/*******************************************************************************
 * END OF Packet handling functions
 *******************************************************************************/

// sending priority command function
// probably can be cleaned up
// Note: SendAll is 253 and SendLow is 250 so we made SendLow-> int priority=1 for checking the device's list of command's priorities.
// got a priority request from destination dst
void handlePriority(uint8_t prio_in, uint8_t * responsePacketBuffer){
  housekeeping_hdr_t *respHdr = (housekeeping_hdr_t *) responsePacketBuffer;
  uint8_t *respData = responsePacketBuffer + hdr_size;
  int priority=0;
  int retval = 0;
  uint16_t sum = 0; // hdr length of data atatched from all those commands data
//  respHdr->cmd = hdr_in->cmd;
  // priority == 4 when this function is called is code for "eSendAll"
  // otherwise priority=1,2,3 and that maps to eSendLowPriority+priority
  if(prio_in==eSendAll) priority=4;
  else priority = prio_in - 249;
//  int retval;
  respHdr->cmd =  prio_in;
  // go through every priority
  for (int i=0;i<NUM_LOCAL_CONTROLS;i++) {
    if (commandPriority[i] == (uint8_t)priority || priority==4) {
      retval=handleLocalRead((uint8_t) i + FIRST_LOCAL_COMMAND, respData+sum);
      // if that read overflowed the data???? fix later?
      sum+= (uint8_t) retval;
    }
    else sum+=0;
  }
  respHdr->len=sum;
  memcpy((uint8_t *) responsePacketBuffer+respHdr->len+hdr_size,(uint8_t *) &seq_number_in, sizeof(seq_number_in));
  downStream1.send(responsePacketBuffer, respHdr->len + hdr_size + 1);
  currentPacketCount++;
}

// function for when a "SetPriority" command is received by this device, adding that commands priority value to the array/list
/*void setCommandPriority(housekeeping_prio_t * prio, uint8_t * respData, uint8_t len) {
//  housekeeping_prio_t * set_prio = (housekeeping_prio_t *) prio;
  commandPriority[prio->command-FIRST_LOCAL_COMMAND] = (uint8_t) prio->prio_type;
  memcpy(respData, (uint8_t*)prio, len);
}*/
// Fn to handle a local command write.
// This gets called when a local command is received
// with data (len != 0).
int handleLocalWrite(uint8_t localCommand, uint8_t * data, uint8_t len, uint8_t * respData) {
  int retval = 0;
  switch(localCommand) {
  case eSetPriority: {
    //setCommandPriority((housekeeping_prio_t *)data,respData,len);
    retval=len;
    break;
  }
  case eConfigureGPIOs: {
    uint8_t configure_in;
    memcpy((uint8_t * ) &configure_in, data, len);
    if(configure_in){
      RF_ON_write(RF_ON_GPIO_ADDR0,0x03, 0x00);
      RF_ON_write(RF_ON_GPIO_ADDR1,0x03, 0x00);
      RF_ON_write(RF_ON_GPIO_ADDR2,0x03, 0x00);
      RF_ON_write(RF_ON_GPIO_ADDR3,0x03, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR0,0x03, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR1,0x03, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR2,0x03, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR3,0x03, 0x00);
    }
    //else configure_in=0;
    //Serial.print("GPIO --> Configured\n");
    //retval=sizeof(configure_in);
    retval=0;
    break;
  }
  case eOutputsHigh: {
    uint8_t outputs_in;
    memcpy((uint8_t * ) &outputs_in, data, len);
    if(outputs_in){
      RF_ON_write(RF_ON_GPIO_ADDR0,0x01, 0xFF);
      RF_ON_write(RF_ON_GPIO_ADDR1,0x01, 0xFF);
      RF_ON_write(RF_ON_GPIO_ADDR2,0x01, 0xFF);
      RF_ON_write(RF_ON_GPIO_ADDR3,0x01, 0xFF);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR0,0x01, 0xFF);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR1,0x01, 0xFF);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR2,0x01, 0xFF);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR3,0x01, 0xFF);
    }
    //else configure_in=0;
    //Serial.print("GPIO --> Configured\n");
    //retval=sizeof(configure_in);
    retval=0;
    break;
  }
  case eOutputsLow: {
    uint8_t outputs_in;
    memcpy((uint8_t * ) &outputs_in, data, len);
    if(outputs_in){
      RF_ON_write(RF_ON_GPIO_ADDR0,0x01, 0x00);
      RF_ON_write(RF_ON_GPIO_ADDR1,0x01, 0x00);
      RF_ON_write(RF_ON_GPIO_ADDR2,0x01, 0x00);
      RF_ON_write(RF_ON_GPIO_ADDR3,0x01, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR0,0x01, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR1,0x01, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR2,0x01, 0x00);
      PWR_CTRL_write(PWR_CTRL_GPIO_ADDR3,0x01, 0x00);
    }
    //else configure_in=0;
    //Serial.print("GPIO --> Configured\n");
    //retval=sizeof(configure_in);
    retval=0;
    break;
  }
  case ePWRTopMuxSelect: {
    uint8_t mux_in;
    memcpy((uint8_t * ) &mux_in, data, len);
    if((int)(mux_in)>=0 && (int) (mux_in) <=7){
      PWR_ANA_mux_ctrl((int)mux_in, PWR_ANA_MUX_ADDR_TOP); // this is to point to remote high byte
      retval=0;
    }
    else {
      mux_in=0xFF;
      memcpy((uint8_t * ) &respData, (uint8_t *) &mux_in, sizeof(mux_in));
      retval=sizeof(mux_in);
    }
    break;
  }
  case ePWRMuxSelect: {
    uint8_t mux_in=0xFF;
    uint8_t addr_in=0xFF;
    memcpy((uint8_t * ) &addr_in, data, sizeof(addr_in));
    memcpy((uint8_t * ) &mux_in, data+1, sizeof(mux_in));
    if((int)(mux_in)>=0 && (int) (mux_in) <=7 && (int)(addr_in)>=0 && (int) (addr_in) <=2){
      PWR_ANA_mux_ctrl((int)(mux_in), PWR_ANA_MUX[(int)(addr_in)]);
      retval=0;
    }
    else {
      mux_in=0xFF;
      addr_in=0xFF;
      memcpy((uint8_t * ) &respData, (uint8_t *) &mux_in, sizeof(mux_in));
      memcpy((uint8_t * ) &respData + sizeof(mux_in), (uint8_t *) &addr_in, sizeof(addr_in));
      retval=sizeof(mux_in)+sizeof(addr_in);
    }
    break;
  }
  default:
    retval=EBADCOMMAND;    
    break;
  }
  return retval;
}

// Fn to handle a local command read.
// This gets called when a local command is received
// with no data (len == 0)
// buffer contains the pointer to where the data
// will be written.
// int returns the number of bytes that were copied into
// the buffer, or EBADCOMMAND if there's no command there
int handleLocalRead(uint8_t localCommand, uint8_t *outbuffer) {
  int retval = 0;
  switch(localCommand) {
  case ePingPong:
    retval=0;
    break;
  case eSetPriority:
    retval = EBADLEN;
    break;
  case eIntSensorRead: {
    float TempC = (float)(1475.0 - (float)(((2475 * TempRead) / 4096))) / 10.0;
    memcpy(outbuffer,(uint8_t *) &TempC,sizeof(TempC));
    retval=sizeof(TempC);
    break;
  }
  case ePacketCount:
    memcpy(outbuffer, (uint8_t *) &currentPacketCount, sizeof(currentPacketCount));
    retval = sizeof(currentPacketCount);
    break;
  case eTimeStore: {
    time_store=millis();
    memcpy(outbuffer, (uint8_t *) &time_store, sizeof(time_store));
    retval=sizeof(time_store);
    break;
  }
  case eISR: {
    float TempC = (float)(1475 - ((2475 * TempRead) / 4096)) / 10;
    memcpy(outbuffer,(uint8_t *) &TempC,sizeof(TempC));
    retval=sizeof(TempC);
    break;
  }
  case eBigPacket: {
    //float TempC = (float)(1475 - ((2475 * TempRead) / 4096)) / 10;
    memcpy((uint8_t *) &Bigpacket,(uint8_t *) &PWR_ANA_READS,sizeof(PWR_ANA_READS));
    //memcpy((uint8_t *) &Bigpacket+sizeof(PWR_ANA_READS),(uint8_t *) &PWR_ANA_EXTRA_READS,sizeof(PWR_ANA_EXTRA_READS));
    memcpy(outbuffer,(uint8_t *) &Bigpacket,sizeof(Bigpacket));
    retval=sizeof(Bigpacket);
    break;
  }
  case eReset: {
    SysCtlReset();
    retval = 0;
    break;
  }
  default:
    retval=EBADCOMMAND;
    break;
  }  
  return retval;
}

// Function to call first when localcommand sent. 
// Store the "result" as retval (which is the bytes read or written, hopefully)
void handleLocalCommand(housekeeping_hdr_t *hdr, uint8_t * data, uint8_t * responsePacketBuffer) {
  int retval=0;
  housekeeping_hdr_t *respHdr = (housekeeping_hdr_t *) responsePacketBuffer;
  respHdr->magic=MAGIC_BYTE_VALUE;
  uint8_t * respData = responsePacketBuffer + hdr_size;
  if (hdr->len) {
    retval = handleLocalWrite(hdr->cmd, data, hdr->len, respData); // retval is negative construct the baderror hdr and send that instead. 
    if(retval>=0) {
//      *respData= 5;
      respHdr->cmd = hdr->cmd;
      respHdr->len = retval; // response bytes of the write.
    }
    else{
      housekeeping_err_t *err = (housekeeping_err_t *) respData;
      buildError(err, respHdr, hdr, retval);
    }  
  } 
  else {
    // local read. by definition these always go downstream.
    retval = handleLocalRead(hdr->cmd, respData);
    if (retval>=0 && retval<=249) {
      respHdr->cmd = hdr->cmd;
      respHdr->len = (uint16_t) retval; //bytes read
    }
    else {
      housekeeping_err_t *err = (housekeeping_err_t *) respData;
      buildError(err, respHdr, hdr, retval); // the err pointer is pointing to the data of the response packet based on the line above so this fn fills that packet. 
    }
  }
  // send to SFC
  memcpy((uint8_t *) responsePacketBuffer+respHdr->len+hdr_size,(uint8_t *) &seq_number_in, sizeof(seq_number_in));
  downStream1.send(responsePacketBuffer, respHdr->len + hdr_size + 1 );
  currentPacketCount++;
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
uint8_t PWR_ANA_read(uint8_t num_bytes, uint8_t temp_addr){
  bool buffer_clear=clear_buffer_with_timeout();
  // now do a request if buffer was clear  but the problem is if we requested 2 bytes, then we can't just wait for nonzero available, need to wait for available to be 2!
  if(buffer_clear){
    wire_PWR_ANA->requestFrom(temp_addr, num_bytes); // request 2 bytes from tmp441
    bool reads_avail=check_reads_avail_with_timeout(num_bytes);
    if(reads_avail){ // yes, we have right bytes to read so read them
      bool real_read_good=do_real_reads();
      if(real_read_good){
        //Serial.println("GOODREADS");
        return 0x00;
      }
      else{
        //Serial.println("loopy reads");
        return 0x01;
      }
    }
    else{
      //Serial.println("reads weren't available");
      return 0x02;
    }
  }
  else {
    //Serial.print("Buffer UNCLEAR");
    return 0x03;
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
  PWR_ANA_READS[29]=((uint16_t) (all_reads[0]) << 8) | (uint16_t) (all_reads[1]);
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
      else if(In==24) Label=4;
      else if(In==25) Label=3;
      else if(In==26) Label=2;
      else if(In==27) Label=1;
      else if(In==28) Label=0;
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
