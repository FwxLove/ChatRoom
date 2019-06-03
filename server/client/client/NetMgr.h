#pragma once

#include <string>

class NetMgr
{
public:
	NetMgr();
	~NetMgr();

	//��ʼ������
	void init_net_work() const;
	
	//����������Ϣ
	void net_work_update();

	//�������ݸ�server
	void send_data_to_server() const;

	//����server����
	void receive_data_from_server(const unsigned char* msg_data) const;

private:
	//����server���͹�������Ϣ
	void _process_server_message() const;

public:
	std::string server_ip;
	unsigned short server_port;
};

