/* 
 * File:   SimpleConnectionManager.cpp
 * Author: Mauricio Matamoros
 *
 * Created on Aug 12, 2013 de 2013, 10:53 AM
 */

#include "uRobotics/api/SimpleConnectionManager.h"

SimpleConnectionManager::~SimpleConnectionManager(){ this->stop();}

SimpleConnectionManager::SimpleConnectionManager() 
	: nccSemaphore(0), port(2000)
{
	this->running = false;
	this->acceptor = NULL;
	this->mainThread = NULL;
	this->service = NULL;
}

SimpleConnectionManager::SimpleConnectionManager(unsigned short port)
    : nccSemaphore(0), port(port)
{
	this->running = false;
	this->acceptor = NULL;
	this->mainThread = NULL;
	this->service = NULL;
}

unsigned short SimpleConnectionManager::getPort()
{
	return this->port;
}

void SimpleConnectionManager::setPort(unsigned short port)
{
	if(port != 0)
		this->port = port;
}

bool SimpleConnectionManager::isRunning()
{
	return this->running;
}

void SimpleConnectionManager::start(){
	if(this->running)
		return;

	running = true;
	emptyPacketsReceivedQ();
	this->mainThread = new boost::thread(boost::bind(&SimpleConnectionManager::mainThreadTask, this));
	
}

void SimpleConnectionManager::stop()
{
	if(!this->running)
		return;
	
	this->running = false;
	//this->service->stop();
	delete this->acceptor;
	delete this->service;
	delete this->mainThread;
	
}

void SimpleConnectionManager::mainThreadTask()
{
	this->service = new boost::asio::io_service();
	while(this->running){
		this->acceptor = NULL;
		try{
			this->acceptor = new tcp::acceptor(*(this->service), tcp::endpoint(tcp::v4(), port));
			beginAcceptConnection();
			break;
		}
		catch( ... ){
			if(this->acceptor)
				delete acceptor;
			continue;
		}
	}
	
	//this->service->run();
	while(this->running)
	{
		this->service->poll_one();
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
}

void SimpleConnectionManager::beginAcceptConnection()
{
	Session* new_session = new Session(*service, packetsReceived);
	this->acceptor->async_accept(
		new_session->socket(),
		boost::bind(
			&SimpleConnectionManager::handleAccept,
			this,
			new_session,
			boost::asio::placeholders::error
		)
	);
}

void SimpleConnectionManager::emptyPacketsReceivedQ(){
	this->packetsReceived.clear();
}

void SimpleConnectionManager::handleAccept(Session *new_session, const boost::system::error_code& error){
    if (!error)
    {
		new_session -> start();
		tcp::socket& sock = new_session->socket();
		tcp::socket* sockPtr = &sock;
		{
			scoped_lock(socketsMutex);
			sockets.push_back(sockPtr);
		}
		nccSemaphore.post();
		std::cout << "Client connected" << std::endl;
		beginAcceptConnection();
    }
    else
    {
      delete new_session;
    }
}

bool SimpleConnectionManager::sendTo(tcp::socket& socket, const std::string& s) const
{
	if(!socket.is_open())
		return false;
	try{
		boost::asio::write(socket,
			boost::asio::buffer(s),
			boost::asio::transfer_all()
		);
	}
	// catch (std::exception& e)
	// {
	//	std::cerr << "Exception: " << e.what() << std::endl;
	// }
	catch(...){ return false; }
	return true;
}

size_t SimpleConnectionManager::sendToAll(const std::string& s)
{
	size_t count = 0;
	tcp::socket *socket;
	scoped_lock(socketsMutex);

	for(size_t i = 0; i < sockets.size(); ++i)
	{
		socket = sockets[i];
		if(socket == NULL)
			continue;
		
		if(sendTo(*socket, s))
			++count;
	}
	return count;
}

ProducerConsumer<TcpPacket*>& SimpleConnectionManager::getPacketsReceived()
{
	return this->packetsReceived;
}


void SimpleConnectionManager::waitClientConnected(){
	if(sockets.size() > 0)
		return;
	nccSemaphore.wait();
}

bool SimpleConnectionManager::waitClientConnected(const int& timeout){
	if(sockets.size() > 0)
		return true;
	if(timeout < 0){
		waitClientConnected();
		return true;
	}
	boost::system_time deadline = boost::get_system_time() + boost::posix_time::milliseconds(timeout);
	return nccSemaphore.timed_wait(deadline);
}