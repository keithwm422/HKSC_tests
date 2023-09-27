#include "Wire_nonblocking_v2.h"
#include <sysctl.h>

TwoWire *wire_ = new TwoWire(2);  // pueo RF ON is line 2 and is also the 1wire i2c click dev board
uint8_t mAddress=0x18;
// temp addresses at PUEO HKSC testbench // 28,EC,1A,65,E,0,0,6E
byte addr1[8]={0x28,0xEC,0x1A,0x65,0x0E,0x00,0x00,0x6E};
byte addr2[8]={0x28,0x6A,0xC6,0x79,0x97,0x06,0x03,0x8D};
byte addr3[8]={0x28,0x0E,0xA4,0x79,0x97,0x06,0x03,0x00};
byte addr4[8]={0x28,0x75,0xBC,0x79,0x97,0x06,0x03,0x22};
unsigned long update_time=0;
#define UPDATE_PERIOD 1000
unsigned long LED_time=0;
#define LED_PERIOD 1000
unsigned long packet_time=0;
#define PACKET_PERIOD 2000
bool ppd_set=false;
uint8_t read_1wire_array[9]={0};
#define NUM_TEMP_SENSORS 1
byte addr[NUM_TEMP_SENSORS][8];
int which_sensor=0;
bool is_high=true;
//int LED_PIN=38;
//int LED_PIN_2=39;
#define LED_PIN RED_LED
#define LED_PIN_2 GREEN_LED
bool is_high_2=true;

// STATE MACHINE for onewire bridge
typedef enum {
  SENSOR_STATE_IDLE = 0, // checks for update_time expired 
  SENSOR_STATE_RESET = 1, // reset the bridge
  SENSOR_STATE_CHECKTIMER1 = 2, // timer for bridge to setup
  SENSOR_STATE_CONFIG = 3, // congfig
  SENSOR_STATE_SELECTCHANNEL = 4, // select bridge channel
  SENSOR_STATE_RESETWIRE1 = 5, // first probe reset on channel
  SENSOR_STATE_SELECTWIRE1 = 6, // addresses probe to communicate with
  SENSOR_STATE_CONVERTWIRE = 7, // command temp probe to convert a temp
  SENSOR_STATE_CHECKTIMER2 = 8, // timer for probe to be ready for read
  SENSOR_STATE_RESETWIRE2 = 9, // second time probe reset on channel, post convert
  SENSOR_STATE_SELECTWIRE2 = 10, // addresses probe to communicate with, post convert
  SENSOR_STATE_REQUESTWIRE = 11, // tell probe to send its data
  SENSOR_STATE_READDATA = 12, // just keep reading the data, should be 9 bytes. 
} SensorState;
SensorState BRIDGE_STATE=SENSOR_STATE_IDLE;
// STATE MACHINE for i2c comms writes
typedef enum {
  I2C_WRITE_IDLE = 0, // checks for i2c not busy all the time 
  I2C_WRITE_BEGIN = 1, // begintransmission
  I2C_WRITE_WRITE = 2, // write the values to the buffer
  I2C_WRITE_END = 3, // end the write // check for busy after.
  I2C_WRITE_COMPLETE = 4, // checks for busy after transmission
} I2CWriteState;
I2CWriteState I2C_WSTATE=I2C_WRITE_IDLE;

// STATE MACHINE for i2c comms reads
typedef enum {
  I2C_READ_IDLE = 0, // checks for i2c not busy all the time 
  I2C_READ_REQ = 1, // request a byte
  I2C_READ_WAIT_AFTER_REQ=2, // wait after a request by checking them bytes
  I2C_READ_AVAIL = 3, // check for bytes received
  I2C_READ_READ = 4, // read the value received // check for busy after? 
  I2C_READ_COMPLETE = 5, // checks for busy after reading the byte 
} I2CReadState;
I2CReadState I2C_RSTATE=I2C_READ_IDLE;

// STATE MACHINE for wireselect comms
typedef enum {
  SELECT_BEGIN = 0, // initialize the element in addressing
  SELECT_WRITE = 1, // write the next byte in selecting the wire address
  SELECT_CHECK1WB = 2, // check for the 1wire busy bit
  SELECT_COMPLETE = 3, // done until we set the state back to begin
} WireSelectState;
WireSelectState WIRESELECT_STATE=SELECT_BEGIN;

// STATE MACHINE for wireselect comms
typedef enum {
  WIREW_BEGIN = 0, // initialize the element in addressing
  WIREW_WRITE = 1, // write the next byte in selecting the wire address
  WIREW_CHECK1WB = 2, // check for the 1wire busy bit
  WIREW_COMPLETE = 3, // done until we set the state back to begin
} WireWriteState;
WireWriteState WIREWRITE_STATE=WIREW_BEGIN;

// STATE MACHINE for wireread comms
typedef enum {
  WIRER_BEGIN = 0, // initialize the element in addressing
  WIRER_SR_SET = 1, // write the next byte in selecting the wire address
  WIRER_CHECK1WB_1 = 2, // check 1wb separate from sending a read 
  WIRER_GEN_READ = 3, // generate 1wire read clocks
  WIRER_CHECK1WB_2 = 4, // check 1wb for read to be done
  WIRER_RR_SET = 5, // point to the read register
  WIRER_READ = 6, // read the byte from the readregister
  WIRER_COMPLETE = 7, // done
} WireReadState;
WireReadState WIREREAD_STATE=WIRER_BEGIN;


typedef enum{
  WIREBUSY_BEGIN=0,
  WIREBUSY_CHECK=1,
  WIREBUSY_COMPLETE=2,
} Check1WBState;
Check1WBState CHECK1WB_STATE=WIREBUSY_BEGIN;
unsigned long timer_post_reset=0;
int got_to_reads=0;
uint16_t status_1wb=0;
/******* DEBUG STUFF ********/
//uint8_t rx_buff[1000]={0};
//uint8_t tx_buff[1000]={0};
//uint8_t stat_buff[1000]={0};
//uint8_t onewb_buff[1000]={0};
//uint8_t onewb_state_buff[1000]={0};
//int onewb_iter_buff[1000]={0};
float celsius[100][4]={0};
float celsius_real=0;
int celsius_iter=0;
uint8_t real_data_read[100][9][4]={0};
uint8_t crcs[100][4]={0};
//int onewb_iter=0;
//int onewb_state_iter=0;
//int stat_iter=0;
//int rx_iter=0;
//int tx_iter=0;
uint8_t config_read_val=0;
uint8_t channel_read_val=0;
bool did_check1wb_fail=false;
uint8_t rx_stat_temp;
uint8_t tx_stat_temp;
bool keep_checking=true;
volatile int avail=0;
uint8_t rx_write_index;
uint8_t rx_read_index;
uint8_t tx_write_index;
uint8_t tx_read_index;
uint8_t req_val;
uint8_t status_reg_val;
uint8_t read_reg_val;
bool keep_checking_1wb=true;
bool avail_failed=false;
bool i2c_wstate_status=false;
bool i2c_rstate_status=false;
uint8_t together_stat_temp=0;
uint8_t read_temp_val=0;
int select_byte=0;
bool wireselect_status=false;
int wb_tries=0;
bool wirewrite_fail=false;
int read_byte_iter=0;
bool wireread_fail=false;
int i2c_tries=0;
int i2c_max_tries=1000;
bool i2c_fail=false;
int success_reads=0;
int tots_reads=0;
bool test_wire_read=false;
int got_to_readdata=0;
SensorState PREV_BRIDGE_STATE=SENSOR_STATE_IDLE;
I2CWriteState PREV_I2C_WSTATE=I2C_WRITE_IDLE;
I2CReadState PREV_I2C_RSTATE=I2C_READ_IDLE;
//WireSelectState PREV_WIRESELECT_STATE=SELECT_BEGIN; //no longer used
WireWriteState PREV_WIREWRITE_STATE=WIREW_BEGIN;
WireReadState PREV_WIREREAD_STATE=WIRER_BEGIN;
Check1WBState PREV_CHECK1WB_STATE=WIREBUSY_BEGIN;
int i2c_rtries=0;
int i2c_wtries=0;
bool test_1wb_sm=false;
void setup() {
  // put your setup code here, to run once: KEITH
  Serial.begin(115200);
  Serial.print("Starting....\n");
  wire_->begin();
  avail=wire_->available();
  wire_->flush();
  avail=wire_->available();
  copy_temp_addresses();
  which_sensor=0;
  update_time=millis()+UPDATE_PERIOD;
  LED_time=millis()+LED_PERIOD;
  packet_time=millis()+PACKET_PERIOD;
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,HIGH);
  pinMode(LED_PIN_2,OUTPUT);
  digitalWrite(LED_PIN_2,HIGH);
}

void loop() {
  if(did_check1wb_fail) {
    BRIDGE_STATE=SENSOR_STATE_IDLE;
    did_check1wb_fail=false;
  }
  if(avail_failed || wireread_fail || i2c_fail) SysCtlReset();
  switch(BRIDGE_STATE){
    case SENSOR_STATE_IDLE:{
      if((long) (millis()- update_time) > 0){
        which_sensor++;
        if(which_sensor>=NUM_TEMP_SENSORS) which_sensor=0;
        update_time=millis()+UPDATE_PERIOD;
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // fall through
        break;
      }
      else break;
    }
    case SENSOR_STATE_RESET:{
      if(write_a_reset()){
        timer_post_reset = millis() + 2000; // 100 is the ms to wait before the bridge should be accessed.
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // fall through
        break;
      }
      else {
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE;
        break;
      }
      // need to then make the timer variable increment by the period to wait.
    }
    case SENSOR_STATE_CHECKTIMER1:{
      if((long) (millis()- timer_post_reset) > 0){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // fall through
        break;
      }
      else break;
    }
    case SENSOR_STATE_CONFIG:{
      if(do_a_config()){
        //BRIDGE_STATE=SENSOR_STATE_IDLE;
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // fall through
        break;
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE;
        break;
      }
    }
    case SENSOR_STATE_SELECTCHANNEL:{
      if(select_a_channel()){
        // now do a read, pass a pointer to the var so that the read can place it straight into that var.
        read_i2c_one_byte((uint8_t *) &channel_read_val);
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // fall through
        break;
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE;
        break;
      }
    }
    case SENSOR_STATE_RESETWIRE1:{  // here is where we do a lot of checking and go to the idle state if bad shit. need to be careful here.
      if(status_reg_set()){ // this is just a write of two bytes i2c
        status_1wb=check_1WB(); // if this is 0x00 we know that we should check the did_check1wb_fail
        if(!did_check1wb_fail){
          wire_reset();
          status_1wb=check_1WB();
          if(!did_check1wb_fail){
            if(status_1wb & 0x0002){
              PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
              BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // keep going if this was fine
            }
            else{
              PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
              BRIDGE_STATE=SENSOR_STATE_IDLE;
            }
          }
          else{
            PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
            BRIDGE_STATE=SENSOR_STATE_IDLE;
          }
        }
        else{
          PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
          BRIDGE_STATE=SENSOR_STATE_IDLE;
        }
        break;
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE;
        break;
      }
    }
    case SENSOR_STATE_SELECTWIRE1:{  // theoretically gets here if wirereset went fine, ppd was detected. this takes some time to do.
      if(new_wire_select()){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // keep going if this was fine, meaning 1wb was gone and all bytes were written.
        break;        
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE; // either avail_failed occured or some other problem
      }
    }
    case SENSOR_STATE_CONVERTWIRE:{  // theoretically gets here if wirereset went fine, ppd was detected. this takes some time to do. make timer start here
      if(wire_convert()){
        timer_post_reset = millis() + 750; // 100 is the ms to wait before the probe is done converting
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // keep going if this was fine
        break;
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE; // either avail_failed occured or some other problem
      }
    }
    case SENSOR_STATE_CHECKTIMER2:{
      if((long) (millis()- timer_post_reset) > 0){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // fall through
        break;
      }
      else break;
    }
    case SENSOR_STATE_RESETWIRE2:{  // here is where we do a lot of checking and go to the idle state if bad shit. need to be careful here.
      status_reg_set(); // this is just a write of two bytes i2c
      status_1wb=check_1WB(); // if this is 0x00 we know that we should check the did_check1wb_fail
      if(!did_check1wb_fail){
        wire_reset();
        status_1wb=check_1WB();
        if(!did_check1wb_fail){
          if(status_1wb & 0x0002){
            PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
            BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // keep going if this was fine
          }
          else{
            PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
            BRIDGE_STATE=SENSOR_STATE_IDLE;
          }
        }
        else{
          PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
          BRIDGE_STATE=SENSOR_STATE_IDLE;
        }
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE;
      }
      break;
    }
    case SENSOR_STATE_SELECTWIRE2:{  // theoretically gets here if wirereset went fine, ppd was detected. this takes some time to do.
      if(new_wire_select()){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // keep going if this was fine, meaning 1wb was gone and all bytes were written.
        break;        
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE; // either avail_failed occured or some other problem
      }
    }
    case SENSOR_STATE_REQUESTWIRE:{  // a bunch of reads will be next.
      if(wire_temp_req()){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  // keep going if this was fine
        break;    
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE; // either avail_failed occured or some other problem
      }
    }
    case SENSOR_STATE_READDATA:{  // a bunch of reads. add period to the update_time. Could be this is where compiler is optimizing away code?
      // change to 9 bytes
      got_to_readdata++;
      if(read_them_all()){
        print_celsius();
        //check_crc();
        got_to_reads++;
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE;
        update_time=millis()+UPDATE_PERIOD;
        break;
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE;
        update_time=millis()+(10*UPDATE_PERIOD);
        break;
      }
    }
    default: {
      BRIDGE_STATE=SENSOR_STATE_IDLE;
    }
  }
  if((long) (millis()- LED_time) > 0){
    LED_time=millis()+LED_PERIOD;
    switch_LED();
  } 
}

void switch_LED(){
  if(is_high){
    is_high=false;
    digitalWrite(LED_PIN,LOW);
  }
  else{    
    is_high=true;
    digitalWrite(LED_PIN,HIGH);
  }
}

void switch_LED_2(){
  if(is_high_2){
    is_high_2=false;
    digitalWrite(LED_PIN_2,LOW);
  }
  else{    
    is_high_2=true;
    digitalWrite(LED_PIN_2,HIGH);
  }
}

uint8_t CheckStates(){
  rx_stat_temp=wire_-> getRxStatus();
  tx_stat_temp=wire_-> getTxStatus();
  together_stat_temp= rx_stat_temp | tx_stat_temp;
  rx_write_index=wire_->getrxWriteIndex();
  rx_read_index=wire_->getrxReadIndex();
  tx_write_index=wire_->gettxWriteIndex();
  tx_read_index=wire_->gettxReadIndex();
  keep_checking=true;
  (rx_stat_temp | tx_stat_temp) ? keep_checking=true : keep_checking=false;
  return ((1 << 0) & together_stat_temp);
}
/*
uint8_t CheckStates(){
  rx_stat_temp=wire_-> getRxStatus();
  tx_stat_temp=wire_-> getTxStatus();
  together_stat_temp= rx_stat_temp | tx_stat_temp;
  rx_write_index=wire_->getrxWriteIndex();
  rx_read_index=wire_->getrxReadIndex();
  tx_write_index=wire_->gettxWriteIndex();
  tx_read_index=wire_->gettxReadIndex();
  keep_checking=true;
  (rx_stat_temp | tx_stat_temp) ? keep_checking=true : keep_checking=false;
  return ((1 << 0) & together_stat_temp);
}
*/


bool i2c_wstate(uint8_t byte1, uint8_t byte2, int num_bytes){
  switch(I2C_WSTATE){
    case I2C_WRITE_IDLE:{
      while(1){
        if(!CheckStates()){
          PREV_I2C_WSTATE=I2C_WSTATE;
          I2C_WSTATE=I2C_WRITE_BEGIN;
          return false;
        }
        else{
          i2c_wtries++;
          if(i2c_wtries>=i2c_max_tries){
            i2c_fail=true;
            return true;
          }
        }
      }
      break;
    }
    case I2C_WRITE_BEGIN:{
      wire_->beginTransmission(mAddress);
      PREV_I2C_WSTATE=I2C_WSTATE;
      I2C_WSTATE=I2C_WRITE_WRITE;
      return false;
    }
    case I2C_WRITE_WRITE:{
      if(num_bytes==2){
        wire_->write(byte1);
        wire_->write(byte2);
      }
      else wire_->write(byte1);
      PREV_I2C_WSTATE=I2C_WSTATE;
      I2C_WSTATE=I2C_WRITE_END;
      return false;
    }
    case I2C_WRITE_END:{
      wire_->endTransmission_nonblocking(1);
      PREV_I2C_WSTATE=I2C_WSTATE;
      I2C_WSTATE=I2C_WRITE_COMPLETE;
      return false;
    }
    case I2C_WRITE_COMPLETE:{
      while(1){
        if(!CheckStates()){
          PREV_I2C_WSTATE=I2C_WSTATE;
          I2C_WSTATE=I2C_WRITE_IDLE;
          return true;
        }
        else{
          i2c_wtries++;
          if(i2c_wtries>=i2c_max_tries){
            i2c_fail=true;
            return true;
          }
        }
      }
      break;
    }
  }
}

bool i2c_rstate(uint8_t * buffer){
  switch(I2C_RSTATE){
    case I2C_READ_IDLE:{
      while(1){
        if(!CheckStates()){
          PREV_I2C_RSTATE=I2C_RSTATE;
          I2C_RSTATE=I2C_READ_REQ;  // do we need to do a flush?
          return false;
        }
        else{
          i2c_rtries++;
          if(i2c_rtries>=i2c_max_tries){
            i2c_fail=true;
            return true;
          }
        }
      }
      break;
    }
    case I2C_READ_REQ:{
      req_val=wire_->requestFrom_nonblocking(mAddress,(uint8_t)1,1);
      PREV_I2C_RSTATE=I2C_RSTATE;
      I2C_RSTATE=I2C_READ_WAIT_AFTER_REQ;
      return false;
    }
    case I2C_READ_WAIT_AFTER_REQ:{
      while(1){
        if(!CheckStates()){
          PREV_I2C_RSTATE=I2C_RSTATE;
          I2C_RSTATE=I2C_READ_AVAIL;
          return false;
        }
        else{
          i2c_rtries++;
          if(i2c_rtries>=i2c_max_tries){
            i2c_fail=true;
            return true;
          }
        }
      }
    }
    case I2C_READ_AVAIL:{
      switch_LED_2();
      avail=(int)wire_->getRxBytesReceived();
      if(avail){
        PREV_I2C_RSTATE=I2C_RSTATE;
        I2C_RSTATE=I2C_READ_READ;
        return false;
      }
      else{
          avail_failed=true;
          return true;
      }
    }
    case I2C_READ_READ:{
      read_temp_val=wire_->read();
      //memcpy(buffer,(uint8_t *) &read_temp_val,sizeof(read_temp_val));
      //buffer=(uint8_t *) read_temp_val;
      PREV_I2C_RSTATE=I2C_RSTATE;
      I2C_RSTATE=I2C_READ_COMPLETE;
      return false;
    }
    case I2C_READ_COMPLETE:{
      while(1){
        if(!CheckStates()){
          PREV_I2C_RSTATE=I2C_RSTATE;
          I2C_RSTATE=I2C_READ_IDLE;
          memcpy(buffer,(uint8_t *) &read_temp_val,sizeof(read_temp_val));
          return true;
        }
        else{
          i2c_rtries++;
          if(i2c_rtries>=i2c_max_tries){
            i2c_fail=true;
            return true;
          }
        }
      }
      break;
    }
  }
}

bool write_i2c_one_byte(uint8_t byte1){ 
  I2C_WSTATE=I2C_WRITE_IDLE;
  i2c_wstate_status=false;
  i2c_wtries=0;
  i2c_fail=false;
  while(!i2c_wstate_status) i2c_wstate_status=i2c_wstate(byte1,0,1);
  if(i2c_fail) return false;
  else return true;
}
bool write_i2c_two_bytes(uint8_t byte1, uint8_t byte2){
  I2C_WSTATE=I2C_WRITE_IDLE;
  i2c_wstate_status=false;
  i2c_wtries=0;
  i2c_fail=false;
  while(!i2c_wstate_status) i2c_wstate_status=i2c_wstate(byte1,byte2,2);
  if(i2c_fail) return false;
  else return true;
}

__attribute__((optimize(0))) bool read_i2c_one_byte(uint8_t * buffer){
  avail=0;
  read_temp_val=0;
  I2C_RSTATE=I2C_READ_IDLE;
  i2c_rstate_status=false;
  i2c_rtries=0;
  i2c_fail=false;
  while(!i2c_rstate_status) i2c_rstate_status=i2c_rstate((uint8_t *) buffer);
  if(i2c_fail) return false;
  else return true;
}
// really a general write may have been getting messed up
//because a flush would occur and then the next thing written and the next thing actually written would now be off. 
bool write_a_reset(){
  ppd_set=false;
  if(write_i2c_one_byte(0xF0)) return true;
  else return false;
}
//  wire_ds->write(config | (~config)<<4); for active Pullup use 1<<0
bool write_a_config(){
  if(write_i2c_two_bytes(0xD2,0xE1)) return true;
  else return false;
}
bool do_a_config(){
  if(write_a_config()){
    read_i2c_one_byte((uint8_t *)&config_read_val);
    if(config_read_val==1) return true;
    else return false;
  }
  else return false;
}
bool write_channel_select(){
  if(write_i2c_two_bytes(0xC3,0xB4)) return true;
  else return false;
}
bool select_a_channel(){
  if(write_channel_select()){
    read_i2c_one_byte((uint8_t *) &channel_read_val);
    if(channel_read_val==0x9C) return true;
    else return false;
  }
  else return false;
}
bool status_reg_set(){
  if(write_i2c_two_bytes(0xE1,0xF0)) return true;
  else return false;
}

__attribute__((optimize(0))) bool check_1WB_SM(){
  switch(CHECK1WB_STATE){
    case WIREBUSY_BEGIN:{
      keep_checking_1wb=true;
      did_check1wb_fail=false;
      wb_tries=0;
      PREV_CHECK1WB_STATE=CHECK1WB_STATE;
      CHECK1WB_STATE=WIREBUSY_CHECK;
      return false;
    }
    case WIREBUSY_CHECK:{
      read_i2c_one_byte((uint8_t *) &status_reg_val);
      switch_LED_2();
      wb_tries++;
      (status_reg_val & 0x01) ? keep_checking_1wb=true : keep_checking_1wb=false;
      if(!keep_checking_1wb){
        PREV_CHECK1WB_STATE=CHECK1WB_STATE;
        CHECK1WB_STATE=WIREBUSY_COMPLETE;
        return false;
      }
      else if(wb_tries>=1000){
        //status_reg_val=0;
      // break out
        did_check1wb_fail=true;
        return true;
      }
      else return false;
    }
    case WIREBUSY_COMPLETE:{
      return true;
    }
  }
}

// its possible that status_reg_val = 0x00 and the check_1WB_SM returns true and is completed correctly
// so need to change the return of this function to make this be true when checking is done "correctly"
// probably should be boolean (or 16-bit type?), and if return true, then integrity of status_reg_val can be accessed globally?

//old fn changed below on 3/16/2023
/*uint8_t check_1WB(){
  CHECK1WB_STATE=WIREBUSY_BEGIN;
  bool test_1wb_sm=false;
  while(!test_1wb_sm) test_1wb_sm=check_1WB_SM();
  if(did_check1wb_fail) return 0x00;
  else{
      // now status_reg_val should not have last bit set, but could(and doesn't have to) have any other bit set.
      // therefore, should return _____
      return status_reg_val;
  }
}*/
/*//made the test_1wb_sm boolean a global variable and it changed the disassembly and seemed to get successful reads, but hit the breakpoint in read_them_all (0xFE) fast, like on 4th readdata attempt
 * thus, this function return false again. B/c I didn't actual fix the issue. 0x01FF & status_reg_val will never be non-zero if status_reg_val is 0 still! duh!
 * so need 0x0100 | (0x00FF & status_reg_val);*/
__attribute__((optimize(0))) uint16_t check_1WB(){
  CHECK1WB_STATE=WIREBUSY_BEGIN;
  test_1wb_sm=false;
  while(!test_1wb_sm) test_1wb_sm=check_1WB_SM();
  if(did_check1wb_fail) return (0x00FF & (uint16_t) (status_reg_val));
  else{
      // now status_reg_val should not have last bit set, but could(and doesn't have to) have any other bit set.
      // therefore, should return 0x01FF & status_reg_val
      return ((0x0100) | (0x00FF & (uint16_t) (status_reg_val)));
  }
}

bool wire_reset(){
  ppd_set=false;
  write_i2c_one_byte(0xB4);
  return true;
}

// if we are writing an address, just do it and take up all the time
bool write_wire_byte(uint8_t byte_to_write){
  WIREWRITE_STATE=WIREW_BEGIN;
  bool test_wirewrite_sm=false;
  while(!test_wirewrite_sm) test_wirewrite_sm=write_wire_byte_sm(byte_to_write);
  if(wirewrite_fail) return false;
  else return true;
}
// checks for 1wb before and after, but assumes already pointing to the status register to check1wb
bool write_wire_byte_sm(uint8_t byte_to_write){
  switch(WIREWRITE_STATE){
    case WIREW_BEGIN:{
      wirewrite_fail=false;
      if(check_1WB()){
        PREV_WIREWRITE_STATE=WIREWRITE_STATE;
        WIREWRITE_STATE=WIREW_WRITE;
        return false;
      }
      else{
        wirewrite_fail=true;
        return true;
      }
    }
    case WIREW_WRITE:{
      write_i2c_two_bytes(0xA5,byte_to_write);
      PREV_WIREWRITE_STATE=WIREWRITE_STATE;
      WIREWRITE_STATE=WIREW_CHECK1WB;
      return false;
    }
    case WIREW_CHECK1WB:{
      if(check_1WB()){
        PREV_WIREWRITE_STATE=WIREWRITE_STATE;
        WIREWRITE_STATE=WIREW_COMPLETE;
        return false;
      }
      else{
        wirewrite_fail=true;
        return true;
      }
    }
    case WIREW_COMPLETE:{
      wirewrite_fail=false;
      return true;
    }
  }
}
bool new_wire_select(){
  select_byte=0;
  while(select_byte<9){
    if(select_byte==0){
      if(write_wire_byte(0x55)) select_byte++;
      else return false;
    }
    else{
      if(write_wire_byte(addr[which_sensor][select_byte-1])) select_byte++;
      else return false;
    }
  }
  return true;
}
bool wire_convert(){
  if(write_wire_byte(0x44)) return true;
  else return false;
}

bool wire_temp_req(){
  if(write_wire_byte(0xBE)) return true;
  else return false;
}


//bool generate_reads(){
//  if(write_wire_byte(0x00)) return true;
//  else return false;}
//
__attribute__((optimize(0))) bool read_reg_set(){
  return write_i2c_two_bytes(0xE1,0xE1);
}


__attribute__((optimize(0))) bool read_wire_byte_sm(){
  switch(WIREREAD_STATE){
    case WIRER_BEGIN:{
      wireread_fail=false;
      PREV_WIREREAD_STATE=WIREREAD_STATE;
      WIREREAD_STATE=WIRER_SR_SET;
      return false;
    }
    case WIRER_SR_SET:{
      if(status_reg_set()){ // this is just a write of two bytes i2c
        PREV_WIREREAD_STATE=WIREREAD_STATE;
        WIREREAD_STATE=WIRER_CHECK1WB_1;
        return false;
      }
      else{
        wireread_fail=true;
        return true;
      }
    }
    case WIRER_CHECK1WB_1:{
      if(check_1WB()){
        PREV_WIREREAD_STATE=WIREREAD_STATE;
        WIREREAD_STATE=WIRER_GEN_READ;
        return false;
      }
      else{
        wireread_fail=true;
        return true;
      }
    }
    case WIRER_GEN_READ:{
      if(write_i2c_one_byte(0x96)){
        switch_LED_2();
        PREV_WIREREAD_STATE=WIREREAD_STATE;
        WIREREAD_STATE=WIRER_CHECK1WB_2;
        return false;
      }
      else{
        wireread_fail=true;
        return true;
      }
    }
    case WIRER_CHECK1WB_2:{
      if(check_1WB()){
        PREV_WIREREAD_STATE=WIREREAD_STATE;
        WIREREAD_STATE=WIRER_RR_SET;
        return false;
      }
      else{
        wireread_fail=true;
        return true;
      }
    }
    case WIRER_RR_SET:{
      if(read_reg_set()){ // this sets the read pointer to the read register
        // to make sure this actually gets done before moving on, put inside if statement also?
        PREV_WIREREAD_STATE=WIREREAD_STATE;
        WIREREAD_STATE=WIRER_READ;
        return false;
      }
      else{
        //memcpy(byte_read,,sizeof(read_temp_val));
        wireread_fail=true;
        return true;
      }
    }
    case WIRER_READ:{
        // actually read the value from that register
      //delayMicroseconds(1);
      switch_LED_2();
      if(read_i2c_one_byte((uint8_t *) &read_reg_val)){
        PREV_WIREREAD_STATE=WIREREAD_STATE;
        WIREREAD_STATE=WIRER_COMPLETE;
        return false;
      }
      else {
        wireread_fail=true;
        return true;
      }
    }
    case WIRER_COMPLETE:{
      wireread_fail=false;
      return true;
    }
  }
}

//
__attribute__((optimize(0))) bool read_wire_byte(){
  WIREREAD_STATE=WIRER_BEGIN;
  wireread_fail=false;
  read_reg_val=0;
  //delayMicroseconds(1);
  test_wire_read=false;
  while(!test_wire_read) test_wire_read=read_wire_byte_sm();
  if(wireread_fail) return false;
  else return true;
}
/*
 *bool read_them_all(){
  read_byte_iter=0;
  read_reg_val=0;
  while(read_byte_iter<9){
    if(read_wire_byte((uint8_t *)&read_reg_val)){
        read_1wire_array[read_byte_iter]=read_reg_val;
        read_byte_iter++;
    }
    else {
        read_reg_val=0xFF;
        read_1wire_array[read_byte_iter]=read_reg_val;
        read_byte_iter++;
        return false;
    }
  }
  return true;
}
*/

// this returns true if it has reached 9 successful 1 byte reads of onewire communications.
// If wireread_fail is true at any point, then this will return false without incrementing read_byte_iter, and place 0xFE at read_byte_iter position in read_1wire_array
// if read_wire_byte is false at any point, same thing.
__attribute__((optimize(0))) bool read_them_all(){
  read_byte_iter=0;
  read_reg_val=0;
  while(1){
    if(read_wire_byte() && !wireread_fail){
        read_1wire_array[read_byte_iter]=read_reg_val;
        read_byte_iter++;
        if(read_byte_iter==9) return true;
    }
    else {
        read_1wire_array[read_byte_iter]=0xFE;
        return false;
    }
  }
}

// this function is used to store celsius from a calculation.
void print_celsius(){
  for(int i=0;i<9;i++){
    real_data_read[celsius_iter][i][which_sensor]=read_1wire_array[i];
  }
  int16_t raw = (read_1wire_array[1] << 8) | read_1wire_array[0];
  byte cfg = (read_1wire_array[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7; // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  float celsius_1 = (float)raw / 16.0;
  celsius[celsius_iter][which_sensor]=celsius_1;
  Serial.println(celsius_1,4);
  uint8_t crc=0;
  for (uint8_t i=0; i<8;i++){
    uint8_t inbyte = read_1wire_array[i];
    for (uint8_t j=0;j<8;j++){
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  crcs[celsius_iter][which_sensor]=crc;
  celsius_iter++;
  if(celsius_iter>=100) celsius_iter=0;
  tots_reads++;
  if(crc==read_1wire_array[8]){
      celsius_real=celsius_1;
      success_reads++;
  }
  else{
      celsius_real=-999;
  }
}

// no return value
//copies addrX declared above to usable array (8 bytes per entry of array ,so 2D array)
void copy_temp_addresses(){
  // seems to need this type of format in order to memcpy successfully?
  memcpy(&addr[0],(uint8_t *) &addr1, sizeof(addr1));
//  memcpy(&addr[1],(uint8_t *) &addr2, sizeof(addr2));
//  memcpy(&addr[2],(uint8_t *) &addr3, sizeof(addr3));
//  memcpy(&addr[3],(uint8_t *) &addr3, sizeof(addr4));

}
