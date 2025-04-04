#include "fractol.h"

static int    interpolation_method(int start_color, int end_color, double factor)
{

}

// iter ... 発散に飛ばずに繰り返せた値 (|z| <= 4)
// return value ... 16進数でRGBを表す値を返す.
int    calc_colour_gradient(int iter)
{
    
}

// start_color ... color

iter == 0; BLACK;
iter == 50(ITER_MAX); WHITE;


int	create_trgb(int t, int r, int g, int b)
{
	return (t << 24 | r << 16 | g << 8 | b);
}

int	get_t(int trgb)
{
	return ((trgb >> 24) & 0xFF);
}

int	get_r(int trgb)
{
	return ((trgb >> 16) & 0xFF);
}

int	get_g(int trgb)
{
	return ((trgb >> 8) & 0xFF);
}

int	get_b(int trgb)
{
	return (trgb & 0xFF);
}





#include <math.h>

// 二つの色の間を補間するヘルパー関数
static int interpolation_method(int start_color, int end_color, double factor)
{
    int start_r = (start_color >> 16) & 0xFF;
    int start_g = (start_color >> 8) & 0xFF;
    int start_b = start_color & 0xFF;

    int end_r = (end_color >> 16) & 0xFF;
    int end_g = (end_color >> 8) & 0xFF;
    int end_b = end_color & 0xFF;

    int r = (int)(start_r + factor * (end_r - start_r));
    int g = (int)(start_g + factor * (end_g - start_g));
    int b = (int)(start_b + factor * (end_b - start_b));

    return (r << 16) | (g << 8) | b;
}

// iter ... 発散に飛ばずに繰り返せた値 (|z| <= 4)
// return value ... 16進数でRGBを表す値を返す.
int calc_colour_gradient(int iter)
{
    int max_iter = 1000; // 最大繰り返し回数
    int start_color = 0x000000; // 黒
    int end_color = 0xFFFFFF; // 白

    double factor = (double)iter / max_iter;
    return interpolation_method(start_color, end_color, factor);
}
