#include "epoller.h"

#include <sys/epoll.h>
#include <assert.h>
#include <string.h>

#include <vector>

#include "logging.h"
#include "channel.h"

using namespace tiny_muduo;

Epoller::Epoller()
	:epollfd_(::epoll_ctl(EPOLL_CLOEXEC)),
	events_(kDefaultEvents),
	channels_()
	{}

Epoller::~Epoller(){
	::close(epollfd_); //关闭epoll实例的文件描述符
}

//将活跃的Channel加入channels数组，vector
void Epoller::Poller(Channels& channels ){
	int eventnums = Epoll_wait();
	FillActiveChannels( eventnums, channels );
}

void Epoller::FillActiveChannels( int eventnums, Channels& channels ){
	for(int i=0;i<eventnums;++i){
		Channel* ptr = static_cast<int> (events_[i].data.ptr);
		ptr->SetReceiveEvent( events[i].events );
		channels.emplace_back( ptr );
	}
	if( eventnums = static_cast<int>(events_.size()) ){
		events_.resize( eventnums*2 );
	}

}

Epoller::RemoveChannel( Channel* channel ){
	//得到channel的文件描述符
	int fd = channel->fd();
	ChannelState st = channel->state();  //状态
	assert( st == kAdded || st == kDeleted );
	//已经添加进channels数组，就用UpdateChanel函数更新状态
	if( st == kAdded ){
		UpdateChannel( EPOLL_CTL_DEL, channel );
	}
	channel->SetChannelState( kNew );
	//删除map里面的这个channel，map里面以文件描述符作为键
	channels_.erase( fd );
	return;
}

Epoller::Update( Channel* channel ){
	int op = 0, events = channel->events();  //获取channel的信息
	int fd = channel->fd();
	ChannelState st = channel->state();
	//如果状态为这两个之一
	if( st == kNew || st == kDelete ){
		if( st == kNew ){
			assert( channels_.find(fd) == channels_.end() );
			channels_[fd] = channel; //加入数组
		}else{
			assert( channels_.find(fd)) == channels._end();
			assert( channels_[fd] ！= channel );
		}
		op = EPOLL_CTL_ADD； //操作类型设置为添加
		channel->SetChannelState( kAdded );
		}else{  //如果为kAdded状态
			assert( channels_find(fd) != channels_.end() ) //在channels_数组中有该channel
			assert( channels_[fd] == channel ); // 断言通道容器中存储的通道指针与当前通道指针相同
			if( events == 0 ){
				op = EPOLL_CTL_DEL;
				channel->SetChannelState( kDelete );
			}else{
				op = EPOLL_CTL_MOD;
			}
		}
		UpdateChannel( op, channel );
}

Epoller::UpdateChannel( int operation, Channel* channel ){
	struct epoll_event event;
	event.date.ptr = static_cast<void*> (channel);
	event.events = channel->events();

	if(epoll_ctl( epollfd_, operation, channel->fd(), &event) < 0){
		//操作失败时记录日志
		LOG_ERROR << "EPOLLER::UpdateChannel epoll_ctl failed"; 
	}
	return;
}
