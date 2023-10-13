#pragma once

#include <string>

#define SHADER_CODE(code)  "#version 330 core\n"#code

namespace Blight {
class Shader {
public:
	unsigned int id;
	short error;

	Shader(char *vPath, char *fPath);	
	void use() const;
	void setUniformBool(const std::string &name, bool value) const;
	void setUniformInt(const std::string &name, int value) const;
	void setUniformFloat(const std::string &name, float value) const;
};
// End namespace
};
