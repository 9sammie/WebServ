# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/13 17:16:36 by vakozhev          #+#    #+#              #
#    Updated: 2026/03/25 14:22:47 by vakozhev         ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

NAME = webserv
TEST_NAME = testLexer

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror
DEPFLAGS = -MMD -MP #creer un fichier de dep

SRC_DIR = srcs
BUILD_DIR = build
TEST_DIR = tests

SRC = \
	srcs/main.cpp \
	srcs/Lexer.cpp \
	srcs/Parser.cpp \
	srcs/Token.cpp \
	srcs/Config.cpp \
	srcs/CgiHandler.cpp \
	srcs/Client.cpp \
	srcs/HttpException.cpp \
	srcs/HttpRequest.cpp \
	srcs/MethodResolver.cpp \
	srcs/RequestHandler.cpp \
	srcs/RequestParser.cpp \
	srcs/ServerManager.cpp \
	srcs/Signal.cpp \
	srcs/TcpListener.cpp \
	srcs/UriResolver.cpp

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:.o=.d)

TEST_SRC = \
	tests/test_main.cpp \
	tests/testLexer.cpp \
	tests/testsHelper.cpp

TEST_OBJ = $(TEST_SRC:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/tests/%.o)
TEST_DEP = $(TEST_OBJ:.o=.d)

TEST_PROJECT_SRC = \
	srcs/Lexer.cpp \
	srcs/Token.cpp

TEST_PROJECT_OBJ = $(TEST_PROJECT_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/test_src/%.o)
TEST_PROJECT_DEP = $(TEST_PROJECT_OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR) #-p si le dossier existe
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -Iheaders -Itests -c $< -o $@

tests: $(TEST_NAME)

$(TEST_NAME): $(TEST_OBJ) $(TEST_PROJECT_OBJ)
	$(CXX) $(CXXFLAGS) $(TEST_OBJ) $(TEST_PROJECT_OBJ) -o $(TEST_NAME)

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)/tests
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -Iheaders -Itests -c $< -o $@

$(BUILD_DIR)/test_src/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)/test_src
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -Iheaders -Itests -c $< -o $@

run_tests: $(TEST_NAME)
	./$(TEST_NAME)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)

re: fclean all

debug: CXXFLAGS += -g3 #quand tu fais make debug, ajoute -g3 aux options de compilation
debug: re #quand tu fais make debug ->fais d abord re

-include $(DEP)
-include $(TEST_DEP)
-include $(TEST_PROJECT_DEP)

.PHONY: all clean fclean re debug tests run_tests
