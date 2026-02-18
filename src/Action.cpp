#include "Action.hpp"

Action::Action() {}

Action::Action(const Action& other)
{
	type = other.type;
	targetPath = other.targetPath;
	statusCode = other.statusCode;
}

Action& Action::operator=(const Action& other)
{
	if (this != &other)
	{
		type = other.type;
		targetPath = other.targetPath;
		statusCode = other.statusCode;
	}
	return *this;
}

Action::~Action() {}