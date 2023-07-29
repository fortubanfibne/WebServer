#ifdef TINY_MUDUO_EPOLLER_H_
#define  TINY_MUDUO_EPOLLER_H_

#include <sys/epoller.h>
#include <vector>
#include <map>

#include "NonCopyAble"

//事件数组的默认大小
static const int kDefaultEvents = 16;

namespace tiny_muduo{	
	class Channel;
	class Epoller:public NonCopyAble{
		public:
			typedef vector<epoll_event> Events;  //事件数组
			typedef vectro<Channel*> Channels;  //Channel数组

			Epoller();
			~Epoller();
			//从Epoller中移除channel
			void RemoveChannel( Channel* channel );
			//进行epoll_wait监听，将活跃就绪的channel存入channels中
			void Poll( int eventnums, Channels channels );
			//返回活跃的事数量
			void Epoll_wait(){
				return epoll_wait( epollfd_, &(*events_.begin()), static_cast<int> events_.size() , -1);
			}
			//将活跃的加入数组
			void FillActiveChannels( int eventsnums, Channel& Channel );
			//更新指定channel
			void Update( Channel* channel );
			//更新指定channel的事件
			void UpdateChannel( int operation, Channel* channel );

		private:
			typedef std::map<int, Channel*> ChannelMap;
			Events events_;  //存储epoll_wait返回的事件
			ChannelMap channels_;  //channel的映射表,以int为键，channel*为值
			int epollfd_;  //epoll文件描述符
	}
}
#endif
