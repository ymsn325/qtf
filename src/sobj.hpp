#pragma once

#include <string>
#include <vector>

using namespace std;

class Sobj {
 protected:
  string m_id;
  vector<Sobj *> m_child_list;
  vector<Sobj *> m_parent_list;

 public:
  Sobj(string id);
  Sobj(string id, Sobj *parent);
  Sobj(string id, Sobj *parent1, Sobj *parent2);
  Sobj(string id, Sobj *parent1, Sobj *parent2, Sobj *parent3);
  virtual ~Sobj();
  void set_id(string id) { m_id = id; }
  void append_parent(Sobj *parent);
};
