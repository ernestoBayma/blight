#include <glad/glad.h>
#include <string.h>
#include <stdlib.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <Shader.h>

typedef unsigned int u32;
struct ShaderCompiledResult {
	u32 id;
	char *error;
	short err_flag;
};

static std::string openAndReadFile(char *path);
static ShaderCompiledResult createAndCompileShader(const char **code, int shaderType);

Blight::Shader::Shader(char *vPath, char *fPath) 
{
using namespace std;

std::string 		v_code, f_code;
char			info_log[512];
int			success;
ShaderCompiledResult 	vertex, frag;

	this->error = 0;
	if(vPath) v_code = openAndReadFile(vPath);
	if(fPath) f_code = openAndReadFile(fPath);

	const char *real_v_code = !v_code.empty() ? v_code.c_str(): "";	
	const char *real_f_code = !f_code.empty() ? f_code.c_str(): "";	

	vertex 	= createAndCompileShader(&real_v_code, GL_VERTEX_SHADER);
	frag   	= createAndCompileShader(&real_f_code, GL_FRAGMENT_SHADER);

	if(vertex.err_flag) {
		if(vertex.error) cout << vertex.error << endl;	
		free(vertex.error);
	}

	if(frag.err_flag) {
		if(frag.error) cout << frag.error << endl;	
		free(frag.error);
	}

	this->id = glCreateProgram();
	glAttachShader(this->id, vertex.id);
	glAttachShader(this->id, frag.id);
	glLinkProgram(this->id);

	glGetProgramiv(this->id, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(this->id, sizeof(info_log), NULL, info_log);
		cout << info_log << endl;
		this->error = 1;
		return;
	}

	glDeleteShader(vertex.id);
	glDeleteShader(frag.id);
}

void Blight::Shader::use() const
{
	glUseProgram(this->id);
}

void Blight::Shader::setUniformBool(const std::string &name, bool value) const 
{
	glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int)value);
}

void Blight::Shader::setUniformInt(const std::string &name, int value) const 
{
	glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}

void Blight::Shader::setUniformFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

static ShaderCompiledResult createAndCompileShader(const char **code, int shaderType)
{
ShaderCompiledResult res = {};
char	info_log[512];	
int	success;
u32	sh;
	
	sh = glCreateShader(shaderType);
	glShaderSource(sh, 1, code, NULL);
	glCompileShader(sh);

	glGetShaderiv(sh, GL_COMPILE_STATUS, &success);
	if(!success) {
		res.err_flag = 1;
		glGetShaderInfoLog(sh, sizeof(info_log), NULL, info_log);
		res.error = (char*)malloc(sizeof(char) * strlen(info_log) + 1);
		if(res.error) {
			memcpy(res.error, info_log, strlen(info_log) + 1);
		}
		return res;
	}

	res.id = sh;
	return res;
}

static std::string openAndReadFile(char *path)
{ 
using namespace std;
char		*error_msg;
string 		ret;
ifstream	st;
stringstream	s_st;
	
	st.open(path);
	if(st.good()) {
		s_st << st.rdbuf(); // Copy to the stringstream
		st.close();
		ret = s_st.str();
	} else {
		ios_base::iostate st_state = st.rdstate();
		if(st_state & ifstream::eofbit) {
			error_msg = "End of file reached";
		} else if(st_state & ifstream::failbit) {
			error_msg = "Logical error on i/o";
		} else if(st_state & ifstream::badbit) {
			error_msg = "Read/Writing error";
		}
		cout << "ERROR: " << error_msg << endl;
	}

	return ret;
}
