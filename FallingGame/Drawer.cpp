#include "Drawer.h"

#include "Layer.h"
#include "Game.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Drawer::Drawer(Layer& layer)
{
	{
		rectangle_program = layer.compile_shader_program("f/shaders/rectangle.vert", "f/shaders/rectangle.frag", "rectangle shader");

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

	{
		image_program = layer.compile_shader_program("f/shaders/image.vert", "f/shaders/image.frag", "image shader");
		float vertices[] = {
			// position   texture coordinates
			-0.5f, -0.5f, 0.f, 0.f,
			-0.5f, 0.5f,  0.f, 1.f,
			0.5f, 0.5f,   1.f, 1.f,
			0.5f, -0.5f,  1.f, 0.f
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		glUseProgram(image_program);
		glGenVertexArrays(1, &image_VAO);
		glBindVertexArray(image_VAO);
		glGenBuffers(1, &image_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, image_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &image_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, image_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// 2 positions and 2 tex coords
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		m_image_u_offset = glGetUniformLocation(image_program, "u_offset");
		m_image_u_rotation = glGetUniformLocation(image_program, "u_rotation");
	}

	// bird texture
	{
		glGenTextures(1, &bird_texture);
		glBindTexture(GL_TEXTURE_2D, bird_texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
		unsigned char* data = stbi_load("f/images/bird.png", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
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
	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glUniform4f(m_rectangle_u_color, color.r, color.g, color.b, color.a);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Drawer::draw_image(float x, float y, float w, float h, float rotation)
{
	glUseProgram(image_program);
	glBindVertexArray(image_VAO);
	glBindTexture(GL_TEXTURE_2D, bird_texture);

	float x_l = -w / 2.f;
	float y_l = -h / 2.f;

	float vertices[] = {
		x_l, y_l,     0.f, 1.f,
		x_l, -y_l,     0.f, 0.f,
		-x_l, -y_l,  1.f, 0.f,
		-x_l, y_l,     1.f, 1.f
	};
	glBindBuffer(GL_ARRAY_BUFFER, image_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

	glUseProgram(image_program);
	glUniform2f(m_image_u_offset, global_offset.x + x, global_offset.y + y);

	glUniform1f(m_image_u_rotation, rotation);
	//glUniform1f(m_image_u_rotation, SDL_GetTicks()/100.f);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Drawer::before_draw(Game& g)
{
	Pos off = g.c.offset();
	glUseProgram(rectangle_program);
	glUniform2f(m_rectangle_u_offset, off.x, off.y);
	global_offset = off;
	
}
