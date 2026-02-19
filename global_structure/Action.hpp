#ifndef ACTION_HPP
#define ACTION_HPP

class Action
{
	public:
		Action();
		Action(const Action& other);
		Action& operator=(const Action& other);
		~Action();

	    enum Type {
	        STATIC_FILE,
	        CGI,
	        ERROR
	    };

	    Type type;
	    std::string targetPath;
	    int statusCode;
};

#endif