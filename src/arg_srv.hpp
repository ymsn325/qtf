// #include <cstring>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

string g_program_name;
string g_verbose_mode;
bool g_reverse_mode;
bool g_bw_mode;

class ArgSrv {
 private:
  stringstream m_arg_stream;
  istringstream m_enum_stream;
  istringstream m_range_stream;
  bool m_flag_param_err;
  bool m_flag_default;
  bool m_flag_label;
  int m_help_level;
  vector<string> m_arg_string;

 public:
  ArgSrv(int argc, char** argv);
  void enumerate(const char* enum_str);
  void range(const char* range_str);
  template <typename T>
  bool check_enumerate(T* var);
  template <typename T>
  bool check_range(T* var);
  template <typename T1, typename T2>
  void set(T1* var, const char* name, const char* comment, T2 default_val);
  void finish();
};

ArgSrv::ArgSrv(int argc, char** argv) {
  int p = 1;
  g_program_name = argv[0];
  g_reverse_mode = false;
  g_bw_mode = false;
  m_flag_default = true;
  m_flag_label = false;
  m_help_level = 0;
  g_verbose_mode = "Q";

  while (p < argc) {
    if (strchr(argv[p], ' ') != nullptr) {
      // スペースがあるなら"1 2 3"のような文字列として処理
      m_arg_stream << "\"" << argv[p] << "\" ";
    } else if (strncmp(argv[p], "-rv", 3) == 0) {
      g_reverse_mode = true;
    } else if (strncmp(argv[p], "-bw", 3) == 0) {
      g_bw_mode = true;
    } else if (strncmp(argv[p], "-d", 2) == 0) {
      m_flag_default = true;
    } else if (strncmp(argv[p], "-l", 2) == 0) {
      m_flag_label = true;
    } else if (strncmp(argv[p], "-h", 2) == 0) {
      if (*(argv[p] + 2) == 0) {
        m_help_level = 4;
      } else {
        sscanf(argv[p] + 2, "%d", &m_help_level);
      }
    } else if (strncmp(argv[p], "-v", 2) == 0) {
      g_verbose_mode = (argv[p] + 2);
      if (g_verbose_mode == "") {
        g_verbose_mode = "a";
      }
    } else {
      m_arg_stream << argv[p] << " ";
      m_arg_string.push_back(argv[p]);
    }
    p++;
  }

  m_flag_param_err = false;
  if (m_help_level > 1) {
    cerr << g_program_name << "-hX -> Help mode" << endl;
    cerr << g_program_name << "-l -> Label mode" << endl;
    cerr << g_program_name << "-d -> Default mode" << endl;
    cerr << g_program_name << "-v -> Verbose mode" << endl;
    cerr << g_program_name << "-rv -> Reverse mode" << endl;
    cerr << g_program_name << "-bw -> Black and white mode" << endl;
  }

  if (m_help_level > 0 && m_help_level < 4) {
    cerr << g_program_name << endl;
  }
}

void ArgSrv::enumerate(const char* enum_str) { m_enum_stream.str(enum_str); }

void ArgSrv::range(const char* range_str) { m_range_stream.str(range_str); }

template <typename T>
bool ArgSrv::check_enumerate(T* var) {
  T enum_var;
  bool correct_input = true;

  m_enum_stream.seekg(0, ios::beg);
  if (!(m_enum_stream >> enum_var)) {
    if (m_enum_stream.eof()) {
      m_enum_stream.clear();
      return true;
    } else {
      cerr << g_program_name << ": Enumerate type error" << endl;
      exit(1);
    }
  }

  while (enum_var != *var) {
    if (m_enum_stream >> enum_var) {
      correct_input = true;
    } else {
      correct_input = false;
      break;
    }
  }

  m_enum_stream.clear();
  return correct_input;
}

template <typename T>
bool ArgSrv::check_range(T* var) {
  T range_min, range_max;
  bool correct_input = true;

  m_range_stream.seekg(0, ios::beg);
  if (!(m_range_stream >> range_min)) {
    if (m_range_stream.eof()) {
      m_range_stream.clear();
      return true;
    } else {
      cerr << g_program_name << ": Range type error" << endl;
      exit(1);
    }
  }

  if (!(m_range_stream >> range_max)) {
    cerr << g_program_name << ": Range max error" << endl;
    exit(1);
  }

  if (!(m_range_stream.eof())) {
    cerr << g_program_name << ": Range count error" << endl;
    exit(1);
  }

  if (*var < range_min || *var > range_max) {
    correct_input = false;
  }

  m_range_stream.clear();
  return correct_input;
}

template <typename T1, typename T2>
void ArgSrv::set(T1* var, const char* name, const char* comment,
                 T2 default_val) {
  bool correct_input;
  string input_string, tmp_string, tmp_string2;
  istringstream input_stream;
  stringstream tmp_stream;
  unsigned int pos;
  int extra;
  vector<string>::iterator p;

  // ヘルプモード
  if (m_help_level > 0) {
    *var = default_val;
    if (m_help_level == 4) {
      cerr << "  " << name << " : " << comment << " (default: " << default_val
           << ")";

      m_enum_stream.seekg(0, ios::beg);
      if (m_enum_stream >> tmp_string) {
        cerr << " (enum: ";
        cerr << tmp_string;
        while (m_enum_stream >> tmp_string) {
          cerr << ", " << tmp_string;
        }
        cerr << ")";
      }
      m_enum_stream.clear();

      m_range_stream.seekg(0, ios::beg);
      if (m_range_stream >> tmp_string) {
        cerr << " (range: ";
        cerr << tmp_string;
        while (m_range_stream >> tmp_string) {
          cerr << "-" << tmp_string;
        }
        cerr << ")";
      }
      m_range_stream.clear();
      cerr << endl;
    }

    if (m_help_level == 3) {
      cerr << name << " " << comment;
    }

    if (m_help_level < 3) {
      cerr << name;
    }
    return;
  }

  // ラベルモード
  if (m_flag_label) {
    *var = default_val;
    for (p = m_arg_string.begin(); p != m_arg_string.end() - 1; p++) {
      if (*p == name) {
        tmp_stream.str("");
        if (*(p + 1) != "+") {
          for (extra = 2;
               *(p + extra) != "+" && p + extra != m_arg_string.end() - 1;
               extra++) {
            tmp_stream << *(p + extra) << "+|+";
          }
        } else {
          extra = 1;
          tmp_stream << *(p + 1);
        }

        tmp_stream >> *var;
        if (!check_enumerate(var) || !check_range(var)) {
          cerr << "Param " << name << " = " << *var
               << " is not correct, so changed to default value" << endl;
          *var = default_val;
        }

        for (; extra >= 0; extra--) {
          m_arg_string.erase(p + extra);
        }
      }
    }
    return;
  }

  // 通常モード
  if (m_arg_stream >> *var) {
    correct_input = true;
    if (typeid(*var) == typeid(string)) {
      tmp_stream << *var;
      tmp_stream >> tmp_string;
      if ((strncmp(tmp_string.c_str(), "--", 2) == 0 &&
           tmp_string.size() == 2) ||
          (strncmp(tmp_string.c_str(), "d", 1) == 0 &&
           tmp_string.size() == 1)) {
        if (m_flag_default) {
          correct_input = true;
          *var = default_val;
        } else {
          correct_input = false;
          m_flag_param_err = true;
        }
      }

      if (strchr(tmp_string.c_str(), '"')) {
        tmp_string.erase(0, 1);
        m_arg_stream >> tmp_string2;
        while ((pos = tmp_string2.find('"')) == string::npos) {
          tmp_string.append("@$@");
          tmp_string.append(tmp_string2);
          m_arg_stream >> tmp_string2;
        }
        tmp_string.append("@$@");
        tmp_string2.erase(pos);
        tmp_string.append(tmp_string2);
        tmp_stream.clear();
        tmp_stream << tmp_string;
        tmp_stream >> *var;
      }
    }
  } else {
    if (m_flag_default) {
      correct_input = true;
      *var = default_val;
    } else {
      correct_input = false;
      m_flag_param_err = true;
    }
    m_arg_stream.clear();
    if (!(m_arg_stream >> tmp_string)) {
      m_arg_stream.clear();
    }
  }

  if (correct_input) {
    if (!check_enumerate(var)) {
      correct_input = false;
    }
    if (!check_range(var)) {
      correct_input = false;
    }
  }

  while (!correct_input) {
    if (m_flag_default) {
      correct_input = true;
      *var = default_val;
    } else {
      cerr << name << " " << comment;
      cerr << " (default: " << default_val << ") ";

      m_enum_stream.seekg(0, ios::beg);
      if (m_enum_stream >> tmp_string) {
        cerr << " (enum: ";
        cerr << tmp_string;
        while (m_enum_stream >> tmp_string) {
          cerr << ", " << tmp_string;
        }
        cerr << ")";
      }
      m_enum_stream.clear();

      m_range_stream.seekg(0, ios::beg);
      if (m_range_stream >> tmp_string) {
        cerr << " (range: ";
        cerr << tmp_string;
        while (m_range_stream >> tmp_string) {
          cerr << "-" << tmp_string;
        }
        cerr << ")";
      }
      m_range_stream.clear();

      cerr << ": " << flush;
      getline(cin, input_string);
      if (input_string.length() == 0) {
        *var = default_val;
        correct_input = true;
      } else {
        if ((pos = input_string.find(" ")) != string::npos) {
          do {
            input_string.replace(pos, 1, "@$@");
          } while ((pos = input_string.find(" ")) != string::npos);
        }

        input_stream.str(input_string);
        if (input_stream >> *var && check_enumerate(var) && check_range(var)) {
          correct_input = true;
        } else {
          cerr << "Bad parameter: " << input_string << endl;
          input_stream.clear();
          correct_input = false;
        }
      }
    }
  }

  m_enum_stream.str("");
  m_range_stream.str("");
}

void ArgSrv::finish() {
  if (m_help_level != 0) {
    if (m_help_level != 4) {
      cerr << endl;
    }
    exit(1);
  }

  if (m_flag_label) {
    if (m_arg_string.size() > 0) {
      cerr << "Size of arg_string: " << m_arg_string.size() << endl;
      cerr << "Probably name is not correct" << endl;
      exit(1);
    }
  }
}