
#include <SPI.h>
#include <MFRC522.h>
#include "RaspberryLinker.h"
#include "toWrite.h"

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
#define LED_VERIF       3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

String command; //Se débarasser des String: trop lourd
String written;
String lu;
char** parameters;
RaspberryLinker *rpy = new RaspberryLinker();

/**
 * Initialize.
 */
void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card
    pinMode(LED_VERIF, OUTPUT);
    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
    Serial.print(F("Using key (for A and B):"));
    Serial.println();

    Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
}

/**
 * Main loop.
 */
void loop() {
  
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
        

    
    command = rpy->receiveCommand();//<=>Serial.readStringUntil('\n')
    command.trim();
    
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    //Cas commande de lecture. Exemple readRFID 4 où 4 est le bloc à lire
    if (command.substring(0,8).equals("readRFID")) {
      String block_str = command.substring(9);//Récupération du numéro de bloc dans la ligne de commande
      block_str.trim();
      int block = block_str.toInt();
      int sector = block/4; //Détermination du secteur
      int trailerBlock = (block/4)*4+3; // Détermination du bloc à authentifier en fonction du bloc à lire
       
      // Authenticate using key A
      Serial.println(F("Authenticating using key A..."));
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
          Serial.print(F("PCD_Authenticate() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
      }
  
      // Show the whole sector as it currently is
     /* Serial.println(F("Current data in sector:"));
      mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
      Serial.println();*/
    
      // Read data from the block
      Serial.print(F("Reading data from block ")); Serial.print(block);
      Serial.println(F(" ..."));
      status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(block, buffer, &size);//Récupération du numéro de bloc dans la ligne de commande
      if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Read() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
      }
      else{
        digitalWrite(LED_VERIF, HIGH);
        delay(500);
        digitalWrite(LED_VERIF, LOW);
        Serial.print(F("Data in block ")); Serial.print(block); Serial.println(F(":"));
        lu = (char*)buffer; //Transformation du buffer lu en string
        rpy->sendCommand("sensor " + (String)NAME + " " + lu );//Serial.println(lu);
        Serial.println();
        Serial.println();
      }

    }

    //Cas commande d'écriture.
    else if (command.substring(0,16).equals("readAndWriteRFID")) {
      Serial.println("ok");
      byte buffr[] = {0x20,0x20,0x20,0x20,
                      0x20,0x20,0x20,0x20,
                      0x20,0x20,0x20,0x20,
                      0x20,0x20,0x20,0x20};//Initialisation avec un buffer vide
            
      String block_str = command.substring(17);//Récupération du numéro de bloc dans la ligne de commande
      block_str.trim();
      int block = block_str.toInt();
      int sector = block/4; //Détermination du secteur
      int trailerBlock = (block/4)*4+3; // Détermination du bloc à authentifier en fonction du bloc à écrire
      
      //Transformation du string à écrire en buffer 
      //if (block<10) command.substring(12).getBytes(buffr, 16); // Exemple: writeRFID 4 yes
      //else command.substring(13).getBytes(buffr, 16);  // Exemple: writeRFID 12 non
      
      // Authenticate using key A
      Serial.println(F("Authenticating using key A..."));
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
          Serial.print(F("PCD_Authenticate() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
      }
  
      // Show the whole sector as it currently is
     /* Serial.println(F("Current data in sector:"));
      mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
      Serial.println();*/
    
      // Read data from the block
      Serial.print(F("Reading data from block ")); Serial.print(block);
      Serial.println(F(" ..."));
      status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(block, buffer, &size);
      if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Read() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
      }
      else{
        digitalWrite(LED_VERIF, HIGH);
        delay(500);
        digitalWrite(LED_VERIF, LOW);
        Serial.print(F("Data in block ")); Serial.print(block); Serial.println(F(":"));
        lu = (char*)buffer; //Transformation du buffer lu en string
        rpy->sendCommand("sensor " + (String)NAME + " " + lu );//Serial.println(lu);
        Serial.println();
        Serial.println();
      }

      written = linkedEvent(lu);
      block = linkedBlock(lu);
      written.getBytes(buffr, 16);
      trailerBlock = (block/4)*4+3;
      
        // Authenticate using key B
      Serial.println(F("Authenticating again using key B..."));
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
          Serial.print(F("PCD_Authenticate() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
      }
  
      // Write data to the block
      Serial.print(F("Writing data into block ")); Serial.print(block);
      Serial.println(F(" ..."));
      Serial.println();
      

      status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(block, buffr, 16);
      if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Write() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
      }
      else{
        Serial.println();
        digitalWrite(LED_VERIF, HIGH);
        delay(500);
        digitalWrite(LED_VERIF, LOW);
        //Vérifier que l'écriture est correcte (avec relecture puis comparaison) 
        // Read data from the block (again, should now be what we have written)
        Serial.print(F("Reading data again from block ")); Serial.print(block);
        Serial.println(F(" ..."));
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(block, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
        Serial.print(F("Data in block ")); Serial.print(block); Serial.println(F(":"));
        String str = (char*)buffer;//Transformation du buffer lu en string
        rpy->sendCommand( str );
        Serial.println();
      }
    }
    
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}
