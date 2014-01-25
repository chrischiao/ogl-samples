//**********************************
// OpenGL Draw Elements
// 10/05/2010 - 12/03/2012
//**********************************
// Christophe Riccio
// ogl-samples@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include "test.hpp"

namespace
{
	char const * VERTEX_SHADER_SOURCE("es-200/flat-color.vert");
	char const * FRAGMENT_SHADER_SOURCE("es-200/flat-color.frag");

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(glm::uint16);
	glm::uint16 const ElementData[ElementCount] =
	{
		0, 1, 2,
		0, 2, 3
	};

	GLsizei const VertexCount(4);
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
	};

	namespace buffer
	{
		enum type
		{
			VERTEX,
			ELEMENT,
			MAX
		};
	}//namespace buffer

	std::vector<GLuint> BufferName(buffer::MAX);
	GLuint ProgramName = 0;
	GLint UniformMVP = 0;
	GLint UniformDiffuse = 0;
}//namespace

class es_200_draw_elements : public test
{
public:
	es_200_draw_elements(int argc, char* argv[]) :
		test(argc, argv, "es-200-draw-elements", test::ES, 2, 0)
	{}

private:
	bool initProgram()
	{
		bool Validated = true;
		
		// Create program
		if(Validated)
		{
			GLuint VertexShaderName = glf::createShader(GL_VERTEX_SHADER, glf::DATA_DIRECTORY + VERTEX_SHADER_SOURCE);
			GLuint FragmentShaderName = glf::createShader(GL_FRAGMENT_SHADER, glf::DATA_DIRECTORY + FRAGMENT_SHADER_SOURCE);

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertexShaderName);
			glAttachShader(ProgramName, FragmentShaderName);
			glDeleteShader(VertexShaderName);
			glDeleteShader(FragmentShaderName);

			glBindAttribLocation(ProgramName, glf::semantic::attr::POSITION, "Position");
			glLinkProgram(ProgramName);
			Validated = glf::checkProgram(ProgramName);
		}

		// Get variables locations
		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
		}

		// Set some variables 
		if(Validated)
		{
			// Bind the program for use
			glUseProgram(ProgramName);

			// Set uniform value
			glUniform4fv(UniformDiffuse, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

			// Unbind the program
			glUseProgram(0);
		}

		return Validated && glf::checkError("initProgram");
	}

	bool initBuffer()
	{
		glGenBuffers(static_cast<GLsizei>(BufferName.size()), &BufferName[0]);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return glf::checkError("initBuffer");
	}

	bool begin()
	{
		bool Validated(true);

		char const * Extensions = (char const *)glGetString(GL_EXTENSIONS);
		printf(Extensions);

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();

		return Validated;
	}

	bool end()
	{
		glDeleteBuffers(static_cast<GLsizei>(BufferName.size()), &BufferName[0]);
		glDeleteProgram(ProgramName);

		return true;
	}

	bool render()
	{
		// Compute the MVP (Model View Projection matrix)
		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -this->TranlationCurrent.y));
		glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, this->RotationCurrent.y, glm::vec3(1.f, 0.f, 0.f));
		glm::mat4 View = glm::rotate(ViewRotateX, this->RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		// Set the display viewport
		glm::ivec2 WindowSize = this->getWindowSize();
		glViewport(0, 0, WindowSize.x, WindowSize.y);

		// Clear color buffer with black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepthf(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind program
		glUseProgram(ProgramName);

		// Set the value of MVP uniform.
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);

		glEnableVertexAttribArray(glf::semantic::attr::POSITION);
			glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0);
		glDisableVertexAttribArray(glf::semantic::attr::POSITION);

		// Unbind program
		glUseProgram(0);

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error(0);

	es_200_draw_elements Test(argc, argv);
	Error += Test();

	return Error;
}

