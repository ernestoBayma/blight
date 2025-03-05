#pragma once

#include <string>
#include <glm/glm.hpp>
#include <BlightTypes.h>

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
		void setUniformMat(const std::string &name, glm::mat4 mat) const;
		void setUniformVec3(const std::string &name, f32 x, f32 y, f32 z) const;
		void setUniformVec3(const std::string &name, glm::vec3 vec) const;
	};
}; // End namespace
