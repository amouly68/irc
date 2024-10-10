
#pragma once


#include "header.hpp"
#include "Channel.hpp"
#include "User.hpp"

class Server {

	public:
							Server( void );
							Server( char *port, char *passwd );
							~Server( void );
		std::string			getLocalIp( void );
		int					createSocket( void );
		void				disconnectChannels( std::string nickname );
		void				handleConnections( void );
		void				newConnection( void );
		void				run( void );
		bool				createChannel( std::string namechannel, std::string user );
		void				sendError(std::string code_Error, int sd);
		bool				generateResponse( User *user );
		std::string			generateDefaultNick( void );
		void				userCmd( Message msg, User *user );
		void				nickCmd( Message msg, User *user );
		void				nickPreRegistration( Message msg, User *user );
		void				passCmd( Message msg, User *user );
		void				joinCmd( Message msg, User *user );
		void				pongCmd( Message msg, User *user );
		void				topicCmd( Message msg, User *user );
		void				partCmd( Message msg, User *user );
		void				prvMsgCmd( Message msg, User *user );
		void				modeCmd( Message msg, User *user );
		void 				splitMode( const std::string& modeArg, std::vector<std::string>& modeChanges );
		void				parseMode( Channel* channel, User* user, const std::string& target, const std::string& modestring, const std::string* modeArgs, int nbArgs );
		void 				quitCmd(Message msg, User *user); 
		void				kickCmd( Message msg, User *user );
		void				inviteCmd( Message msg, User *user );
		void				whoCmd( Message msg, User* user );
		int					getPortno( void ) const;
		bool				passOK( void );
		void				setPassOK( bool passOK );
		void				connectServer( int sd , User *user );
		void				sendClient( int sd, std::string response );
		bool				is_valid( const std::string nickname );
		bool				isUserInServer( std::string nickname );
		int					getUserSd( std::string nickname );
		std::string			currentDate( void );
		void				serverFree( void );


		std::map<std::string, Channel*>	*getChannels( void ) ;

	private:
		
		std::string						_name;
		int								_nbClients;
		int								_portno;
		std::vector<int>				_clientSockets;
		struct sockaddr_in				_address;
		int								_addrLen;
		int								_masterSocket;
		fd_set							_readfds;
		std::string						_buffer;
		int								_max_sd;
		std::string						_passwd;
		std::map<std::string, Channel*>	_channels;
		std::vector<User>				_users;
		std::vector<Message>		    _messages;
		bool							_passOK;
		std::time_t						_creationTime;
		std::string						_creationDate;
		int								_numGuest;
		unsigned long					_maxUsers;
};

