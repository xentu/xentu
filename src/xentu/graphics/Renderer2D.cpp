#ifndef XEN_SPRITE_BATCH_CPP
#define XEN_SPRITE_BATCH_CPP

#include <GLEW/GL/glew.h>

#include "Renderer2D.h"
#include "../XentuGame.h"

// Specify a macro for storing information about a class and method name,
// this needs to go above any class that will be exposed to lua
#define method(class, name, realname) {#name, &class::realname}

namespace xen
{
	Renderer2D::Renderer2D(lua_State* L) :
		m_rotation(0),
		m_origin_x(0),
		m_origin_y(0),
		m_scale_x(1),
		m_scale_y(1),
		ibo(-1),
		vbo(-1),
		m_sprite(),
		m_initialized(false),
		m_clear_color(LuaColor::White())
	{ }


	Renderer2D::~Renderer2D()
	{
		std::cout << "Deleted instance of Renderer2D." << std::endl;
	}


	// macro for calculating the byte offset of vertex information.
	#define BUFFER_OFFSET(i) ((char*)NULL + (i))


	void Renderer2D::initialize()
	{
		if (!this->m_initialized)
		{
			// prepare vertex buffer.
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			// prepare index buffer.
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

			/* Specify the vertex layout (pos + uv). */
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(8));
			this->m_initialized = true;
		}
	}


	int Renderer2D::lua_begin(lua_State* L)
	{
		if (!this->m_initialized) {
			this->initialize();
		}

		m_origin_x = 0;
		m_origin_y = 0;
		m_scale_x = 1;
		m_scale_y = 1;
		m_rotation = 0;
		this->clear();
		return 1;
	}


	int Renderer2D::lua_present(lua_State* L)
	{
		const int vertex_size = 4 * sizeof(Vertex);
		const int element_size = 6 * sizeof(unsigned int);

		//glClearColor(0, 0, 0, 0);
		float r = this->m_clear_color.red;
		float g = this->m_clear_color.green;
		float b = this->m_clear_color.blue;
		glClearColor(r, g, b, 1);

		glDisable(GL_CULL_FACE);
		//glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (const Batch* batch : m_batches)
		{
			const Texture* texture = batch->m_texture;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture->gl_texture_id);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, batch->m_count * vertex_size, batch->m_vertices.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch->m_count * element_size, batch->m_indices.data(), GL_DYNAMIC_DRAW);

			// draw stuff.
			glDrawElements(GL_TRIANGLES, batch->m_count * 6, GL_UNSIGNED_INT, nullptr);
			//glDrawArrays(GL_TRIANGLES, 0, batch->m_count * 6);
		}

		return 1;
	}


	int Renderer2D::lua_draw_sprite(lua_State* L)
	{
		// make sure the table is on the stack.
		lua_pushnil(L);
		// iterate the values to populate an LuaSprite struct.
		LuaSprite s = parse_lua_sprite(L);
		XentuGame* game = XentuGame::get_instance(L);

		m_sprite.ResetTransform();
		m_sprite.set_position(s.x, s.y);
		m_sprite.set_origin(m_origin_x, m_origin_y);
		m_sprite.set_rotation(m_rotation);
		m_sprite.set_scale(m_scale_x, m_scale_y);

		m_sprite.m_width = s.width;
		m_sprite.m_height = s.height;
		m_sprite.m_texture = game->assets->get_texture(s.texture);

		SpriteMap* sprite_map = game->assets->get_spritemap(s.spritemap);
		if (sprite_map != NULL) {
			Rect* r = sprite_map->get_region(s.region);
			m_sprite.m_rect = *r;
		}

		if (m_sprite.m_texture == NULL)
			return 0;

		find_batch(m_sprite)->draw(m_sprite);

		return 1;
	}


	int Renderer2D::lua_draw_text(lua_State* L)
	{
		// read the other variables first.
		std::string text = lua_tostring(L, -4);
		float left = lua_tonumber(L, -3);
		float top = lua_tonumber(L, -2);
		float max_width = lua_tonumber(L, -1);

		XentuGame* game = XentuGame::get_instance(L);

		// remove the 4 arguments (text, left, top, max_w) from the stack so we can read the font object.
		lua_pop(L, 1);
		lua_pop(L, 1);
		lua_pop(L, 1);
		lua_pop(L, 1);

		// push nil to get the table data to parse.
		lua_pushnil(L);
		LuaFont f = parse_lua_font(L);

		SpriteMap* sprite_map = game->assets->get_spritemap(f.spritemap_id);

		m_sprite.ResetTransform();
		m_sprite.set_position(left, top);
		m_sprite.set_origin(m_origin_x, m_origin_y);
		m_sprite.set_rotation(m_rotation);
		m_sprite.set_scale(m_scale_x, m_scale_y);

		m_sprite.m_width = 10;
		m_sprite.m_height = 22;
		m_sprite.m_texture = game->assets->get_texture(f.texture_id);

		int x_offset = 0, y_offset = 0;
		if (sprite_map != NULL) {
			for (std::string::size_type i = 0; i < text.size(); ++i) {
				std::string cs = std::to_string(text[i]);
				Rect* rect = sprite_map->get_region(cs);

				if (rect)
				{
					m_sprite.m_width = m_sprite.m_texture->width * rect->width;
					m_sprite.m_height = m_sprite.m_texture->height * rect->height;
					m_sprite.m_rect = *rect;
					find_batch(m_sprite)->draw(m_sprite);
				}

				x_offset += m_sprite.m_width + f.letter_spacing; // 2px is a placeholder

				if (x_offset > max_width && text[i] == 32) {
					x_offset = 0;
					y_offset += f.line_height;
				}

				m_sprite.set_position(left + x_offset, top + y_offset);
			}
		}

		return 0;
	}


	int Renderer2D::lua_set_origin(lua_State* L)
	{
		m_origin_x = lua_tonumber(L, -2);
		m_origin_y = lua_tonumber(L, -1);
		return 1;
	}


	int Renderer2D::lua_set_rotation(lua_State* L)
	{
		m_rotation = lua_tonumber(L, -1);
		return 1;
	}


	int Renderer2D::lua_set_scale(lua_State* L)
	{
		m_scale_x = lua_tonumber(L, -2);
		m_scale_y = lua_tonumber(L, -1);
		return 1;
	}


	void Renderer2D::clear()
	{
		for (unsigned int i = 0; i < m_batches.size(); i++)
		{
			m_batches[i]->clear();

			if (m_batches[i]->m_inactive > 256)
			{
				std::swap(m_batches[i], m_batches.back());
				delete m_batches.back();
				m_batches.pop_back();
				i--;
			}
		}
	}


	Batch* Renderer2D::find_batch(const Sprite& sprite)
	{
		for (Batch* batch : m_batches)
		{
			if (batch->m_texture == sprite.m_texture && batch->m_layer == sprite.m_layer)
				return batch;
		}

		Batch* batch = new Batch();
		batch->m_texture = sprite.m_texture;
		batch->m_layer = sprite.m_layer;
		batch->m_inactive = 0;
		batch->m_count = 0;

		m_batches.push_back(batch);
		return m_batches.back();
	}


	void Renderer2D::draw(const Sprite& sprite)
	{
		if (sprite.m_texture == NULL)
			return;

		find_batch(sprite)->draw(sprite);
	}


	void Renderer2D::order()
	{
		// Sort the batches
		std::sort(m_batches.begin(), m_batches.end(), [&](const Batch* a_batch, const Batch* b_batch)
			{
				if (a_batch->m_layer == b_batch->m_layer)
					return a_batch->m_texture < b_batch->m_texture;

				return false; //a_batch->m_layer < a_batch->m_layer;
			});
	}


	int Renderer2D::lua_debug_sprite(lua_State* L)
	{
		printf("--- Sprite Debug ---\n");
		int count = lua_gettop(L);
		if (count == 1) {
			int type = lua_type(L, 1);
			if (type == LUA_TTABLE) {
				lua_pushnil(L);
				LuaSprite s = parse_lua_sprite(L);
				std::cout << "sprite: (" << s.x << "," << s.y << "," << s.width << "," << s.height << ") [" << s.region << "]" << std::endl;
				printf("success: everything looks normal!\n");
			}
			else {
				printf("error: first item on stack is not a table.\n");
			}
		}
		else {
			printf("error: no sprite detected.\n");
		}
		printf("--- Sprite Debug ---\n");
		return 0;
	}


	int Renderer2D::lua_enbable_blend(lua_State* L)
	{
		bool enable = lua_toboolean(L, -1);

		if (enable) {
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}

		return 0;
	}


	int Renderer2D::lua_set_clear_color(lua_State* L)
	{
		// push nil to get the table data to parse.
		lua_pushnil(L);
		LuaColor f = parse_lua_color(L);
		this->m_clear_color = f;
		return 0;
	}


	int Renderer2D::lua_set_blend_func(lua_State* L)
	{
		int sfactor = lua_tonumber(L, -2);
		int dfactor = lua_tonumber(L, -1);
		glBlendFunc(sfactor, dfactor);
		return 0;
	}


	LuaSprite Renderer2D::parse_lua_sprite(lua_State* L)
	{
		LuaSprite s;
		while (lua_next(L, -2)) {
			std::string key = lua_tostring(L, -2);
			if (key == "x")         s.x = lua_tointeger(L, -1);
			if (key == "y")         s.y = lua_tointeger(L, -1);
			if (key == "width")     s.width = lua_tointeger(L, -1);
			if (key == "height")    s.height = lua_tointeger(L, -1);
			if (key == "region")    s.region = lua_tostring(L, -1);
			if (key == "texture")   s.texture = lua_tointeger(L, -1);
			if (key == "spritemap") s.spritemap = lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
		return s;
	}


	LuaFont Renderer2D::parse_lua_font(lua_State* L)
	{
		LuaFont f;
		while (lua_next(L, -2)) {
			std::string key = lua_tostring(L, -2);
			if (key == "texture") 	f.texture_id = lua_tointeger(L, -1);
			if (key == "spritemap") f.spritemap_id = lua_tointeger(L, -1);
			if (key == "line_height") f.line_height = lua_tointeger(L, -1);
			if (key == "letter_spacing") f.letter_spacing = lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		return f;
	}


	LuaColor Renderer2D::parse_lua_color(lua_State* L)
	{
		LuaColor c;
		while (lua_next(L, -2)) {
			std::string key = lua_tostring(L, -2);
			if (key == "red") c.red = lua_tonumber(L, -1);
			if (key == "green") c.green = lua_tonumber(L, -1);
			if (key == "blue")  c.blue = lua_tonumber(L, -1);
			if (key == "alpha") c.alpha = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		return c;
	}


	const char Renderer2D::className[] = "Renderer2D";


	const Luna<Renderer2D>::PropertyType Renderer2D::properties[] = {
		{0,0}
	};


	const Luna<Renderer2D>::FunctionType Renderer2D::methods[] = {
		method(Renderer2D, begin, lua_begin),
		method(Renderer2D, draw_sprite, lua_draw_sprite),
		method(Renderer2D, draw_text, lua_draw_text),
		method(Renderer2D, present, lua_present),
		method(Renderer2D, set_rotation, lua_set_rotation),
		method(Renderer2D, set_origin, lua_set_origin),
		method(Renderer2D, set_scale, lua_set_scale),
		method(Renderer2D, debug_sprite, lua_debug_sprite),
		method(Renderer2D, enbable_blend, lua_enbable_blend),
		method(Renderer2D, set_clear_color, lua_set_clear_color),
		method(Renderer2D, set_blend_func, lua_set_blend_func),
		{0,0}
	};
}

#endif