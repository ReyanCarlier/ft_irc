/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 17:12:48 by frrusso           #+#    #+#             */
/*   Updated: 2023/03/22 17:12:49 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <header.hpp>

class Message
{
	private:
		std::string	_message;
	public:
		Message(void);
		~Message();
};

#endif
