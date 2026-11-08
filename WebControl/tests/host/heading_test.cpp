#include "Heading.h"
#include <cassert>
#include <cmath>
#include <iostream>
uint32_t testTime = 0;
MotionSensor::Reading sample;
void tick(Heading& h, float rate, bool stationary = true, uint32_t step = 20) {
  testTime += step;
  sample.sampledAt = testTime;
  sample.gz = rate;
  h.update(sample, stationary);
}
void calibrate(Heading& h) {
  sample.valid = true; sample.ax = sample.ay = 0; sample.az = 1;
  assert(h.calibrate(true, sample));
  for (int i = 0; i < 100; ++i) tick(h, 0.7f);
  assert(h.ready() && !h.calibrating());
  assert(std::abs(h.bias() - 0.7f) < 0.001f);
}
int main() {
  Heading h;
  assert(!h.ready() && !h.zero(true));
  assert(!h.calibrate(false, sample));
  calibrate(h);
  for (int i = 0; i < 50; ++i) tick(h, 0.7f);
  assert(std::abs(h.angle()) < 0.001f);
  for (int i = 0; i < 50; ++i) tick(h, 90.7f, false);
  assert(std::abs(h.angle() - 89.1f) < 0.01f); // Trapezregel am Sprung 0 -> 90.
  h.update(sample, false); assert(std::abs(h.angle() - 89.1f) < 0.01f);
  for (int i = 0; i < 50; ++i) tick(h, 90.7f, false);
  assert(std::abs(h.angle() - 179.1f) < 0.02f);
  assert(!h.zero(false) && h.zero(true) && h.angle() == 0);
  tick(h, -89.3f, false); // Vorzeichenwechsel: mittlere Rate null.
  for (int i = 0; i < 50; ++i) tick(h, -89.3f, false);
  assert(std::abs(h.angle() + 90) < 0.02f);
  tick(h, 0.7f, true, 151); assert(!h.ready());
  calibrate(h); sample.valid = false; h.update(sample, true); assert(!h.ready());
  sample.valid = true; assert(h.calibrate(true, sample)); tick(h, 0.7f, false);
  assert(!h.calibrating() && !h.ready());
  assert(h.calibrate(true, sample)); tick(h, 0.7f); tick(h, 2);
  assert(!h.ready() && !h.calibrating());
  // Regression: Anfahrbeschleunigung darf nicht sofort als Kippen gelten.
  calibrate(h); sample.ax = 0.6f;
  for (int i = 0; i < 30; ++i) tick(h, 0.7f, false);
  assert(h.ready());
  // Kurze vertikale Entlastung und Querstoesse werden ebenfalls ueberstanden.
  sample.ax = 1.2f; sample.az = 0.3f;
  for (int i = 0; i < 5; ++i) tick(h, 0.7f, false);
  assert(h.ready());
  sample.ax = 0; sample.az = 1;
  for (int i = 0; i < 30; ++i) tick(h, 0.7f, false);
  assert(h.ready());
  // Kurze reale Neigung soll ebenfalls keinen Einzelwert-Abbruch ausloesen.
  sample.ax = 0.8660254f; sample.az = 0.5f;
  for (int i = 0; i < 5; ++i) tick(h, 0.7f, false);
  assert(h.ready());
  sample.ax = 0; sample.az = 1;
  for (int i = 0; i < 30; ++i) tick(h, 0.7f, false);
  assert(h.ready());
  // Eine laenger bestehende 60-Grad-Neigung wird weiterhin erkannt.
  sample.ax = 0.8660254f; sample.az = 0.5f;
  for (int i = 0; i < 100 && h.ready(); ++i) tick(h, 0.7f, false);
  assert(!h.ready());
  // Zeitueberlauf: kurze, neue Abstaende bleiben gueltig.
  sample.ax = 0; testTime = 0xFFFFF000u; calibrate(h);
  for (int i = 0; i < 150; ++i) tick(h, 0.7f);
  assert(h.ready() && std::abs(h.angle()) < 0.001f);
  std::cout << "Heading: bias, 90/180 degrees, sign, duplicate sample, gaps, motion rejection, wraparound passed\n";
}
