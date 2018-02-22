#define KEY   13
#define NTC   A0
#define RELAY A1

const float R2 = 10000;
const float R0 = 9500;
const float T0 = 273.15 + 25;
const float B = 3950;

float Th_opts[] = { 19, 5 };
float Th = 0, V = 0, T = 0;
byte opt = 0;
byte buf[] = { 0,0 };
byte segs[] = { 0,1,2,3,5,6,7,A4 };
byte digs[] = { 4,A3 };
byte segments[] = {
  0b01010000,
  0b11111100,
  0b01001001,
  0b11001000,
  0b11100100,
  0b11000010,
  0b01000010,
  0b11110000,
  0b01000000,
  0b11000000,
};

#define MINUS 0b11101111
#define POINT 0xbf

void setup() {
  pinMode(NTC, INPUT);
  pinMode(KEY, INPUT_PULLUP);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  for (byte i=0; i<sizeof(segs); i++) {
    pinMode(segs[i], OUTPUT);
    digitalWrite(segs[i], HIGH);
  }
  for (byte i=0; i<sizeof(digs); i++) {
    pinMode(digs[i], OUTPUT);
    digitalWrite(digs[i], LOW);
  }
  Th = Th_opts[opt];
  blinkTh();
}

void loop() {
  debounce();
  measure();
  adjust();
  refresh();
}

void debounce() {
  static byte input = 0;
  input = input/2 + (digitalRead(KEY) ? 0 : 128);
  if (input > 253) {
    byte nopts = sizeof(Th_opts) / sizeof(Th_opts[0]);
    opt = (opt+1) % nopts;
    Th = Th_opts[opt];
    blinkTh();
  }
}

void measure() {
  V = (4*V + analogRead(NTC))/5;
  float R = R2 * V / (1023 - V);
  T = 1 / (1/T0 + log(R/R0)/B) - 273.15;
}

void adjust() {
  static long t0 = 0;
  long t = millis();
  if (t - t0 > 5000) {
    t0 = t;
    if (T > Th + 1) {
      digitalWrite(RELAY, LOW);
    } else if (T < Th - 1) {
      digitalWrite(RELAY, HIGH);
    }
  }
}

void blinkTh() {
  T = Th;
  for (byte i=0; i<3; i++) {
    for (byte j=0; j<50; j++)
      refresh();
    delay(500);
  }
}

void refresh() {
  if (T < 0) {
    buf[0] = MINUS;
    buf[1] = segments[int(abs(T))];
  } else if (T < 10) {
    buf[0] = segments[int(T)] & POINT;
    buf[1] = segments[int(T * 10) % 10];
  } else {
    T = round(T);
    buf[0] = segments[int(T / 10)];
    buf[1] = segments[int(T) % 10];
  }
  for (byte d=0; d<sizeof(digs); d++) {
    digitalWrite(digs[d], HIGH);
    for (byte i=0; i<sizeof(segs); i++) {
      digitalWrite(segs[i], bitRead(buf[d],7-i));
      delayMicroseconds(100);
      digitalWrite(segs[i], HIGH);
    }
    digitalWrite(digs[d], LOW);
  }
}

