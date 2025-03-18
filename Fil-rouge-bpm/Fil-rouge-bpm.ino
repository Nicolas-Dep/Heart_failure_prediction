#define USE_ARDUINO_INTERRUPTS true
// Include necessary libraries
#include <PulseSensorPlayground.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

//Paramétrage de l'écran
#define SCREEN_WIDTH 128  //on défini la longueur de l'écran OLED en pixels
#define SCREEN_HEIGHT 64  //on défini la hauteur de l'écran OLED en pixels
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TIME_HEADER "T"  // Header tag for serial time sync message
#define TIME_REQUEST 7   // ASCII bell character requests a time sync message

#define USE_ARDUINO_INTERRUPTS true

//Initialisation du RTC
ThreeWire myWire(4, 5, 2);
RtcDS1302<ThreeWire> Rtc(myWire);

// Constants
const int PULSE_SENSOR_PIN = 0;  // Analog PIN where the PulseSensor is connected
const int LED_PIN = 7;           // On-board LED PIN
const int THRESHOLD = 550;       // Threshold for detecting a heartbeat
//660
// Create PulseSensorPlayground object
PulseSensorPlayground pulseSensor;

int maladie = 2;
void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  
  if (!Rtc.IsDateTimeValid()) {
    //Peut être du à une batterie basse ou si c'est la première fois que l'on lance le code
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }
  if (Rtc.GetIsWriteProtected()) {
    //Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }
  if (!Rtc.GetIsRunning()) {
    //Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    //Serial.println("RTC is older than compile time! (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  // Configure PulseSensor
  pulseSensor.analogInput(PULSE_SENSOR_PIN);
  pulseSensor.blinkOnPulse(LED_PIN);
  pulseSensor.setThreshold(THRESHOLD);

  // Check if PulseSensor is initialized
  if (pulseSensor.begin()) {
    Serial.println("PulseSensor object created successfully!");
  }

  // Initialise la communication I²C à l’adresse 0x3C.
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);             // Pause for 2 seconds
}

void loop() {
  //si une seconde s'est écoulé, on affiche l'heure
  RtcDateTime now = Rtc.GetDateTime();
  //Serial.println(now.Hour() + now.Minute() + now.Second());
  if (!now.IsValid()){
    //Surement du à une batterie faible ou un problème de branchement
    Serial.println("RTC lost confidence in DateTime");
  }

  // Get the current Beats Per Minute (BPM)
  int currentBPM = pulseSensor.getBeatsPerMinute();
  /*if(maladie == 1){
    display.setTextSize(1);
    display.setCursor(100, 55);
    display.println("Risque maladie cardiaque");
  }else if(maladie == 0){
    display.setTextSize(1);
    display.setCursor(100, 55);
    display.println("Pas de risque de maladie cardiaque");
  }*/
  
  // Check if a heartbeat is detected
  if (pulseSensor.sawStartOfBeat()) {
    //Serial.println("♥ A HeartBeat Happened!");
    Serial.print("BPM: ");
    Serial.println(currentBPM);
    display.setTextSize(2);
    display.setCursor(35, 47);
    display.println(currentBPM);
    display.display();
    //fic = currentBPM;
  }


  // Add a small delay to reduce CPU usage
  delay(1000);  //On ajoute un delay de 1s
  //affichage de l'heure et de la date
  testtimeRTC(now);
  /*
  if (Serial.read() == 1){
    maladie = 1;
  } else if (Serial.read()== 0){
    maladie = 0;
  }*/
}

void testtimeRTC(RtcDateTime now) {
  //affichage structure écran
  display.clearDisplay();
  display.setTextSize(1);       //On définie la taille d'écriture sur l'écran OLED
  display.setTextColor(WHITE);  //On définie la couleur de l'écriture sur l'écran OLED
  display.setCursor(0, 10);     //On détermine le pixel de début de phrase
  display.println("Time:");

  display.drawLine(0, 8, 127, 8, WHITE);  //on déssine des lignes sur l'écran pour faire plus esthétique
  display.drawLine(0, 42, 127, 42, WHITE);


  display.setCursor(40, 0);  //On détermine le pixel de début de phrase
  display.println("Date:");

  display.setCursor(0, 45);  //On détermine le pixel de début de phrase
  display.println("BPM");
  display.display();

  //affichage de l'heure 
  display.setTextSize(1);       //on choisi la taille de l'écriture
  display.setTextColor(WHITE);  //la couleur du text
  display.setCursor(70, 0);     //on choisi l'emplacement du text
  display.println(now.Day());      //on affiche le jour en temps réel
  display.setTextSize(1);
  display.setCursor(78, 0);
  display.print("/");

  display.setTextSize(1);     //on choisi la taille du texte
  display.setCursor(85, 0);  //on choisi son emplacement
  display.println(now.Month());  //on affiche les mois en temps réel
  display.setCursor(93, 0);
  display.setTextSize(1);
  display.print("/");

  display.setTextSize(1);     //on choisi la taille du texte
  display.setCursor(100, 0);  //on choisi son emplacement
  display.println(now.Year());  //on affiche l'année en temps réel
  display.display();



  display.setTextSize(3);       //on choisi la taille de l'écriture
  display.setTextColor(WHITE);  //la couleur du text
  display.setCursor(0, 19);     //on choisi l'emplacement du text
  display.println(now.Hour());      //on affiche l'heure en temps réel
  display.setTextSize(2);
  display.setCursor(34, 24);
  display.print("h");

  display.setTextSize(3);     //on choisi la taille du texte
  display.setCursor(45, 19);  //on choisi son emplacement
  display.println(now.Minute());  //on affiche les minutes en temps réel
  display.setCursor(80, 24);
  display.setTextSize(2);
  display.print("m");

  display.setTextSize(2);     //on choisi la taille du texte
  display.setCursor(92, 24);  //on choisi son emplacement
  display.println(now.Second());  //on affiche les secondes en temps réel
  display.setCursor(115, 24);
  display.print("s");
  display.display();
}



void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600;  // Jan 1 2013

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if (pctime >= DEFAULT_TIME) {  // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime);             // Sync Arduino clock to the time received on the serial port
    }
  }
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

time_t requestSync() {
  Serial.write(TIME_REQUEST);
  return 0;  // the time will be sent later in response to serial mesg
}