
#include <unistd.h>
#include <string.h>
#include <wiringPiI2C.h>
#include <time.h>

// commands
#define LCD_CLEARDISPLAY  0x01
#define LCD_RETURNHOME  0x02
#define LCD_ENTRYMODESET  0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT  0x10
#define LCD_FUNCTIONSET  0x20
#define LCD_SETCGRAMADDR  0x40
#define LCD_SETDDRAMADDR  0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT  0x00
#define LCD_ENTRYLEFT  0x02
#define LCD_ENTRYSHIFTINCREMENT  0x01
#define LCD_ENTRYSHIFTDECREMENT  0x00

// flags for display on/off control
#define LCD_DISPLAYON  0x04
#define LCD_DISPLAYOFF  0x00
#define LCD_CURSORON  0x02
#define LCD_CURSOROFF  0x00
#define LCD_BLINKON  0x01
#define LCD_BLINKOFF  0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE  0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT  0x04
#define LCD_MOVELEFT  0x00

// flags for function set
#define LCD_8BITMODE  0x10
#define LCD_4BITMODE  0x00
#define LCD_2LINE  0x08
#define LCD_1LINE  0x00
#define LCD_5x10DOTS  0x04
#define LCD_5x8DOTS  0x00

// flags for backlight control
#define LCD_BACKLIGHT  0x08
#define LCD_NOBACKLIGHT  0x00

#define En  0b00000100 // Enable bit
#define Rw  0b00000010 // Read/Write bit
#define Rs  0b00000001 // Register select bit

#define I2C_ADDRESS 0x27

typedef int bool;
enum { false, true };

int fd;
bool backlight = true;
char backlight_mask = LCD_BACKLIGHT;

#define DELAY_STROBE_1 200
#define DELAY_STROBE_2 200
#define DELAY_WRITE_COMMAND 200

#define DELAY_INIT 200
#define DELAY_INIT_SHORT 200

// clocks EN to latch command
void strobe(char data) {
  wiringPiI2CWrite(fd, (data & ~En) | backlight_mask);
  usleep(DELAY_STROBE_1);
  wiringPiI2CWrite(fd, data | En | backlight_mask);
  usleep(DELAY_STROBE_2);
  wiringPiI2CWrite(fd, ((data & ~En) | backlight_mask));
  usleep(DELAY_STROBE_1);
}


void write_four_bits(char data) {
  strobe(data);
}

void write_command(char cmd) {
  write_four_bits(0 | (cmd & 0xF0));
  write_four_bits(0 | ((cmd << 4) & 0xF0));
  usleep(DELAY_WRITE_COMMAND);
}
void write_data(char cmd) {
  write_four_bits(Rs | (cmd & 0xF0));
  write_four_bits(Rs | ((cmd << 4) & 0xF0));
  usleep(DELAY_WRITE_COMMAND);
}


void init() {
  wiringPiI2CWrite(fd, 0);
  usleep(50000);
  wiringPiI2CWrite(fd, (0 | backlight_mask));

  // strobe(0x03);
  // usleep(DELAY_INIT);

  // strobe(0x03);
  // usleep(DELAY_INIT);

  // strobe(0x03);
  // usleep(DELAY_INIT);

  // strobe(0x03);
  // usleep(DELAY_INIT);

  // strobe(0x02);
  // usleep(DELAY_INIT_SHORT);
  write_command(0x33);

  write_command(0x32);

  write_command(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);
  usleep(DELAY_INIT_SHORT);

  write_command(LCD_DISPLAYCONTROL);
  usleep(DELAY_INIT_SHORT);

  // write_command(LCD_CLEARDISPLAY);
  // usleep(DELAY_INIT);

  // write_command(LCD_CLEARDISPLAY);
  // usleep(DELAY_INIT);

  write_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
  usleep(DELAY_INIT_SHORT);

  write_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_BLINKOFF | LCD_CURSOROFF);
  usleep(DELAY_INIT_SHORT);

}


void set_backlight(bool value) {
  backlight = value;
  backlight_mask = backlight ? LCD_BACKLIGHT : LCD_NOBACKLIGHT;
  write_command(0x80);
}

void print(char * str, int line) {
  int i;
  static const int addr[] = {0x80, 0xC0, 0x94, 0xD4};
  write_command(addr[line]);
  for(i = 0; i < 16 && *str; ++i, *str++) {
    write_data(*str);
  }
}

int main(int argc, char** argv) {
  fd = wiringPiI2CSetup(I2C_ADDRESS);
  init();
  // init();
  set_backlight(true);
  write_command(LCD_CLEARDISPLAY);
  usleep(DELAY_INIT);
  print("Please wait...",0);
  printf("Done\n");
}
