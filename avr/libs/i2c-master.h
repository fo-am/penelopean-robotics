#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#define I2C_READ 0x01
#define I2C_WRITE 0x00

void i2c_init(void);
unsigned char i2c_start(unsigned char address);
unsigned char i2c_write(unsigned char data);
unsigned char i2c_read_ack(void);
unsigned char i2c_read_nack(void);
unsigned char i2c_transmit(unsigned char address, unsigned char* data, unsigned short length);
unsigned char i2c_receive(unsigned char address, unsigned char* data, unsigned short length);
unsigned char i2c_write_reg(unsigned char devaddr, unsigned char regaddr, unsigned char* data, unsigned short length);
unsigned char i2c_read_reg(unsigned char devaddr, unsigned char regaddr, unsigned char* data, unsigned short length);
void i2c_stop(void);

#endif // I2C_MASTER_H
