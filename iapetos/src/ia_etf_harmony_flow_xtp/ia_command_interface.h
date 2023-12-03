
#ifndef __IA_COMMAND_INTERFACE_H__
#define __IA_COMMAND_INTERFACE_H__

using namespace std;

class IaCommandInterface
{
public:
    virtual void OnCommandRtn(const char* type, const char* command) = 0;
public:
	IaCommandInterface(){};
	virtual ~IaCommandInterface(){};

};

#endif