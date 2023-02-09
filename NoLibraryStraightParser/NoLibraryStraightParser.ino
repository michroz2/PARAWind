#include <SoftwareSerial.h>

// Дебагирование: раскомментить для использования 1 строчку:
#define DEBUG_ENABLE
#ifdef DEBUG_ENABLE
#define DEBUG(x) Serial.print(x)
#define DEBUGln(x) Serial.println(x)
#else
#define DEBUG(x)
#define DEBUGln(x)
#endif

SoftwareSerial gps(4, 3);

struct GPSData {
  bool good;    //достоверны ли данные
  long time;    //секунды*100
  long lat;     //latitude minutes*10^5 "+" = N
  long lon;     //longitude минуты*10^5 "+" = E
  long speed;   //knots*100
  long course;  //degrees*100
  String date;
};

String NMEAString;
String NMEAType;

GPSData rmc;



// ===================== SETUP ===================
void setup() {
  Serial.begin(115200);
  gps.begin(9600);

  Serial.println("START++++++++++++");
}


// ================================== LOOP==========
void loop() {
  while (gps.available()) {
    if (gps.read() == '$') {
      NMEAType = gps.readStringUntil(',');
      // Serial.println(NMEAType);
      if (NMEAType.endsWith("RMC")) {
        // NMEAString = gps.readStringUntil(10);
        // Serial.println(NMEAString);
        // rmc = parse(NMEAString);
        rmc = parseRMC();
        if (rmc.good) {
          Serial.println("Tim= " + String(rmc.time));
          Serial.println("Lat= " + String(rmc.lat));
          Serial.println("Lon= " + String(rmc.lon));
          Serial.println("Spd= " + String(rmc.speed));
          Serial.println("Crs= " + String(rmc.course));
          Serial.println("Dat= " + rmc.date);
        }
        // while (gps.available()) gps.read();
      }
    }
  }
}  // void loop() {

GPSData parseRMC() {

  String timeS = gps.readStringUntil('.');
  DEBUGln(timeS);
  String timeSD = gps.readStringUntil(',');
  String goodS = gps.readStringUntil(',');
  String latS = gps.readStringUntil('.');
  String latSD = gps.readStringUntil(',');
  String NS = gps.readStringUntil(',');
  String lonS = gps.readStringUntil('.');
  String lonSD = gps.readStringUntil(',');
  String EW = gps.readStringUntil(',');
  String speedS = gps.readStringUntil('.');
  String speedSD = gps.readStringUntil(',');
  String courseS = gps.readStringUntil('.');
  String courseSD = gps.readStringUntil(',');
  String dateS = gps.readStringUntil(',');
  // gps.readStringUntil(13);

  //parse time into centiseconds:
  long t = timeS.toInt();
  long cs = timeSD.substring(0, 2).toInt();
  long h, m, s;
  s = t % 100;
  m = (t / 100) % 100;
  h = t / 10000;
  t = (((h * 60) + m) * 60 + s) * 100 + cs;  // centi-seconds

  //parse Lat into 10th of microminutes:
  long lat = latS.toInt();
  long mmin = latSD.substring(0, 5).toInt();
  long ld = lat / 100;
  long lm = lat % 100;
  lat = ((ld * 60) + lm) * 10000 + mmin;  //10th of milli-minutes
  if (NS == "S") lat = lat * (-1);

  //parse Lon into 10th of microminutes:
  long lon = lonS.toInt();
  mmin = lonSD.substring(0, 5).toInt();
  ld = lon / 100;
  lm = lon % 100;
  lon = ((ld * 60) + lm) * 10000 + mmin;  //10th of milli-minutes
  if (EW == "W") lon = lon * (-1);

  //parse speed into centi-knots:
  long spd = speedS.toInt();
  cs = speedSD.substring(0, 2).toInt();
  spd = spd * 100 + cs;  // centi-knots

  //parse course into centi-degrees:
  long crs = courseS.toInt();
  cs = courseSD.substring(0, 2).toInt();
  crs = crs * 100 + cs;  //centi-degrees

  //format date:
  String dat = dateS.substring(0, 2) + "." + dateS.substring(2, 4) + "." + dateS.substring(4);

  GPSData res;
  res.good = (goodS == "A");
  res.time = t;
  res.lat = lat;
  res.lon = lon;
  res.speed = spd;
  res.course = crs;
  res.date = dat;
  return res;
}  // void parse(String RMC) {

GPSData parse(String RMC) {

  GPSData res;
  res.good = true;
  res.time = 123;
  return res;
}  // void parse(String RMC) {
