#include "image.hpp"

#include <cmath>
#include <iostream>

using namespace std;

void double2rgb(double x, unsigned char *r, unsigned char *g,
                unsigned char *b) {
  double top = 1.0, bottom = 0.0;

  if (x <= bottom) {
    x = 0.0;
  } else if (x < top) {
    x = (x - bottom) / (top - bottom);
  } else {
    x = 1.0;
  }

  x *= RGB_MAX;

  if (x < R0) {
    *r = 0;
  } else if (x < R1) {
    *r = (unsigned char)(255.0 * pow((x - R0) / (R1 - R0), 0.6));
  } else {
    *r = 255;
  }

  if (x < G0) {
    *g = 0;
  } else if (x < G1) {
    *g = (unsigned char)(255.0 * pow((x - G0) / (G1 - G0), 0.7));
  } else if (x < G2) {
    *g = 255;
  } else if (x < G3) {
    *g = (unsigned char)(255.0 * pow((G3 - x) / (G3 - G2), 0.8));
  } else if (x < G4) {
    *g = 0;
  } else {
    *g = (unsigned char)(255.0 * pow((x - G4) / (RGB_MAX - G4), 0.7));
  }

  if (x < B0) {
    *b = 0;
  } else if (x < B1) {
    *b = (unsigned char)(255.0 * pow((x - B0) / (B1 - B0), 1.5));
  } else if (x < B2) {
    *b = 255;
  } else if (x < B3) {
    *b = (unsigned char)(255.0 * pow((B3 - x) / (B3 - B2), 0.7));
  } else if (x < B4) {
    *b = 0;
  } else if (x < B5) {
    *b = (unsigned char)(255.0 * pow((x - B4) / (B5 - B4), 2.5));
  } else {
    *b = 255;
  }
}

void double2rgb_bw(double x, unsigned char *r, unsigned char *g,
                   unsigned char *b) {
  double top = 1.0, bottom = 0.0;

  if (x <= bottom) {
    x = 0.0;
  } else if (x < top) {
    x = (x - bottom) / (top - bottom);
  } else {
    x = 1.0;
  }

  *r = *g = *b = (unsigned char)(255.0 * x);
}