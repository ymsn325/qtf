#define RGB_MAX 0.75

// 一般用
#define B0 0.00
#define B1 0.15
#define B2 0.20
#define B3 0.30
#define B4 0.55
#define B5 0.60

#define G0 0.15
#define G1 0.25
#define G2 0.40
#define G3 0.55
#define G4 0.60

#define R0 0.30
#define R1 0.45

void double2rgb(double x, unsigned char *r, unsigned char *g, unsigned char *b);
void double2rgb_bw(double x, unsigned char *r, unsigned char *g,
                   unsigned char *b);