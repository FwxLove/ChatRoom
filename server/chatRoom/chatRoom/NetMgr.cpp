#include "NetMgr.h"
#include "RakPeerInterface.h"						//���ӹ���ӿ�
#include "RakNetTypes.h"							//��������RakNet��ʹ�õĽṹ��
#include "MessageIdentifiers.h"						//������һ���޴��ö�����ݣ���ʾ��RakNet���ڷ�����Ϣ�ı�ʶ��������Ͽ�����֪ͨ��
#include "BitStream.h"								//������һ����д�롢��ȡ���������ࡣ

#include <iostream>
#include "ServerClientMessage.h"
#include "ServerClientStruct.h"

RakNet::RakPeerInterface* net_peer;					//������Ϣ
RakNet::Packet* net_packet;							//������Ϣ��

NetMgr::NetMgr():server_port(10001),max_client(1000)
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

	std::cout << "Start Game Server ......." << server_port << std::endl;

	//���������߳���������Ӧ�Ķ˿�
	//startup�����ĵ�һ�������ǽ��յ�������������ͻ���һ����Ϊ1
	//�ڶ�����������Ҫ�����Ķ˿ڣ�SocketDescriptor�����ƶ��Ķ˿ڣ�����һ���׽��֣�������һ������ΪSocketDescriptor��server_prot,0��
	//������������SocketDescriptor����Ĵ�С����1��������ȷ
	int start_up = net_peer->Startup(1, &RakNet::SocketDescriptor(server_port, 0), 1);
	if (start_up > 0)
	{
		std::cout << "Startup failed" << std::endl;
	}

	//SetMaxiumIncomingConnections����������������
	net_peer->SetMaximumIncomingConnections(max_client);
}

//����������Ϣ
void NetMgr::net_work_update() const
{
	//Receive����Ϣ�����л�ȡ��Ϣ
	for (net_packet = net_peer->Receive(); net_packet; net_peer->DeallocatePacket(net_packet), net_packet = net_peer->Receive())
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
			break;
		case ID_NEW_INCOMING_CONNECTION:
			std::cout << "A connection is incoming" << std::endl;
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
			_process_client_message();
			break;
		default:
			std::cout << "Message with identifier %d has arrived" << net_packet->data[0] << std::endl;
			break;
		}
	}
}

//�������ݸ�client
void NetMgr::send_data_to_client() const
{

}

//����client������
void NetMgr::receive_data_from_client(const unsigned char* msg_data) const
{
	connect_server* receive_data = (connect_server*)msg_data;
	std::cout << "receive from client data is:" << receive_data->content << std::endl;

	connect_server_return send_data;
	memset(&send_data, 0, sizeof(connect_server_return));

	std::string content = receive_data->content;
	content += ". server already receive client data, this is server data";

	sprintf_s(send_data.content_return, "%s", content.c_str());

	std::cout << "server data is:" << send_data.content_return << std::endl;

	//ʹ��Bitstream��������װ����
	RakNet::BitStream str;
	//��д��raknet����Ϣ���ͣ�raknet���Զ������ͣ�ID_USER_PACKET_ENUM��)
	str.Write((RakNet::MessageID)ID_USER_PACKET_ENUM);
	//��д�������Զ������Ϣ����
	str.Write(msg_connect_server_return);
	//������д��str�У���һ��������Ҫд��������ֽ����飬�ڶ��������ݵ�λ��
	str.WriteBits((unsigned char*)&send_data, sizeof(connect_server_return) * 8);
	//�����ݷ��͵�ָ���ĵط�
	//��һ��������Ҫ���͵�bitstream����
	//�ڶ��������Ƿ��͵����ȼ�
	//�����������Ƿ��͵Ŀɿ��ԣ�����ʹ��RELIABLE_ORDERED��������PacketPriority.hö�ٵ�PacketReliability���о���˵��
	//�Լ���api���������ó�0
	//���շ���ַ
	//�Ƿ�㲥��ע���£�Ҫ�����Ǹ�true����һ���������ǲ���Ҫ�������ݵĵ�ַ
	net_peer->Send(&str, HIGH_PRIORITY, RELIABLE_ORDERED, 0, net_packet->systemAddress, false);
}

//����server���͹���������
void NetMgr::_process_client_message() const
{
	int message_id = 0;
	//����Ϣ����������ʹ��bitstream
	//IgnoreBytes���Ե�������raknet����Ϣ����
	RakNet::BitStream bs_in(net_packet->data, net_packet->length, false);
	bs_in.IgnoreBytes(sizeof(RakNet::MessageID));
	bs_in.Read(message_id);

	//ƫ�Ƶ��Զ������Ϣ����ȡʵ�����ݣ�raknet����Ϣ���ʹ�С��һ���ֽڣ��Զ���Ľṹ����4���ֽ�
	unsigned char* msg_data = net_packet->data;
	msg_data += sizeof(unsigned long) + sizeof(unsigned char);

	switch (message_id)
	{
	case msg_connect_server:
		this->receive_data_from_client(msg_data);
		break;


	default:
		break;
	}
}