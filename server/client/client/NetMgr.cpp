#include "NetMgr.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"

#include <iostream>
#include "ServerClientStruct.h"
#include "ServerClientMessage.h"

RakNet::RakPeerInterface* net_peer;					//������Ϣ
RakNet::Packet* net_packet;							//������Ϣ��
RakNet::SystemAddress server_address;				//server��ַ


NetMgr::NetMgr():server_ip("localhost"),server_port(10001)
{

}


NetMgr::~NetMgr()
{

}


//��ʼ������
void NetMgr::init_net_work() const
{
	//��ʼ��������Ϣ
	net_peer = RakNet::RakPeerInterface::GetInstance();
	if (NULL == net_peer)
	{
		std::cout << "GetInstance failed" << std::endl;
		return;
	}

	int start_up = net_peer->Startup(1, &RakNet::SocketDescriptor(), 1);
	if (start_up > 0)
	{
		std::cout << "Startup failed" << std::endl;
		return;
	}

	//connect ����server
	//successful ����CONNECTION_ATTEMPT_STARTED
	bool rs = (net_peer->Connect(server_ip.c_str(), server_port, NULL, 0, 0) == RakNet::CONNECTION_ATTEMPT_STARTED);
	if (!rs)
	{
		std::cout << "connect server failed" << std::endl;
		return;
	}
}

//����������Ϣ
void NetMgr::net_work_update()
{
	for (net_packet=net_peer->Receive();net_packet;net_peer->DeallocatePacket(net_packet),net_packet=net_peer->Receive())
	{
		switch (net_packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			std::cout << "Another client has disconnected" << std::endl;
			break;
		case ID_REMOTE_CONNECTION_LOST:
			std::cout << "Another client has lost the connection" << std::endl;
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			std::cout << "Another client has connected" << std::endl;
			break;
			//client������server��ᴥ��
		case ID_CONNECTION_REQUEST_ACCEPTED:
			std::cout << "Our connection request has been accepted for server" << std::endl;
			server_address = net_packet->systemAddress;
			this->send_data_to_server();
			break;
		case ID_NEW_INCOMING_CONNECTION:
			std::cout << "A connection is incoming " << std::endl;
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			std::cout << "The server is full" << std::endl;
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "A client has disconnected" << std::endl;
			break;
		case ID_CONNECTION_LOST:
			std::cout << "A client lost the connection" << std::endl;
			break;
		case ID_USER_PACKET_ENUM:
			_process_server_message();											//�û��Զ�����Ϣ���
			break;

		default:
			std::cout << "Message with identifier %d has arrived" << net_packet->data[0] << std::endl;
			break;
		}
	}
}

//�������ݸ�server
void NetMgr::send_data_to_server() const
{
	connect_server send_data;
	memset(&send_data, 0, sizeof(connect_server));
	sprintf_s(send_data.content, "%s", "hello,this is client send to server");

	std::cout << "send to server data is: " << send_data.content << std::endl;

	RakNet::BitStream str;
	str.Write((RakNet::MessageID)ID_USER_PACKET_ENUM);
	str.Write(msg_connect_server);
	str.WriteBits((unsigned char*)&send_data, sizeof(connect_server) * 8);
	net_peer->Send(&str, HIGH_PRIORITY, RELIABLE_ORDERED, 0, server_address, false);
}

//����server����  ��Ӧ�����ݲ���ӡ����
void NetMgr::receive_data_from_server(const unsigned char* msg_data) const
{
	connect_server_return* receive_data = (connect_server_return*)msg_data;

	std::cout << "receive from server data is: " << receive_data->content_return << std::endl;
}

//����server���͹�������Ϣ
void NetMgr::_process_server_message() const
{
	int message_id = 0;
	RakNet::BitStream bs_in(net_packet->data, net_packet->length, false);
	bs_in.IgnoreBytes(sizeof(RakNet::MessageID));
	bs_in.Read(message_id);

	unsigned char* msg_data = net_packet->data;
	msg_data += sizeof(unsigned long) + sizeof(unsigned char);
	switch (message_id)
	{
	case msg_connect_server_return:
		this->receive_data_from_server(msg_data);
		break;
	default:
		break;
	}
}