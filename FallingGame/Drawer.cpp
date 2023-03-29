#include "Drawer.h"

#include "Game.h"

#include <iostream>
#include <tuple>
#include <cmath>

#include <ft2build.h>
#include FT_FREETYPE_H  

void Drawer::init(Game& g)
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

	// load all programs
	this->load_all_programs();

	// font stuff with freetype
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft)) {
			SDL_Log("ERROR::FREETYPE: Failed to load FreeType");
			std::cin.get();
		}

		FT_Face face;
		// can ONLY be freed after FT_Done_Face
		unsigned char* font_buffer = NULL;

		{
		// Load font by first loading it into memory

			SDL_RWops* io = SDL_RWFromFile("f/fonts/arial.ttf", "rb");
			if (io == NULL) SDL_Log("ERROR CANT LOAD arial.ttf file");
			auto file_length = io->size(io);
			font_buffer = new unsigned char[file_length];

			io->read(io, font_buffer, file_length, 1); // read to buffer from file
			SDL_RWclose(io);

			if (FT_New_Memory_Face(ft, font_buffer, (long)file_length, 0, &face)) {
				SDL_Log("ERROR::FREETYPE: Font failed to load arial");
			}
		}
		
		// set width to 0 to dynamically calculate it
		FT_Set_Pixel_Sizes(face, 0, 48);

		/*
		* How to render text efficiently
		* The easy method is having a seperate texture for each character, 
		* and then issuing a draw call for each one of them,
		* where you specify the texture in question and it's rectangular information
		* precisely. Which is very costly.
		* Couldn't you just store all the characters in one big image?
		* Then in an array indexed by the character, you should store where all the
		* characters are exactly in 2D space, and how big they are.
		* Let's say you want to render the character A. You pass the shader a uniform
		* containing the starting position. In the vertex parameters you give it the character.
		* It indexes into the array buffer stored into the GPU containing the correct offsets
		* for characters. There it gets the x, y, w and h of the image, which it uses to
		* successfully query the texture in question and render a part of it. Let's now say we
		* want to do this for a larger amount of characters at a time; We shall then use instanced
		* rendering and give each instance it's own letter, but we also have to give it it's
		* offset compared to the previous characters. They can't all be drawn onto the same
		* place. So the CPU should first examine the text in question, query all the advancements
		* of each character and compute where they all should be offset to. Load all that into
		* an array containing all offsets. Then instancing will take care of moving through that
		* array one time per instance. Thus, all the vertices in a single instance can use it's
		* GL_VertexID to compute it's offset as well from the starting position of the character
		* because they all share the offset, width and height. The instanced array should also
		* contain the character in question, because there will be static memory storing the 
		* texture offsets for all the characters in an SSBO, which the fragment shader will index 
		* into, recieving the correct 4 coordintes in some way. 
		*/

		unsigned int width = 0, height = 0;

		// the offset between characters in the texture
		static constexpr int PIX_OFFSET = 1;

		for (unsigned char c = CHARACTERS_START_AT; c < CHARACTERS; c++)
		{
			// load character glyph   
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph:" << c << std::endl;
				continue;
			}
			width += face->glyph->bitmap.width + PIX_OFFSET;
			height = std::max(height, face->glyph->bitmap.rows);
			// now store character for later use
			Character character = {
				face->glyph->bitmap.width, face->glyph->bitmap.rows,
				face->glyph->bitmap_left, face->glyph->bitmap_top,
				face->glyph->advance.x
			};
			m_characters[c] = character;
		}

		// generate all standard ascii characters
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

		// create texture for use
		glGenTextures(1, &m_main_font_tex);
		glBindTexture(GL_TEXTURE_2D, m_main_font_tex); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		int x = PIX_OFFSET;

		// exclude loading space by starting at 33.
		for (unsigned char c = CHARACTERS_START_AT + 1; c < CHARACTERS; c++)
		{
			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph:" << c << std::endl;
				continue;
			}

			// generate texture
		    glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0,
				face->glyph->bitmap.width, face->glyph->bitmap.rows,
				GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			x += face->glyph->bitmap.width + PIX_OFFSET;
			
		}

		FT_Done_Face(face);
		delete[] font_buffer;

		FT_Done_FreeType(ft);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // turn back on?

		/////////////////  OPENGL buffer stuff

		m_text_program = g.l.compile_shader_program("f/shaders/text.vert", "f/shaders/text.frag", "text shader");
		glUseProgram(m_text_program);

		glGenVertexArrays(1, &m_text_VAO);
		glBindVertexArray(m_text_VAO);

		glGenBuffers(1, &m_text_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_text_VBO);
		glBufferData(GL_ARRAY_BUFFER, TEXT_ATTRIBUTE_BYTES, NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, standard_rect_EBO);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, TEXT_ATTRIBUTES, GL_FLOAT, GL_FALSE, TEXT_BYTES_PER, (void*)0);

		glVertexAttribDivisor(0, 1); // one per instance
		//glVertexBindingDivisor(0,1);


		m_text_u_offset_scale = glGetUniformLocation(m_text_program, "u_offset_scale");

		{
			// make it the correct amount of data
			float data[CHARACTERS][U_TEXT_ATTRIBUTES_PER]; // store the width,height,bearingx,bearingy
			for (int i = CHARACTERS_START_AT; i < CHARACTERS; ++i) {
				data[i][0] = (float)m_characters[i].width; // width
				data[i][1] = (float)m_characters[i].height; // height
				data[i][2] = (float)m_characters[i].bearing_x; // bearing X
				data[i][3] = (float)m_characters[i].bearing_y; // bearing Y
			}

			glUniform4fv(glGetUniformLocation(m_text_program, "u_text"), CHARACTERS, (float*)data);
		}

		{
			float char_x_offset_data[CHARACTERS];
			float x = 0.f;
			for (int i = CHARACTERS_START_AT; i < CHARACTERS; ++i) {
				char_x_offset_data[i] = x;
				x += m_characters[i].width + PIX_OFFSET;
			}

			glUniform1fv(glGetUniformLocation(m_text_program, "u_char_x_offsets"), CHARACTERS, (float*)char_x_offset_data);
		}

		{
			m_text_u_color = glGetUniformLocation(m_text_program, "u_color");
		}
	}

	// rectangle shader
	{
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
	}


	// image shader
	{
		
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
		const float y_imgs = 4.f * g.l.HEIGHT;
		const float x_imgs = 4.f * g.l.WIDTH;

		sky_height_per_sky = (2.f * g.G_HEIGHT) / y_imgs;

		const float left = -Game::G_WIDTH * 2.f;
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
		glGenVertexArrays(1, &sides_VAO);
		glBindVertexArray(sides_VAO);

		constexpr float y_imgs = 2.5f, x_imgs = 1.f;

		constexpr float img_height = (Layer::HEIGHT * 2.f) / y_imgs;
		sides_height_per_image = img_height;

		constexpr float top = Layer::HEIGHT;
		constexpr float bottom = -Layer::HEIGHT - img_height;
		
		constexpr float left = -Layer::WIDTH;
		constexpr float right = -Game::G_WIDTH;

		const float vertices[] = {
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

		glBindBufferBase(GL_UNIFORM_BUFFER, UBO_GLOBAL_BIND, ubo_globals);
	}

	// load textures
	{
		
		static constexpr std::array<std::tuple<const char*, int, int>, TEX::TOTAL> data =
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
			tex_sizes[i] = Layer::load_texture(std::get<0>(d), &texs[i], std::get<1>(d), std::get<2>(d));
		}
	}
	
	// mess with a texture2D
	/* {
		unsigned char data[100][100][4];
		for (int i = 0; i < 100; ++i) {
			for (int j = 0; j < 100; ++j) {
				data[i][j][0] = 255*sin(i/20.0);
				data[i][j][1] = 0;
				data[i][j][2] = 0;
				data[i][j][3] = 255;
			}
		}

		for (int i = 0; i < 3; ++i) {
			glBindTexture(GL_TEXTURE_2D, texs[TEX::side_background]);
			glTexSubImage2D(GL_TEXTURE_2D, i, 100, 100, 100, 100, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
	}*/
}

void Drawer::draw_text(const char* text, Color color, float x, float y, float scale)
{
	glUseProgram(m_text_program);
	glBindVertexArray(m_text_VAO);

	glBindTexture(GL_TEXTURE_2D, m_main_font_tex);

	float data[TEXT_MAX_CHARS_RENDER][TEXT_ATTRIBUTES];
	float char_x_offset = 0;
	// i will be the total amount of elements later
	int i = 0;
	for (; text[i] != '\0'; ++i) {
		// char
		SDL_assert(i < TEXT_MAX_CHARS_RENDER);
		char c = text[i];
		data[i][0] = (float)c;
		data[i][1] = (float)char_x_offset;

		//advance is number of 1/64 pixels for some reason
		char_x_offset += (m_characters[c].advance >> 6) * scale; //bitshift by 6 == * 2^6
	}
	const int nr_of_chars = i;

	glBindBuffer(GL_ARRAY_BUFFER, m_text_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (size_t)nr_of_chars * TEXT_BYTES_PER, data);

	glUniform3f(m_text_u_offset_scale, x, y, scale);
	glUniform4f(m_text_u_color, color.r, color.g, color.b, color.a);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, nr_of_chars);
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
	y -= (sky_height_per_sky) * std::floor(y / sky_height_per_sky);
	
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
	// bind unfiform buffer object: Globals
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_globals);
		float data[UBO_GOBAL_FLOATS] = { g.m_death_y, g.c.y, g.m_timer, g.l.WIDTH };
		glBufferSubData(GL_UNIFORM_BUFFER, 0, UBO_GLOBAL_SIZE, &data);
	}	

	 // hot reloading of shaders
	if constexpr (DEV_TOOLS) { 
		if (g.l.key_just_down(SDL_SCANCODE_R)) {
			load_all_programs();
		}
	}
}

void Drawer::draw_fps(float dt)
{
	static constexpr int SIZE = 10;
	char fps_string[SIZE];
	snprintf(fps_string, SIZE, "%f", 1.f/dt);

	draw_text(fps_string, Color{ 0,0,1,1 }, -Layer::WIDTH + 0.1f, Layer::HEIGHT - 0.1f, 0.001f);
}


void Drawer::load_all_programs()
{
	// the uniforms for the text shader are only set once, and they are removed when reloading shader...
	//glDeleteProgram(m_text_program);
	//m_text_program = l.compile_shader_program("f/shaders/text.vert", "f/shaders/text.frag", "text shader");

	glDeleteProgram(rectangle_program);
	rectangle_program = Layer::compile_shader_program("f/shaders/rectangle.vert", "f/shaders/rectangle.frag", "rectangle shader");

	glDeleteProgram(image_program);
	image_program = Layer::compile_shader_program("f/shaders/image.vert", "f/shaders/image.frag", "image shader");

	glDeleteProgram(sky_program);
	sky_program = Layer::compile_shader_program("f/shaders/sky.vert", "f/shaders/sky.frag", "sky shader");

	glDeleteProgram(sides_program);
	sides_program = Layer::compile_shader_program("f/shaders/sides.vert", "f/shaders/sides.frag", "sides shader");

	glDeleteProgram(cloud_program);
	cloud_program = Layer::compile_shader_program("f/shaders/cloud.vert", "f/shaders/cloud.frag", "cloud shader");

	glDeleteProgram(coin_particle_program);
	coin_particle_program = Layer::compile_shader_program("f/shaders/coin_particle.vert", "f/shaders/coin_particle.frag", "coin particle shader");

	glDeleteProgram(bird_program);
	bird_program = Layer::compile_shader_program("f/shaders/bird.vert", "f/shaders/bird.frag", "bird shader");
}

