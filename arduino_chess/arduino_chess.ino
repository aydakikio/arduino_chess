#include <U8g2lib.h>


// ===== HARDWARE CONFIGURATION =====
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, -1, A5, A4);

#define BTN_ACTION 12
#define BTN_DOWN 9
#define BTN_UP 6

#define block_size 9

bool game_over = false;


void draw_game();

void setup() {
  u8g2.begin();

  // Initialize button pins
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_ACTION, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (!game_over) {
    draw_game();
  }
}

void draw_game_board(){
  for(int I=0; I<=6; I+=2){
    u8g2.drawBox(0,I*block_size,block_size,block_size);
    u8g2.drawBox(18,I*block_size,block_size,block_size);
    u8g2.drawBox(36,I*block_size,block_size,block_size);
    u8g2.drawBox(54,I*block_size,block_size,block_size);
    if(I <=4){
      u8g2.drawBox(9,(I+1)*block_size,block_size,block_size);
      u8g2.drawBox(27,(I+1)*block_size,block_size,block_size);
      u8g2.drawBox(45,(I+1)*block_size,block_size,block_size);
    }

  }

  u8g2.drawFrame(0,0,64,64);
}


void draw_game(){
  u8g2.firstPage();

  do {
    draw_game_board();

  }while (u8g2.nextPage());
}