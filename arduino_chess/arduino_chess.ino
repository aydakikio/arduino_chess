#include <U8g2lib.h>

// ===== HARDWARE CONFIGURATION =====
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, -1, A5, A4);

#define BTN_ACTION 12
#define BTN_DOWN 9
#define BTN_UP 6
#define block_size 8
#define piece_size 8

bool game_over = false;

// WHITE PIECES
const unsigned char white_pawn[] PROGMEM = {
  0x00, 0x3C, 0x24, 0x42, 0x42, 0x42, 0x24, 0x7E,
};

const unsigned char white_rook[] PROGMEM = {
  0x5A, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E, 
};

const unsigned char white_knight[] PROGMEM = {
  0x7E, 0x83, 0x8A, 0x63, 0x12, 0x13, 0x22, 0x7E,
};

const unsigned char white_bishop[] PROGMEM = {
  0x18, 0x24, 0x72, 0x5A, 0x42, 0x42, 0x42, 0x7E,
};

const unsigned char white_queen[] PROGMEM = {
  0x5A, 0x7E, 0x24, 0x18, 0x18, 0x24, 0x24, 0x7E, 
};

const unsigned char white_king[] PROGMEM = {
  0x18, 0x3C, 0x18, 0x3C, 0x24, 0x66, 0xC3, 0x7E,
};


//BLACK PIECES
const unsigned char black_rook[] PROGMEM = {
  0x00, 0x5A, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E,  
};

const unsigned char black_knight[] PROGMEM = {
  0x00, 0x7E, 0xFF, 0xF6, 0x7F, 0x1E, 0x1F, 0x3E, 
};

const unsigned char black_bishop[] PROGMEM = {
  0x00, 0x18, 0x1C, 0x4E, 0x66, 0x7E, 0x7E, 0x7E,
};

const unsigned char black_qween[] PROGMEM = {
  0x00, 0x5A, 0x7E, 0x3C, 0x18, 0x18, 0x3C, 0x3C,
};

const unsigned char black_king[] PROGMEM = {
  0x00, 0x18, 0x3C, 0x18, 0x3C, 0x3C, 0x7E, 0xFF,
};

const unsigned char black_pawn[] PROGMEM = {
  0x00, 0x3C, 0x7E, 0x7E, 0x7E, 0x3C, 0x18, 0x7E,
};

//==== FORWARD DECLARATION =======
void draw_game();
void draw_game_board();

void setup() {
  u8g2.begin();
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_ACTION, INPUT_PULLUP);
}

void loop() {
  if (!game_over) {
    draw_game();
    delay(100);
  }
}

void draw_game_board() {
  // Draw checkerboard pattern
  for(int row = 0; row < 8; row++) {
    for(int col = 0; col < 8; col++) {
      if ((row + col) % 2 == 1) {
        u8g2.drawBox(col * block_size, row * block_size, block_size, block_size);
      }
    }
  }
  u8g2.drawFrame(0, 0, 65, 64);
}

void draw_pieces(){
  // WHITE PIECES (bottom row)
  u8g2.drawXBMP(0, 0, piece_size, piece_size, white_rook);
  u8g2.drawXBMP(block_size , 0, piece_size, piece_size, white_knight);
  u8g2.drawXBMP(block_size * 2, 0, piece_size, piece_size, white_bishop);
  u8g2.drawXBMP(block_size * 3, 0, piece_size, piece_size, white_queen);
  u8g2.drawXBMP(block_size * 4, 0, piece_size, piece_size, white_king);
  u8g2.drawXBMP(block_size * 5, 0, piece_size, piece_size, white_bishop);
  u8g2.drawXBMP(block_size * 6, 0, piece_size, piece_size, white_knight);
  u8g2.drawXBMP(block_size * 7, 0, piece_size, piece_size, white_rook);
    
  // WHITE PAWNS
  for(int I = 0; I < 8; I++) {
    u8g2.drawXBMP(I * block_size, block_size, piece_size, piece_size, white_pawn);
  }
    

  //BLACK PIECES (buttom row)
  u8g2.drawXBMP(0,block_size*7, piece_size, piece_size, black_rook);
  u8g2.drawXBMP(block_size, block_size*7, piece_size, piece_size, black_knight);
  u8g2.drawXBMP(block_size *2, block_size*7, block_size, block_size, black_bishop);
  u8g2.drawXBMP(block_size *3, block_size*7, block_size, block_size, black_qween);
  u8g2.drawXBMP(block_size *4, block_size*7, block_size, block_size, black_king);
  u8g2.drawXBMP(block_size *5, block_size*7, block_size, block_size, black_bishop);
  u8g2.drawXBMP(block_size *6, block_size*7, piece_size, piece_size, black_knight);
  u8g2.drawXBMP(block_size *7, block_size*7, piece_size, piece_size, black_rook);

  for(int I =0; I<8 ; I++){
    u8g2.drawXBMP(I*block_size, block_size*6, block_size, block_size, black_pawn);
  }
}

void draw_game() {
  u8g2.firstPage();
  do {
    draw_game_board();
    draw_pieces();

  } while (u8g2.nextPage());
}