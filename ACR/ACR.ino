#include <L298N.h>
#include <Servo.h>

//====================== MOTOR DC ======================

const int ENA = 3;
const int IN1 = 5;
const int IN2 = 6;
const int IN3 = 9;
const int IN4 = 10;
const int ENB = 11;

L298N driver(ENA, IN1, IN2, IN3, IN4, ENB);

int speedMotor = 100;      // 0-255

//====================== ESC ===========================

Servo esc;
const int ESC_PIN = 12;

//====================== HC-SR04 =======================

// Depan
#define TRIG_F A0
#define ECHO_F A1

// Kiri
#define TRIG_L A2
#define ECHO_L A3

// Kanan
#define TRIG_R A4
#define ECHO_R A5

const int batas = 20;

//======================================================

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_F, OUTPUT);
  pinMode(ECHO_F, INPUT);

  pinMode(TRIG_L, OUTPUT);
  pinMode(ECHO_L, INPUT);

  pinMode(TRIG_R, OUTPUT);
  pinMode(ECHO_R, INPUT);

  // ESC
  esc.attach(ESC_PIN);

  Serial.println("Arming ESC...");

  esc.writeMicroseconds(1000);

  delay(5000);

  Serial.println("Vacuum ON");

  esc.writeMicroseconds(1300);   // Kecepatan vakum
}

//======================================================

long bacaJarak(int trig, int echo)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  long durasi = pulseIn(echo, HIGH, 30000);

  if (durasi == 0)
    return 300;

  return durasi * 0.0343 / 2;
}

//======================================================

void drive(int motor, int s1, int s2)
{
  driver.setup_motor(motor, s1, s2);
  driver.drive_motor(motor, speedMotor);
}

void maju()
{
  drive(L298N::MOTOR_A, LOW, HIGH);
  drive(L298N::MOTOR_B, LOW, HIGH);
}

void mundur()
{
  drive(L298N::MOTOR_A, HIGH, LOW);
  drive(L298N::MOTOR_B, HIGH, LOW);
}

void kiri()
{
  drive(L298N::MOTOR_A, HIGH, LOW);
  drive(L298N::MOTOR_B, LOW, HIGH);
}

void kanan()
{
  drive(L298N::MOTOR_A, LOW, HIGH);
  drive(L298N::MOTOR_B, HIGH, LOW);
}

void stopRobot()
{
  driver.drive_motor(L298N::MOTOR_A, 0);
  driver.drive_motor(L298N::MOTOR_B, 0);
}

//======================================================

void loop()
{

  long depan = bacaJarak(TRIG_F, ECHO_F);

  Serial.print("Depan : ");
  Serial.print(depan);
  Serial.println(" cm");

  // Jalan kosong
  if (depan > batas)
  {
    maju();
  }

  // Ada halangan
  else
  {
    Serial.println("HALANGAN!");

    stopRobot();
    delay(200);

    // Mundur sedikit
    mundur();
    delay(350);

    stopRobot();
    delay(200);

    long kiriJarak = bacaJarak(TRIG_L, ECHO_L);
    long kananJarak = bacaJarak(TRIG_R, ECHO_R);

    Serial.print("Kiri  : ");
    Serial.println(kiriJarak);

    Serial.print("Kanan : ");
    Serial.println(kananJarak);

    if(kiriJarak > kananJarak)
    {
      Serial.println("Belok Kiri");

      kiri();

      delay(500);
    }
    else
    {
      Serial.println("Belok Kanan");

      kanan();

      delay(500);
    }

    stopRobot();

    delay(200);
  }
}