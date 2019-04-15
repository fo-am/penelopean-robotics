#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <nrf24l01.h>
#include <i2c-master.h>
#include <robot.h>

#define UPDATE_FREQ_HZ 10.0
#define PENELOPE_NUM_SERVOS 3

// registers

// 0x00: ping LEDs command
// 0x01: sync command 
// 0x03: servo 1 amp [fixed]
// 0x04: servo 1 bias_degrees
// 0x05: servo 1 sequence speed [fixed]
// 0x06: servo 1 smoothing [fixed]
// 0x07 - 0x0a servo 2
// 0x0b - 0x0e servo 3

// new packet for modifying registers
typedef struct {
  unsigned char id;              // 1 - not needed
  unsigned int address;
  int value; // could be fixed point in some circumstances
} register_write_packet;

// incoming data packet over the radio
// this is the original packet used for the motion sequencer
typedef struct {
  unsigned char id;              // 1 - not needed
  // [sequence speed depreciated, via registers instead]
  unsigned int speed;            // 2 (fixed -> *= 1000) 
  unsigned int length;           // 2 
  char pattern[MAX_PATTERN_LENGTH]; // 26
} seq_pattern_packet;                 // = 32 bytes


ISR(TIMER1_COMPA_vect) {
  servo_pulse_update();
}

// for switching between tx and rx via standby I
void nRF24L01p_reboot() {
  nRF24L01p_disable();
  _delay_ms(10);
  nRF24L01p_enable();
  _delay_ms(10);
}

#define RATE 20
const unsigned int telemetry_count = 100;
unsigned int telemetry_tick = 0;

int main (void) {
  _delay_us(nRF24L01p_TIMING_INITIAL_US);
  nRF24L01p_init(0, 0);
  nRF24L01p_config_pipe(nRF24L01p_PIPE_0, 0xA7A7A7A7A7, 32); 
  nRF24L01p_config_pipe(nRF24L01p_PIPE_1, 0xA7A7A7A7AA, 32); 
  nRF24L01p_config_channel(100);
  nRF24L01p_config_auto_ack(nRF24L01p_MASK_EN_AA_ENAA_ALL, TRUE);
  nRF24L01p_config_transceiver_mode(nRF24L01p_VALUE_CONFIG_PRIM_RX);
  //nRF24L01p_config_transceiver_mode(nRF24L01p_VALUE_CONFIG_PRIM_TX);

  nRF24L01p_enable_ack_payload();
  char ack_payload[] = "Ack Payload\n";
  nRF24L01p_ack_payload(0,ack_payload,12);
  
  i2c_init();
  gy91_init();

  servo_init();
  sei();   

  DDRB |= 0x01;
  PORTB |= 0x01;
  _delay_ms(1000);
  PORTB &= ~0x01;

  // init walk pattern (running starts as off)

  robot_t robot;
  robot_init(&robot);

  char msg[32];
  // clear packet as it might be possible to 
  // parse junk on startup if nrf status is set
  // due to noise but no message is present
  memset(&msg, 0, 32);

  float x,y,z;
  
  for(;;) {    
    robot_tick(&robot);
    
    /*    gy91_read_accel(&x,&y,&z);
    
    if (y>0.1) {
      PORTB |= 0x01;
      _delay_ms(50);
      } */
    
    if (nRF24L01p_read_status(nRF24L01p_PIPE_0)) {
      nRF24L01p_read((void*)msg, 32, nRF24L01p_PIPE_0);
      PORTB |= 0x01;

      char msg_type = msg[0];
      
      // update servo pattern
      // msgtype id speed speed len data ...
      if (msg_type=='M') {
	//seq[message.id].speed = message.speed;
	seq_pattern_packet *p=(seq_pattern_packet *)&msg[1];
	robot.seq.speed = p->speed;
	robot.seq.length = p->length;
	for (int n=0; n<p->length*NUM_SERVOS; n++) {
	  robot.seq.pattern[n] = p->pattern[n];
	}
      }

      // returns telemetry information (1 message latency)
      if (msg_type=='T') {
	nRF24L01p_enable_ack_payload();
	nRF24L01p_ack_payload(0,(byte *)robot.machine.m_heap,32);	
      }
      
      // update a register/carry out a command
      if (msg_type=='W') {
	register_write_packet *p = (register_write_packet*)&msg[1];	
	switch (p->address) {  
	
	  // flash LEDs
	case 0x00 : {	
	  PORTB |= 0x01;
	  _delay_ms(50);
	  PORTB &= ~0x01;
	  _delay_ms(50);
	  PORTB |= 0x01;
	  _delay_ms(50);
	  PORTB &= ~0x01;
	} break;
	
	  // sync
	case 0x02: {
	  robot.seq.timer = MAKE_FIXED(1.0); // cause next update to trigger 0
	  robot.seq.position = 0;
	} break;

	}	
      }
    }

    /*    if (telemetry_tick>telemetry_count) {
      telemetry_tick=0;
      PORTB |= 0x01;
      // Set TX mode.
      nRF24L01p_config_transceiver_mode(nRF24L01p_VALUE_CONFIG_PRIM_TX);
      nRF24L01p_disable();
      //nRF24L01p_reboot();
      nRF24L01p_config_transceiver_mode(nRF24L01p_VALUE_CONFIG_PRIM_TX);

      nRF24L01p_write((byte *)robot.machine.m_heap, 32, nRF24L01p_PIPE_1);
      _delay_ms(100);

      ////nRF24L01p_status_tx_sent_clear();


      nRF24L01p_disable();
     nRF24L01p_config_transceiver_mode(nRF24L01p_VALUE_CONFIG_PRIM_RX);

  // Flush the FIFOs
  nRF24L01p_tx_fifo_flush();
  nRF24L01p_rx_fifo_flush();

  // Clear the interrupts.
  nRF24L01p_status_rx_ready_clear();
  nRF24L01p_status_tx_sent_clear();
  nRF24L01p_status_max_retries_clear();


      nRF24L01p_enable();
      
  //    nRF24L01p_reboot();
      nRF24L01p_config_transceiver_mode(nRF24L01p_VALUE_CONFIG_PRIM_RX);

      PORTB &= ~0x01;
    }

    telemetry_tick++;
    */
    _delay_ms(RATE);
    PORTB &= ~0x01;
  }
}
