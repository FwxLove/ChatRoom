#include "NetMgr.h"
#include <iostream>

int main()
{
	NetMgr* p = new NetMgr;

	p->init_net_work();

	while (true)
	{
		p->net_work_update();
	}

	delete p;
	p = NULL;

	return 0;
}