#include "Drawer.h"

#include "Layer.h"
#include "Game.h"

Drawer::Drawer(Layer& layer)
{
	rectangle_program = layer.compile_shader_program("rectangle.vert", "rectangle.frag", "rectangle shader");

	float vertices[] = {
		-0.5f, -0.5f,
		-0.5f, 0.5f,
		0.5f, 0.5f,
		0.5f, -0.5f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glUseProgram(rectangle_program);
	glGenVertexArrays(1, &rectangle_VAO);
	glBindVertexArray(rectangle_VAO);
	glGenBuffers(1, &rectangle_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &rectangle_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangle_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 3. then set our vertex attributes pointers
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	m_rectangle_u_color = glGetUniformLocation(rectangle_program, "u_color");
	m_rectangle_u_offset = glGetUniformLocation(rectangle_program, "u_offset");
}

void Drawer::draw_rectangle(float x, float y, float w, float h, const Color& color)
{
	glUseProgram(rectangle_program);
	glBindVertexArray(rectangle_VAO);
	float vertices[] = {
		x, y,
		x, y + h,
		x + w, y + h,
		x + w, y
	};
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glUniform4f(m_rectangle_u_color, color.r, color.g, color.b, color.a);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Drawer::before_draw(Game& g)
{
	Pos off = g.c.offset();
	glUniform2f(m_rectangle_u_offset, off.x, off.y);
}