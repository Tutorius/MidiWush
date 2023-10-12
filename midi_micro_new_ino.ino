#include <EEPROM.h>

//#define ARDUINONANO 1
#define ARDUINOPROMICRO 1

//#define NEOLED_INIT 1
//#define NEOLED 1

#define OLED_INIT 1
#define OLED 1

#define RGBMatrixPin 9

#define NL_FASTER 7
#define NL_FAST 15
#define NL_SLOW 100

#ifdef OLED_INIT
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #define OLED_RESET -1
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels
#endif

#ifdef OLED_INIT
  Adafruit_SSD1306 OLEDMatrix(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif


#ifdef NEOLED_INIT
  #include <Adafruit_NeoMatrix.h>

#endif

#ifdef NEOLED_INIT
  Adafruit_NeoMatrix RGBMatrix = Adafruit_NeoMatrix(8, 8, RGBMatrixPin,
  NEO_MATRIX_TOP+NEO_MATRIX_LEFT+NEO_MATRIX_COLUMNS+NEO_MATRIX_ROWS+NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);
#endif

#ifdef ARDUINONANO
  // Port-Definitionen
  #define AnalogPort 0

  #define Key1 2
  #define KeyMeasure 2
  
  #define Key2 3
  #define KeyInvert 3

  #define Key3 4
  #define KeyMinMinus 4
  
  #define Key4 5
  #define KeyMinPlus 5
  
  #define Key5 6
  #define KeyMaxMinus 6
  
  #define Key6 7
  #define KeyMaxPlus 7
  
  #define Key7 8
  #define KeyGlatt 8
#endif

#ifdef ARDUINOPROMICRO
  // Port-Definitionen
  #define AnalogPort 0

  #define Key1 4
  #define KeyMeasure 4
  
  #define Key2 5
  #define KeyInvert 5

  #define Key3 6
  #define KeyMinMinus 6
  
  #define Key4 7
  #define KeyMinPlus 7
  
  #define Key5 8
  #define KeyMaxMinus 8
  
  #define Key6 9
  #define KeyMaxPlus 9
  
  #define Key7 10
  #define KeyGlatt 10
#endif


#define USBMIDI 1
//#define STDMIDI 1

/*
Midi-Controller V 0.9

This Controller uses an Arduino (Nano in this case) and ouputs MIDI
by USB-Midi or 5-Pin-DIN (in this case) Midi.
It uses an Input that can be changed for controlling a Synthesizers
parameters, Input can be a combination of LDR and Resistor, and many
other sensors.

There are 7 push-buttons that control following functions:

1: Push to measure Minimum and Maximum from Input, push again to acknoledge
2: Push to invert the signal, push again to revert
3: Decrease Minimum-Level
4: Increase Minimum-level
5: Decrease Maximum-Level
6: Increase Maximum-level
7: Set Smoothing-factor (Off, 1-8)

The Display is a Nano-Pixel-Display with 8x8 RGBLeds

After Startup: Display shows a red checker, a push on button 1 is needed
to set Minimum-Maximum-values.
After first push, the display changes to a blue rectangle. Use the sensor to 
reach its minimum and maximum values you will need.
Push the button again, Display will start to display values from the sensor.

If the sensor is not reacting the way you wish, use button 2 to invert the signal.
Display shows an "X" for not inverting, "I" for inverting. Standard-setting is inverting.

You can now work with the sonsor, the display shows about one second of the input, scrolling
from left to right.

Standard-smoothing is set to factor 1.

By pressing button 7 you increase the smoothing-value. Maximum-value is 8, creating ramps of about one second.
Display shows a "N" for no smoothing or the value for smoothing "1" to "8".

Minimum-Maximum-values for Controller:

Controller uses Midi-Controller 70.

On startup, the sensor creates values between 0 and 127 as output.

You can use the four Minimum-Maximum-buttons to change the minimum and maximum-value you want to send to your synthesizer.
When pressing the buttons, the signal of the sensor is muted and the output-value follows the minuímum or maximum-value.

No values are stored, so you have to do the settings after each startup.

*/

/*
0 bis 9 -> Controller 70 bis 79
10-25 -> Midi-Kanal 1 bis 16
*/

//#define ARDUINOMICRO 1
//#define ARDUINOMEGA 1
//#define ARDUINOUNO 1 

#define MILISEC 10
#define HOLDTIME 20
#define HOLDTIME2 10

// Repeat-Wert für MinMax-Keys
#define REPEAT 10

// Maximales Glättungs-Intervall
#define GLATTO 32

// Glättung-Stufen
#define GLATTO1 4
#define GLATTO2 8
#define GLATTO3 12
#define GLATTO4 16
#define GLATTO5 20
#define GLATTO6 24
#define GLATTO7 28
#define GLATTO8 32

#define DISPDELAY 5

#define MODESHOW 4

// Midi-Controller Nr.
// #define Controller 70




#ifdef USBMIDI
  #include <MIDIUSB.h>
#endif

#ifdef STDMIDI
  #include <MIDI.h>
#endif




#ifdef STDMIDI
  MIDI_CREATE_DEFAULT_INSTANCE();
#endif

int i,j,k,l;

unsigned char Channel[4]={1,1,1,1};
int val;
int Value[4],Value2[4],ValueX[4];
int Mini[4],Maxi[4];

int Omini[4],Omaxi[4];

int Correct;
int Setmode;

float Factor;

int Measure;
unsigned char Invert[4];
// int Glatt;

// int Glattvar[GLATTO];
// int Glatti;
// int Glattj;
long Lvalue;

char s[20];
byte Valsave[8];
int DispDelay;

int LowPoint;

unsigned char colr[8]={255,   255,  255,  255,  170,  140,  80,   20};
unsigned char colg[8]={0,     80,   130,  180,  200,  250,  200,  80};
unsigned char colb[8]={0,     0,    0,    0,    0,    100,  180,  200};

// int GlattLevel; // 0 : Aus; 1-8 : Level 5-40

unsigned long time1,time2,dimediff;

unsigned char justi=false;

unsigned char Controller[4]={70,71,72,73};
unsigned char MidiChannels[4]={1,1,1,1};
char ContChannel[4]={0,0,0,0};

unsigned char ResetMode=true; 
unsigned char ChangeMode=0;
unsigned char ActChannel=0; // Aktiver, angezeigter ContChannel

char KeyTimeCounter[7]={-1,-1,-1,-1,-1,-1,-1};
unsigned char OldKey[7];
int Solo;
int Nachleucht;
int ContChan=-1; // 0 : Controller ; 1 : Midi-Channel ; -1 : Nix

int ControllerChannelMinimax[2]; // 0 : Minimum; 1 : Maximum; 2 : Average

int MidiEnable;

/*

Resetmode: Wird nach dem Einschalten oder durch langes Drücken Taste 1 gewählt
Anzeige:
blau Wush-kanal, rote Sensornummer normal, gelbe Sensornummer invertiert

Funktionen in diesem Modus:
Taste 1: durchtoggeln des Wush-Kanals (1 bis 4) , startet wieder bei 1 nach Kanal 4
Taste 2 lang gdrückt (3 Sekunden) : Minimax-Wert für gewählten Kanal/Sensor durchführen, 1 Sekunde Vorpause, nach weiteren 5 Sekunden beendet, Anzeige blaues ungefülltes Rechteck, welches von unten nach oben gefüllt wird
Taste 3 (Min-Minus) und Taste 4 (Min-Plus) : Sensor wählen (1-4)
Taste 5 (max-Minus) und Taste 6 (Max-Plus) : Midi-Contoller (70-79, Anzeige blau) oder Midi-Kanal (1-16, Anzeige rot) wählen
Taste 7 : Umschalten ob Midi-Controller (blau) oder Midi-Kanal (rot)

Taste 2 kurz : Invertieren ja/nein

Taste 1 lang (3 Sekunden) : Einrichtug beenden

*/




#ifdef USBMIDI
  midiEventPacket_t event;
#endif

#ifdef USBMIDI
  void controllerChange(byte ChannelY, byte ControlY, byte ValueY)
  {
    midiEventPacket_t event = {0x0B, 0xB0 | ChannelY, ControlY, ValueY};
    MidiUSB.sendMIDI(event);
  }
#endif

void setup()
{
  //Serial.begin(9600);
  #ifdef OLED
    if(!OLEDMatrix.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
      //Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    delay(100);
    OLEDMatrix.display();
    delay(2000);
    OLEDMatrix.clearDisplay();
  #endif
  
  #ifdef STDMIDI
    MIDI.begin(MIDI_CHANNEL_OMNI);
  #endif
  
  #ifdef NEOLED
    RGBMatrix.setBrightness(10);
    // RGBMatrix starten
    RGBMatrix.begin();
  #endif
  #ifdef NEOLED
    NEODrawResetMode();
    RGBMatrix.show();
  #endif
  #ifdef OLED
    OLEDDrawResetMode();;
    OLEDMatrix.display();
  #endif
  LowPoint=0;
  //Glatti=0;
  for (i=0;i<4;i++)
  {
    Mini[i]=1023;
    Maxi[i]=0;
    Omini[i]=0;
    Omaxi[i]=127;
    Invert[i]=true;
  }
  // Channel=1;
  pinMode(KeyMeasure,INPUT_PULLUP); // Key 1
  pinMode(KeyInvert,INPUT_PULLUP);  // Key 2
  
  pinMode(KeyMinMinus,INPUT_PULLUP);// Key 3
  pinMode(KeyMinPlus,INPUT_PULLUP); // Key 4
  pinMode(KeyMaxMinus,INPUT_PULLUP);// Key 5 
  pinMode(KeyMaxPlus,INPUT_PULLUP); // Key 6
  
  pinMode(KeyGlatt,INPUT_PULLUP);   // Key 7
  
  DispDelay=DISPDELAY;
  //Glatt=true; GlattLevel=1; 
  ResetMode=true;
  for (i=0;i<7;i++) OldKey[i]=false;

  MidiEnable=true;

  do
  {
    // Read all 4 analog Inputs
    for (k=0;k<4;k++)
      Value[k]=analogRead(AnalogPort+k);
    // Save Time
    time1=millis();
    
    // Key1-Abfrage incl. Wechsel ResetMode per langem Druck
    // Im Resetmode sowie im Normal-Mode wechselt die Taste 1 bei kurzem Druck den aktiven Kanal

    ScanKey1();

    // Key2-Abfrage
    // Im ResetMode : Minimax-für gewählten Kanal/Sensor
    // Im Normal-Mode : Invertieren des aktiven Kanals umschalten
    // Langer Druck im Normal-Mode : Solo-Ausgabe aktiver Kanal/Sensor

    ScanKey2();

    // Key3-Abfrage 
    // Im Resetmode wird Sensor gewählt, im Normalmodus der Minimum-Wert gesenkt

    ScanKey3();

    // Key4-Abfrage 
    // Im Resetmode wird Sensor gewählt, im Normalmodus der Minimum-Wert angehoben

    ScanKey4();

    // Key5-Abfrage 
    // Im Resetmode wird  bei erstem Drücken Controller gewählt,
    // gewähltes Gerät bei nächstem Druck erniedrigt 
    // im Normalmodus der Maximum-Wert gesenkt

    ScanKey5();

    // Key6-Abfrage 
    // Im Resetmode wird  bei erstem Drücken Midi-Kanal gewählt,
    // gewähltes Gerät bei nächstem Druck erhöht 
    // im Normalmodus der Maximum-Wert erhöht

    ScanKey6();

    // Key7-Abfrage
    // ResetMode: Kurzer Druck : EEPROM lesen ; Langer Druck : EEPROM schreiben

    ScanKey7();

    // Show Screen
    ShowScreen();

    do
    {
      time2=millis();
    } while (time2-time1<MILISEC);
  } while (1==1);
}


  #ifdef STDMIDI
  void controllerChange(byte ChannelY, byte ControlY,byte ValueY)
  {
    MIDI.sendControlChange(ControlY,ValueY,ChannelY);
  }
  #endif


#ifdef NEOLED
  void NEODisplay(int Valo)
  {
    float scal;
    int dispo;
    int rr;
    int j;
    scal=9/(float)(Maxi[ActChannel]-Mini[ActChannel]);
    dispo=(int)(((float)(Valo-Mini[ActChannel]))*scal);
    Valsave[0]=(byte)dispo;
    if (Nachleucht--<=0)
    {
      Nachleucht=-1;
      for (j=0;j<8;j++)
      {
        for (i=0;i<8;i++)
        {
          dispo=Valsave[j];
          if (Invert[ActChannel]) dispo=9-dispo;
          if (dispo<i+1)
          {
            RGBMatrix.drawPixel(j,7-i,RGBMatrix.Color(0,0,0));
          }
          else
          {
            if (!Solo)
            {
              RGBMatrix.drawPixel(j, 7-i,RGBMatrix.Color(colr[j],colg[j],colb[j]));
            }
            else
            {
              if ((dispo==i+1)||((dispo>=7)&&(i==7)))
              {
                RGBMatrix.drawPixel(j, 7-i,RGBMatrix.Color(colr[j],colg[j],colb[j]));
              }
              else
              {
                RGBMatrix.drawPixel(j,7-i,RGBMatrix.Color(0,0,0));
              }
            }
          }
          if ((dispo==0)&&(LowPoint==j))
            RGBMatrix.drawPixel(j,7,RGBMatrix.Color(colr[j],colg[j],colb[j]));
        }
      }
      if(LowPoint++==7) LowPoint=0;
      for (i=7;i>0;i--)
      {
        Valsave[i]=Valsave[i-1];
      }
    }
  }
#endif


#ifdef OLED

  void OLEDDisplay(int Valo)
  {
    float scal;
    int dispo;
    int rr;
    int j;
    scal=9/(float)(Maxi[ActChannel]-Mini[ActChannel]);
    dispo=(int)(((float)(Valo-Mini[ActChannel]))*scal);
    Valsave[0]=(byte)dispo;
    if (Nachleucht==0) OLEDMatrix.clearDisplay();
    if (Nachleucht--<=0)
    {
      Nachleucht=-1;
      for (j=0;j<8;j++)
      {
        for (i=0;i<32;i++)
        {
          dispo=Valsave[j];
          if (Invert[ActChannel]) dispo=9-dispo;
          if (dispo<i+1)
          {
            //for(k=0;k<4;k+=2)
              //for(l=0;l<4;l+=2)
                //OLEDMatrix.drawPixel(j*8+k,63-i*8-l,0);
                OLEDMatrix.fillRect(j*8,63-i*8,8,8,0);
          }
          else
          {
            if (!Solo)
            {
              //for (k=0;k<4;k+=2)
                //for(l=0;l<4;l+=2)
                  //OLEDMatrix.drawPixel(j*8+k, 63-i*8-l,1);
                  OLEDMatrix.fillRect(j*8,63-i*8,8,8,1);
            }
            else
            {
              if ((dispo==i+1)||((dispo>=7)&&(i==7)))
              {
                //for (k=0;k<4;k+=2)
                  //for (l=0;l<4;l+=2)
                    //OLEDMatrix.drawPixel(j*8+k, 63-i*8-l,1);
                    OLEDMatrix.fillRect(j*8,63-i*8,8,8,1);
              }
              else
              {
                //for (k=0;k<4;k+=2)
                  //for (l=0;l<4;l+=2)
                    //OLEDMatrix.drawPixel(j*8+k,63-i*8-l,0);
                    OLEDMatrix.fillRect(j*8,63-i*8,8,8,0);
              }
            }
          }
          if ((dispo==0)&&(LowPoint==j))
          {
            //for (k=0;k<4;k+=2)
              //for (l=0;l<4;l+=2)
                //OLEDMatrix.drawPixel(j*8+k,63-l,1);
                OLEDMatrix.fillRect(j*8,63-8,8,8,1);
          }
        }
      }
      if(LowPoint++==7) LowPoint=0;
      for (i=7;i>0;i--)
      {
        Valsave[i]=Valsave[i-1];
      }
    }
  }

/*
  void OLEDDisplay(int Valo)
  {
    float scal;
    int dispo;
    int rr;
    int j;
    scal=65/(float)(Maxi[ActChannel]-Mini[ActChannel]);
    dispo=(int)(((float)(Valo-Mini[ActChannel]))*scal);
    if (Nachleucht==0) OLEDMatrix.clearDisplay();
    if (Nachleucht--<=0)
    {
      Nachleucht=-1;
      OLEDMatrix.startscrollright(0, 7);
      delay(1);
      OLEDMatrix.stopscroll();
      if (Invert[ActChannel]) dispo=65-dispo;
      if (!Solo)
      {
        OLEDMatrix.fillRect(0, 0, 1, dispo, 0);
        OLEDMatrix.fillRect(0,dispo+1,1,63-dispo,1);
      }
      else
      {
        OLEDMatrix.fillRect(0,0,1,63,0);
        OLEDMatrix.drawPixel(0, dispo, 1);
      }
      if ((dispo==0)&&(LowPoint==j))
      {
        OLEDMatrix.drawPixel(0,0,1);
      }
      if(LowPoint++==7) LowPoint=0;
      for (i=7;i>0;i--)
      {
        Valsave[i]=Valsave[i-1];
      }
    }
  }
*/
#endif

static const int PROGMEM charpix[]=
// int charpix[42][8]=
{
 // Midi-controller (70-79)
 0xE6,0x29,0x29,0x49,0x49,0x89,0x89,0x86, // C70 0 bis 9
 0xE1,0x23,0x25,0x49,0x41,0x81,0x81,0x81, // C71
 0xE6,0x29,0x21,0x42,0x44,0x88,0x88,0x8F, // C72
 0xE6,0x29,0x21,0x42,0x41,0x81,0x89,0x86, // C73
 0xE1,0x23,0x25,0x49,0x49,0x8F,0x82,0x82, // C74
 0xEF,0x28,0x28,0x46,0x41,0x81,0x89,0x86, // C75
 0xE3,0x24,0x28,0x4E,0x49,0x89,0x89,0x86, // C76
 0xEE,0x21,0x21,0x42,0x42,0x84,0x84,0x84, // C77
 0xE6,0x29,0x29,0x46,0x49,0x89,0x89,0x86, // C78
 0xE6,0x29,0x29,0x47,0x41,0x82,0x84,0x88, // C79
 
 // Midi-channels (1-16)
 0x01,0x03,0x05,0x01,0x01,0x01,0x01,0x01, // K1 10 bis 25
 0x06,0x09,0x01,0x02,0x04,0x08,0x08,0x0F, // K2
 0x06,0x09,0x01,0x02,0x01,0x01,0x09,0x06, // K3
 0x01,0x02,0x04,0x08,0x0A,0x0F,0x02,0x02, // K4
 0x0F,0x08,0x08,0x06,0x01,0x01,0x09,0x06, // K5
 0x03,0x04,0x08,0x0E,0x09,0x09,0x09,0x06, // K6
 0x0F,0x01,0x02,0x02,0x04,0x04,0x08,0x08, // K7
 0x06,0x09,0x09,0x06,0x09,0x09,0x09,0x06, // K8
 0x06,0x09,0x09,0x09,0x07,0x01,0x02,0x0D, // K9
 0x26,0x69,0xA9,0x29,0x29,0x29,0x29,0x26, // K10
 0x21,0x63,0xA5,0x21,0x21,0x21,0x21,0x21, // K11
 0x26,0x69,0xA1,0x22,0x24,0x28,0x28,0x2F, // K12
 0x26,0x69,0xA1,0x22,0x21,0x21,0x21,0x26, // K13
 0x21,0x62,0xA4,0x28,0x28,0x2F,0x22,0x22, // K14
 0x2F,0x68,0xA8,0x26,0x21,0x21,0x29,0x26, // K15
 0x23,0x64,0xA8,0x2E,0x29,0x29,0x29,0x26, // K16

 // Plus Minus
 0x20,0x20,0x27,0x20,0x20,0xF8,0x70,0x20, // MinMinus 26
 0x20,0x22,0x27,0x22,0x20,0xF8,0x70,0x20, // MinPlus 27
 0x20,0x70,0xF8,0x20,0x20,0x27,0x20,0x20, // MaxMinus 28
 0x20,0x70,0xF8,0x20,0x22,0x27,0x22,0x20,  // MaxPlus 29

 // ActChannel (at front)
 0x20,0x60,0xA0,0x20,0x20,0x20,0x20,0x20,   // 1 front 30
 0x60,0xA0,0x20,0x20,0x40,0x80,0x80,0xE0,   // 2 front 31
 0x40,0xA0,0x20,0x20,0xC0,0x20,0xA0,0x40,   // 3 front 32
 0x20,0x40,0x80,0xC0,0XE0,0x40,0x40,0x40,   // 4 front 33

 // ActSensor (at back)
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   // NC back 34
 0x01,0x03,0x05,0x09,0x01,0x01,0x01,0x01,   // 1 back 35
 0x06,0x09,0x01,0x02,0x04,0x08,0x08,0x0F,   // 2 back 36
 0x06,0x09,0x01,0x01,0x02,0x01,0x09,0x06,   // 3 back 37
 0x01,0x02,0x04,0x0A,0x0A,0x0F,0x02,0x02,    // 4 back 38

  // SAVE & LOAD
 0x0F,0x11,0x29,0x2D,0xFF,0x2D,0x29,0x3F,   // SAVE 39
 0x38,0x48,0x8C,0x8E,0xFB,0x8E,0x48,0xF8    // LOAD 40
};


#ifdef NEOLED
void NEOPutChar(int c,int col,int clear)
{
  int i,j;
  unsigned char vali,andi;
  if (clear)
    RGBMatrix.clear();
  for (j=0;j<8;j++)
  {
    andi=0x80;
    for (i=0;i<8;i++)
    {
      vali = pgm_read_byte_near(charpix+c*8+j);
      if (vali&andi)
        RGBMatrix.drawPixel(i,j,col);
      else
      {
        if (clear)
          RGBMatrix.drawPixel(i,j,0);
      }
      andi/=2;
    }
  }
  RGBMatrix.show();
}
#endif

#ifdef OLED
void OLEDPutChar(int c,int col,int clear,int show)
{
  int i,j,k,l;
  unsigned char vali,andi;
  if (clear)
    OLEDMatrix.clearDisplay();
  for (j=0;j<8;j++)
  {
    andi=0x80;
    for (i=0;i<8;i++)
    {
      vali=pgm_read_byte_near(charpix+c*8+j);
      for (k=0;k<4;k++)
      {
        for (l=0;l<4;l++)
        {
          if (vali&andi)
            OLEDMatrix.drawPixel(64+i*4+k,(1-col)*32+j*4+l,1);
          else
          {
            if (clear)
              OLEDMatrix.drawPixel(64+i*4+k,(1-col)*32+j*4+l,0);
          }
        }
      }
      andi/=2;
    }
  }
  if (show)
    OLEDMatrix.display();
}
#endif

#ifdef NEOLED
void NEODrawResetMode()
{
  int j,k;
  switch (ContChan)
  {
    case -1 :
      NEOPutChar(ActChannel+30,RGBMatrix.Color(255,0,0),true);
      NEOPutChar(ContChannel[ActChannel]+35,RGBMatrix.Color(0,0,255),false);
      break;
    case 0 :
      k=0;
      for (j=0;j<4;j++)
      {
        if (j!=ActChannel)
        {
          if ((Controller[ActChannel]==Controller[j])&&(Channel[ActChannel]==Channel[j]))
            k=1;
        }
      }
      if (k==1)
        NEOPutChar(Controller[ActChannel]-70,RGBMatrix.Color(255,0,160),true);
      else
        NEOPutChar(Controller[ActChannel]-70,RGBMatrix.Color(0,255,0),true);
      break;
    case 1 :
      NEOPutChar(Channel[ActChannel]+9,RGBMatrix.Color(255,180,0),true);
      break;
  }
  RGBMatrix.show();
}
#endif

#ifdef OLED
void OLEDDrawResetMode()
{
  int j,k;
  switch (ContChan)
  {
    case -1 :
      OLEDPutChar(ActChannel+30,1,true,false);
      OLEDPutChar(ContChannel[ActChannel]+35,1,false,true);
      break;
    case 0 :
      k=0;
      for (j=0;j<4;j++)
      {
        if (j!=ActChannel)
        {
          if ((Controller[ActChannel]==Controller[j])&&(Channel[ActChannel]==Channel[j]))
            k=1;
        }
      }
      if (k==1)
        OLEDPutChar(Controller[ActChannel]-70,0,true,true);
      else
        OLEDPutChar(Controller[ActChannel]-70,1,true,true);
      break;
    case 1 :
      OLEDPutChar(Channel[ActChannel]+9,1,true,true);
      break;
  }
  OLEDMatrix.display();
}
#endif

#ifdef NEOLED
void NEODrawMiniMax()
{
  char s[20];
  RGBMatrix.clear();
  sprintf(s,"%d",ActChannel+1);
  RGBMatrix.drawChar(1,1,s[0],RGBMatrix.Color(255,0,0),RGBMatrix.Color(0,0,0),1);
  for (i=0;i<8;i++)
  {
    RGBMatrix.drawPixel(i,0,RGBMatrix.Color(0,0,255));
    //RGBMatrix.drawPixel(i,7,RGBMatrix.Color(0,0,255));
    RGBMatrix.drawPixel(0,i,RGBMatrix.Color(0,0,255));
    RGBMatrix.drawPixel(7,i,RGBMatrix.Color(0,0,255));
  }
  RGBMatrix.show();
}
#endif

#ifdef OLED
void OLEDDrawMiniMax()
{
  char s[20];
  OLEDMatrix.clearDisplay();
  
  for (i=0;i<64;i+=2)
  {
    OLEDMatrix.drawPixel(i,0,1);
    OLEDMatrix.drawPixel(i, 63, 1);
    OLEDMatrix.drawPixel(0,i,1);
    OLEDMatrix.drawPixel(63,i,1);
    OLEDMatrix.drawLine(0,0,63,63,1);
    OLEDMatrix.drawLine(0,63,63,0,1);
  }
  OLEDMatrix.display();
}
#endif

void EEPROM_SAVE()
{
  int i,j;
  int eeadr;
  eeadr=0;
  for (i=0;i<4;i++)
  {
    EEPROM.put(eeadr,ContChannel[i]);
    eeadr+=sizeof(ContChannel[i]);
  }
  for (i=0;i<4;i++)
  {
    EEPROM.put(eeadr,Controller[i]);
    eeadr+=sizeof(Controller[i]);
  }
  for (i=0;i<4;i++)
  {
    EEPROM.put(eeadr,Channel[i]);
    eeadr+=sizeof(Channel[i]);
  }
  for (i=0;i<4;i++)
  {
    EEPROM.put(eeadr,Invert[i]);
    eeadr+=sizeof(Invert[i]);
  }
  for (i=0;i<4;i++)
  {
    EEPROM.put(eeadr,Mini[i]);
    eeadr+=sizeof(Mini[i]);
    EEPROM.put(eeadr,Maxi[i]);
    eeadr+=sizeof(Maxi[i]);
    EEPROM.put(eeadr,Omini[i]);
    eeadr+=sizeof(Omini[i]);
    EEPROM.put(eeadr,Omaxi[i]);
    eeadr+=sizeof(Omaxi[i]);
  }
}

void EEPROM_LOAD()
{
  int i,j;
  int eeadr;
  eeadr=0;
  for (i=0;i<4;i++)
  {
    EEPROM.get(eeadr,ContChannel[i]);
    eeadr+=sizeof(ContChannel[i]);
  }
  for (i=0;i<4;i++)
  {
    EEPROM.get(eeadr,Controller[i]);
    eeadr+=sizeof(Controller[i]);
  }
  for (i=0;i<4;i++)
  {
    EEPROM.get(eeadr,Channel[i]);
    eeadr+=sizeof(Channel[i]);
  }
  for (i=0;i<4;i++)
  {
    EEPROM.get(eeadr,Invert[i]);
    eeadr+=sizeof(Invert[i]);
  }
  for (i=0;i<4;i++)
  {
    EEPROM.get(eeadr,Mini[i]);
    eeadr+=sizeof(Mini[i]);
    EEPROM.get(eeadr,Maxi[i]);
    eeadr+=sizeof(Maxi[i]);
    EEPROM.get(eeadr,Omini[i]);
    eeadr+=sizeof(Omini[i]);
    EEPROM.get(eeadr,Omaxi[i]);
    eeadr+=sizeof(Omaxi[i]);
  }
}

void ScanKey1()
{
    if (!digitalRead(Key1))
    {
      ContChan=-1;
      if (OldKey[0])
      // OldKey1 was already set
      {
        if (KeyTimeCounter[0]>0)
          KeyTimeCounter[0]--;
      }
      else
      {
        // OldKey1 will be newly set, Timecounter reset
        OldKey[0]=true;
        KeyTimeCounter[0]=HOLDTIME2;
      }
    }
    /*  KeyTimeCounter==-1 -> No Key1 pressed
        KeyTimeCounter==0 -> Key1pressed for long enough
        KeytimeCounter>0 -> Look, if released now
    */
    if (KeyTimeCounter[0]>=0)
    {
      if (KeyTimeCounter[0]==0)
      {
        // Key1 pressed needed time to get or leave ResetMode
        if (ResetMode)
        {
          ResetMode=false;
          OLEDMatrix.clearDisplay();
        }

        else
        {
          ResetMode=true;
          #ifdef NEOLED
            NEODrawResetMode();
          #endif
          #ifdef OLED
            OLEDDrawResetMode();
          #endif
        }
        KeyTimeCounter[0]=-1;
        // Wait for release of Key1-Press
        do
        {
          ;
        } while (!digitalRead(Key1));
        OldKey[0]=false;
      }
      else
      {
        if (digitalRead(Key1))
        {
   // Key1 is released, toggle Watch-channel
          ActChannel++;
          if (ActChannel>3) ActChannel=0;
          #ifdef NEOLED
            NEODrawResetMode();
          #endif
          #ifdef OLED
            OLEDDrawResetMode();
          #endif
          KeyTimeCounter[0]=-1;
          OldKey[0]=false;
          Nachleucht=NL_FASTER;
        }
        // Otherwise still pressed
      }
    }

}

void ScanKey2()
{
      if (!digitalRead(Key2))
    {
      ContChan=-1;
      if (OldKey[1])
      // OldKey2 was already set
      {
        if (KeyTimeCounter[1]>0)
          KeyTimeCounter[1]--;
      }
      else
      {
        // OldKey1 will be newly set, Timecounter reset
        OldKey[1]=true;
        KeyTimeCounter[1]=HOLDTIME;
      }
    }
    /*  KeyTimeCounter==-1 -> No Key1 pressed
        KeyTimeCounter==0 -> Key1pressed for long enough
        KeytimeCounter>0 -> Look, if released now
    */
    if (KeyTimeCounter[1]>=0)
    {
      if (KeyTimeCounter[1]==0)
      {
        // Key2 pressed needed time to switch Solo
        if (Solo)
          Solo=false;
        else
          Solo=true;
        KeyTimeCounter[1]=-1;
        // Wait for release of Key1-Press
        do
        {
          ;
        } while (!digitalRead(Key2));
        OldKey[1]=false;
      }
      else
      {
        if (digitalRead(Key2))
        {
   // Key2 is released, Normalmode : Invert active Channel
          if (!ResetMode)
          {
            if (Invert[ActChannel])
            {
              Invert[ActChannel]=false;
            }
            else
            {
              Invert[ActChannel]=true;
            }
          }
          else
          // ResetMode : Measure Minimax
          {
            #ifdef NEOLED
              NEODrawMiniMax();
            #endif
            #ifdef OLED
              OLEDDrawMiniMax();
            #endif
            Mini[ActChannel]=1023; Maxi[ActChannel]=0;
            do
            {
              val=analogRead(ContChannel[ActChannel]);
              if (val<Mini[ActChannel]) Mini[ActChannel]=val;
              if (val>Maxi[ActChannel]) Maxi[ActChannel]=val;
            } while(digitalRead(Key2));
            do
            {
              ;
            } while(!digitalRead(Key2));
            #ifdef NEOLED
              NEODrawResetMode();
            #endif
            #ifdef OLED
              OLEDDrawResetMode();
            #endif
          }
          KeyTimeCounter[1]=-1;
          OldKey[1]=false;
        }
        // Otherwise still pressed
      }
    }

}

void ScanKey3()
{
      if (!digitalRead(Key3))
    {
      ContChan=-1;
      if (OldKey[2])
      // OldKey3 was already set
      {
        if (KeyTimeCounter[2]>0)
          KeyTimeCounter[2]--;
      }
      else
      {
        // OldKey3 will be newly set, Timecounter reset
        OldKey[2]=true;
        if (!ResetMode) KeyTimeCounter[2]=10; else KeyTimeCounter[2]=HOLDTIME;       
      }
    }
    /*  KeyTimeCounter==-1 -> No Key1 pressed
        KeyTimeCounter==0 -> Key1pressed for long enough
        KeytimeCounter>0 -> Look, if released now
    */
    if (KeyTimeCounter[2]>=0)
    {
      if (KeyTimeCounter[2]==0)
      {
          if (ResetMode)
          {
            ContChannel[ActChannel]--;
            if (ContChannel[ActChannel]<0) ContChannel[ActChannel]=3;
            #ifdef NEOLED
              NEODrawResetMode();
            #endif
            #ifdef OLED
              OLEDDrawResetMode();
            #endif
          }
          else
          {
            if (Omini[ActChannel]>0) Omini[ActChannel]--;
            controllerChange(Channel[ActChannel],Controller[ActChannel],Omini[ActChannel]);
            #ifdef NEOLED
              NEOPutChar(26,RGBMatrix.Color(0,255,180),true);
            #endif
            #ifdef OLED
              OLEDPutChar(26,1,true,true);
            #endif
            Nachleucht=NL_FASTER;
          }
          if (!ResetMode) KeyTimeCounter[2]=10; else KeyTimeCounter[2]=HOLDTIME;       
        // Otherwise still pressed
      }
      else
      {
        if (digitalRead(Key3))
        {
          if (ResetMode)
          {
            ContChannel[ActChannel]--;
            if (ContChannel[ActChannel]<0) ContChannel[ActChannel]=3;
            #ifdef NEOLED
              NEODrawResetMode();
            #endif
            #ifdef OLED
              OLEDDrawResetMode();
            #endif
          }
          else
          {
            if (Omini[ActChannel]>0) Omini[ActChannel]--;
            controllerChange(Channel[ActChannel],Controller[ActChannel],Omini[ActChannel]);
            #ifdef NEOLED
              NEOPutChar(26,RGBMatrix.Color(0,255,180),true);
            #endif
            #ifdef OLED
              OLEDPutChar(26,1,true,true);
            #endif
            Nachleucht=NL_FASTER;
          }
          KeyTimeCounter[2]=-1;
          //if (!ResetMode) KeyTimeCounter3=10; else KeyTimeCounter3=20;       
          OldKey[2]=false;
        }
      }
    }

}

void ScanKey4()
{
      if (!digitalRead(Key4))
    {
      ContChan=-1;
      if (OldKey[3])
      // OldKey4 was already set
      {
        if (KeyTimeCounter[3]>0)
          KeyTimeCounter[3]--;
      }
      else
      {
        // OldKey4 will be newly set, Timecounter reset
        OldKey[3]=true;
        if (!ResetMode) KeyTimeCounter[3]=10; else KeyTimeCounter[3]=HOLDTIME;       
      }
    }
    /*  KeyTimeCounter==-1 -> No Key1 pressed
        KeyTimeCounter==0 -> Key1pressed for long enough
        KeytimeCounter>0 -> Look, if released now
    */
    if (KeyTimeCounter[3]>=0)
    {
      if (KeyTimeCounter[3]==0)
      {
          if (ResetMode)
          {
            ContChannel[ActChannel]++;
            if (ContChannel[ActChannel]>3) ContChannel[ActChannel]=0;
            #ifdef NEOLED
              NEODrawResetMode();
            #endif
            #ifdef OLED
              OLEDDrawResetMode();
            #endif
          }
          else
          {
            if (Omini[ActChannel]<127) Omini[ActChannel]++;
            controllerChange(Channel[ActChannel],Controller[ActChannel],Omini[ActChannel]);
            #ifdef NEOLED
              NEOPutChar(27,RGBMatrix.Color(0,255,180),true);
            #endif
            #ifdef OLED
              OLEDPutChar(27,1,true,true);
            #endif
            Nachleucht=NL_FASTER;
          }
          if (!ResetMode) KeyTimeCounter[3]=10; else KeyTimeCounter[3]=HOLDTIME;       
        // Otherwise still pressed
      }
      else
      {
        if (digitalRead(Key4))
        {
          if (ResetMode)
          {
            ContChannel[ActChannel]++;
            if (ContChannel[ActChannel]>3) ContChannel[ActChannel]=0;
            #ifdef NEOLED
              NEODrawResetMode();
            #endif
            #ifdef OLED
              OLEDDrawResetMode();
            #endif
          }
          else
          {
            if (Omini[ActChannel]<127) Omini[ActChannel]++;
            controllerChange(Channel[ActChannel],Controller[ActChannel],Omini[ActChannel]);
            #ifdef NEOLED
              NEOPutChar(27,RGBMatrix.Color(0,255,180),true);
            #endif
            #ifdef OLED
              OLEDPutChar(27,1,true,true);
            #endif
            Nachleucht=NL_FASTER;
          }
          KeyTimeCounter[3]=-1;
          //if (!ResetMode) KeyTimeCounter3=10; else KeyTimeCounter3=20;       
          OldKey[3]=false;
        }
      }
    }

}

void ScanKey5()
{
    if (!digitalRead(Key5))
    {
      if (OldKey[4])
      // OldKey5 was already set
      {
        if (KeyTimeCounter[4]>0)
          KeyTimeCounter[4]--;
      }
      else
      {
        // OldKey5 will be newly set, Timecounter reset
        OldKey[4]=true;
        if (!ResetMode) KeyTimeCounter[4]=10; else KeyTimeCounter[4]=HOLDTIME;       
      }
    }
    /*  KeyTimeCounter==-1 -> No Key1 pressed
        KeyTimeCounter==0 -> Key1pressed for long enough
        KeytimeCounter>0 -> Look, if released now
    */
    if (KeyTimeCounter[4]>=0)
    {
      if (KeyTimeCounter[4]==0)
      {
          if (ResetMode)
          {
            switch (ContChan)
            {
              case -1 :
                ContChan=0;
                Nachleucht=NL_SLOW;
                break;
              case 0 :
                Controller[ActChannel]--;
                if (Controller[ActChannel]<70) Controller[ActChannel]=79;
                Nachleucht=NL_SLOW;
                break;
              case 1 :
                Channel[ActChannel]--;
                if (Channel[ActChannel]<1) Channel[ActChannel]=16;
                Nachleucht=NL_SLOW;
                break;
            }
            #ifdef NEOLED
              NEODrawResetMode();
            #endif
            #ifdef OLED
              OLEDDrawResetMode();
            #endif
          }
          else
          {
            if (Omaxi[ActChannel]>0) Omaxi[ActChannel]--;
            controllerChange(Channel[ActChannel],Controller[ActChannel],Omaxi[ActChannel]);
            #ifdef NEOLED
              NEOPutChar(28,RGBMatrix.Color(0,255,180),true);
            #endif
            #ifdef OLED
              OLEDPutChar(28,1,true,true);
            #endif
            Nachleucht=NL_FASTER;
          }
          if (!ResetMode) KeyTimeCounter[4]=10; else KeyTimeCounter[4]=HOLDTIME;       
        // Otherwise still pressed
      }
      else
      {
        if (digitalRead(Key5))
        {
          if (ResetMode)
          {
            switch (ContChan)
            {
              case -1 :
                MidiEnable=true;
                ContChan=0;
                Nachleucht=NL_SLOW;
                break;
              case 0 :
                Controller[ActChannel]--;
                if (Controller[ActChannel]<70) Controller[ActChannel]=79;
                Nachleucht=NL_SLOW;
                break;
              case 1 :
                Channel[ActChannel]--;
                if (Channel[ActChannel]<1)  Channel[ActChannel]=16;
                Nachleucht=NL_SLOW;
                break;
            }
            #ifdef NEOLED
              NEODrawResetMode();
            #endif
            #ifdef OLED
              OLEDDrawResetMode();
            #endif
          }
          else
          {
            if (Omaxi[ActChannel]>0) Omaxi[ActChannel]--;
            controllerChange(Channel[ActChannel],Controller[ActChannel],Omaxi[ActChannel]);
            #ifdef NEOLED
              NEOPutChar(28,RGBMatrix.Color(0,255,180),true);
            #endif
            #ifdef OLED
              OLEDPutChar(28,1,true,true);
            #endif
            Nachleucht=NL_FASTER;
          }
          KeyTimeCounter[4]=-1;
          //if (!ResetMode) KeyTimeCounter3=10; else KeyTimeCounter3=20;       
          OldKey[4]=false;
        }
      }
    }

}

void ScanKey6()
{
      if (!digitalRead(Key6))
    {
      if (OldKey[5])
      // OldKey6 was already set
      {
        if (KeyTimeCounter[5]>0)
          KeyTimeCounter[5]--;
      }
      else
      {
        // OldKey6 will be newly set, Timecounter reset
        OldKey[5]=true;
        if (!ResetMode) KeyTimeCounter[5]=10; else KeyTimeCounter[5]=HOLDTIME;       
      }
    }
    /*  KeyTimeCounter==-1 -> No Key1 pressed
        KeyTimeCounter==0 -> Key1pressed for long enough
        KeytimeCounter>0 -> Look, if released now
    */
    if (KeyTimeCounter[5]>=0)
    {
      if (KeyTimeCounter[5]==0)
      {
          if (ResetMode)
          {
            switch (ContChan)
            {
              case -1 :
                ContChan=1;
                Nachleucht=NL_SLOW;
                break;
              case 0 :
                Controller[ActChannel]++;
                if (Controller[ActChannel]>79) Controller[ActChannel]=70;
                Nachleucht=NL_SLOW;
                break;
              case 1 :
                Channel[ActChannel]++;
                if (Channel[ActChannel]>16)  Channel[ActChannel]=1;
                Nachleucht=NL_SLOW;
                break;
            }
            #ifdef NEOLED
              NEODrawResetMode();
            #endif
            #ifdef OLED
              OLEDDrawResetMode();
            #endif
          }
          else
          {
            if (Omaxi[ActChannel]<127) Omaxi[ActChannel]++;
            controllerChange(Channel[ActChannel],Controller[ActChannel],Omaxi[ActChannel]);
            #ifdef NEOLED
              NEOPutChar(29,RGBMatrix.Color(0,255,180),true);
            #endif
            #ifdef OLED
              OLEDPutChar(29,1,true,true);
            #endif
            Nachleucht=NL_FASTER;
          }
          if (!ResetMode) KeyTimeCounter[5]=10; else KeyTimeCounter[5]=HOLDTIME;       
        // Otherwise still pressed
      }
      else
      {
        if (digitalRead(Key6))
        {
          if (ResetMode)
          {
            switch (ContChan)
            {
              case -1 :
                ContChan=1;
                Nachleucht=NL_SLOW;
                break;
              case 0 :
                Controller[ActChannel]++;
                if (Controller[ActChannel]>79) Controller[ActChannel]=70;
                Nachleucht=NL_SLOW;
                break;
              case 1 :
                Channel[ActChannel]++;
                if (Channel[ActChannel]>16)  Channel[ActChannel]=1;
                Nachleucht=NL_SLOW;
                break;
            }
            #ifdef NEOLED
              NEODrawResetMode();
            #endif
            #ifdef OLED
              OLEDDrawResetMode();
            #endif
          }
          else
          {
            if (Omaxi[ActChannel]<127) Omaxi[ActChannel]++;
            controllerChange(Channel[ActChannel],Controller[ActChannel],Omaxi[ActChannel]);
            #ifdef NEOLED
              NEOPutChar(29,RGBMatrix.Color(0,255,180),true);
            #endif
            #ifdef OLED
              OLEDPutChar(29,1,true,true);
            #endif
            Nachleucht=NL_FASTER;
          }
          KeyTimeCounter[5]=-1;
          //if (!ResetMode) KeyTimeCounter3=10; else KeyTimeCounter3=20;       
          OldKey[5]=false;
        }
      }
    }

}

void ScanKey7()
{
      if (!digitalRead(Key7))
    {
      if (OldKey[6])
      // OldKey7 was already set
      {
        if (KeyTimeCounter[6]>0)
          KeyTimeCounter[6]--;
      }
      else
      {
        // OldKey1 will be newly set, Timecounter reset
        OldKey[6]=true;
        KeyTimeCounter[6]=HOLDTIME;
      }
    }
    /*  KeyTimeCounter==-1 -> No Key1 pressed
        KeyTimeCounter==0 -> Key1pressed for long enough
        KeytimeCounter>0 -> Look, if released now
    */
    if (KeyTimeCounter[6]>=0)
    {
      if (KeyTimeCounter[6]==0)
      {
        // Key7 pressed needed time to save EEPROM
        if (ResetMode)
        {
          MidiEnable=true;
          EEPROM_SAVE();
          #ifdef NEOLED
            NEOPutChar(39,RGBMatrix.Color(255,0,180),true);
          #endif
          #ifdef OLED
            OLEDPutChar(39,1,true,true);
          #endif
          Nachleucht=NL_SLOW;
        }
        else
        {
          ;
        }
        KeyTimeCounter[6]=-1;
        // Wait for release of Key7-Press
        do
        {
          ;
        } while (!digitalRead(Key7));
        OldKey[6]=false;
      }
      else
      {
        if (digitalRead(Key7))
        {
   // Key7 is released, Load EEPROM
          if (ResetMode)
          {
            MidiEnable=true;
            EEPROM_LOAD();
            #ifdef NEOLED
              NEOPutChar(40,RGBMatrix.Color(255,0,180),true);
            #endif
            #ifdef OLED
              OLEDPutChar(40,1,true,true);
            #endif
            Nachleucht=NL_SLOW;
          }
          else
          {
            // Change Smoothing
            ;
          }
          KeyTimeCounter[6]=-1;
          OldKey[6]=false;
          Nachleucht=NL_FAST;
        }
        // Otherwise still pressed
      }
    }

}

void ShowScreen()
{
  int Diff;
  if (!ResetMode)
  {
    for (k=0;k<4;k++)
    {
      ValueX[k]=Value[ContChannel[k]];
      if (ValueX[k]<Mini[k]) ValueX[k]=Mini[k];
      if (ValueX[k]>Maxi[k]) ValueX[k]=Maxi[k];
    }
    for (k=0;k<4;k++)
    {
      Diff=(Maxi[k]-Mini[k]);
      if (Invert[k])
      {
        Factor=(float)((Omaxi[k]-Omini[k])/(float)Diff);
        Value2[k]=Omaxi[k]-(int)(Factor*(float)(ValueX[k]-Mini[k]));
      }
      else
      {
        Factor=(float)((Omaxi[k]-Omini[k])/(float)Diff);
        Value2[k]=Omini[k]+(int)(Factor*(float)(ValueX[k]-Mini[k]));
      }
      if (k==ActChannel)
      { 
        DispDelay--;
        if (DispDelay<=0)
        {
          DispDelay=DISPDELAY;
          #ifdef NEOLED
            NEODisplay(ValueX[ActChannel]);
          #endif
          #ifdef OLED
            OLEDDisplay(ValueX[ActChannel]);
          #endif
          #ifdef NEOLED
            RGBMatrix.show();
          #endif
          #ifdef OLED
            OLEDMatrix.display();
          #endif
        }
      }
      if (Value2[k]>127) Value2[k]=127;
      if (Value2[k]<0) Value2[k]=0;
      if (Nachleucht<=0)
      {
        if (!Solo)
        {
          if (MidiEnable)
            controllerChange(Channel[k],Controller[k],Value2[k]);
        }
        else
        {
          if (k==ActChannel)
          {
            if (MidiEnable)
            {
              controllerChange(Channel[k],Controller[k],Value2[k]);
            }
          }
        }
      }
    }
  }
  else
  {
    if (Nachleucht--<0)
    {
      Nachleucht=-1;
      ContChan=-1;
      #ifdef NEOLED
        NEODrawResetMode();
      #endif
      #ifdef OLED
        OLEDDrawResetMode();
      #endif
    }
  }
}



void loop()
{
}

