#include "RequestHandler.hpp"

void RequestHandler::applyHtmlTemplates(std::string& body, const HttpRequest& request)
{
	std::string cookieVal = request.getCookie("mouse_type");
	if (cookieVal.empty() || cookieVal == "default")
		return;

	size_t headEnd = body.find("</head>");
	size_t pos = body.find("<body", (headEnd != std::string::npos ? headEnd : 0));

	if (pos != std::string::npos) {
		std::string newTag = "<body class=\"cursor-" + cookieVal + "\"";
		body.replace(pos, 5, newTag);
	}
}
