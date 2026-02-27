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
uint8_t win_state = -1;
bool still_in_check = false;

// ===== CURSOR & SELECTION =====
int cursor_col = 0, cursor_row = 7;
int sel_col = -1, sel_row = -1;
bool piece_selected = false;

//Controls 
unsigned long las_btn_time =0;

bool valid_moves[8][8] = {};

// ===== SPRITES =====

// WHITE PIECES
const unsigned char white_knight[] PROGMEM = {0x00,0x7E, 0x83, 0x8A, 0x63, 0x12, 0x13, 0x3E,};
const unsigned char white_bishop[] PROGMEM = {0x00,0x18, 0x24, 0x72, 0x5A, 0x42, 0x42, 0x7E,};
const unsigned char white_queen[] PROGMEM = {0x00, 0x5A, 0x7E, 0x24, 0x18, 0x18, 0x24, 0x7E,};
const unsigned char white_king[] PROGMEM = {0x00, 0x18, 0x3C, 0x18, 0x3C, 0x24, 0x66, 0xFF,};
const unsigned char white_pawn[] PROGMEM = {0x00, 0x3C, 0x24, 0x42, 0x42, 0x42, 0x24, 0x7E,};
const unsigned char white_rook[] PROGMEM = {0x00,0x5A, 0x7E, 0x42, 0x42, 0x42, 0x42,  0x7E,};

//BLACK PIECES
const unsigned char black_knight[] PROGMEM = {0x00, 0x7E, 0xFF, 0xF6, 0x7F, 0x1E, 0x1F, 0x3E,};
const unsigned char black_bishop[] PROGMEM = {0x00, 0x18, 0x1C, 0x4E, 0x66, 0x7E, 0x7E, 0x7E,};
const unsigned char black_qween[] PROGMEM = {0x00, 0x5A, 0x7E, 0x3C, 0x18, 0x18, 0x3C, 0x3C,};
const unsigned char black_rook[] PROGMEM = {0x00, 0x5A, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E,};
const unsigned char black_king[] PROGMEM = {0x00, 0x18, 0x3C, 0x18, 0x3C, 0x3C, 0x7E, 0xFF,};
const unsigned char black_pawn[] PROGMEM = {0x00, 0x3C, 0x7E, 0x7E, 0x7E, 0x3C, 0x18, 0x7E,};

//Others
const unsigned char optional_move[] PROGMEM = {0x00, 0x42 , 0x24 , 0x18 , 0x18 , 0x24 , 0x42 , 0x00};

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
void handle_inputs();
void reset_game();

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
    if(white_turn){
      handle_inputs();
    }else{
      //AI thinks 
    }
    draw_game();
    delay(100);
  } else {

    draw_game_over();
    if(digitalRead(BTN_ACTION)== LOW){

      reset_game();

      delay(200);
    }
  }
}

void reset_game(){
  // Reset everything
  game_over = false;
  white_turn = true;
  piece_selected = false;
  cursor_row = 7; cursor_col = 0;
  sel_row = sel_col = -1;
  win_state = -1;
  memset(valid_moves, 0, sizeof(valid_moves));


  // Restore board
  int fresh[8][8] = {
    {BLACK_ROOK,BLACK_KNIGHT,BLACK_BISHOP,BLACK_QUEEN,BLACK_KING,BLACK_BISHOP,BLACK_KNIGHT,BLACK_ROOK},
    {BLACK_PAWN,BLACK_PAWN,BLACK_PAWN,BLACK_PAWN,BLACK_PAWN,BLACK_PAWN,BLACK_PAWN,BLACK_PAWN},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {WHITE_PAWN,WHITE_PAWN,WHITE_PAWN,WHITE_PAWN,WHITE_PAWN,WHITE_PAWN,WHITE_PAWN,WHITE_PAWN},
    {WHITE_ROOK,WHITE_KNIGHT,WHITE_BISHOP,WHITE_QUEEN,WHITE_KING,WHITE_BISHOP,WHITE_KNIGHT,WHITE_ROOK},
  };

  memcpy(board, fresh, sizeof(board));
}

void compute_valid_moves(int row, int col) {
  // Clear first
  memset(valid_moves, 0, sizeof(valid_moves));
  
  int piece = board[row][col];
  if (piece == 0) return;

  bool is_white = piece > 0;

  switch (piece) {
    case WHITE_PAWN: {
      // Move forward (row decreases for white)
      if (row > 0 && board[row-1][col] == EMPTY) {
        valid_moves[row-1][col] = true;

        if (row == 6 && board[row-2][col] == EMPTY)
          valid_moves[row-2][col] = true;
      }
      // Captures
      if (row > 0 && col > 0 && board[row-1][col-1] < 0) valid_moves[row-1][col-1] = true;
      if (row > 0 && col < 7 && board[row-1][col+1] < 0) valid_moves[row-1][col+1] = true;
      break;
    }
    case BLACK_PAWN: {
      if (row < 7 && board[row+1][col] == EMPTY) {
        valid_moves[row+1][col] = true;
        if (row == 1 && board[row+2][col] == EMPTY)
          valid_moves[row+2][col] = true;
      }
      if (row < 7 && col > 0 && board[row+1][col-1] > 0) valid_moves[row+1][col-1] = true;
      if (row < 7 && col < 7 && board[row+1][col+1] > 0) valid_moves[row+1][col+1] = true;
      break;
    }
    case WHITE_ROOK: case BLACK_ROOK: {
      // 4 directions
      int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
      for (auto& d : dirs) {
        int r = row + d[0], c = col + d[1];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
          if (board[r][c] == EMPTY) { valid_moves[r][c] = true; }
          else { if ((board[r][c] > 0) != is_white) valid_moves[r][c] = true; break; }
          r += d[0]; c += d[1];
        }
      }
      break;
    }
    case WHITE_BISHOP: case BLACK_BISHOP: {
      int dirs[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
      for (auto& d : dirs) {
        int r = row + d[0], c = col + d[1];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
          if (board[r][c] == EMPTY) { valid_moves[r][c] = true; }
          else { if ((board[r][c] > 0) != is_white) valid_moves[r][c] = true; break; }
          r += d[0]; c += d[1];
        }
      }
      break;
    }
    case WHITE_QUEEN: case BLACK_QUEEN: {
      int dirs[8][2] = {{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}};
      for (auto& d : dirs) {
        int r = row + d[0], c = col + d[1];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
          if (board[r][c] == EMPTY) { valid_moves[r][c] = true; }
          else { if ((board[r][c] > 0) != is_white) valid_moves[r][c] = true; break; }
          r += d[0]; c += d[1];
        }
      }
      break;
    }
    case WHITE_KNIGHT: case BLACK_KNIGHT: {
      int jumps[8][2] = {{-2,-1},{-2,1},{2,-1},{2,1},{-1,-2},{-1,2},{1,-2},{1,2}};
      for (auto& j : jumps) {
        int r = row + j[0], c = col + j[1];
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
          if (board[r][c] == EMPTY || (board[r][c] > 0) != is_white)
            valid_moves[r][c] = true;
        }
      }
      break;
    }
    case WHITE_KING: case BLACK_KING: {
      int dirs[8][2] = {{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}};
      for (auto& d : dirs) {
        int r = row + d[0], c = col + d[1];
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
          if (board[r][c] == EMPTY || (board[r][c] > 0) != is_white)
            valid_moves[r][c] = true;
        }
      }
      break;
    }
  }
}

void handle_inputs() {
  unsigned long now = millis();
  if (now - las_btn_time < 150) return;

  bool acted = false;

  if (digitalRead(BTN_UP) == LOW) {
    if (cursor_row > 0) cursor_row--;
    acted = true;
  } else if (digitalRead(BTN_DOWN) == LOW) {
    if (cursor_row < 7) cursor_row++;
    acted = true;
  } else if (digitalRead(BTN_LEFT) == LOW) {
    if (cursor_col > 0) cursor_col--;
    acted = true;
  } else if (digitalRead(BTN_RIGHT) == LOW) {
    if (cursor_col < 7) cursor_col++;
    acted = true;
  } else if (digitalRead(BTN_ACTION) == LOW) {
    int piece = board[cursor_row][cursor_col];
    bool is_own = white_turn ? (piece > 0) : (piece < 0);

    if (!piece_selected) {
      // Only allow selecting own pieces
      if (piece != EMPTY && is_own) {
        piece_selected = true;
        sel_row = cursor_row;
        sel_col = cursor_col;
        compute_valid_moves(sel_row, sel_col);
      }
    } else {
      if (cursor_row == sel_row && cursor_col == sel_col) {
        // Deselect
        piece_selected = false;
        sel_row = sel_col = -1;
        memset(valid_moves, 0, sizeof(valid_moves));
      } else if (piece != EMPTY && is_own) {
        // Switch to another own piece
        sel_row = cursor_row;
        sel_col = cursor_col;
        compute_valid_moves(sel_row, sel_col);
      } else if (valid_moves[cursor_row][cursor_col]) {
        // Execute move
        board[cursor_row][cursor_col] = board[sel_row][sel_col];
        board[sel_row][sel_col] = EMPTY;
        piece_selected = false;
        sel_row = sel_col = -1;
        memset(valid_moves, 0, sizeof(valid_moves));
        white_turn = !white_turn;

        check_pawn_promotion();

        if (in_check(white_turn)) {
          if (!has_legal_moves(white_turn)) {
            game_over = true;
            win_state = !white_turn ? 1 : 0;
          }
        } else {
          if (!has_legal_moves(white_turn)) {
            game_over = true;
            win_state = 2; 
          }
        }
      }
    }
    acted = true;
  }

  if (acted) las_btn_time = now;
}


bool find_king(bool white, int &king_row, int &king_coloumn){
  int target = white ? WHITE_KING : BLACK_KING;

  for (int row = 0; row < 8; row++) {
    for (int coloumn = 0; coloumn < 8; coloumn++){
      if(board[row][coloumn]==target) {
        king_row = row;
        king_coloumn = coloumn;
        return true;
      }
    }
  }

  return false;
}
bool square_attacked(int row, int coloumn, bool by_white){
  // Check by pawns
  if (by_white) {
    if (row+1 < 8 && coloumn-1 >= 0 && board[row+1][coloumn-1] == WHITE_PAWN) return true;
    if (row+1 < 8 && coloumn+1 < 8  && board[row+1][coloumn+1] == WHITE_PAWN) return true;
  } else {
    if (row-1 >= 0 && coloumn-1 >= 0 && board[row-1][coloumn-1] == BLACK_PAWN) return true;
    if (row-1 >= 0 && coloumn+1 < 8  && board[row-1][coloumn+1] == BLACK_PAWN) return true;
  }


  // Check by knights
  int knight_jumps[8][2] = {{-2,-1},{-2,1},{2,-1},{2,1},{-1,-2},{-1,2},{1,-2},{1,2}};
  int knight = by_white ? WHITE_KNIGHT : BLACK_KNIGHT;
  for (auto& j : knight_jumps) {
    int next_row = row+j[0], next_coloumn = coloumn+j[1];
    if (next_row>=0&&next_row<8&&next_coloumn>=0&&next_coloumn<8 && board[next_row][next_coloumn]==knight) return true;
  }
  
  // Check by rook/queen (straight lines)
  int straight[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
  for (auto& dir : straight) {
    int next_row = row+dir[0], next_coloumn = coloumn+dir[1];
    while (next_row>=0&&next_row<8&&next_coloumn>=0&&next_coloumn<8) {
      if (board[next_row][next_coloumn] != EMPTY) {
        int p = board[next_row][next_coloumn];
        if (by_white && (p==WHITE_ROOK||p==WHITE_QUEEN)) return true;
        if (!by_white && (p==BLACK_ROOK||p==BLACK_QUEEN)) return true;
        break;
      }
      next_row+=dir[0]; next_coloumn+=dir[1];
    }
  }

  // Check by bishop/queen (diagonals)
  int diag[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
  for (auto& dir : diag) {
    int next_row = row+dir[0], next_coloumn = coloumn+dir[1];
    while (next_row>=0&&next_row<8&&next_coloumn>=0&&next_coloumn<8) {
      if (board[next_row][next_coloumn] != EMPTY) {
        int p = board[next_row][next_coloumn];
        if (by_white && (p==WHITE_BISHOP||p==WHITE_QUEEN)) return true;
        if (!by_white && (p==BLACK_BISHOP||p==BLACK_QUEEN)) return true;
        break;
      }
      next_row+=dir[0]; next_coloumn+=dir[1];
    }
  }

  // Check by king
  int king_dirs[8][2] = {{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}};
  int king = by_white ? WHITE_KING : BLACK_KING;
  for (auto& dir : king_dirs) {
    int next_row = row+dir[0], next_coloumn = coloumn+dir[1];
    if (next_row>=0&&next_row<8&&next_coloumn>=0&&next_coloumn<8 && board[next_row][next_coloumn]==king) return true;
  }

  return false;
}

bool in_check(bool white) {
  int king_row, king_coloumn;
  find_king(white, king_row, king_coloumn);
  return square_attacked(king_row, king_coloumn, !white);  
}

void check_pawn_promotion(){
  for(int coloumn = 0; coloumn < 8; coloumn++){
    if(board[0][coloumn]==WHITE_PAWN){
      board[0][coloumn] = WHITE_QUEEN;
    }
    if(board[7][coloumn] == BLACK_PAWN){
      board[7][coloumn] = BLACK_QUEEN;
    }
  }
}

bool has_legal_moves(bool white) {
  for (int front_row = 0; front_row < 8; front_row++) {
    for (int front_coloumn = 0; front_coloumn < 8; front_coloumn++) {
      int piece = board[front_row][front_coloumn];
      if (piece == EMPTY) continue;
      if (white && piece < 0) continue;
      if (!white && piece > 0) continue;

      for (int target_row = 0; target_row < 8; target_row++) {
        for (int target_coloumn = 0; target_coloumn < 8; target_coloumn++) {
          int target = board[target_row][target_coloumn];
          if (white && target > 0) continue;   
          if (!white && target < 0) continue;
          if (front_row == target_row && front_coloumn == target_coloumn) continue;  

          // Temporarily compute moves for this piece
          memset(valid_moves, 0, sizeof(valid_moves));
          
          bool saved[8][8];
          memcpy(saved, valid_moves, sizeof(valid_moves));
          compute_valid_moves(front_row, front_coloumn);
          bool is_valid = valid_moves[target_row][target_coloumn];
          memcpy(valid_moves, saved, sizeof(valid_moves));

          // skip illegal moves
          if (!is_valid) continue; 

          // Simulate
          board[target_row][target_coloumn] = piece;
          board[front_row][front_coloumn] = EMPTY;
          bool safe = !in_check(white);

          // Undo
          board[front_row][front_coloumn] = piece;
          board[target_row][target_coloumn] = target;

          if (safe) return true;
        }
      }
    }
  }
  return false;
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

void draw_cursor() {
  // Draw valid move hints
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      if (valid_moves[r][c]) {
        // Small cross in center of square
        u8g2.drawXBMP(c * block_size, r * block_size , block_size , block_size, optional_move);
      }
    }
  }

  if (piece_selected) {
    u8g2.drawFrame(sel_col * block_size, sel_row * block_size, block_size, block_size);
  }

  u8g2.setDrawColor(1);
  u8g2.drawFrame(cursor_col * block_size, cursor_row * block_size, block_size, block_size);
}

void draw_panel(){
  u8g2.setFont(u8g2_font_5x7_tr); 
  if(white_turn){
    if (piece_selected) {
      u8g2.drawStr(67, 35, "SEL:");
      char pos[3];
      pos[0] = 'A' + sel_col;
      pos[1] = '0' + (8 - sel_row);
      pos[2] = '\0';
      u8g2.drawStr(67, 45, pos);
      u8g2.drawStr(67, 55, "->?");
    } else {
      u8g2.drawStr(67, 35, "PICK"); 
      u8g2.drawStr(67, 45, "piece");
    }
  } else {
    u8g2.drawStr(67, 35, "BLK");
    u8g2.drawStr(67, 45, "TURN");
  }
}

void draw_game() {
  u8g2.firstPage();
  do {
    draw_game_board();

    draw_pieces();
    if(white_turn){
      draw_cursor();
    }

    draw_panel();

  } while (u8g2.nextPage());
}

void draw_game_over(){
  u8g2.firstPage();

  do {
    u8g2.setFont(u8g2_font_7x13_tr);
    if(win_state == 1){
      u8g2.drawStr(10, 25, "WHITE WINS!");
    } else if (win_state == 0 ) {
      u8g2.drawStr(10, 25, "BLACK WINS!");
    }else {
      u8g2.drawStr(20, 25, "STALEMATE");
      u8g2.drawStr(25, 40, "DRAW!");
    }

    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(20, 55, "Press ACTION..");

  }while (u8g2.nextPage());
}