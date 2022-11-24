#include "Drawer.h"

#include "Game.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Drawer::Drawer(Game& g)
{
	// standard rect EBO
	{
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,

			4, 5, 6,
			6, 7, 4
		};

		glGenBuffers(1, &standard_rect_EBO);
		glBindBuffer(GL_ARRAY_BUFFER, standard_rect_EBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	// rectangle shader
	{
		rectangle_program = g.l.compile_shader_program("f/shaders/rectangle.vert", "f/shaders/rectangle.frag", "rectangle shader");

		float vertices[] = {
			-0.5f, -0.5f,
			-0.5f, 0.5f,
			0.5f, 0.5f,
			0.5f, -0.5f
		};

		glUseProgram(rectangle_program);
		glGenVertexArrays(1, &rectangle_VAO);
		glBindVertexArray(rectangle_VAO);

		glGenBuffers(1, &rectangle_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, standard_rect_EBO);
		// 3. then set our vertex attributes pointers
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		m_rectangle_u_color = glGetUniformLocation(rectangle_program, "u_color");
		m_rectangle_u_offset = glGetUniformLocation(rectangle_program, "u_offset");
	}

	// image shader
	{
		image_program = g.l.compile_shader_program("f/shaders/image.vert", "f/shaders/image.frag", "image shader");
		float vertices[] = { // changed later
			// position   texture coordinates
			-0.5f, -0.5f, 0.f, 0.f,
			-0.5f, 0.5f,  0.f, 1.f,
			0.5f, 0.5f,   1.f, 1.f,
			0.5f, -0.5f,  1.f, 0.f
		}; 

		glUseProgram(image_program);
		glGenVertexArrays(1, &image_VAO);
		glBindVertexArray(image_VAO);

		glGenBuffers(1, &image_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, image_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, standard_rect_EBO);

		// 2 positions and 2 tex coords
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		m_image_u_offset = glGetUniformLocation(image_program, "u_offset");
		m_image_u_rotation = glGetUniformLocation(image_program, "u_rotation");
	}

	// sky shader
	{
		sky_program = g.l.compile_shader_program("f/shaders/sky.vert", "f/shaders/sky.frag", "sky shader");
		
		const float y_imgs = 5.f * g.l.HEIGHT;
		const float x_imgs = 5.f * g.l.WIDTH;

		sky_height_per_sky = (2.f * g.G_HEIGHT) / y_imgs;

		const float left = -g.G_WIDTH * 2.f;
		const float bottom = -g.G_HEIGHT - sky_height_per_sky;
		const float y_top = g.G_HEIGHT;
		
		const float vertices[] = {
			// position   texture coordinates
			left, bottom, 0.f, 0.f,
			left, y_top,  0.f, y_imgs + 1,
			-left, y_top,   x_imgs, y_imgs + 1,
			-left, bottom,  x_imgs, 0.f
		};

		glUseProgram(sky_program);
		glGenVertexArrays(1, &sky_VAO);
		glBindVertexArray(sky_VAO);
		glGenBuffers(1, &sky_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, sky_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, standard_rect_EBO);

		// 2 positions and 2 tex coords
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		sky_u_offset = glGetUniformLocation(sky_program, "u_offset");
	}

	// sides shader
	{
		sides_program = g.l.compile_shader_program("f/shaders/sides.vert", "f/shaders/sides.frag", "sides shader");
		glGenVertexArrays(1, &sides_VAO);
		glBindVertexArray(sides_VAO);

		constexpr float y_imgs = 2.5f, x_imgs = 1.f;

		constexpr float img_height = (g.l.HEIGHT * 2.f) / y_imgs;
		sides_height_per_image = img_height;

		constexpr float top = g.l.HEIGHT;
		constexpr float bottom = -g.l.HEIGHT - img_height;
		
		constexpr float left = -g.l.WIDTH;
		constexpr float right = -g.G_WIDTH;

		constexpr float vertices[] = {
			// position    texture coordinates

			// left side of screen
			left, bottom,   0.f, 0.f,
			left, top,      0.f, y_imgs + 1.f,
			right, top,     x_imgs, y_imgs + 1.f,
			right, bottom,  x_imgs, 0.f,
			
			// right side of screen
			-right, bottom, x_imgs, 0.f,
			-right, top,    x_imgs, y_imgs + 1.f,
			-left, top,     0.f, y_imgs + 1.f,
			-left, bottom,  0.f, 0.f
		};

		glGenBuffers(1, &sides_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, sides_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, standard_rect_EBO);

		// 2 positions and 2 tex coords
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		sides_u_offset = glGetUniformLocation(sides_program, "u_offset");
	}

	// load textures
	load_texture("f/images/bird.png", &bird_texture, GL_CLAMP_TO_BORDER);
	load_texture("f/images/bird_closed.png", &bird_closed_texture, GL_CLAMP_TO_BORDER);
	load_texture("f/images/mushroom_cap.png", &mushroom_cap_texture, GL_CLAMP_TO_BORDER);
	load_texture("f/images/mushroom_stem.png", &mushroom_stem_texture, GL_CLAMP_TO_BORDER);
	load_texture("f/images/side_background.png", &side_background_texture, GL_REPEAT);
	load_texture("f/images/sky.png", &sky_texture, GL_REPEAT);
	load_texture("f/images/sky_blurred.png", &sky_blurred_texture, GL_REPEAT);
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

void Drawer::draw_image(Camera& c, unsigned int texture, float x, float y, float w, float h, float rotation)
{
	glUseProgram(image_program);
	glBindVertexArray(image_VAO);
	glBindTexture(GL_TEXTURE_2D, texture);

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

	{
		Pos global_offset = c.offset();
		glUniform2f(m_image_u_offset, global_offset.x + x, global_offset.y + y);
	}
	glUniform1f(m_image_u_rotation, rotation);
	//glUniform1f(m_image_u_rotation, SDL_GetTicks()/100.f);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Drawer::draw_sky(Player& p)
{
	glUseProgram(sky_program);
	glBindVertexArray(sky_VAO);
	glBindTexture(GL_TEXTURE_2D, sky_texture);

	float y = -p.h.y/5.f;
	while (y > sky_height_per_sky) { y -= sky_height_per_sky; }
	
	glUniform2f(sky_u_offset, 0.f, y);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Drawer::draw_sides(Player& p) {
	glUseProgram(sides_program);
	glBindVertexArray(sides_VAO);

	glBindTexture(GL_TEXTURE_2D, side_background_texture);

	float y = -p.h.y;
	while (y > sides_height_per_image) { y -= sides_height_per_image; };
	glUniform2f(sides_u_offset, 0.f, y);
	glDrawElements(GL_TRIANGLES, 120, GL_UNSIGNED_INT, 0);
}

void Drawer::before_draw(Game& g)
{
	Pos off = g.c.offset();
	glUseProgram(rectangle_program);
	glUniform2f(m_rectangle_u_offset, off.x, off.y);
}

void Drawer::load_texture(const char* path, unsigned int* image, int wrapping)
{
	glGenTextures(1, image);
	glBindTexture(GL_TEXTURE_2D, *image); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
	{
		float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: \'" << path << '\'' << std::endl;
	}
	stbi_image_free(data);
}
