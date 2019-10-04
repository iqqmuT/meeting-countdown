#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class FontTexture {
	public:
		FontTexture();
		~FontTexture();
		void set_renderer(SDL_Renderer *renderer);
		bool LoadFont(const char *font_path, int size);
		bool LoadFromRenderedText(const char *text, SDL_Color color);
		void Free();
		void CloseFont();
		void Render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0,
		            SDL_Point* center = NULL,
								SDL_RendererFlip flip = SDL_FLIP_NONE);
		int width() { return width_; }
		int height() { return height_; }

	private:
		SDL_Renderer* renderer_;
		SDL_Texture* texture_;
		TTF_Font* font_;
		int width_;
		int height_;
};
