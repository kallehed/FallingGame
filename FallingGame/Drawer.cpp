#include "Drawer.h"

#include "Game.h"

#include <iostream>
#include <tuple>
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
		
		const float y_imgs = 4.f * g.l.HEIGHT;
		const float x_imgs = 4.f * g.l.WIDTH;

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

	// cloud program
	{
		cloud_program = g.l.compile_shader_program("f/shaders/cloud.vert", "f/shaders/cloud.frag", "cloud shader");
		glGenVertexArrays(1, &cloud_VAO);
		glBindVertexArray(cloud_VAO);

		glEnableVertexAttribArray(0);
		glGenBuffers(1, &cloud_VBO_pos);
		glBindBuffer(GL_ARRAY_BUFFER, cloud_VBO_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * FLOATS_PER_CLOUD_POS * Game::NR_CLOUDS, NULL, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glVertexAttribDivisor(0, 1);

		glEnableVertexAttribArray(1);
		glGenBuffers(1, &cloud_VBO_tex_z);
		glBindBuffer(GL_ARRAY_BUFFER, cloud_VBO_tex_z);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* TEX_AND_Z_FLOATS* Game::NR_CLOUDS, NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glVertexAttribDivisor(1, 1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, standard_rect_EBO);
	}

	// coin particle program
	{
		coin_particle_program = g.l.compile_shader_program("f/shaders/coin_particle.vert", "f/shaders/coin_particle.frag", "coin particle shader");
		glGenVertexArrays(1, &coin_particle_VAO);
		glBindVertexArray(coin_particle_VAO);

		float vertices[] = // rectangle for screen
		{
			-1.f, -1.f,
			-1, 1.f,
			1.f, 1.f,
			1.f, -1.f
		};

		glGenBuffers(1, &coin_particle_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, coin_particle_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, standard_rect_EBO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	}

	// bird program
	{
		bird_program = g.l.compile_shader_program("f/shaders/bird.vert", "f/shaders/bird.frag", "bird shader");
		glGenVertexArrays(1, &bird_VAO);
		glBindVertexArray(bird_VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, standard_rect_EBO);

		m_bird_u_offset = glGetUniformLocation(bird_program, "u_offset");
		m_bird_u_rotation_width_height = glGetUniformLocation(bird_program, "u_rotation_width_height");
		m_bird_u_time_since_coin = glGetUniformLocation(bird_program, "u_time_since_coin");
	}

	// uniform buffer object: Globals
	{
		glGenBuffers(1, &ubo_globals);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_globals);
		glBufferData(GL_UNIFORM_BUFFER, UBO_GLOBAL_SIZE, NULL, GL_DYNAMIC_DRAW);

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_globals);
	}

	// load textures
	constexpr std::array<std::tuple<const char*, int, int>, TEX::TOTAL> data =
	{ {
		{"f/images/bird.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/bird_closed.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/mushroom_cap.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/mushroom_stem.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/side_background.png", GL_CLAMP_TO_BORDER, GL_REPEAT },
		{"f/images/vines.png", GL_REPEAT, GL_REPEAT },
		{"f/images/sky.png", GL_REPEAT, GL_REPEAT},
		{"f/images/sky_blurred.png", GL_REPEAT, GL_REPEAT},
		{"f/images/sky2.png", GL_REPEAT, GL_REPEAT},
		{"f/images/sky3.png", GL_REPEAT, GL_REPEAT},
		{"f/images/storm.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/cloud_1.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/cloud_2.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/cloud_3.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/cloud_4.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/coin.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
		{"f/images/coin_blurred.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER}
	} };

	for (int i = 0; i < TEX::TOTAL; ++i) {
		auto& d = data[i];
		tex_sizes[i] = load_texture(std::get<0>(d), &texs[i], std::get<1>(d), std::get<2>(d));
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

void Drawer::draw_image(Camera& c, TEX::_ tex, float x, float y, float w, float h, float rotation)
{
	glUseProgram(image_program);
	glBindVertexArray(image_VAO);
	glBindTexture(GL_TEXTURE_2D, texs[tex]);

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

void Drawer::draw_sky(Game& g)
{
	glUseProgram(sky_program);
	glBindVertexArray(sky_VAO);
	glBindTexture(GL_TEXTURE_2D, texs[TEX::sky2]);

	float y = -g.p.r.y/5.f;
	while (y > sky_height_per_sky) { y -= sky_height_per_sky; }
	
	glUniform2f(sky_u_offset, 0.f, y);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Drawer::draw_sides(Player& p)
{
	glUseProgram(sides_program);
	glBindVertexArray(sides_VAO);

	glBindTexture(GL_TEXTURE_2D, texs[TEX::side_background]);
	//glBindTexture(GL_TEXTURE_2D, texs[TEX::vines]);

	float y = -p.r.y;
	while (y > sides_height_per_image) { y -= sides_height_per_image; };
	glUniform2f(sides_u_offset, 0.f, y);
	glDrawElements(GL_TRIANGLES, 120, GL_UNSIGNED_INT, 0);
}

void Drawer::draw_clouds(Game& g)
{
	glUseProgram(cloud_program);
	glBindVertexArray(cloud_VAO);

	// bind cloud textures
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texs[TEX::cloud_1]);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texs[TEX::cloud_2]);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texs[TEX::cloud_3]);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, texs[TEX::cloud_4]);

	glActiveTexture(GL_TEXTURE0); // unbind

	// cloud_VBO_pos and cloud_VBO_tex_z initialization
	{
		float positions[Game::NR_CLOUDS * FLOATS_PER_CLOUD_POS];
		
		float tex_z_values[Game::NR_CLOUDS * TEX_AND_Z_FLOATS];
		int i_t_z = 0, i_pos = 0;
		for (auto& e : g.m_clouds) {
			float w = e.w, h = e.h;
			float x = e.x;
			float y = e.y;
			tex_z_values[i_t_z++] = (float)(e.tex - TEX::cloud_1);
			tex_z_values[i_t_z++] = e.z;

			positions[i_pos++] = x;
			positions[i_pos++] = w;
			positions[i_pos++] = y;
			positions[i_pos++] = h;
		}
		glBindBuffer(GL_ARRAY_BUFFER, cloud_VBO_pos);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), positions);

		glBindBuffer(GL_ARRAY_BUFFER, cloud_VBO_tex_z);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tex_z_values), tex_z_values);
	}
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, Game::NR_CLOUDS);
}

void Drawer::draw_cloud(Game& g, TEX::_ tex, float x, float y, float z, float w, float h)
{
	glUseProgram(cloud_program);
	glBindVertexArray(cloud_VAO);

	GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS; // total textures allowed to use at the same time in shader
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texs[tex]);
	
	glActiveTexture(GL_TEXTURE0);
	{
		float vertices[] =
		{
			x, y, 
			x, y + h,
			x + w, y + h,
			x + w, y,	
		};

		//glBindBuffer(GL_ARRAY_BUFFER, cloud_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	}
	//glUniform1f(m_cloud_u_z, z);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Drawer::draw_coin_particle(CoinParticle& c)
{
	glUseProgram(coin_particle_program);
	glBindVertexArray(coin_particle_VAO);
	
	float u_pos_and_vel_and_time[COIN_PARTICLE_FLOATS_IN_ARRAY] =
	{
		c.start_x, c.start_y, c.x_vel, c.y_vel, c.start_time
	};

	glUniform1fv(glGetUniformLocation(coin_particle_program, "u_pos_and_vel_and_time"), COIN_PARTICLE_FLOATS_IN_ARRAY, u_pos_and_vel_and_time);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Drawer::draw_bird(Camera& c, TEX::_ bird_tex, float x, float y, float rotation, float width, float height, float time_since_coin)
{
	glUseProgram(bird_program);
	glBindVertexArray(bird_VAO);

	glBindTexture(GL_TEXTURE_2D, texs[bird_tex]);

	glUniform2f(m_bird_u_offset, x, y);
	glUniform3f(m_bird_u_rotation_width_height, rotation, width, height);
	glUniform1f(m_bird_u_time_since_coin, time_since_coin);

	{
		Pos global_offset = c.offset();
		glUniform2f(m_image_u_offset, global_offset.x + x, global_offset.y + y);
	}

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Drawer::before_draw(Game& g)
{
	Pos off = g.c.offset();
	glUseProgram(rectangle_program);
	glUniform2f(m_rectangle_u_offset, off.x, off.y);

	// bind unfiform buffer object: Globals
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_globals);
		float data[4] = { g.m_death_y, g.c.y, g.m_timer, g.l.WIDTH };
		glBufferSubData(GL_UNIFORM_BUFFER, 0, UBO_GLOBAL_SIZE, &data);
	}	
}

std::array<int, 2> Drawer::load_texture(const char* path, unsigned int* image, int wrapping_x, int wrapping_y)
{
	glGenTextures(1, image);
	glBindTexture(GL_TEXTURE_2D, *image); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_x);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_y);
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
	return { width, height };
}
