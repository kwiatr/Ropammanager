#include "tcp.h"
#include "logger.h"

TcpClient::TcpClient():sock(-1),port(0)
{
}


bool TcpClient::disconnect()
{
	close(sock);
	return true;
}

bool TcpClient::ropamConnect(std::string address , int port)
{
	if(sock == -1)
	{
		sock = socket(AF_INET , SOCK_STREAM , 0);
		if (sock == -1)
		{
			LOG(ERROR) << "Error creating socket";
		}

		LOG(INFO) << "Socket created\n";
	}

	if(inet_addr(address.c_str()) == -1)
	{
		struct hostent *host;
		struct in_addr **addr_list;

		if ( (host = gethostbyname( address.c_str() ) ) == NULL)
		{
			LOG(ERROR) <<"UNABLE to resolve hostname";
			return false;
		}

		addr_list = (struct in_addr **) host->h_addr_list;
		for(int i = 0; addr_list[i] != NULL; i++)
		{
			server.sin_addr = *addr_list[i];
			break;
		}
	}
	else
		server.sin_addr.s_addr = inet_addr(address.c_str());

	server.sin_family = AF_INET;
	server.sin_port = htons( port );

	if( connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		LOG(ERROR) << "Connect failed";
		return false;
	}

	LOG(WARNING) << "Connected\n";
	return true;
}



bool TcpClient::send_binary(std::vector<unsigned char> data)
{
	size_t len = data.size();
	unsigned char const *buffer = &data[0];

	while( len > 0 )
	{
		int bytesSent = ::send( sock, buffer, len, 0);
		if( bytesSent < 0 )
		{
			LOG(ERROR) << "send failed";
			return false;
		}
		len -= bytesSent;
		buffer += bytesSent;
	}
	LOG(DEBUG) << "data sent correctly";	
	return true;
}

std::vector<unsigned char> TcpClient::receive_binary(int size)
{
	char buffer[size];
	std::vector<unsigned char> reply;
	if( recv(sock , buffer , size > sizeof(buffer) ? sizeof(buffer) : size , 0) < 0)
	{
		LOG(ERROR) << "recv failed";
		return reply;
	}

	std::copy(&buffer[0], &buffer[size], std::back_inserter(reply));
	return reply;
}
