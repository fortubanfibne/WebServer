//一个Channel自始自终只能负责一个fd的IO事件分发
#ifdef TINY_MUDUO_CHANNEL_H_
#define TINY_MUDUO_CHANNEL_H_

#include <sys/epoll.h> //包含epoll系统调用相关的头文件

#include <utility> //std::move函数的头文件
#include <memroy>

#include "eventloop.h"
#include "callback.h"
#include "nocopyable.h"
namespace tiny_muduo{
	emun ChannelState{
		kNew; //新建状态
		kAdded; //已经添加到Epoller里面的Channels_数组
		kDeleted; //已经从Epoller里面的Channels_数组删除
	};

	class Channel ： public NonCopyAble{
		public:
			typedef std::function< void() > ErrorCallback;

			Channel( EventLoop* loop, const& int fd );
			~channel()；
			
			//处理事件的回调函数
			void HandleEvent();
			//带保护的处理时间的回调函数
			void HandleEventWithGuard( );
			
			//设置读事件的回调函数，参数为常量对象
			void SetReadCallback( const ReadCallback& callback ){
				//实现拷贝操作
				read_callback_ = callback;
			}
			//参数为一个临时对象或一个可被移动的对象
			void SetReadCallback( ReadCallback&& callback ){
				//用move实现资源的转移，避免拷贝操作
				read_callback_ = std::move( callback );
			}
			void SetWriteCallback( const WriteCallback& callback ){
				write_callback_ = callback;
			}
			void SetWriteCallback( WriteCallback&& callback ){
				write_callback_ = std::move( callback );
			}
			void SetErrorCallback( const ErrnoCallback& callback ){
				error_callback_ = callback;
			}
			void SetErrorCallback( ErrnoCallback&& callback ){
				error_callback_ = std::move( callback );
			}
			
			//将一个shared_ptr<viod>对象绑定到weak_ptr<viod>类型tie_，解决悬挂指针巍问题，调用Tie函数时，tie_可以调用lock函数判断对象是否存在，避免回调函数访问销毁的Channel对象
			void Tie( const std::shared_ptr<void>& ptr )
			{
				tie_ = ptr；
				tied_ = true;  //表示以及绑定了tie_指针
			}

			void EnableReading()
			{	
				events_ |= ( EPOLLIN | EPOPRI );  //开启读事件
				Update();  //更新Channel对象在EventLoop中的状态
			}

			void EnableWriting()
			{
				events_ |= EPOLLOUT;  //开启写事件
				Update();
			}

			void DisableAll()  //禁用所有事件
			{
				events_ = 0;
				Update();
			}
			void DisableWrite()  //禁用写事件
			{
				events_ &= ~EPOLLOUT;
				Update();
			}

			//更新Channel,将Channel添加到EventLoop中
			void Update(){
				loop_ -> Update( this );
			}

			
			void SetReceiveEvent( int events__ ) //设置接收到的事件
			{	
				recv_events_ = events__; 
			}
			void SetChannelState( ChannelState state__ ) //设置Channel状态
			{
				state_ = state__;
			}

			int fd() const  //返回文件描述符
			{
				return fd_;
			}
			int events() const  //返回需要更新的事件
			{
				return events_;
			}
			int recv_events() const  //返回接收到的事件
			{
				return recv_events_; 
			}
			ChannelState state() const  //返回Channel的状态				
			{
				return state_;
			}
			 //判断是否启用了写事件
			bool IsWriting() const{ return ( events_ &= EPOLLOUT ); }
			//判断是否启用了读事件
			bool IsReading() const{ return ( events_ &= (EPOLLIN | EPOLLPRI) ); }

			private:
				EventLoop* loop_; //所属的EventLoop对象
				int fd_; 
				int events_; //待更新的事件 
				int recv_events_; //接收到的事件
				std::weak_ptr <void> tie_;  //弱指针引用
				bool tied_;
				int error_; //错误码

				ChannelState state_;
				ReadCallback read_callback;
				WriteCallback write_callback;
				ErrorCallback error_calllback;
			
	};

}

#endif
