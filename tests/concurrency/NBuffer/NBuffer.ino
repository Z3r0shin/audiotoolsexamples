/**
 * @file NBuffer.ino
 * @author Phil Schatzmann
 * @brief multi tasks test with unsynchronized NBuffer
 * @version 0.1
 * @date 2022-11-25
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "AudioTools.h"

NBuffer<int16_t> buffer(512, 4);

Task writeTask("write", 3000, 10, 0);

Task readTask("read", 3000, 10, 1);

void setup() {
  Serial.begin(115200);

  writeTask.begin([]() {
    int16_t data[512];
    for (int j = 0; j < 512; j++) {
      data[j] = j;
    }
    assert(buffer.writeArray(data, 512)==512);
    // prevent watchdog
    delay(1);

  });

  readTask.begin([]() {
    static uint64_t start = millis();
    static size_t total_bytes = 0;
    static size_t errors = 0;
    static int16_t data[512];

    // read data
    size_t read = buffer.readArray(data, 512);
    delay(1);
    if (read==0) return;
    assert(read==512);

    // check data
    for (int j = 0; j < 512; j++) {
      if (data[j] != j) errors++;
    }
    // calculate bytes per second
    total_bytes += sizeof(int16_t) * 512;
    if (total_bytes >= 1024000) {
      uint64_t duration = millis() - start;
      float mbps = static_cast<float>(total_bytes) / duration / 1000.0;

      // print result
      Serial.print("Mbytes per second: ");
      Serial.print(mbps);
      Serial.print(" with ");
      Serial.print(errors);
      Serial.println(" errors");

      start = millis();
      errors = 0;
      total_bytes = 0;
    }
  });
}

void loop() { delay(1000); }