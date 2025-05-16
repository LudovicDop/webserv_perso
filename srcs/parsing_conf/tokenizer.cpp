/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 00:45:22 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/02 16:48:25 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::vector<Token>	assign_token(std::vector<Token> words)
{
	for (size_t i = 0; i < words.size(); i++)
	{
		if (words[i].value == ":")
			words[i].type = COLON;
		else if (words[i].value == "{")
			words[i].type = BRACE_OPEN;
		else if (words[i].value == "}")
			words[i].type = BRACE_CLOSE;
		else if (words[i].value == "[")
			words[i].type = BRACKET_OPEN;
		else if (words[i].value == "]")
			words[i].type = BRACKET_CLOSE;
		else if (words[i].value == ",")
			words[i].type = COMMA;
		else
			words[i].type = UNKNOWN;
	}
	for (size_t i = 0; i < words.size(); i++)
	{
		if (words[i].type == UNKNOWN && words[i + 1].type == COLON)
			words[i].type = KEY;
		else if (words[i].type == UNKNOWN)
			words[i].type = VALUE;
	}
	return (words);
}

std::vector<Token>	tokenize(std::string& line)
{
	std::vector<Token>			words;
	Token						word;
	size_t						i;

	i = 0;
	while (line[i])
	{
		if (line[i] == '"')
		{
			word.value += '"';
			i++;
			while (line[i] && line[i] != '"')
				word.value += line[i++];
			word.value += line[i++];
			words.push_back(word);
			word.value.clear();
		}
		else if ((line[i] >= 'a' && line[i] <= 'z') || (line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= '0' && line[i] <= '9'))
		{
			while (line[i] && (line[i] != ':' && line[i] != ',' && line[i] != ' ' && line[i] != '{' && line[i] != '}'\
				&& line[i] != '[' && line[i] != ']'))
			{
				word.value += line[i++];
			}
			words.push_back(word);
			word.value.clear();
		}
		else if (line[i] == ':' || line[i] == ',' || line[i] == '{' || line[i] == '}'\
			|| line[i] == '[' || line[i] == ']')
		{
			word.value += line[i++];
			words.push_back(word);
			word.value.clear();
		}
		if (line[i] == ' ')
			i++;
	}
	return (assign_token(words));
}
