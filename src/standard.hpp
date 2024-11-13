#define MAX_DBL 1.7976931348623158e+308
#define MAX_INT 2147483647

string int2string(int n) {
  stringstream ss;

  ss.str("");
  ss.clear();
  ss << n;
  return ss.str();
}

int string2int(string s, int defalut_val = 0) {
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