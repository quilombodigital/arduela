#include "elapsedMillis.h"
#include "RunningMedian.h"
#include "Wire.h"
#include "SFE_MMA8452Q.h"
#include "Keyboard.h"

int potPin = A10;
int ledPin = A9;
int enabledPin = A8;
int tolerance = 0;
int val = 0;
bool blocked = false;
bool pressed = false;
MMA8452Q acelerometro(0x1C);
RunningMedian samplesx = RunningMedian(15);
RunningMedian samplesy = RunningMedian(15);
RunningMedian samplesz = RunningMedian(15);
elapsedMillis timeElapsed;

void setup()
{
    pinMode(ledPin, OUTPUT);
    pinMode(enabledPin, INPUT_PULLUP);
    digitalWrite(ledPin, LOW);
    acelerometro.init();
    Keyboard.begin();
}

void loop()
{
    bool enabled = digitalRead(enabledPin);
    if (enabled) {
        val = analogRead(potPin); // read the value from the sensor
        tolerance = val;
        if (acelerometro.available()) {
            acelerometro.read();
            samplesx.add(acelerometro.x);
            samplesy.add(acelerometro.y);
            samplesz.add(acelerometro.z);
            if (!blocked) {
                bool tmpx = (acelerometro.x > (samplesx.getAverage() + tolerance)) || (acelerometro.x < (samplesx.getAverage() - tolerance));
                bool tmpy = (acelerometro.y > (samplesy.getAverage() + tolerance)) || (acelerometro.y < (samplesy.getAverage() - tolerance));
                bool tmpz = (acelerometro.z > (samplesz.getAverage() + tolerance)) || (acelerometro.z < (samplesz.getAverage() - tolerance));
                if (tmpx || tmpy || tmpz) {
                    Keyboard.press(0xCD); //F12

                    digitalWrite(ledPin, HIGH);
                    blocked = true;
                    pressed = true;
                    timeElapsed = 0;
                }
            }
        }
    }
    if (pressed && (timeElapsed > 100) ) {
        Keyboard.release(0xCD); //F12
        pressed = false;
    }
    if (blocked && (timeElapsed > 500) ) {
        digitalWrite(ledPin, LOW);
        blocked = false;
    }
}
