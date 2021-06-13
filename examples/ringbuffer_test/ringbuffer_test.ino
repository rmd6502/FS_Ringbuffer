#include "fs_ringbuffer.h"


void setup() {
    Serial.begin(115200);
    Serial.println("Starting test");
    FS_RingBuffer buffer("test", 100);
    RBStatus status = buffer.begin();
    if (buffer.freeSpace() != 100) {
        Serial.print("expected 100 bytes free, found "); Serial.println(buffer.freeSpace());
    }
    String testData = "0123456789";
    for (int i=0; i < 10; ++i) {
        status = buffer.write(testData);
        if (status != RB_OK) {
            Serial.print("write "); Serial.print(i); Serial.print(" returned "); Serial.println(status);
            break;
        }
    }
    if (status == RB_OK) {
        status = buffer.write("A");
        if (status != RB_INSUFFICIENT_SPACE) {
            Serial.print("write 101th byte returned "); Serial.println(status);
        }
    }
    String ret = buffer.read(25);
    if (ret != "0123456789012345678901234") {
        Serial.print("read 25 bytes returned "); Serial.println(ret);
    }
    if (buffer.freeSpace() != 75) {
        Serial.print("expected 75 bytes free, got "); Serial.println(buffer.freeSpace());
    }
    ret = buffer.read(25);
    if (ret != "5678901234567890123456789") {
        Serial.print("read 25 bytes returned "); Serial.println(ret);
    }
    if (buffer.freeSpace() != 50) {
        Serial.print("expected 50 bytes free, got "); Serial.println(buffer.freeSpace());
    }
}

void loop() {

}
