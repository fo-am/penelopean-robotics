/* Here in lies a concerted and complete effort to get my Nordic
 * nRF24L01+ working. I've had this thing collecting dust for
 * about a year, and it's time to buckle down and figure it out.
 */

//#include <avrm/uart.h>
#include <nrf24l01.h>
#include <util/delay.h>

int main(void)
{
  // Setup the UART, necessary for stdio actions.
  //uart_init();

  // Wait for the nRF24L01p to be ready.
  _delay_us(nRF24L01p_TIMING_INITIAL_US);

  DDRC = 0xff;
  PORTC = 0xff;

  // Initialize the nRF24L01p.
  // TODO: Actual info for arguments.
  nRF24L01p_init(0, 0);

  // Enable pipe 0.
  nRF24L01p_config_pipe(nRF24L01p_PIPE_0, 0x0101010101, 32);

  // Set RX mode.
  nRF24L01p_config_transceiver_mode(nRF24L01p_VALUE_CONFIG_PRIM_RX);

  byte str[96];

  PORTC = 0x00;

  while (1)
  {
    if (nRF24L01p_read_status(nRF24L01p_PIPE_0))
    {
      nRF24L01p_read(str, 96, nRF24L01p_PIPE_0);
      
      if (str[0]=='H' && 
	  str[1]=='E' && 
	  str[2]=='L' && 
	  str[3]=='O') {
	PORTC = 0xff;
	_delay_ms(50);
	PORTC = 0x00;
      }
      
      //printf("%s\n", str);
    }
    _delay_ms(50);
  }

  return 0;
}
