#include <Share/EntityEventHandler.h>

class GateEntity;

class LoginMgr : public EntityEventHandler
{
public:
	LoginMgr(GateEntity* entity);
    virtual void onNotify(EntityEventType event) override;
protected:

private:
	GateEntity* entity_;
};
