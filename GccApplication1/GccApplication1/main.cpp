/*
 * GccApplication1.cpp
 *
 * Created: 29/11/2024 08:36:26 p. m.
 * Author : emili
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define LCD_I2C_ADDR 0x27

// Definiciones de bits para control de LCD
#define LCD_EN 2
#define LCD_RW 1
#define LCD_RS 0
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_BACKLIGHT 3

void I2C_init() {
	TWSR = 0x00;
	TWBR = 72;  // Para 100kHz a 16MHz
	TWCR = (1 << TWEN);
}

void I2C_start() {
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
}

void I2C_stop() {
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
	_delay_us(10);
}

void I2C_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

void LCD_write_nibble(uint8_t nibble, uint8_t mode) {
	uint8_t data = nibble | mode | (1 << LCD_BACKLIGHT);
	
	I2C_start();
	I2C_write(LCD_I2C_ADDR << 1);
	
	// Enviar con enable
	I2C_write(data | (1 << LCD_EN));
	_delay_us(50);
	
	// Enviar sin enable
	I2C_write(data & ~(1 << LCD_EN));
	_delay_us(50);
	
	I2C_stop();
}

void LCD_send_cmd(uint8_t cmd) {
	uint8_t highnib = cmd & 0xF0;
	uint8_t lownib = cmd << 4;
	
	LCD_write_nibble(highnib, 0);
	LCD_write_nibble(lownib, 0);
	_delay_ms(5);
}

void LCD_send_data(uint8_t data) {
	uint8_t highnib = data & 0xF0;
	uint8_t lownib = data << 4;
	
	LCD_write_nibble(highnib, (1 << LCD_RS));
	LCD_write_nibble(lownib, (1 << LCD_RS));
	_delay_ms(5);
}

void LCD_init() {
	_delay_ms(50);
	
	// Secuencia de inicialización para LCD 4-bit
	LCD_write_nibble(0x30, 0);
	_delay_ms(5);
	LCD_write_nibble(0x30, 0);
	_delay_ms(5);
	LCD_write_nibble(0x30, 0);
	_delay_ms(5);
	
	// Cambiar a modo 4 bits
	LCD_write_nibble(0x20, 0);
	_delay_ms(5);
	
	// Configuraciones
	LCD_send_cmd(0x28); // 4-bit, 2 líneas, 5x8 caracteres
	LCD_send_cmd(0x0C); // Display on, cursor off
	LCD_send_cmd(0x06); // Incrementar cursor
	LCD_send_cmd(0x01); // Limpiar pantalla
	_delay_ms(5);
}

void LCD_print(const char* str) {
	while (*str) {
		LCD_send_data(*str);
		str++;
	}
}

int main() {
	I2C_init();
	LCD_init();
	
	// Limpiar pantalla
	LCD_send_cmd(0x01);
	_delay_ms(10);
	
	// Posicionar cursor al inicio
	LCD_send_cmd(0x80);
	
	// Imprimir mensaje
	LCD_print("Prueba 1 2 3");
	
	while(1) {
		// Bucle infinito
	}
	
	return 0;
}
