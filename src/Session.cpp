/* 
 * File:   Session.cpp
 * Author: Mauricio Matamoros
 * 
 * Created on Aug 8, 2013, 10:55 AM
 */

#include <uRobotics/sockets/Session.h>
using namespace Robotics::Sockets;
using namespace Robotics::Utilities;

Session::~Session() {

}

Session::Session(boost::asio::io_service& io_service, ProducerConsumer<TcpPacket*>& q)
: sckt(io_service), packetsReceived(q) { }

 tcp::socket& Session::socket()
  {
    return this->sckt;
  }

 void Session::start() {
	this->sckt.async_read_some(
		boost::asio::buffer(this->data, MAX_LENGTH),
		boost::bind(
			&Session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
		);
  }

void Session::handle_read(const boost::system::error_code& error, size_t bytes_transferred){
	if (!error) {
		/*
		boost::asio::async_write(socket_,
		boost::asio::buffer(data_, bytes_transferred),
		boost::bind(&session::handle_write, this,
		boost::asio::placeholders::error));
		*/
		//this->print(data_, bytes_transferred);
		TcpPacket *packet = new TcpPacket(this->data, bytes_transferred);
		packetsReceived.produce(packet);

		//this->sckt.
			
		this->sckt.async_read_some(boost::asio::buffer(this->data, MAX_LENGTH),
			boost::bind(&Session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	else {
		//std::cout << "error reading from socket" << std::endl;
		delete this;
	}
}

 void Session::handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
		this->sckt.async_read_some(boost::asio::buffer(this->data, MAX_LENGTH),
				boost::bind(&Session::print, this, this->data, boost::asio::placeholders::bytes_transferred));
	/*	
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
	  */
    }
    else
    {
		//std::cout << "error writing to socket" << std::endl;
		delete this;
    }
  }

 void Session::print(char *data, int length){
	 int isASCII = 1;
	 int i = 0;
	 std::cout << "isASCII: " << isASCII << std::endl;
	 for(i = 0; i < length; ++i)
	 {
		 if(data[i] == 0)
			 break;
		 if((data[i] > 127) || (data[i] < 32))
		 {
			 isASCII = 0;
			 break;
		 }
	 }
 
	 std::cout << "Received " << length << "bytes: ";
	 if(isASCII)
		 std::cout << data;
	 else
	 {
		 std::cout << "0x";
		for(i = 0; i < length; ++i)
			printf("%x2", data[i]);
	 }
	std::cout << std::endl;	 

 }

