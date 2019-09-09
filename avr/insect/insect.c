// Penelopean Robotics Copyright (C) 2019 FoAM Kernow
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <nrf24l01.h>
#include <i2c-master.h>
#include <robot.h>
#include <insect.h>

#define ROBOT_ID 0x03
#define RADIO_ID 0xA7A7A7A704
#define RADIO_PI 0xA7A7A7A7AA

#define UPDATE_FREQ_HZ 50
#define PENELOPE_NUM_SERVOS 3
#define UPDATE_TICK_MS (1000/UPDATE_FREQ_HZ)

// Messages supported (all 32 bytes)
// P: Ping (flash LEDs twice)
// S: Sync servo sequence
// M: Update servo sequence
// T: Request telemetry
// H: Halt machine
// R: Reset and start machine
// W: Write to yarn machine memory
// E: Save current heap to eeprom
// C: Calibrate and return calibration data

ISR(TIMER1_COMPA_vect) {
  servo_pulse_update();
}

int main (void) {
  // set up the radio first
  _delay_us(nRF24L01p_TIMING_INITIAL_US);
  nRF24L01p_init(0,0);
  nRF24L01p_config_pipe(nRF24L01p_PIPE_0, RADIO_ID, 32); 
  nRF24L01p_config_pipe(nRF24L01p_PIPE_1, RADIO_PI, 32); 
  nRF24L01p_config_channel(100);
  nRF24L01p_config_auto_ack(nRF24L01p_MASK_EN_AA_ENAA_ALL, TRUE);
  nRF24L01p_config_transceiver_mode(nRF24L01p_VALUE_CONFIG_PRIM_RX);
  nRF24L01p_enable_ack_payload();
  char ack_payload[] = "Ack Payload\n";
  nRF24L01p_ack_payload(0,(const byte *)ack_payload,12);

  // now the rest of the peripherals
  i2c_init();
  gy91_init();
  servo_init();
  sei();   

  // init robot 
  robot_t robot;
  robot_init(&robot,ROBOT_ID);
  // load saved code
  //robot_read_ee_heap(&robot);

  // switch on indicator
  DDRB |= 0x01;
  PORTB |= 0x01;
  _delay_ms(1000);
  PORTB &= ~0x01;


  char msg[32];
  // clear packet as it might be possible to 
  // parse junk on startup if nrf status is set
  // due to noise but no message is present
  memset(&msg, 0, 32);

  for(;;) {    
    robot_tick(UPDATE_TICK_MS,&robot);
    
    if (nRF24L01p_read_status(nRF24L01p_PIPE_0)) {
      nRF24L01p_read((void*)msg, 32, nRF24L01p_PIPE_0);
      // message recieved indicator flash
      PORTB |= 0x01;

      char msg_type = msg[0];

      // dispatch to correct message (todo: break these out into functions)

      if (msg_type=='P') {
 	PORTB |= 0x01;
	_delay_ms(50);
	PORTB &= ~0x01;
	_delay_ms(50);
	PORTB |= 0x01;
	_delay_ms(50);
	PORTB &= ~0x01;
      }

      // calibrate sensors
      if (msg_type=='C') {
	request_calibration_packet *p=(request_calibration_packet *)&msg[1];

	if (p->calibrate==1) {
	  PORTB |= 0x01;
	  gy91_calibrate_mag(p->samples);
	  PORTB &= ~0x01;
	}	

	// return either sense data from rom...
	if (!p->return_sense) {
	  float *calibration_data = gy91_mag_calibration_data();	
	  nRF24L01p_enable_ack_payload();
	  nRF24L01p_ack_payload(0,(const byte *)calibration_data,sizeof(float)*9);
	  free(calibration_data);
	} else { // or calculated calibration data
	  float *sense_data = gy91_mag_sense_data();	
	  nRF24L01p_enable_ack_payload();
	  nRF24L01p_ack_payload(0,(const byte *)sense_data,sizeof(float)*3);
	  free(sense_data);
	}
      }
      
      // super sync
      if (msg_type=='S') {
	sync_packet *p=(sync_packet *)&msg[1];

	// cause next update to trigger pattern step 0
	robot.seq.timer = p->ms_per_step; 
	robot.seq.position = 0;
	robot.seq.ms_per_step=p->ms_per_step;
	// set ms_per_step reg too
	robot.machine.m_heap[REG_SERVO_MS_PER_STEP]=p->ms_per_step;

	// optionally set a bunch of registers
	if (p->reg_1!=0) robot.machine.m_heap[p->reg_1]=p->reg_1_val;
	if (p->reg_2!=0) robot.machine.m_heap[p->reg_2]=p->reg_2_val;
	if (p->reg_3!=0) robot.machine.m_heap[p->reg_3]=p->reg_3_val;
	if (p->reg_4!=0) robot.machine.m_heap[p->reg_4]=p->reg_4_val;
	if (p->reg_5!=0) robot.machine.m_heap[p->reg_5]=p->reg_5_val;
	if (p->reg_6!=0) robot.machine.m_heap[p->reg_6]=p->reg_6_val;
	if (p->reg_7!=0) robot.machine.m_heap[p->reg_7]=p->reg_7_val;

	// return heap info
 	nRF24L01p_enable_ack_payload();
	nRF24L01p_ack_payload(0,(const byte*)&robot.machine.m_heap[0],32);
      }
      
      // update servo pattern
      // msgtype id speed speed len data ...
      if (msg_type=='M') {
	//seq[message.id].speed = message.speed;
	seq_pattern_packet *p=(seq_pattern_packet *)&msg[1];
	robot.seq.ms_per_step = p->ms_per_step;
	robot.seq.length = p->length;
	for (int n=0; n<p->length*NUM_SERVOS; n++) {
	  robot.seq.pattern[n] = p->pattern[n];
	}
      }

      // request telemetry information (1 message latency)
      if (msg_type=='T') {
	request_telemetry_packet *p=(request_telemetry_packet *)&msg[1];
 	nRF24L01p_enable_ack_payload();
	nRF24L01p_ack_payload(0,(const byte*)&robot.machine.m_heap[p->start_address],32);
      }

      if (msg_type=='R') { robot_reset(&robot, ROBOT_ID); }
      if (msg_type=='H') { robot_halt(&robot); }

      // write into robot memory
      if (msg_type=='W') {
	// should we bounds check this stuff, do we care???
	bulk_write_packet *p = (bulk_write_packet*)&msg[1];	
	// copy over the data
	memcpy(&robot.machine.m_heap[p->start_address],p->data,p->size);
      }

      // save to eeprom
      if (msg_type=='E') { robot_write_ee_heap(&robot); }

      PORTB &= ~0x01;
    }

    _delay_ms(UPDATE_TICK_MS);
  }
}
