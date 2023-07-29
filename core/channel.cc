#include "channel.h"

#include <sys/epoll.h>
#include <sys/poll.h>

Channel::Channel( Eventloop* loop, const int& fd )
	:fd_(fd),
	loop_(loop),
	state_(knew),
	tied(false),
	events_(0),
	recv_events_(0){}
Channel::~Channel{}

void Channel::HandleEvent(){
	//如果绑定了
	if( tied_ ){
		//调用weak_ptr的lock函数判断对象是否存在，存在返回shared_ptr类型的对象，不存在返回nullptr
		std::shared_ptr< void >  = tie_.lock();
		HandleEventWithGuard();
	}else{
		HandleEventWithGuard();
	}
}

void Channel::HandleEventWithGuard(){
	//文件描述符未打开
	if( events_ & EPOLLNVAL ){
		LOG_ERROR <<"Channel::HandleEventWithGuard EPOLLNAL";
	}
	else if( events_ & (EPOLLERR | EPOLLNVAL) ){
		//如果错误回调函数存在就调用
		if( error_callback_ ){ error_callback_(); }
	}
	else if( events_ & (EPOLLIN | EPOLLPRI) ){
		if( read_callback_ ){ read_callback_(); }
	}
	else if( evnets_ & ( (EPOLLERR | EPOLLNVAL) ){
		if( write_callback_ ){ write_callback_(); }
	}
}
