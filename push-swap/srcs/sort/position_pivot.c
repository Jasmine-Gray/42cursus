/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   position_pivot.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 14:54:28 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/25 21:07:53 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	position_pivot(t_node **right, t_node **pivot)
{
	if ((*pivot)->index > (*pivot)->prev->index)
	{
		while ((*pivot)->index > (*pivot)->prev->index)
		{
			(*pivot) = (*pivot)->prev;
			*right = (*pivot)->prev;
		}
	}
}
