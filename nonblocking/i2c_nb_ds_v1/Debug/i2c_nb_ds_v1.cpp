#include "Energia.h"

#line 1 "/home/mcbride.342/workspace_v11/i2c_nb_ds_v1/i2c_nb_ds_v1.ino"
#include "Wire_nonblocking_v2.h"
#include <sysctl.h>

void setup();
void loop();
void switch_LED();
void switch_LED_2();
uint8_t CheckStates();
bool i2c_wstate(uint8_t byte1, uint8_t byte2, int num_bytes);
bool i2c_rstate(uint8_t * buffer);
bool write_i2c_one_byte(uint8_t byte1);
bool write_i2c_two_bytes(uint8_t byte1, uint8_t byte2);
bool read_i2c_one_byte(uint8_t * buffer);
bool write_a_reset();
bool write_a_config();
bool do_a_config();
bool write_channel_select();
bool select_a_channel();
bool status_reg_set();
bool check_1WB_SM();
uint16_t check_1WB();
bool wire_reset();
bool write_wire_byte(uint8_t byte_to_write);
bool write_wire_byte_sm(uint8_t byte_to_write);
bool new_wire_select();
bool wire_convert();
bool wire_temp_req();
bool read_reg_set();
bool read_wire_byte_sm();
bool read_wire_byte();
bool read_them_all();
void print_celsius();
void copy_temp_addresses();

#line 4
TwoWire *wire_ = new TwoWire(2);
uint8_t mAddress=0x18;

byte addr1[8]={0x28,0x64,0x27,0xD2,0x75,0x22,0x01,0x7A};
byte addr2[8]={0x28,0x6A,0xC6,0x79,0x97,0x06,0x03,0x8D};
byte addr3[8]={0x28,0x0E,0xA4,0x79,0x97,0x06,0x03,0x00};
byte addr4[8]={0x28,0x75,0xBC,0x79,0x97,0x06,0x03,0x22};
unsigned long update_time=0;
#define UPDATE_PERIOD 10000
unsigned long LED_time=0;
#define LED_PERIOD 1000
unsigned long packet_time=0;
#define PACKET_PERIOD 20000
bool ppd_set=false;
uint8_t read_1wire_array[9]={0};
#define NUM_TEMP_SENSORS 1
byte addr[NUM_TEMP_SENSORS][8];
int which_sensor=0;
bool is_high=true;


#define LED_PIN RED_LED
#define LED_PIN_2 GREEN_LED
bool is_high_2=true;


typedef enum {
  SENSOR_STATE_IDLE = 0, 
  SENSOR_STATE_RESET = 1, 
  SENSOR_STATE_CHECKTIMER1 = 2, 
  SENSOR_STATE_CONFIG = 3, 
  SENSOR_STATE_SELECTCHANNEL = 4, 
  SENSOR_STATE_RESETWIRE1 = 5, 
  SENSOR_STATE_SELECTWIRE1 = 6, 
  SENSOR_STATE_CONVERTWIRE = 7, 
  SENSOR_STATE_CHECKTIMER2 = 8, 
  SENSOR_STATE_RESETWIRE2 = 9, 
  SENSOR_STATE_SELECTWIRE2 = 10, 
  SENSOR_STATE_REQUESTWIRE = 11, 
  SENSOR_STATE_READDATA = 12, 
} SensorState;
SensorState BRIDGE_STATE=SENSOR_STATE_IDLE;

typedef enum {
  I2C_WRITE_IDLE = 0, 
  I2C_WRITE_BEGIN = 1, 
  I2C_WRITE_WRITE = 2, 
  I2C_WRITE_END = 3, 
  I2C_WRITE_COMPLETE = 4, 
} I2CWriteState;
I2CWriteState I2C_WSTATE=I2C_WRITE_IDLE;


typedef enum {
  I2C_READ_IDLE = 0, 
  I2C_READ_REQ = 1, 
  I2C_READ_WAIT_AFTER_REQ=2, 
  I2C_READ_AVAIL = 3, 
  I2C_READ_READ = 4, 
  I2C_READ_COMPLETE = 5, 
} I2CReadState;
I2CReadState I2C_RSTATE=I2C_READ_IDLE;


typedef enum {
  SELECT_BEGIN = 0, 
  SELECT_WRITE = 1, 
  SELECT_CHECK1WB = 2, 
  SELECT_COMPLETE = 3, 
} WireSelectState;
WireSelectState WIRESELECT_STATE=SELECT_BEGIN;


typedef enum {
  WIREW_BEGIN = 0, 
  WIREW_WRITE = 1, 
  WIREW_CHECK1WB = 2, 
  WIREW_COMPLETE = 3, 
} WireWriteState;
WireWriteState WIREWRITE_STATE=WIREW_BEGIN;


typedef enum {
  WIRER_BEGIN = 0, 
  WIRER_SR_SET = 1, 
  WIRER_CHECK1WB_1 = 2, 
  WIRER_GEN_READ = 3, 
  WIRER_CHECK1WB_2 = 4, 
  WIRER_RR_SET = 5, 
  WIRER_READ = 6, 
  WIRER_COMPLETE = 7, 
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







float celsius[100][4]={0};
float celsius_real=0;
int celsius_iter=0;
uint8_t real_data_read[100][9][4]={0};
uint8_t crcs[100][4]={0};





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

WireWriteState PREV_WIREWRITE_STATE=WIREW_BEGIN;
WireReadState PREV_WIREREAD_STATE=WIRER_BEGIN;
Check1WBState PREV_CHECK1WB_STATE=WIREBUSY_BEGIN;
int i2c_rtries=0;
int i2c_wtries=0;
bool test_1wb_sm=false;
void setup() {
  
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
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
        break;
      }
      else break;
    }
    case SENSOR_STATE_RESET:{
      if(write_a_reset()){
        timer_post_reset = millis() + 100; 
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
        break;
      }
      else {
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE;
        break;
      }
      
    }
    case SENSOR_STATE_CHECKTIMER1:{
      if((long) (millis()- timer_post_reset) > 0){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
        break;
      }
      else break;
    }
    case SENSOR_STATE_CONFIG:{
      if(do_a_config()){
        
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
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
        
        read_i2c_one_byte((uint8_t *) &channel_read_val);
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
        break;
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE;
        break;
      }
    }
    case SENSOR_STATE_RESETWIRE1:{  
      if(status_reg_set()){ 
        status_1wb=check_1WB(); 
        if(!did_check1wb_fail){
          wire_reset();
          status_1wb=check_1WB();
          if(!did_check1wb_fail){
            if(status_1wb & 0x0002){
              PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
              BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
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
    case SENSOR_STATE_SELECTWIRE1:{  
      if(new_wire_select()){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
        break;        
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE; 
      }
    }
    case SENSOR_STATE_CONVERTWIRE:{  
      if(wire_convert()){
        timer_post_reset = millis() + 100; 
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
        break;
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE; 
      }
    }
    case SENSOR_STATE_CHECKTIMER2:{
      if((long) (millis()- timer_post_reset) > 0){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
        break;
      }
      else break;
    }
    case SENSOR_STATE_RESETWIRE2:{  
      status_reg_set(); 
      status_1wb=check_1WB(); 
      if(!did_check1wb_fail){
        wire_reset();
        status_1wb=check_1WB();
        if(!did_check1wb_fail){
          if(status_1wb & 0x0002){
            PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
            BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
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
    case SENSOR_STATE_SELECTWIRE2:{  
      if(new_wire_select()){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
        break;        
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE; 
      }
    }
    case SENSOR_STATE_REQUESTWIRE:{  
      if(wire_temp_req()){
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=(SensorState) ((unsigned char) BRIDGE_STATE + 1);  
        break;    
      }
      else{
        PREV_BRIDGE_STATE=(SensorState) BRIDGE_STATE;
        BRIDGE_STATE=SENSOR_STATE_IDLE; 
      }
    }
    case SENSOR_STATE_READDATA:{  
      
      got_to_readdata++;
      if(read_them_all()){
        print_celsius();
        
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
          I2C_RSTATE=I2C_READ_REQ;  
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


bool write_a_reset(){
  ppd_set=false;
  if(write_i2c_one_byte(0xF0)) return true;
  else return false;
}
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




















__attribute__((optimize(0))) uint16_t check_1WB(){
  CHECK1WB_STATE=WIREBUSY_BEGIN;
  test_1wb_sm=false;
  while(!test_1wb_sm) test_1wb_sm=check_1WB_SM();
  if(did_check1wb_fail) return (0x00FF & (uint16_t) (status_reg_val));
  else{
      
      
      return ((0x0100) | (0x00FF & (uint16_t) (status_reg_val)));
  }
}

bool wire_reset(){
  ppd_set=false;
  write_i2c_one_byte(0xB4);
  return true;
}


bool write_wire_byte(uint8_t byte_to_write){
  WIREWRITE_STATE=WIREW_BEGIN;
  bool test_wirewrite_sm=false;
  while(!test_wirewrite_sm) test_wirewrite_sm=write_wire_byte_sm(byte_to_write);
  if(wirewrite_fail) return false;
  else return true;
}

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
      if(status_reg_set()){ 
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
      if(read_reg_set()){ 
        
        PREV_WIREREAD_STATE=WIREREAD_STATE;
        WIREREAD_STATE=WIRER_READ;
        return false;
      }
      else{
        
        wireread_fail=true;
        return true;
      }
    }
    case WIRER_READ:{
        
      
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


__attribute__((optimize(0))) bool read_wire_byte(){
  WIREREAD_STATE=WIRER_BEGIN;
  wireread_fail=false;
  read_reg_val=0;
  
  test_wire_read=false;
  while(!test_wire_read) test_wire_read=read_wire_byte_sm();
  if(wireread_fail) return false;
  else return true;
}























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


void print_celsius(){
  for(int i=0;i<9;i++){
    real_data_read[celsius_iter][i][which_sensor]=read_1wire_array[i];
  }
  int16_t raw = (read_1wire_array[1] << 8) | read_1wire_array[0];
  byte cfg = (read_1wire_array[4] & 0x60);
  
  if (cfg == 0x00) raw = raw & ~7; 
  else if (cfg == 0x20) raw = raw & ~3; 
  else if (cfg == 0x40) raw = raw & ~1; 
  float celsius_1 = (float)raw / 16.0;
  celsius[celsius_iter][which_sensor]=celsius_1;
  
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



void copy_temp_addresses(){
  
  memcpy(&addr[0],(uint8_t *) &addr1, sizeof(addr1));




}



