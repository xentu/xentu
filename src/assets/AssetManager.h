#pragma once

#define AUTO_FREE 1

#include <string>
#include <map>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "../graphics/Texture.h"
#include "../graphics/TextBox.h"
#include "../graphics/SpriteMap.h"

namespace xen
{
	class AssetManager
	{
		public:
			AssetManager();
			~AssetManager();
			static AssetManager* GetInstance();

			/**
			 * Load a texture into memory, and return it's asset id.
			 */
			int LoadTexture(uint8_t* buffer, uint64_t length);

			/**
			 * Load a ttf font into memory, and return it's asset id.
			 */
			int LoadFont(uint8_t* buffer, uint64_t length, int font_size);

			/**
			 * Load an audio sample into memory, and return it's asset id (can be used in audio manager).
			 */
			int LoadAudio(uint8_t* buffer, uint64_t length);

			/**
			 * Load a music file (ogg/flac) into memory, and return it's asset id (can be used in audio manager).
			 */
			int LoadMusic(uint8_t* buffer, uint64_t length);

			/**
			 * Load a sprite map into memory, and return it's asset id.
			 */
			int LoadSpriteMap(std::string const& json);

			/**
			 * Load a shader into memory and return it's asset id.
			 */
			int LoadShader(string vertex_shader, string frag_shader);

			/**
			 * Create a textbox with specific dimensions, and return it's asset id.
			 */
			int CreateTextBox(int x, int y, int width, int height);

			/**
			 * Create a sprite map, and return it's asset id.
			 */
			int CreateSpriteMap();


			Texture* GetTexture(int id);

			TTF_Font* GetFont(int id);
			
			TextBox* GetTextBox(int id);

			unsigned int GetShader(int id);

			SpriteMap* GetSpriteMap(int id);


			int UnloadTexture(int id);
			int UnloadFont(int id);
			int UnloadAudio(int id);
			int UnloadMusic(int id);
			int UnloadSpriteMap(int id);
			int UnloadShader(int id);


		private:
			static AssetManager* instance;

			/* textures */
			map<int, Texture*> m_textures;
			int m_textures_iter = 0;

			/* fonts */
			map<int, TTF_Font*> m_fonts;
			int m_fonts_iter = 0;

			/* textboxes */
			map<int, TextBox*> m_textboxes;
			int m_textboxes_iter = 0;

			/* shaders */
			map<int, unsigned int> m_shaders;
			int m_shaders_iter = 0;

			/* sprite maps */
			map<int, SpriteMap*> m_sprite_maps;
			int m_sprite_map_iter = 0;
	};
}