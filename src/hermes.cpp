#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BMI160Gen.h>
#include <cstdlib>

// Display OLED (96x64)
constexpr unsigned SCREEN_WIDTH = 96;
constexpr unsigned SCREEN_HEIGHT = 64;
constexpr unsigned SDA_GPIO = 21;
constexpr unsigned SCL_GPIO = 22;
constexpr unsigned BMI160_I2C_ADDRESS = 0x68;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {
   Serial.begin(115200);
   delay(1000);

   // initialize I2C
   Wire.begin(SDA_GPIO, SCL_GPIO);

   // initialize display
   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println("SSD1306 non found");
      while (true)
         ;
   }

   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(SSD1306_WHITE);
   display.setCursor(0, 0);
   display.println("Init BMI160...");
   display.display();

   // initialize BMI160
   if (!BMI160.begin(BMI160GenClass::I2C_MODE)) {
      Serial.println("BMI160 not found, restarting firmware...");
      display.display();
      delay(2000);
      exit(EXIT_FAILURE); // restart firmware
   }
   uint8_t dev_id = BMI160.getDeviceID();
   display.print("DEVICE ID: ");
   display.println(dev_id, HEX);
   display.display();
   BMI160.setGyroRange(250);
}

void loop() {
   int gxRaw, gyRaw, gzRaw; // raw gyro values
   float gx, gy, gz;

   // read raw gyro measurements from device
   BMI160.readGyro(gxRaw, gyRaw, gzRaw);

   // convert the raw gyro data to degrees/second
   gx = convertRawGyro(gxRaw);
   gy = convertRawGyro(gyRaw);
   gz = convertRawGyro(gzRaw);

   // display tab-separated gyro x/y/z values
   display.clearDisplay();
   display.setCursor(0, 10);
   display.print("gx: ");
   display.println(gx);
   display.print("gy: ");
   display.println(gy);
   display.print("gz: ");
   display.println(gz);
   display.display();

   delay(500);
}

float convertRawGyro(int gRaw) {
   // since we are using 250 degrees/seconds range
   // -250 maps to a raw value of -32768
   // +250 maps to a raw value of 32767

   float g = (gRaw * 250.0) / 32768.0;

   return g;
}
