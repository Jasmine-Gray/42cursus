/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calc_colour.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 15:39:01 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/10 16:16:00 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

static int	interpolate_color(int start, int end, double factor);

int	calc_color_gradient(int iterations)
{
	double	t;

	if (iterations == 0)
		return (BLACK);
	t = (double)iterations / (double)ITER_MAX;
	if (t < 0.2)
		return (interpolate_color(BLACK, BLUE, t / 0.2));
	if (t < 0.4)
		return (interpolate_color(BLUE, GREEN, (t - 0.2) / 0.2));
	if (t < 0.6)
		return (interpolate_color(GREEN, YELLOW, (t - 0.4) / 0.2));
	if (t < 0.8)
		return (interpolate_color(YELLOW, WHITE, (t - 0.6) / 0.2));
	return (WHITE);
}

static int	interpolate_color(int start, int end, double factor)
{
	int	r;
	int	g;
	int	b;

	r = ((start >> 16) & 0xFF) + factor * (((end >> 16) & 0xFF)
			- ((start >> 16) & 0xFF));
	g = ((start >> 8) & 0xFF) + factor * (((end >> 8) & 0xFF)
			- ((start >> 8) & 0xFF));
	b = (start & 0xFF) + factor * ((end & 0xFF) - (start & 0xFF));
	return (r << 16 | g << 8 | b);
}
