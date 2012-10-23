// **********************************************************
// ************************ openLRSng ***********************
// **********************************************************
// ** by Kari Hautio - kha @ AeroQuad/RCGroups/IRC(Freenode)
//
// This code is based on original OpenLRS and thUndeadMod
//
// This code
// - extend resolution to 10bits (1024 positions)
// - use HW timer for input capture mode for PPM input
// - use HW timer for PPM generation in waveform generator mode
// - collapse everything on single file
//
// Donations for development tools and utilities (beer) here
// https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=DSWGKGKPRX5CS

// **********************************************************
// ************ based on: OpenLRS thUndeadMod ***************
// Mihai Andrian - thUndead http://www.fpvuk.org/forum/index.php?topic=3642.0

// **********************************************************
// *************** based on: OpenLRS Code *******************
// ***  OpenLRS Designed by Melih Karakelle on 2010-2011  ***
// **  an Arudino based RC Rx/Tx system with extra futures **
// **       This Source code licensed under GPL            **
// **********************************************************

// **********************************************************
// ******************** OpenLRS DEVELOPERS ******************
// Mihai Andrian - thUndead http://www.fpvuk.org/forum/index.php?topic=3642.0
// Melih Karakelle (http://www.flytron.com) (forum nick name: Flytron)
// Jan-Dirk Schuitemaker (http://www.schuitemaker.org/) (forum nick name: CrashingDutchman)
// Etienne Saint-Paul (http://www.gameseed.fr) (forum nick name: Etienne)

//#############################
//### CONFIGURATION SECTION ###
//#############################

//####### COMPILATION TARGET #######
// Enable to compile transmitter code, default is RX
//#define COMPILE_TX

//####### TX BOARD TYPE #######
// tbd. 0 = Original M1 Tx Board
// tbd. 1 = OpenLRS Rx Board works as TX, reads your PPM signals from first servo port.
// tbd. 2 = Original M2 Tx Board
// 3 = OpenLRS Rx v2 Board works as TX, reads your PPM signals from first servo port.
#define TX_BOARD_TYPE 3

//####### RX BOARD TYPE #######
// tbd. 1 = OpenLRS Rx Board
// 3 = OpenLRS Rx v2 Board
#define RX_BOARD_TYPE 3


//####### BOOSTER (limit maximum power, affects only TX) #######
// 0 = No booster
// 1 = Booster
#define BOOSTER 0

//######### Band Select ##########
// 0 = 433Mhz
// 1 = 459Mhz
#define BAND 0

//######### TRANSMISSION VARIABLES ##########
#define CARRIER_FREQUENCY 435000  //  startup frequency

//####### Freq Hopping #######
// 1 = Enabled  0 = Disabled
#define FREQUENCY_HOPPING 1

//###### HOPPING CHANNELS #######
static unsigned char hop_list[6] = {22,19,19,34,49,42};

//###### RF DEVICE ID HEADERS #######
// Change this 4 byte values for isolating your transmission,
// RF module accepts only data with same header
static unsigned char RF_Header[4] = {'@','K','H','a'};

//###### SERIAL PORT SPEED #######
#define SERIAL_BAUD_RATE 115200 //115.200 baud serial port speed

//#############################
//### CONFIGURATION SECTION ###
//#############################

#include <Arduino.h>
#include <EEPROM.h>

#if defined(COMPILE_TX)
  #define BOARD_TYPE TX_BOARD_TYPE
#else
  #define BOARD_TYPE RX_BOARD_TYPE
#endif

//####### Board Pinouts #########

#if (BOARD_TYPE == 0)
    #define PPM_IN A5
    #define RF_OUT_INDICATOR A4
    #define BUZZER 9
    #define BTN 10
    #define Red_LED 12
    #define Green_LED 11

    #define Red_LED_ON  PORTB |= _BV(4);
    #define Red_LED_OFF  PORTB &= ~_BV(4);

    #define Green_LED_ON   PORTB |= _BV(3);
    #define Green_LED_OFF  PORTB &= ~_BV(3);

    #define PPM_Pin_Interrupt_Setup  PCMSK1 = 0x20;PCICR|=(1<<PCIE1);
    #define PPM_Signal_Interrupt PCINT1_vect
    #define PPM_Signal_Edge_Check (PINC & 0x20)==0x20

      //## RFM22B Pinouts for Public Edition (M1 or Rx v1)
      #define  nIRQ_1 (PIND & 0x08)==0x08 //D3
      #define  nIRQ_0 (PIND & 0x08)==0x00 //D3

      #define  nSEL_on PORTD |= (1<<4) //D4
      #define  nSEL_off PORTD &= 0xEF //D4

      #define  SCK_on PORTD |= (1<<2) //D2
      #define  SCK_off PORTD &= 0xFB //D2

      #define  SDI_on PORTC |= (1<<1) //C1
      #define  SDI_off PORTC &= 0xFD //C1

      #define  SDO_1 (PINC & 0x01) == 0x01 //C0
      #define  SDO_0 (PINC & 0x01) == 0x00 //C0

      #define SDO_pin A0
      #define SDI_pin A1
      #define SCLK_pin 2
      #define IRQ_pin 3
      #define nSel_pin 4

#endif

#if (BOARD_TYPE == 1)
    #define PPM_IN 5
    #define RF_OUT_INDICATOR 6
    #define BUZZER 7
    #define BTN 8

    #define Red_LED A3
    #define Green_LED A2

    #define Red_LED_ON  PORTC &= ~_BV(2);PORTC |= _BV(3);
    #define Red_LED_OFF  PORTC &= ~_BV(2);PORTC &= ~_BV(3);

    #define Green_LED_ON  PORTC &= ~_BV(3);PORTC |= _BV(2);
    #define Green_LED_OFF  PORTC &= ~_BV(3);PORTC &= ~_BV(2);

    #define PPM_Pin_Interrupt_Setup  PCMSK2 = 0x20;PCICR|=(1<<PCIE2);
    #define PPM_Signal_Interrupt PCINT2_vect
    #define PPM_Signal_Edge_Check (PIND & 0x20)==0x20

      //## RFM22B Pinouts for Public Edition (M1 or Rx v1)
      #define  nIRQ_1 (PIND & 0x08)==0x08 //D3
      #define  nIRQ_0 (PIND & 0x08)==0x00 //D3

      #define  nSEL_on PORTD |= (1<<4) //D4
      #define  nSEL_off PORTD &= 0xEF //D4

      #define  SCK_on PORTD |= (1<<2) //D2
      #define  SCK_off PORTD &= 0xFB //D2

      #define  SDI_on PORTC |= (1<<1) //C1
      #define  SDI_off PORTC &= 0xFD //C1

      #define  SDO_1 (PINC & 0x01) == 0x01 //C0
      #define  SDO_0 (PINC & 0x01) == 0x00 //C0

      #define SDO_pin A0
      #define SDI_pin A1
      #define SCLK_pin 2
      #define IRQ_pin 3
      #define nSel_pin 4
#endif

#if (BOARD_TYPE == 2)
    #define PPM_IN 3
    #define RF_OUT_INDICATOR A0
    #define BUZZER 10
    #define BTN 11
    #define Red_LED 13
    #define Green_LED 12

    #define Red_LED_ON  PORTB |= _BV(5);
    #define Red_LED_OFF  PORTB &= ~_BV(5);

    #define Green_LED_ON   PORTB |= _BV(4);
    #define Green_LED_OFF  PORTB &= ~_BV(4);

    #define PPM_Pin_Interrupt_Setup  PCMSK2 = 0x08;PCICR|=(1<<PCIE2);
    #define PPM_Signal_Interrupt PCINT2_vect
    #define PPM_Signal_Edge_Check (PIND & 0x08)==0x08

      //## RFM22B Pinouts for Public Edition (M2)
      #define  nIRQ_1 (PIND & 0x04)==0x04 //D2
      #define  nIRQ_0 (PIND & 0x04)==0x00 //D2

      #define  nSEL_on PORTD |= (1<<4) //D4
      #define  nSEL_off PORTD &= 0xEF //D4

      #define  SCK_on PORTD |= (1<<7) //D7
      #define  SCK_off PORTD &= 0x7F //D7

      #define  SDI_on PORTB |= (1<<0) //B0
      #define  SDI_off PORTB &= 0xFE //B0

      #define  SDO_1 (PINB & 0x02) == 0x02 //B1
      #define  SDO_0 (PINB & 0x02) == 0x00 //B1

      #define SDO_pin 9
      #define SDI_pin 8
      #define SCLK_pin 7
      #define IRQ_pin 2
      #define nSel_pin 4
#endif

#if (BOARD_TYPE == 3)

  #ifdef COMPILE_TX
    #define PPM_IN 8 // ICP1
    #define RF_OUT_INDICATOR 5
    #define BUZZER 6
    #define BTN 7
  #else
    #define PPM_OUT 9 // OCP1A
  #endif

  #define Red_LED A3
  #define Green_LED 13

  #define Red_LED_ON  PORTC |= _BV(3);
  #define Red_LED_OFF  PORTC &= ~_BV(3);    // Was originally #define Green_LED_OFF  PORTB |= _BV(5);   E.g turns it ON not OFF

  #define Green_LED_ON  PORTB |= _BV(5);
  #define Green_LED_OFF  PORTB &= ~_BV(5);

  //## RFM22B Pinouts for Public Edition (Rx v2)
  #define  nIRQ_1 (PIND & 0x04)==0x04 //D2
  #define  nIRQ_0 (PIND & 0x04)==0x00 //D2

  #define  nSEL_on PORTD |= (1<<4) //D4
  #define  nSEL_off PORTD &= 0xEF //D4

  #define  SCK_on PORTC |= (1<<2) //A2
  #define  SCK_off PORTC &= 0xFB //A2

  #define  SDI_on PORTC |= (1<<1) //A1
  #define  SDI_off PORTC &= 0xFD //A1

  #define  SDO_1 (PINC & 0x01) == 0x01 //A0
  #define  SDO_0 (PINC & 0x01) == 0x00 //A0

  #define SDO_pin A0
  #define SDI_pin A1
  #define SCLK_pin A2
  #define IRQ_pin 2
  #define nSel_pin 4

  #define IRQ_interrupt 0

#endif

//############ common prototypes ########################

void RF22B_init_parameter(void);
void to_tx_mode(void);
void to_rx_mode(void);
volatile unsigned char tx_buf[11]; // TX buffer
volatile unsigned char rx_buf[11]; // RX buffer

unsigned char RF_channel = 0;
#define PPM_CHANNELS 8
volatile int          PPM[PPM_CHANNELS] = { 512,512,512,512,512,512,512,512 };

#ifdef COMPILE_TX

//############ VARIABLES ########################

unsigned char FSstate = 0; // 1 = waiting timer, 2 = send FS, 3 sent waiting btn release
unsigned long FStime = 0;  // time when button went down...

volatile unsigned int newData = 0; // new data to be sent


volatile unsigned int startPulse = 0;
volatile byte         ppmCounter = PPM_CHANNELS; // ignore data until first sync pulse

#define TIMER1_FREQUENCY_HZ 50
#define TIMER1_PRESCALER    8
#define TIMER1_PERIOD       (F_CPU/TIMER1_PRESCALER/TIMER1_FREQUENCY_HZ)

/****************************************************
 * Interrupt Vector
 ****************************************************/
ISR(TIMER1_CAPT_vect) {
  unsigned int stopPulse = ICR1;

  // Compensate for timer overflow if needed
  unsigned int pulseWidth = ((startPulse > stopPulse) ? TIMER1_PERIOD : 0) + stopPulse - startPulse;

  if (pulseWidth > 5000) {      // Verify if this is the sync pulse (2.5ms)
    ppmCounter = 0;             // -> restart the channel counter
    newData++;              // new data to be TX:ed
  }
  else {
    if (ppmCounter < PPM_CHANNELS) { // extra channels will get ignored here

      int out = ((int)pulseWidth - 1976) / 2; // convert to 0-1023 (1976 - 4024 ; 0.988 - 2.012 ms)
      if (out<0) out=0;
      if (out>1023) out=1023;

      PPM[ppmCounter] = out; // Store measured pulse length
      ppmCounter++;                     // Advance to next channel
    }
  }
  startPulse = stopPulse;         // Save time at pulse start
}

void setupPPMinput() {
  // Setup timer1 for input capture (PSC=8 -> 0.5ms precision, top at 20ms)
  TCCR1A = ((1<<WGM10)|(1<<WGM11));
  TCCR1B = ((1<<WGM12)|(1<<WGM13)|(1<<CS11)|(1<<ICES1));
  OCR1A = TIMER1_PERIOD;
  TIMSK1 |= (1<<ICIE1); // Enable timer1 input capture interrupt
}

void setup() {

  //RF module pins
  pinMode(SDO_pin, INPUT); //SDO
  pinMode(SDI_pin, OUTPUT); //SDI
  pinMode(SCLK_pin, OUTPUT); //SCLK
  pinMode(IRQ_pin, INPUT); //IRQ
  pinMode(nSel_pin, OUTPUT); //nSEL

  //LED and other interfaces
  pinMode(Red_LED, OUTPUT); //RED LED
  pinMode(Green_LED, OUTPUT); //GREEN LED
  pinMode(BUZZER, OUTPUT); //Buzzer
  pinMode(BTN, INPUT); //Buton

  pinMode(PPM_IN, INPUT); //PPM from TX
  pinMode(RF_OUT_INDICATOR, OUTPUT);

  Serial.begin(SERIAL_BAUD_RATE);

  setupPPMinput();

  RF22B_init_parameter();

  sei();

  digitalWrite(BUZZER, HIGH);
  digitalWrite(BTN, HIGH);
  Red_LED_ON ;
  delay(100);

  Check_Button();

  Red_LED_OFF;
  digitalWrite(BUZZER, LOW);

  digitalWrite(RF_OUT_INDICATOR,LOW);
  //digitalWrite(PPM_IN,HIGH);

  newData = 0;
  rx_reset();

}


//############ MAIN LOOP ##############
void loop() {

  /* MAIN LOOP */

  if (_spi_read(0x0C)==0) // detect the locked module and reboot
  {
    Serial.println("module locked?");
    Red_LED_ON;
    RF22B_init_parameter();
    rx_reset();
    Red_LED_OFF;
  }

  if (newData) {
    unsigned long start = micros();
    unsigned int now = newData;
    newData = 0;

    if (FSstate == 2) {
      tx_buf[0] = 0xF5; //DO FAILSAFE
    } else {
      tx_buf[0] = 0x5E; //servo positions
    }

    cli(); // disable interrupts when copying servo positions, to avoid race on 2 byte variable
    tx_buf[1]=(PPM[0] & 0xff);
    tx_buf[2]=(PPM[1] & 0xff);
    tx_buf[3]=(PPM[2] & 0xff);
    tx_buf[4]=(PPM[3] & 0xff);
    tx_buf[5]=((PPM[0] >> 8) & 3) | (((PPM[1] >> 8) & 3)<<2) | (((PPM[2] >> 8) & 3)<<4) | (((PPM[3] >> 8) & 3)<<6);
    tx_buf[6]=(PPM[4] & 0xff);
    tx_buf[7]=(PPM[5] & 0xff);
    tx_buf[8]=(PPM[6] & 0xff);
    tx_buf[9]=(PPM[7] & 0xff);
    tx_buf[10]=((PPM[4] >> 8) & 3) | (((PPM[5] >> 8) & 3)<<2) | (((PPM[6] >> 8) & 3)<<4) | (((PPM[7] >> 8) & 3)<<6);
    sei();

    //Green LED will be on during transmission
    Green_LED_ON ;

    // Send the data over RF
    to_tx_mode();
    Serial.print("TX:");
    Serial.print(now);
    Serial.print(';');
    Serial.print(RF_channel);
    Serial.print(';');
    Serial.println(micros()-start);
    #if (FREQUENCY_HOPPING==1)
    Hopping();//Hop to the next frequency
    #endif
  }

  //Green LED will be OFF
  Green_LED_OFF;

  checkFS();
}

//############# BUTTON CHECK #################
void Check_Button(void)
{
  unsigned long time,loop_time;

  if (digitalRead(BTN)==0) // Check the button
    {
    delay(1000); // wait for 1000mS when buzzer ON
    digitalWrite(BUZZER, LOW); // Buzzer off

    time = millis();  //set the current time
    loop_time = time;

    while ((digitalRead(BTN)==0) && (loop_time < time + 4000)) {
      // wait for button reelase if it is already pressed.
      loop_time = millis();
    }

    //Check the button again. If it is already pressed start the binding proscedure
    if (digitalRead(BTN)!=0) {
      // if button released, reduce the power for range test.
      spiWriteRegister(0x6d, 0x00);
    }
  }
}


void checkFS(void)
{
  switch (FSstate) {
    case 0:
      if (digitalRead(BTN) == 0) {
        FSstate=1;
        FStime=millis();
      }
      break;
    case 1:
      if (digitalRead(BTN) == 0) {
        if ((millis() - FStime) > 1000) {
          FSstate = 2;
          Red_LED_ON
        }
      } else {
        FSstate = 0;
      }
      break;
    case 2:
      if (digitalRead(BTN)) {
        FSstate=0;
        Red_LED_OFF
      }
      break;
  }
}

#else // COMPILE_RX

volatile unsigned char RF_Mode = 0;
#define Available 0
#define Transmit 1
#define Transmitted 2
#define Receive 3
#define Received 4

unsigned long time;
unsigned long last_pack_time ;

unsigned char RSSI_count = 0;
unsigned short RSSI_sum = 0;

int ppmCountter=0;
int ppmTotal=0;

short FShop,firstpack =0;
short lostpack =0;

boolean willhop =0;

void RFM22B_Int()
{
 if (RF_Mode == Transmit)
    {
     RF_Mode = Transmitted;
    }
 if (RF_Mode == Receive)
    {
     RF_Mode = Received;
    }
}

ISR(TIMER1_OVF_vect) {
  if (ppmCountter >= PPM_CHANNELS) {
    ICR1 = 40000 - ppmTotal; // 20ms total frame
    ppmCountter=0;
    ppmTotal=0;
  } else {
    int  ppmOut = 1976 + PPM[ppmCountter++] * 2; // 0-1023 -> 1976 - 4023 (0.988 - 2.012ms)
    ppmTotal += ppmOut;
    ICR1 = ppmOut;
  }
}

void setupPPMout() {
  TCCR1A = (1<<WGM11)|(1<<COM1A1)|(1<<COM1A0);
  TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
  ICR1 = 40000; // just initial value, will be constantly updated
  OCR1A = 600;  // 0.3ms pulse
  TIMSK1 |= (1<<TOIE1);

  pinMode(PPM_OUT, OUTPUT);
}

void setup() {
  //LEDs
  pinMode(Green_LED, OUTPUT);
  pinMode(Red_LED, OUTPUT);

  //RF module pins
  pinMode(SDO_pin, INPUT); //SDO
  pinMode(SDI_pin, OUTPUT); //SDI
  pinMode(SCLK_pin, OUTPUT); //SCLK
  pinMode(IRQ_pin, INPUT); //IRQ
  pinMode(nSel_pin, OUTPUT); //nSEL

  pinMode(0, INPUT); // Serial Rx
  pinMode(1, OUTPUT);// Serial Tx

  Serial.begin(SERIAL_BAUD_RATE); //Serial Transmission

  setupPPMout();

  attachInterrupt(IRQ_interrupt,RFM22B_Int,FALLING);

//  receiver_mode = check_modes(); // Check the possible jumper positions for changing the receiver mode.

//  load_failsafe_values();   // Load failsafe values on startup

  Red_LED_ON;

  RF22B_init_parameter(); // Configure the RFM22B's registers

  sei();

  //################### RX SYNC AT STARTUP #################
  RF_Mode = Receive;
  to_rx_mode();

  firstpack =0;

}

//############ MAIN LOOP ##############
void loop() {

  time = millis();

  if (_spi_read(0x0C)==0) { // detect the locked module and reboot
    Serial.println("RX hang");
    RF22B_init_parameter();
    to_rx_mode();
  }

  if(RF_Mode == Received) {  // RFM22B INT pin Enabled by received Data
    Serial.print("R");
    RF_Mode = Receive;
    last_pack_time = time; // record last package time
    lostpack=0;

    if (firstpack ==0)  firstpack =1;

    Red_LED_OFF;
    Green_LED_ON;

    send_read_address(0x7f); // Send the package read command

    for (int i=0; i<11; i++) {
      rx_buf[i] = read_8bit_data();
    }

    cli();
    PPM[0]= rx_buf[1] + ((rx_buf[5] & 0x03) << 8);
    PPM[1]= rx_buf[2] + ((rx_buf[5] & 0x0c) << 6);
    PPM[2]= rx_buf[3] + ((rx_buf[5] & 0x30) << 4);
    PPM[3]= rx_buf[4] + ((rx_buf[5] & 0xc0) << 2);
    PPM[4]= rx_buf[6] + ((rx_buf[10] & 0x03) << 8);
    PPM[5]= rx_buf[7] + ((rx_buf[10] & 0x0c) << 6);
    PPM[6]= rx_buf[8] + ((rx_buf[10] & 0x30) << 4);
    PPM[7]= rx_buf[9] + ((rx_buf[10] & 0xc0) << 2);
    sei();

    RSSI_sum += _spi_read(0x26); // Read the RSSI value
    RSSI_count++;
    rx_reset();


    for (int i=0; i<8; i++) {
      Serial.print(PPM[i]);
      Serial.print(',');
    }
    Serial.println();


    if (RSSI_count > 20) {
      RSSI_sum /=20;
      //Serial.println(Rx_RSSI,DEC);
      RSSI_sum = 0;
      RSSI_count = 0;
    }

    willhop =1;

    Green_LED_OFF;
  }

#if (FREQUENCY_HOPPING==1)
  if (willhop==1) {
    Hopping();//Hop to the next frequency
    willhop =0;
  }
#endif

}

#endif


//####### FUNCTIONS #########

void Red_LED_Blink(unsigned short blink_count) {
  for (unsigned char i=0; i<blink_count; i++)
  {
    delay(100);
    Red_LED_ON;
    delay(100);
    Red_LED_OFF;
  }
}

//############# GREEN LED BLINK #################
void Green_LED_Blink(unsigned short blink_count) {
  for (unsigned char i=0; i<blink_count; i++)
  {
    delay(100);
    Green_LED_ON;
    delay(100);
    Green_LED_OFF;
  }
}

//############# FREQUENCY HOPPING ################# thUndead FHSS
#if (FREQUENCY_HOPPING==1)
void Hopping(void)
{
  RF_channel++;
  Serial.print(RF_channel);
  if ( RF_channel >= (sizeof(hop_list) / sizeof(hop_list[0])) ) RF_channel = 0;
  spiWriteRegister(0x79, hop_list[RF_channel]);
}
#endif



// **********************************************************
// **      RFM22B/Si4432 control functions for OpenLRS     **
// **       This Source code licensed under GPL            **
// **********************************************************

#define NOP() __asm__ __volatile__("nop")

#define RF22B_PWRSTATE_POWERDOWN    0x00
#define RF22B_PWRSTATE_READY        0x01
#define RF22B_PACKET_SENT_INTERRUPT 0x04
#define RF22B_PWRSTATE_RX           0x05
#define RF22B_PWRSTATE_TX           0x09

#define RF22B_Rx_packet_received_interrupt   0x02

unsigned char ItStatus1, ItStatus2;

unsigned char read_8bit_data(void);
void send_8bit_data(unsigned char i);
void send_read_address(unsigned char i);
void spiWriteRegister(unsigned char address, unsigned char data);

void port_init(void);
unsigned char _spi_read(unsigned char address);
void Write0( void );
void Write1( void );
void Write8bitcommand(unsigned char command);
void to_sleep_mode(void);


//*****************************************************************************
//*****************************************************************************

//--------------------------------------------------------------
void Write0( void ) {
    SCK_off;
    NOP();
    SDI_off;
    NOP();
    SCK_on;
    NOP();
}
//--------------------------------------------------------------
void Write1( void ) {
    SCK_off;
    NOP();
    SDI_on;
    NOP();
    SCK_on;
    NOP();
}
//--------------------------------------------------------------
void Write8bitcommand(unsigned char command) {   // leave sel low

  nSEL_on;
  SCK_off;
  nSEL_off;
  for (unsigned char n=0; n<8 ; n++) {
    if(command&0x80)
      Write1();
    else
      Write0();
    command = command << 1;
  }
  SCK_off;
}


//--------------------------------------------------------------
unsigned char _spi_read(unsigned char address) {
  unsigned char result;
  send_read_address(address);
  result = read_8bit_data();
  nSEL_on;
  return(result);
}

//--------------------------------------------------------------
void spiWriteRegister(unsigned char address, unsigned char data) {
  address |= 0x80; // 
  Write8bitcommand(address);
  send_8bit_data(data);
  nSEL_on;
}


//-------Defaults 38.400 baud----------------------------------------------
void RF22B_init_parameter(void) {
  
  ItStatus1 = _spi_read(0x03); // read status, clear interrupt
  ItStatus2 = _spi_read(0x04);
  spiWriteRegister(0x06, 0x00);    // no wakeup up, lbd,
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);      // disable lbd, wakeup timer, use internal 32768,xton = 1; in ready mode
  spiWriteRegister(0x09, 0x7f);  // c = 12.5p
  spiWriteRegister(0x0a, 0x05);
  spiWriteRegister(0x0b, 0x12);    // gpio0 TX State
  spiWriteRegister(0x0c, 0x15);    // gpio1 RX State

  spiWriteRegister(0x0d, 0xfd);    // gpio 2 micro-controller clk output
  spiWriteRegister(0x0e, 0x00);    // gpio    0, 1,2 NO OTHER FUNCTION.

  spiWriteRegister(0x70, 0x2C);    // disable manchest

       // 9.6Kbps data rate
  spiWriteRegister(0x6e, 0x27); // WAS 0x27 (for 4k8)
  spiWriteRegister(0x6f, 0x52); // WAS 0x52 (for 4k8)

  spiWriteRegister(0x1c, 0x1A); // case RATE_384K
  spiWriteRegister(0x20, 0xA1);//  0x20 calculate from the datasheet= 500*(1+2*down3_bypass)/(2^ndec*RB*(1+enmanch))
  spiWriteRegister(0x21, 0x20); // 0x21 , rxosr[10--8] = 0; stalltr = (default), ccoff[19:16] = 0;
  spiWriteRegister(0x22, 0x4E); // 0x22    ncoff =5033 = 0x13a9
  spiWriteRegister(0x23, 0xA5); // 0x23
  spiWriteRegister(0x24, 0x00); // 0x24
  spiWriteRegister(0x25, 0x1B); // 0x25
  spiWriteRegister(0x1D, 0x40); // 0x25
  spiWriteRegister(0x1E, 0x0A); // 0x25

  spiWriteRegister(0x2a, 0x1e);


  spiWriteRegister(0x30, 0x8c);    // enable packet handler, msb first, enable crc,

  spiWriteRegister(0x32, 0xf3);    // 0x32address enable for headere byte 0, 1,2,3, receive header check for byte 0, 1,2,3
  spiWriteRegister(0x33, 0x42);    // header 3, 2, 1,0 used for head length, fixed packet length, synchronize word length 3, 2,
  spiWriteRegister(0x34, 0x01);    // 7 default value or   // 64 nibble = 32byte preamble
  spiWriteRegister(0x36, 0x2d);    // synchronize word
  spiWriteRegister(0x37, 0xd4);
  spiWriteRegister(0x38, 0x00);
  spiWriteRegister(0x39, 0x00);
  spiWriteRegister(0x3a, RF_Header[0]); // tx header
  spiWriteRegister(0x3b, RF_Header[1]);
  spiWriteRegister(0x3c, RF_Header[2]);
  spiWriteRegister(0x3d, RF_Header[3]);
  spiWriteRegister(0x3e, 11);           // 11 byte normal packet

  //RX HEADER
  spiWriteRegister(0x3f, RF_Header[0]);   // verify header
  spiWriteRegister(0x40, RF_Header[1]);
  spiWriteRegister(0x41, RF_Header[2]);
  spiWriteRegister(0x42, RF_Header[3]);
  spiWriteRegister(0x43, 0xff);    // all the bit to be checked
  spiWriteRegister(0x44, 0xff);    // all the bit to be checked
  spiWriteRegister(0x45, 0xff);    // all the bit to be checked
  spiWriteRegister(0x46, 0xff);    // all the bit to be checked

  #if (BOOSTER == 0)
    spiWriteRegister(0x6d, 0x07); // 7 set power max power
  #else
    spiWriteRegister(0x6d, 0x06); // 6 set power 50mw for booster
  #endif

  #if (FREQUENCY_HOPPING==1)
    spiWriteRegister(0x79, hop_list[0]);    // start channel
  #else
    spiWriteRegister(0x79, 0);    // no hopping
  #endif

  #if (BAND== 0)
    spiWriteRegister(0x7a, 0x06);    // 60khz step size (10khz x value) // no hopping
  #else
    spiWriteRegister(0x7a, 0x05); // 50khz step size (10khz x value) // no hopping
  #endif

  spiWriteRegister(0x71, 0x23); // Gfsk, fd[8] =0, no invert for Tx/Rx data, fifo mode, txclk -->gpio
  spiWriteRegister(0x72, 0x30); // frequency deviation setting to 19.6khz (for 38.4kbps)

  spiWriteRegister(0x73, 0x00);
  spiWriteRegister(0x74, 0x00);    // no offset

  //band 435.000

  #if (BAND== 0)
    spiWriteRegister(0x75, 0x53);
  #else
    spiWriteRegister(0x75, 0x55);  //450 band
  #endif

  // frequency formulation from Si4432 chip's datasheet
  // original formulation is working with mHz values and floating numbers, I replaced them with kHz values.
  long frequency = CARRIER_FREQUENCY;
  frequency = frequency / 10;
  frequency = frequency - 24000;
  #if (BAND== 0)
  frequency = frequency - 19000; // 19 for 430?439.9 MHz band from datasheet
  #else
  frequency = frequency - 21000; // 21 for 450?459.9 MHz band from datasheet
  #endif
  frequency = frequency * 64; // this is the Nominal Carrier Frequency (fc) value for register setting

  spiWriteRegister(0x76, (byte) (frequency >> 8));
  spiWriteRegister(0x77, (byte) frequency);

}


//--------------------------------------------------------------
void send_read_address(unsigned char i) {
 i &= 0x7f;
 Write8bitcommand(i);
}

//--------------------------------------------------------------
void send_8bit_data(unsigned char i) {
  unsigned char n = 8;
  SCK_off;
    while(n--)
    {
         if(i&0x80) {
           Write1();
         } else {
          Write0();
         }
         i = i << 1;
    }
   SCK_off;
}
//--------------------------------------------------------------

unsigned char read_8bit_data(void) {

  unsigned char Result, i;
  SCK_off;
  Result=0;
  for(i=0;i<8;i++)
  {                    //read fifo data byte
    Result=Result<<1;
    SCK_on;
    NOP();
    if(SDO_1)
    {
      Result|=1;
    }
    SCK_off;
    NOP();
  }
  return(Result);
}
//--------------------------------------------------------------

//-----------------------------------------------------------------------
void rx_reset(void) {

  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  spiWriteRegister(0x7e, 36);    // threshold for rx almost full, interrupt when 1 byte received
  spiWriteRegister(0x08, 0x03);    //clear fifo disable multi packet
  spiWriteRegister(0x08, 0x00);    // clear fifo, disable multi packet
  spiWriteRegister(0x07, RF22B_PWRSTATE_RX );  // to rx mode
  spiWriteRegister(0x05, RF22B_Rx_packet_received_interrupt);
  ItStatus1 = _spi_read(0x03);  //read the Interrupt Status1 register
  ItStatus2 = _spi_read(0x04);
}
//-----------------------------------------------------------------------


void to_tx_mode(void) {

  // ph +fifo mode
  spiWriteRegister(0x34, 0x06);  // 64 nibble = 32byte preamble
  spiWriteRegister(0x3e, 11);    // total tx 10 byte

  for (unsigned char i=0; i<11; i++) {
    spiWriteRegister(0x7f, tx_buf[i]);
  }

  spiWriteRegister(0x05, RF22B_PACKET_SENT_INTERRUPT);
  ItStatus1 = _spi_read(0x03);      //read the Interrupt Status1 register
  ItStatus2 = _spi_read(0x04);
  spiWriteRegister(0x07, RF22B_PWRSTATE_TX);    // to tx mode

  while(nIRQ_1);
}

void to_rx_mode(void)
{
  ItStatus1 = _spi_read(0x03);
  ItStatus2 = _spi_read(0x04);
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  delay(10);
  rx_reset();
  NOP();
}

//--------------------------------------------------------------
