#pragma once

#include<vector>
#include<string>
#include<iostream>

#include<solutionInterface/modelFwd.hpp>

class ModelDataImpl;
class ModelData{
  public:
    ModelData();
    void load(std::string const&fileName);
    ~ModelData();
    void createModelView(Model&model);
  private:
    friend class ModelDataImpl;
    ModelDataImpl*impl = nullptr;
};
