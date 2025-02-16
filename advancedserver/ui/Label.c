#include <ui/Components.h>
#include <UTF8.h>

bool label_update(SDL_Renderer* renderer, struct _Component* component)
{
	//text rendering
	Label* label = (Label*)component;
	int x = label->x;
	int y = label->y;
	//texture indexing and font properties
	int font_gap = 1;
	int width = 6;
	int height = 6;
	int offset = (width + font_gap);
	int pos_x = 0;
	int pos_y = 0;

	SDL_Color clr = COLOR_WHITE;

	for (int i = 0; i < utf8_strlen(label->text); i++)
	{
		utf8_char c = utf8_tolower(utf8_get(label->text, i));

		int ind;

		if (c >= 'a' && c <= 'z') {
			pos_x = (c - 97) * offset;
			pos_y = 0;
			/*if (c >= 'j' && c <= 'm') {
				pos_x--;
			}*/
			switch (c)
			{
			case 'j':
			case 'k':
			case 'l':
			case 'm':
				pos_x--;
				break;
			}
		}
		else if (c >= 0x03B1 && c <= 0x03C9) {
			height = 9;
			pos_x = (c - 0x03B1) * offset;
			pos_y = 9;
		}
		else if (c >= 0x0430 && c <= 0x044F) {
			height = 9;
			pos_x = (c - 0x0430) * offset;
			pos_y = 18;
			/*if (c >= 0x0434) {
				pos_x += offset;
			}*/
		}
		else if (c >= '0' && c <= '9') {
			pos_x = (c - '0') * (width + font_gap);
			pos_y = 27;
		} else {
			pos_x = 92;
			pos_y = 36;
		}

		switch (c)
		{
		case '\n':
			x = label->x;
			y += 8;
			continue;

		case ' ':
			x += 5;
			continue;

		case '	':
			x += 10;
			continue;

		case 'm':
			pos_x--;
			width++;
			break;

		case '-':
		case '_':
			ind = 26;
			break;

		case ',':
			ind = 27;
			break;

		case '1':
			ind = 28;
			break;

		case '2':
			ind = 29;
			break;

		case '3':
			ind = 30;
			break;

		case '4':
			ind = 31;
			break;

		case '5':
			ind = 32;
			break;

		case '6':
			ind = 33;
			break;

		case '7':
			ind = 34;
			break;

		case '8':
			ind = 35;
			break;

		case '9':
			ind = 36;
			break;

		case '0':
			ind = 37;
			break;

		case '!':
		case '.':
			ind = 38;
			break;

		case '\'':
			ind = 39;
			break;

		case ':':
			ind = 40;
			break;

		case '(':
		case '[':
		case '<':
			ind = 41;
			break;

		case ')':
		case ']':
		case '>':
			ind = 42;
			break;

		case '%':
			ind = 43;
			break;

		case '+':
			ind = 77;
			break;

		case '\\':
			clr = COLOR_RED;
			continue;

		case '@':
			clr = COLOR_GRN;
			continue;

		case '&':
			clr = COLOR_PUR;
			continue;

		case '/':
			clr = COLOR_BLU;
			continue;

		case '|':
			clr = COLOR_GRA;
			continue;

		case '`':
			clr = COLOR_YLW;
			continue;

		case '~':
			clr = COLOR_WHITE;
			continue;

		case 0x2116:
			clr = COLOR_ORG;
			continue;
		}

		SDL_SetTextureColorMod(g_letter0Sheet, clr.r, clr.g, clr.b);
		SDL_Rect src = (SDL_Rect){ pos_x, pos_y, width, height };
		SDL_Rect dst = (SDL_Rect){ x * label->scale, y * label->scale, width * label->scale, height * label->scale };
		SDL_RenderCopy(renderer, g_letter0Sheet, &src, &dst);
		x += 8;

	}

	SDL_SetTextureColorMod(g_letter0Sheet, 255, 255, 255);
	return true;
}