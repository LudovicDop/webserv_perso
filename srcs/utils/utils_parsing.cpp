/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parsing.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 14:11:45 by ldoppler          #+#    #+#             */
/*   Updated: 2025/04/28 13:20:18 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

const std::string  getExtensionFile(const std::string& file)
{
	size_t pos = file.find_last_of(".");
	if (pos == std::string::npos)
		return ("");
	return (file.substr(pos));
}
