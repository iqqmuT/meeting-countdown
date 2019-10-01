#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class FontTexture {
	public:
		FontTexture();
		~FontTexture();
		void setRenderer(SDL_Renderer *renderer);
		bool loadFont(const char *fontPath, int size);
		bool loadFromRenderedText(const char *text, SDL_Color color);
		void free();
		void closeFont();
		void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
		int getWidth();
		int getHeight();

	private:
		SDL_Renderer* mRenderer;
		SDL_Texture* mTexture;
		TTF_Font* mFont;
		int mWidth;
		int mHeight;
};
