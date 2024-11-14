#include "pixmap.hpp"
#include "tf.hpp"

#define CONTRAST_MIN_DEFAULT 0.40
#define CONTRAST_MAX_DEFAULT 0.85
#define MAX_DB_LOWER_FROM_UPPER 160.0

class TFmap : public Pixmap {
 private:
  double m_contrast_min, m_contrast_max;
  double m_limit_lower, m_limit_upper;
  int m_k_bgn, m_k_end, m_k_step, m_k_size;
  int m_k_dup;
  int m_n_bgn, m_n_end, m_n_step, m_n_size;
  int m_n_dup;
  TF *m_parent_tf;
  double *m_pow;

 public:
  TFmap(string id, TF *tf, int k_bgn, int k_end, int height, int n_dup,
        double contrast_min = CONTRAST_MIN_DEFAULT,
        double contrast_max = CONTRAST_MAX_DEFAULT, double limit_lwoer = 0.0,
        double limit_upper = 0.0);
  ~TFmap();
  bool modify(int n_bgn, int n_end);
};