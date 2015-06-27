#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include <memory>
#include <atomic>
#include <map>
#include <thread>
#include <array>
#include "core/err.h"
#include <glm/glm.hpp>

#include "ui/vertexattrib.h"

using namespace std;
using namespace glm;

struct PositionGroup{
  VertexAttrib<float,2>         m_position;
  VertexAttrib<float,2>         m_size;
  VertexAttrib<float,1>         m_depth;
  VertexAttrib<int,  1>         m_id;
  vector<int>                   m_parent;
  vector<array<float,2>>        m_relative_position;
  std::unordered_map<int,int>   m_item_id;
public:
  struct virtual_entry {
    array<float,2> &m_position;
    array<float,2> &m_size;;
    array<float,1> &m_depth;
    array<int,  1> &m_id;;
    int       &m_parent;
    array<float,2>&m_relative_position;
    virtual_entry(array<float,2>&pos, array<float,2> &size, array<float,1> &depth, array<int,1>&id, int &parent,array<float,2>&rel_pos)
    : m_position(pos),m_size(size),m_depth(depth),m_id(id),m_parent(parent),m_relative_position(rel_pos){
    }
  };
  explicit PositionGroup()
    : m_position("a_position")
    , m_size    ("a_size")
    , m_depth   ("a_depth")
    , m_id      ("a_id"){
  }
  virtual_entry at(int item_id ){
    int index;
    if(!m_item_id.count(item_id)){
      index = m_position.size();
      array<float,2> tmp;;
      m_position.emplace_back(tmp);
      m_size.emplace_back(tmp);
      m_depth.resize(index+1);
      m_id.resize(index+1);
      m_parent.resize(index+1);
      m_relative_position.push_back(tmp);
      m_item_id.insert(make_pair(item_id,index));
    }
    index = m_item_id[item_id];
    return virtual_entry(m_position[index],m_size[index],m_depth[index],m_id[index],m_parent[index],m_relative_position[index]);
  }
};
extern PositionGroup position_group;
