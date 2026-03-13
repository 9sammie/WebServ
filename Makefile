# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/13 17:16:36 by vakozhev          #+#    #+#              #
#    Updated: 2026/03/13 17:58:15 by vakozhev         ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
DEPFLAGS = -MMD -MP #creer un fichier de dep

SRC_DIR = srcs
BUILD_DIR = build

SRC = \
	srcs/main.cpp \
	srcs/Lexer.cpp \
	srcs/Parser.cpp \
	srcs/Token.cpp \
	srcs/Config.cpp

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR) #-p si le dossier existe
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug: CXXFLAGS += -g3 #quand tu fais make debug, ajoute -g3 aux options de compilation
debug: re #quand tu fais make debug ->fais d abord re

-include $(DEP)

.PHONY: all clean fclean re debug
