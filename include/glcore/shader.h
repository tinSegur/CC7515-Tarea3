#ifndef _SHADER_H_
#define _SHADER_H_

#include "glm/fwd.hpp"
#include <string>
class Shader {
public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  Shader(const std::string &vertexPath, const std::string &geometryPath,
         const std::string &fragmentPath);
  Shader(const std::string &vertexPath, const std::string &tessControl, const std::string &tessEval,
               const std::string &fragmentPath);

  Shader(const std::string &vertexPath,
  const std::string &tessControl, const std::string &tessEval,
  const std::string &geometryPath,
  const std::string &fragmentPath);

  Shader(Shader &&);
  Shader &operator=(Shader &&);
  ~Shader();

  void use();
  void set(const std::string &name, float value);
  void set(const std::string &name, int value);
  void set(const std::string &name, bool value);
  void set(const std::string &name, const glm::mat4 &value);
  void set(const std::string &name, const glm::vec2 &value);
  void set(const std::string &name, const glm::vec3 &value);

  unsigned int getSubroutine(int type, const std::string &name);
  void setSubroutine(int type, const std::vector<unsigned int> &value);

private:
  unsigned int id_;
  int get(const std::string &name);
};

#endif // _SHADER_H_
