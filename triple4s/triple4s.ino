#include <bluefruit.h>

BLEDis bledis;
// cd689ca0-a5f3-4215-b0f1-dec4f83846ce
// ce4638f8-c4de-f1b0-1542-f3a5a09c68cd
uint8_t guid[16] = { 0xcd, 0x68, 0x9c, 0xa0, 0xa5, 0xf3, 0x42, 0x15, 0xb0, 0xf1, 0xde, 0xc4, 0xf8, 0x38, 0x46, 0xce };
BLEService myService = BLEService(guid);
BLECharacteristic notifyChar = BLECharacteristic(0x0001);
BLECharacteristic readChar = BLECharacteristic(0x0002);

void setup() {
  Bluefruit.begin();
  Bluefruit.setName("Triple4s");
  
  myService.begin();
  notifyChar.setProperties(CHR_PROPS_NOTIFY);
  notifyChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  notifyChar.setFixedLen(1); // Alternatively .setMaxLen(uint16_t len)
  notifyChar.begin();

  readChar.setProperties(CHR_PROPS_READ);
  readChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  readChar.setFixedLen(1); // Alternatively .setMaxLen(uint16_t len)
  readChar.begin();

  bledis.setManufacturer("Triple 4s");
  bledis.setModel("BLE Pepper Spray");
  bledis.begin();

  startAdv();
  
  pinMode(LED_BUILTIN, OUTPUT);
}

bool lastState = false;
void loop() {
  int AIN0 = analogRead(A0);
  float Vout0 = AIN0 * (3.3 / 1023.0);
  bool active = (Vout0 < 0.20);
  if(active != lastState) {
    if(active) {
      digitalWrite(LED_BUILTIN, HIGH);
      setActive();
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      setInactive();
    }
  }
  lastState = active;
  delay(100);
//  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  setActive();
//  delay(2500);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  setInactive();
//  delay(2500);                       // wait for a second
}
uint8_t dat[1] = { 0x01 };
void setActive() {
  dat[0] = { 0x01 };
  readChar.write8(0x01);
  notifyChar.notify(dat, 1);
}
void setInactive() {
  dat[0] = { 0x00 };
  readChar.write8(0x00);
  notifyChar.notify(dat, 1);
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  
  // Include myService
  Bluefruit.Advertising.addService(myService);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}
