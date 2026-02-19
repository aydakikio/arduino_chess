#include <U8g2lib.h>

// ===== HARDWARE CONFIGURATION =====
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, -1, A5, A4);

#define BTN_ACTION 12

#define BTN_DOWN 9
#define BTN_UP 6

#define BTN_RIGHT 10
#define BTN_LEFT 7


// ===== DRAWING HELPERS =====
#define block_size 8
#define piece_size 8

// ===== PIECE DEFINITIONS =====

#define EMPTY 0

// white pieces
#define WHITE_PAWN 1
#define WHITE_ROOK  2
#define WHITE_KNIGHT 3
#define WHITE_BISHOP 4
#define WHITE_QUEEN 5
#define WHITE_KING  6

// black pieces
#define BLACK_PAWN  -1
#define BLACK_ROOK  -2
#define BLACK_KNIGHT -3
#define BLACK_BISHOP -4
#define BLACK_QUEEN -5
#define BLACK_KING  -6

// ===== GAME STATUS =====
bool game_over = false;
bool white_turn = true;

// ===== CURSOR & SELECTION =====
int cursor_col = 0, cursor_row = 7;
int sel_col = -1, sel_row = -1;
bool piece_selected = false;

// ===== SPRITES =====

// WHITE PIECES
const unsigned char white_knight[] PROGMEM = {0x7E, 0x83, 0x8A, 0x63, 0x12, 0x13, 0x22, 0x7E,};
const unsigned char white_bishop[] PROGMEM = {0x18, 0x24, 0x72, 0x5A, 0x42, 0x42, 0x42, 0x7E,};
const unsigned char white_queen[] PROGMEM = {0x5A, 0x7E, 0x24, 0x18, 0x18, 0x24, 0x24, 0x7E,};
const unsigned char white_king[] PROGMEM = {0x18, 0x3C, 0x18, 0x3C, 0x24, 0x66, 0xC3, 0x7E,};
const unsigned char white_pawn[] PROGMEM = {0x00, 0x3C, 0x24, 0x42, 0x42, 0x42, 0x24, 0x7E,};
const unsigned char white_rook[] PROGMEM = {0x5A, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E,};

//BLACK PIECES
const unsigned char black_knight[] PROGMEM = {0x00, 0x7E, 0xFF, 0xF6, 0x7F, 0x1E, 0x1F, 0x3E,};
const unsigned char black_bishop[] PROGMEM = {0x00, 0x18, 0x1C, 0x4E, 0x66, 0x7E, 0x7E, 0x7E,};
const unsigned char black_qween[] PROGMEM = {0x00, 0x5A, 0x7E, 0x3C, 0x18, 0x18, 0x3C, 0x3C,};
const unsigned char black_rook[] PROGMEM = {0x00, 0x5A, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E,};
const unsigned char black_king[] PROGMEM = {0x00, 0x18, 0x3C, 0x18, 0x3C, 0x3C, 0x7E, 0xFF,};
const unsigned char black_pawn[] PROGMEM = {0x00, 0x3C, 0x7E, 0x7E, 0x7E, 0x3C, 0x18, 0x7E,};

// ===== BOARD STATE =====
int board[8][8] = {
  {BLACK_ROOK, BLACK_KNIGHT , BLACK_BISHOP , BLACK_QUEEN , BLACK_KING , BLACK_BISHOP , BLACK_KNIGHT , BLACK_ROOK },
  {BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN },
  {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
  {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
  {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
  {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
  {WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN },
  {WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK },
};

// ==== FORWARD DECLARATION =====
void draw_game();
void draw_game_board();

void setup() {
  u8g2.begin();
    
  //buttons
  pinMode(BTN_ACTION, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  
}

void loop() {
  if (!game_over) {
    draw_game();
    delay(100);
  }
}

void draw_game_board() {
  // Draw checkerboard pattern O(N)
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if ((row + col) % 2 == 1) {
        u8g2.drawBox(col * block_size, row * block_size, block_size, block_size);
      }
    }
  }
  u8g2.drawFrame(0, 0, 65, 64);
}


void draw_piece_at(int row, int col, int piece){
  int draw_x = col * block_size;
  int draw_y = row * block_size;

  const unsigned char* piece_bitmap = nullptr;

  switch (piece) {
    case WHITE_PAWN:
        piece_bitmap = white_pawn;
      break;
    case WHITE_ROOK:
        piece_bitmap = white_rook;
      break;
    case WHITE_KNIGHT:
        piece_bitmap = white_knight;
      break;
    case WHITE_BISHOP:
        piece_bitmap = white_bishop;
      break;
    case WHITE_QUEEN:
        piece_bitmap = white_queen;
      break;
    case WHITE_KING:
        piece_bitmap = white_king;
      break;
    case BLACK_PAWN:
        piece_bitmap = black_pawn;
      break;
    case BLACK_ROOK:
        piece_bitmap = black_rook;
      break;
    case BLACK_KNIGHT:
        piece_bitmap = black_knight;
      break;
    case BLACK_BISHOP:
        piece_bitmap = black_bishop;
      break;
    case BLACK_QUEEN:
        piece_bitmap = black_qween;
      break;
    case BLACK_KING:
        piece_bitmap = black_king;
      break;
  }

  if (piece_bitmap) {
    u8g2.drawXBMP(draw_x, draw_y, piece_size, piece_size, piece_bitmap);
  }
}

void draw_pieces(){
  for(int row = 0; row <8; row++){
    for(int col = 0; col <8; col++){
      draw_piece_at(row, col, board[row][col]);
    }
  }
}

void draw_game() {
  u8g2.firstPage();
  do {
    draw_game_board();
    draw_pieces();

  } while (u8g2.nextPage());
}