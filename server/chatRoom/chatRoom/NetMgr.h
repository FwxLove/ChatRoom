#pragma once
class NetMgr
{
public:
	NetMgr();
	~NetMgr();

	//��ʼ������
	void init_net_work() const;

	//����������Ϣ
	void net_work_update() const;

	//�������ݸ�client
	void send_data_to_client() const;

	//����client������
	void receive_data_from_client(const unsigned char* msg_data) const;

private:
	//����server���͹���������
	void _process_client_message() const;

public:
	unsigned int server_port;
	int max_client;

};

