#pragma once

class RAFError : public std::exception
{
private:
	const char* err_msg;
	
public:
	const char* what() const throw() { return err_msg; };
	RAFError(const char* msg) : err_msg(msg) {};
	~RAFError() throw() {};
};

