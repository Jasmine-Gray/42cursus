/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:37:45 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:37:47 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*tmp;
	t_list	*tmp_next;

	if (!lst || !del)
		return ;
	tmp = *lst;
	while (tmp)
	{
		tmp_next = tmp->next;
		del(tmp->content);
		free(tmp);
		tmp = tmp_next;
	}
	*lst = NULL;
}
