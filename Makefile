# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ctheveno <ctheveno@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/13 17:16:36 by vakozhev          #+#    #+#              #
#    Updated: 2026/04/02 13:26:29 by vakozhev         ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                                 EXECUTABLES                                  #
# **************************************************************************** #
NAME = webserv
TEST_NAME = test

# **************************************************************************** #
#                              COMPILER AND FLAGS                              #
# **************************************************************************** #

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
DEPFLAGS = -MMD -MP #creer un fichier de dep

# **************************************************************************** #
#                           DIRECTORIES AND PATHS                              #
# **************************************************************************** #
SRC_DIR = srcs
BUILD_DIR = build
TEST_DIR = tests

# **************************************************************************** #
#                              PROJECT SOURCES                                 #
# **************************************************************************** #
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
	srcs/UriResolver.cpp \
	srcs/fillCgiData.cpp

# **************************************************************************** #
#                              PROJECT OBJECTS                                 #
# **************************************************************************** #
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:.o=.d)

# **************************************************************************** #
#                                TEST SOURCES                                  #
# **************************************************************************** #
TEST_SRC = \
	tests/test_main.cpp \
	tests/testLexer.cpp \
	tests/testsHelper.cpp \
	tests/testParser.cpp

# **************************************************************************** #
#                                 TEST OBJECTS                                 #
# **************************************************************************** #
TEST_OBJ = $(TEST_SRC:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/tests/%.o)
TEST_DEP = $(TEST_OBJ:.o=.d)

TEST_PROJECT_SRC = \
	srcs/Lexer.cpp \
	srcs/Token.cpp \
	srcs/Parser.cpp \
	srcs/Config.cpp

TEST_PROJECT_OBJ = $(TEST_PROJECT_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/test_src/%.o)
TEST_PROJECT_DEP = $(TEST_PROJECT_OBJ:.o=.d)

# **************************************************************************** #
#                           	  BUILD RULES                                  #
# **************************************************************************** #
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR) #-p if folder exists
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

# **************************************************************************** #
#                                 UTILITY RULES                                #
# **************************************************************************** #
run_tests: $(TEST_NAME)
	./$(TEST_NAME)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)

re: fclean all

debug: CXXFLAGS += -g3 ##When you run `make debug`, add `-g3` to the compilation options
debug: re ##When you run “make debug” -> first run “re”

-include $(DEP)
-include $(TEST_DEP)
-include $(TEST_PROJECT_DEP)

.PHONY: all clean fclean re debug tests run_tests
