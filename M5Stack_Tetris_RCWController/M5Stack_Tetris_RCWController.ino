//========================================================================
// TETRIS on M5STACK controlled by RCWController app on iPhone

// TETRIS code originally from MhageGH - https://github.com/MhageGH/esp32_ST7735_Tetris
// Ported to M5Stack by macsbug - 2018.01.20 - Article:   https://macsbug.wordpress.com/2018/01/20/tetris-with-m5stack/
// RCWController app by Micono Utilities - App details:  https://appadvice.com/app/rcwcontroller/1084628679
// RCWController support for Tetris on 5Stack by Micono Utilities 
// Video demo + link to orginal source for this code - https://www.youtube.com/watch?v=jeuHP-NbdJg&feature=youtu.be
//
// Variations in this version of the code compared to the original = Very minor =  On-screen intro text: font size increased & colour added.
// Github source for the code in this file =  https://github.com/PartsandCircuits/M5Stack-Tetris-using-RCWController
 
//=========================================================================

#include <M5Stack.h>                                       // M5STACK
#include <WiFi.h>
#include <WiFiUdp.h>

const char ssid[] = "SSID";         //   << Enter your WiFi SSID (name) here  (avoid using a space in the SSID)
const char pass[] = "PASSWORD";     //   << Enter your WiFi password here 


WiFiUDP udp;
unsigned int localPort = 10000;
const int PACKET_SIZE = 256;
char packetBuffer[PACKET_SIZE];
bool bConnect=false;

//Buttons
unsigned char nBtn[3],pBtn[3];

//Analog Left
unsigned char nAnL[3],pAnL[3];
int anLDiv=4;//Sensitivity
float anLMax=64.0/127.0-1.0;//Max value=127

//Analog Right
unsigned char nAnR[3],pAnR[3];
int anRDiv=4;//Sensitivity
float anRMax=64.0/127.0-1.0;//Max value=127

//acceleration sensor
unsigned char nAcc[4],pAcc[4];
int accDiv=4;//Sensitivity
float accMax=64.0/127.0-1.0;//Max value=127

uint16_t BlockImage[8][12][12];                            // Block
uint16_t backBuffer[240][120];                             // GAME AREA
const int Length = 12;     // the number of pixels for a side of a block
const int Width  = 10;     // the number of horizontal blocks
const int Height = 20;     // the number of vertical blocks
int screen[Width][Height] = {0}; //it shows color-numbers of all positions
struct Point {int X, Y;};
struct Block {Point square[4][4]; int numRotate, color;};
Point pos; Block block;
int rot, fall_cnt = 0;
bool started = false, gameover = false;
boolean but_A = false, but_B = false, but_LEFT = false, but_RIGHT = false;
int game_speed = 25; // 25msec
Block blocks[7] = {
  {{{{-1,0},{0,0},{1,0},{2,0}},{{0,-1},{0,0},{0,1},{0,2}},
  {{0,0},{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0},{0,0}}},2,1},
  {{{{0,-1},{1,-1},{0,0},{1,0}},{{0,0},{0,0},{0,0},{0,0}},
  {{0,0},{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0},{0,0}}},1,2},
  {{{{-1,-1},{-1,0},{0,0},{1,0}},{{-1,1},{0,1},{0,0},{0,-1}},
  {{-1,0},{0,0},{1,0},{1,1}},{{1,-1},{0,-1},{0,0},{0,1}}},4,3},
  {{{{-1,0},{0,0},{0,1},{1,1}},{{0,-1},{0,0},{-1,0},{-1,1}},
  {{0,0},{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0},{0,0}}},2,4},
  {{{{-1,0},{0,0},{1,0},{1,-1}},{{-1,-1},{0,-1},{0,0},{0,1}},
  {{-1,1},{-1,0},{0,0},{1,0}},{{0,-1},{0,0},{0,1},{1,1}}},4,5},
  {{{{-1,1},{0,1},{0,0},{1,0}},{{0,-1},{0,0},{1,0},{1,1}},
  {{0,0},{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0},{0,0}}},2,6},
  {{{{-1,0},{0,0},{1,0},{0,-1}},{{0,-1},{0,0},{0,1},{-1,0}},
  {{-1,0},{0,0},{1,0},{0,1}},{{0,-1},{0,0},{0,1},{1,0}}},4,7}
};
extern uint8_t tetris_img[];

//========================================================================

void setup(void) {
  M5.begin();                   // M5STACK INITIALIZE
  M5.Lcd.setBrightness(200);    // BRIGHTNESS = MAX 255
  M5.Lcd.fillScreen(BLACK);     // CLEAR SCREEN

  Serial.begin(115200);
  while (!Serial) {
    ;// wait for serial port to connect. Needed for Leonardo only
  }

// Changes to set larger text size & facilitate option to change colours:
    
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0,0);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);


  WiFi.begin(ssid, pass);
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to Access Point");
  M5.Lcd.println("Connecting to Access Point");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    M5.Lcd.print("."); 
  }
  IPAddress myIP = WiFi.localIP();
  
  /*
  WiFi.softAP(ssid, pass);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("'AP IP address: ");
  */

  M5.Lcd.clearDisplay();
  M5.Lcd.setCursor(0, 0);

  M5.Lcd.setTextColor(GREEN);

  Serial.println(""); Serial.print("SSID: "); Serial.println(ssid);
  M5.Lcd.println(""); M5.Lcd.print("SSID: "); M5.Lcd.println(ssid);

  Serial.println("");
  M5.Lcd.println("");

  Serial.print(myIP); Serial.print(":"); Serial.println(localPort);
  M5.Lcd.print(myIP); M5.Lcd.print(":"); M5.Lcd.println(localPort);

  Serial.println("");
  M5.Lcd.println("");

  M5.Lcd.setTextColor(WHITE);
    
  Serial.println("Starting UDP");
  M5.Lcd.println("Starting UDP");
  
  udp.begin(localPort);

delay(2000);

// Clear screen , reprint SSID and IP address:

  M5.Lcd.clearDisplay();
  M5.Lcd.setCursor(0, 0);
 
  M5.Lcd.setTextColor(GREEN);

  Serial.println(""); Serial.print("SSID: "); Serial.println(ssid);
  M5.Lcd.println(""); M5.Lcd.print("SSID: "); M5.Lcd.println(ssid);

  Serial.println("");
  M5.Lcd.println("");

  Serial.print(myIP); Serial.print(":"); Serial.println(localPort);
  M5.Lcd.print(myIP); M5.Lcd.print(":"); M5.Lcd.println(localPort);

  Serial.println("");
  M5.Lcd.println("");
  Serial.println("");
  M5.Lcd.println("");
  
  M5.Lcd.setTextColor(WHITE);
  
  //Serial.print("'local port: ");
  //Serial.println(udp.localPort());
  

  M5.Lcd.println("1. Select WiFi on iPhone");
  M5.Lcd.println("2. Start RCWController");
  M5.Lcd.println("3. Set IP, press Connect");
  M5.Lcd.println("4. Use button B to start");
  M5.Lcd.println("5. Reboot to restart");

delay(7000);

  
  nBtn[0]=0x80;
  nAnL[0]=0x81;
  nAnR[0]=0x82;
  nAcc[0]=0x83;
  //nGyA[0]=0x84;
  //nGyU[0]=0x85;
  //Audio=0x86;
  //================================================
  //================================================

  //----------------------------// Make Block ----------------------------
  make_block( 0, BLACK);        // Type No, Color
  make_block( 1, 0x00F0);       // DDDD     RED
  make_block( 2, 0xFBE4);       // DD,DD    PUPLE 
  make_block( 3, 0xFF00);       // D__,DDD  BLUE
  make_block( 4, 0xFF87);       // DD_,_DD  GREEN 
  make_block( 5, 0x87FF);       // __D,DDD  YELLO
  make_block( 6, 0xF00F);       // _DD,DD_  LIGHT GREEN
  make_block( 7, 0xF8FC);       // _D_,DDD  PINK
  //----------------------------------------------------------------------
  // M5.Lcd.drawJpgFile(SD, "/tetris.jpg");     // Load background from SD

  M5.Lcd.drawJpg(tetris_img, 34215); // Load background from file data
  PutStartPos();                             // Start Position
  for (int i = 0; i < 4; ++i) screen[pos.X + 
   block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
  Draw();                                    // Draw block
}

//========================================================================

void loop() {
  if (gameover) return;
  Point next_pos;
  int next_rot = rot;
  ReadUDP();
  GetNextPosRot(&next_pos, &next_rot);
  ReviseScreen(next_pos, next_rot);
  M5.update();
  delay(game_speed);                                      // SPEED ADJUST
}
//========================================================================
void Draw() {                               // Draw 120x240 in the center
  for (int i = 0; i < Width; ++i) for (int j = 0; j < Height; ++j)
   for (int k = 0; k < Length; ++k) for (int l = 0; l < Length; ++l)
    backBuffer[j * Length + l][i * Length + k] = BlockImage[screen[i][j]][k][l];
    M5.Lcd.pushImage(100, 0, 120, 240, (uint16_t *)backBuffer);
}
//========================================================================
void PutStartPos() {
  pos.X = 4; pos.Y = 1;
  block = blocks[random(7)];
  rot = random(block.numRotate);
}
//========================================================================
bool GetSquares(Block block, Point pos, int rot, Point* squares) {
  bool overlap = false;
  for (int i = 0; i < 4; ++i) {
    Point p;
    p.X = pos.X + block.square[rot][i].X;
    p.Y = pos.Y + block.square[rot][i].Y;
    overlap |= p.X < 0 || p.X >= Width || p.Y < 0 || p.Y >= 
      Height || screen[p.X][p.Y] != 0;
    squares[i] = p;
  }
  return !overlap;
}
//========================================================================
void GameOver() {
  for (int i = 0; i < Width; ++i) for (int j = 0; j < Height; ++j)
  if (screen[i][j] != 0) screen[i][j] = 4;
  gameover = true;
}
//========================================================================
void ClearKeys() { but_A=false; but_B=false; but_LEFT=false; but_RIGHT=false;}
//========================================================================
bool KeyPadLoop(){
  if(M5.BtnA.wasPressed()){ClearKeys();but_LEFT =true;return true;}
  if(M5.BtnB.wasPressed()){ClearKeys();but_RIGHT=true;return true;}
  if(M5.BtnC.wasPressed()){ClearKeys();but_A    =true;return true;}
  return false;
}
//========================================================================
void GetNextPosRot(Point* pnext_pos, int* pnext_rot) {
  bool received = KeyPadLoop();
  if(received==false) {
    if(but_LEFT==true||but_RIGHT==true||but_A==true||but_B==true) {
      received=true;
      Serial.println("true");
    }
  }
  if (but_A) started = true;
  if (!started) return;
  pnext_pos->X = pos.X;
  pnext_pos->Y = pos.Y;
  if ((fall_cnt = (fall_cnt + 1) % 10) == 0) pnext_pos->Y += 1;
  else if (received) {
    if (but_LEFT) { but_LEFT = false; pnext_pos->X -= 1;}
    else if (but_RIGHT) { but_RIGHT = false; pnext_pos->X += 1;}
    else if (but_A) { but_A = false;
      *pnext_rot = (*pnext_rot + block.numRotate - 1)%block.numRotate; 
    }
    else if (but_B) { but_B = false;
      *pnext_rot = (*pnext_rot + block.numRotate + 1)%block.numRotate; 
    }
  }
}
//========================================================================
void DeleteLine() {
  for (int j = 0; j < Height; ++j) {
    bool Delete = true;
    for (int i = 0; i < Width; ++i) if (screen[i][j] == 0) Delete = false;
    if (Delete) for (int k = j; k >= 1; --k) 
    for (int i = 0; i < Width; ++i) screen[i][k] = screen[i][k - 1];
  }
}
//========================================================================
void ReviseScreen(Point next_pos, int next_rot) {
  if (!started) return;
  Point next_squares[4];
  for (int i = 0; i < 4; ++i) screen[pos.X + 
    block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = 0;
  if (GetSquares(block, next_pos, next_rot, next_squares)) {
   for (int i = 0; i < 4; ++i){
     screen[next_squares[i].X][next_squares[i].Y] = block.color;
   }
   pos = next_pos; rot = next_rot;
  }
  else {
   for (int i = 0; i < 4; ++i) screen[pos.X + 
    block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
   if (next_pos.Y == pos.Y + 1) {
    DeleteLine(); PutStartPos();
    if (!GetSquares(block, pos, rot, next_squares)) {
     for (int i = 0; i < 4; ++i) screen[pos.X + 
      block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
      GameOver();
    }
   }
  }
  Draw();
}
//========================================================================
void make_block( int n , uint16_t color ){            // Make Block color       
  for ( int i =0 ; i < 12; i++ ) for ( int j =0 ; j < 12; j++ ){
    BlockImage[n][i][j] = color;                           // Block color
    if ( i == 0 || j == 0 ) BlockImage[n][i][j] = 0;       // BLACK Line
  } 
}
//========================================================================

//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================

/*
 *  ESP_RCWController_Receiver by Michio Ono.
 *  ESP-WROOM-02 RCWController Receiver
 *  CC BY
 *  Ver 1.0.1
 *  
 *  latest version
 *  http://ijutilities.micutil.com
 *  
 */

/******************************************
 * References
 * ****************************************
 * 
 * Unipad x ESP8266
 * http://unipad.digitalray.co.jp
 * 
 * tested on ESPr Developer
 * https://www.switch-science.com/catalog/2500/
 * 
 * IchigoJam
 * http://ichigojam.net
 * 
 */

/*********************************************
 * ReadUDP
 * 
 * データ受診
 * 
 **********************************************/

void ReadUDP() {
  bool b=false;
  int i,rlen=0;
  char *pb;
  
  if ((rlen = udp.parsePacket())) {

    /*パケット読み込み*/
    udp.read(packetBuffer, (rlen > PACKET_SIZE) ? PACKET_SIZE : rlen);

    pb=packetBuffer;

    //Button
    for(i=1;i<3;i++) {
      nBtn[i]=pb[i-1];
      if(pBtn[i]!=nBtn[i]) {b=true;pBtn[i]=nBtn[i];}
    }
    if(b) for(i=0;i<3;i++) {Serial.write(nBtn[i]);}
    if(b) {
      if(nBtn[2]&(0x1<<6)) {
        but_A=true;
      } if(nBtn[2]&(0x1<<5)) {
        but_B=true;
      } if(nBtn[2]&(0x1<<3)) {
        but_LEFT=true;
      } if(nBtn[2]&(0x1<<2)) {
        but_RIGHT=true;
      }
    } else {
      ClearKeys();
    }

    //Analog Left
    pb+=2; b=false;
    for(i=1;i<3;i++) {
      nAnL[i]=(unsigned char)MaxMin(64+anLDiv*(int)(((float)pb[i-1]-128.0)*anLMax/(float)anLDiv));
      if(pAnL[i]!=nAnL[i]) {b=true;pAnL[i]=nAnL[i];}
    }
    if(b) for(i=0;i<3;i++) {Serial.write(nAnL[i]);}
    
    //Analog Right
    pb+=2; b=false;
    for(i=1;i<3;i++) {
      nAnR[i]=(unsigned char)MaxMin(64+anRDiv*(int)(((float)pb[i-1]-128.0)*anRMax/(float)anRDiv));
      if(pAnR[i]!=nAnR[i]) {b=true;pAnR[i]=nAnR[i];}
    }
    if(b) for(i=0;i<3;i++) {Serial.write(nAnR[i]);}
    
    //Accel
    pb+=2; b=false;
    for(i=1;i<4;i++) {
      nAcc[i]=(unsigned char)MaxMin(64+accDiv*(int)(((float)pb[i-1]-128.0)*accMax/(float)accDiv));
      if(pAcc[i]!=nAcc[i]) {b=true;pAcc[i]=nAcc[i];}
    }
    if(b) for(i=0;i<4;i++) {Serial.write(nAcc[i]);}
  }
  
}

/*********************************************
 * 最大値・最小値
 * 
 * 1-127: center 64, from -63 to 63 
 * 
 **********************************************/

inline int max(int a,int b) {return ((a)>(b)?(a):(b)); }
inline int min(int a,int b) {return ((a)<(b)?(a):(b)); }
int MaxMin(int n) {
  return min(max(1,n),127);
}

//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================


