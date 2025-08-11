/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:38:20 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:38:24 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*new;

	new = (t_list *)malloc(sizeof(t_list));
	if (!new)
		return (NULL);
	new->content = content;
	new->next = NULL;
	return (new);
}

// #include <stdio.h>
//
// int	main(void)
//{
//	t_list	*head;
//	t_list	*current;
//	t_list	*temp;
//
//	head = ft_lstnew((void *)42);
//	head->next = ft_lstnew((void *)10);
//	head->next->next = ft_lstnew((void *)20);
//	current = head;
//	while (current)
//	{
//		printf("%d -> ", (int)current->content);
//		current = current->next;
//	}
//	printf("NULL\n");
//	current = head;
//	while (current)
//	{
//		temp = current->next;
//		free(current);
//		current = temp;
//	}
//	return (0);
// }
