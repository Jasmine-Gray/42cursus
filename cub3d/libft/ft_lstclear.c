/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 05:31:02 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/11 05:49:42 by tkusama          ###   ########.fr       */
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
