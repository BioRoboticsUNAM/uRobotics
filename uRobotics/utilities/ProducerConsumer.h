#ifndef __PRODUCER_CONSUMER_H__
#define __PRODUCER_CONSUMER_H__

#include <queue>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/thread/thread_time.hpp>
#include <uRobotics/utilities/Stopwatch.h>
#include <uRobotics/utilities/Utils.h>

namespace Robotics{ namespace Utilities{

template<typename T>
class ProducerConsumer
{
private:
	std::queue<T> queue;
	boost::interprocess::interprocess_mutex mutex;
	boost::interprocess::interprocess_condition cond_empty;
	boost::interprocess::interprocess_condition cond_full;
	size_t _count;
	size_t _capacity;
	bool _fixedSize;
	bool _discardExcedent;

private:
	ProducerConsumer(const ProducerConsumer<T>& other);
public:
	ProducerConsumer(void);
	ProducerConsumer(const int& capacity);
	ProducerConsumer(const int& capacity, const bool& fixedSize, const bool& discardExcedent);
	~ProducerConsumer(void);
	bool fixedSize() const;
	bool discardExcedent() const;
	size_t count() const;
	size_t capacity() const;
	void clear();
	bool empty() const;
	void produce(const T& item);
	const T& consume();
	const bool timedConsume(const int& timeOut, T& item);
};

template<typename T>
ProducerConsumer<T>::ProducerConsumer(const ProducerConsumer<T>& other){}

template<typename T>
ProducerConsumer<T>::ProducerConsumer(void) : _count(0), _capacity(10), _fixedSize(true), _discardExcedent(true)
{}

template<typename T>
ProducerConsumer<T>::ProducerConsumer(const int& capacity):
	_count(0),
	_capacity(capacity < 1 ? 10 : capacity),
	_fixedSize(capacity >= 1),
	_discardExcedent(true){}

template<typename T>
ProducerConsumer<T>::ProducerConsumer(const int& capacity, const bool& fixedSize, const bool& discardExcedent):
_count(0), _capacity(capacity), _fixedSize(fixedSize), _discardExcedent(discardExcedent)
{}

template<typename T>
ProducerConsumer<T>::~ProducerConsumer(void)
{
}

template<typename T>
size_t ProducerConsumer<T>::count() const
{
	return this->_count;
}

template<typename T>
size_t ProducerConsumer<T>::capacity() const
{
	return this->capacity;
}

template<typename T>
bool ProducerConsumer<T>::discardExcedent() const
{
	return this->_discardExcedent;
}

template<typename T>
bool ProducerConsumer<T>::fixedSize() const
{
	return this->_fixedSize;
}

template<typename T>
void ProducerConsumer<T>::clear()
{
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
	while(!queue.empty()){
		if(isPointer<T>::value) delete queue.front();
		queue.pop();
	}
	_count = 0;
}

template<typename T>
bool ProducerConsumer<T>::empty() const{
	return this->_count == 0;
}

template<typename T>
void ProducerConsumer<T>::produce(const T& item)
{
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
	if (_fixedSize && (queue.size() >= _capacity))
	{
		// If the queue cannot be resized and excedent
		// will be discarded, then dequeue and discard
		if (_discardExcedent){
			while(queue.size() >= _capacity){
				if(isPointer<T>::value) delete queue.front();
				queue.pop();
			}
		}
		// If elements will not be discarded the producer must wait until
		// there is space available in the queue
		else
		{
			while (queue.size() >= _capacity)
			{
				// This releases the queue, only reacquiring it
				// after being woken up by a call to Pulse
				cond_full.wait(lock);
			}
		}
	}
	else
		++_count;

	queue.push(item);
	if (queue.size() > _capacity)
		_capacity = queue.size();
	cond_empty.notify_all();
}

template<typename T>
const T& ProducerConsumer<T>::consume()
{
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
	// If the queue is empty, wait for an item to be added
	// Note that this is a while loop, as we may be pulsed
	// but not wake up before another thread has come in and
	// consumed the newly added object. In that case, we'll
	// have to wait for another pulse.
	while (queue.size() < 1)
	{
		// This releases queue, only reacquiring it
		// after being woken up by a call to Pulse
		cond_empty.wait(lock);
	}
	--_count;
	T& item = queue.front();
	queue.pop();
	// If there is a producer waiting, notify the count has changed
	if (_fixedSize && !_discardExcedent)
		cond_full.notify_all();
	return item;
}

template<typename T>
const bool ProducerConsumer<T>::timedConsume(const int& timeOut, T& item){
	Stopwatch stopwatch;
	long remaining = timeOut;
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
	stopwatch.start();
	// If the queue is empty, wait for an item to be added
	// Note that this is a while loop, as we may be pulsed
	// but not wake up before another thread has come in and
	// consumed the newly added object. In that case, we'll
	// have to wait for another notify().
	while (queue.size() < 1){
		// This releases queue, only reacquiring it
		// after being woken up by a call to notify()
		boost::posix_time::ptime deadline = boost::get_system_time() + boost::posix_time::milliseconds(remaining);
		if(!cond_empty.timed_wait(lock, deadline)){
			stopwatch.stop();
			return false;
		}
		remaining-= stopwatch.elapsedMilliseconds();
		if(remaining <= 0)
		{
			stopwatch.stop();
			return false;
		}
	}

	stopwatch.stop();
	--_count;
	item = queue.front();
	queue.pop();
	// If there is a producer waiting, notify the count has changed
	if (_fixedSize && !_discardExcedent)
		cond_full.notify_all();
	return true;
}

} /* Utilities */ } /* Robotics */

#endif
