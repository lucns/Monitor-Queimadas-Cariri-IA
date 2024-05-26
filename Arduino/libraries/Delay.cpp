// Developed by @lucns

#include "Delay.h"
#include "Arduino.h"

Delay::Delay() {
}

Delay::Delay(unsigned long interval) {
  timeStipulated = interval;
}

void Delay::reset() {
  start = millis();
  locked = false;
}

void Delay::cancel() {
  locked = true;
}

bool Delay::isRunning() {
  return !locked;
}

void Delay::setTime(unsigned long interval) {
  timeStipulated = interval;
  // reset();
}

bool Delay::gate() {
  if (millis() - start < timeStipulated || locked) return false;
  locked = true;
  return true;
}

bool Delay::isOverTime() {
  return millis() - start >= timeStipulated;
}

void Delay::forceGate() {
  locked = false;
  start = millis() - timeStipulated - 1;
}
