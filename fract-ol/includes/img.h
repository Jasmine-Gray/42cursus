/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   img.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 18:14:07 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/13 18:14:14 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMG_H
# define IMG_H

typedef struct s_img
{
	void	*img_ptr;
	char	*img_addr;
	int		bpp;
	int		size_line;
	int		endian;
}			t_img;
#endif
