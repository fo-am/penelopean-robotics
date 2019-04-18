#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <nrf24l01.h>
#include <i2c-master.h>
#include <robot.h>

#define UPDATE_FREQ_HZ 10.0
#define PENELOPE_NUM_SERVOS 3

// Messages supported (all 32 bytes)
// P: Ping (flash LEDs twice)
// S: Sync servo sequence
// M: Update servo sequence
// T: Request telemetry
// W: Bulk write to yarn machine memory

// incoming data packets over the radio
// this is the original packet used for the motion sequencer
typedef struct {
  // [sequence speed depreciated, via registers instead]
  char pad;
  unsigned int speed;            // 2 (fixed -> *= 1000) 
  unsigned int length;           // 2 
  char pattern[MAX_PATTERN_LENGTH]; // 26
} seq_pattern_packet;                 // = 32 bytes

typedef struct {
  char pad; // 1
  unsigned short start_address;  // 2
  unsigned short size;           // 2
  char data[25];
} bulk_write_packet;

typedef struct {
  char pad;
  unsigned short start_address; // 2
} request_telemetry_packet;

typedef struct {
  char pad;
  unsigned short samples; // 2
} request_calibration_packet;

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
  nRF24L01p_config_pipe(nRF24L01p_PIPE_0, 0xA7A7A7A701, 32); // 0xA7A7A7A7A7
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

  // switch on indicator
  DDRB |= 0x01;
  PORTB |= 0x01;
  _delay_ms(1000);
  PORTB &= ~0x01;

  // init walk pattern (running starts as off)

  robot_t robot;
  robot_init(&robot);
  int calibrated=0;

  char msg[32];
  // clear packet as it might be possible to 
  // parse junk on startup if nrf status is set
  // due to noise but no message is present
  memset(&msg, 0, 32);

  for(;;) {    
    robot_tick(&robot);
    
    if (nRF24L01p_read_status(nRF24L01p_PIPE_0)) {
      nRF24L01p_read((void*)msg, 32, nRF24L01p_PIPE_0);
      // message recieved indicator
      PORTB |= 0x01;

      char msg_type = msg[0];

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
	if (!calibrated) {
	  PORTB |= 0x01;
	  gy91_calibrate_mag(p->samples);
	  PORTB &= ~0x01;
	  calibrated=1;
	}	
	float *calibration_data = gy91_mag_calibration_data();	
	// return calibration results for testing (sent on next ping)
 	nRF24L01p_enable_ack_payload();
	nRF24L01p_ack_payload(0,calibration_data,sizeof(float)*6);
	free(calibration_data);
      }
      
      if (msg_type=='S') {
	// sync
	robot.seq.timer = MAKE_FIXED(1.0); // cause next update to trigger 0
	robot.seq.position = 0;
      }
      
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

      // should we bounds check this stuff, do we care???

      // request telemetry information (1 message latency)
      if (msg_type=='T') {
	request_telemetry_packet *p=(request_telemetry_packet *)&msg[1];
 	nRF24L01p_enable_ack_payload();
	nRF24L01p_ack_payload(0,(byte*)&robot.machine.m_heap[p->start_address],32);
      }
      
      // update a register/carry out a command
      if (msg_type=='W') {
	bulk_write_packet *p = (bulk_write_packet*)&msg[1];	
	memcpy(&robot.machine.m_heap[p->start_address],
	       p->data,p->size);
      }
    }

    _delay_ms(RATE);
    PORTB &= ~0x01;
  }
}
