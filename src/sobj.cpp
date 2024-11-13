#include "sobj.hpp"

vector<Sobj *> g_sobj_list;

Sobj::Sobj(string id) : m_id(id) { g_sobj_list.push_back(this); }

Sobj::Sobj(string id, Sobj *parent) : m_id(id) {
  if (parent != nullptr) {
    parent->m_child_list.push_back(this);
    m_parent_list.push_back(parent);
  }
  g_sobj_list.push_back(this);
}

Sobj::Sobj(string id, Sobj *parent1, Sobj *parent2) : m_id(id) {
  if (parent1 != nullptr) {
    parent1->m_child_list.push_back(this);
    m_parent_list.push_back(parent1);
  }
  if (parent2 != nullptr) {
    parent2->m_child_list.push_back(this);
    m_parent_list.push_back(parent2);
  }
  g_sobj_list.push_back(this);
}

Sobj::Sobj(string id, Sobj *parent1, Sobj *parent2, Sobj *parent3) : m_id(id) {
  if (parent1 != nullptr) {
    parent1->m_child_list.push_back(this);
    m_parent_list.push_back(parent1);
  }
  if (parent2 != nullptr) {
    parent2->m_child_list.push_back(this);
    m_parent_list.push_back(parent2);
  }
  if (parent3 != nullptr) {
    parent3->m_child_list.push_back(this);
    m_parent_list.push_back(parent3);
  }
  g_sobj_list.push_back(this);
}

Sobj::~Sobj() {
  for (auto child : m_child_list) {
    delete child;
  }
}
