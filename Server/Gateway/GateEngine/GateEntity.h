#pragma once
#include <Share/MgrController.h>
#include <memory>

class GateEntity : public MgrController
{
public:
	void load();

public:
	class LoginMgr* getLoginMgr() {return loginMgr_.get();}
private:
	std::unique_ptr<class LoginMgr> loginMgr_;
};
