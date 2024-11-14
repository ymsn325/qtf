#include <sstream>

#define MAX_DBL 1.7976931348623158e+308
#define MAX_INT 2147483647
#define PI 3.141592653589793238462643383279

extern string g_verbose_mode;

template <typename T>
inline T max2(T a, T b) {
  return a > b ? a : b;
}
template <typename T>
inline T min2(T a, T b) {
  return a < b ? a : b;
}
template <typename T>
inline T sqr(T a) {
  return a * a;
}
template <typename T>
inline void swap2(T &a, T &b) {
  T tmp = a;
  a = b;
  b = tmp;
}

inline string int2string(int n) {
  stringstream ss;

  ss.str("");
  ss.clear();
  ss << n;
  return ss.str();
}

inline int string2int(string s, int defalut_val = 0) {
  stringstream ss;
  int n;

  if (s.size() == 0) {
    return defalut_val;
  }
  ss.str("");
  ss.clear();
  ss << s;
  ss >> n;
  return n;
}

inline bool verbose(char c) {
  if ('a' <= c && c <= 'z') {
    if (g_verbose_mode.find(c) != string::npos ||
        (g_verbose_mode.find(c - ('a' - 'A') == string::npos &&
                             g_verbose_mode.find('A') == string::npos))) {
      return true;
    } else {
      return false;
    }
  } else if ('A' <= c && c <= 'Z') {
    if (g_verbose_mode.find(c + ('a' - 'A')) != string::npos ||
        (g_verbose_mode.find(c) == string::npos &&
         g_verbose_mode.find('a') == string::npos)) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}