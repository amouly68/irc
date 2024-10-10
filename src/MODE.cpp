#include "../includes/Server.hpp"

#include <iostream>
#include <string>
#include <vector>

bool onlyDigits( const std::string& str ) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

void Server::parseMode( Channel* channel, User* user, const std::string& target, const std::string& modestring, const std::string* modeArgs, int nbArgs ) {

    std::string nick = user->getNickName();
    int sd = user->getSd();
    int i = 0;
    bool isAdding = true;

    if (modestring[0] == 'b')
        return;
    if ((modestring[0] != '+') && (modestring[0] != '-')) {
        sendClient(sd, ERR_MODESTRINGERR(nick, modestring));
        return;
    }
    for (int j = 0; j < (int)modestring.size(); j++) {
        char modeLetter = modestring[j];
		if ((modeLetter == '+' ) || (modeLetter == '-')) {
			if(modeLetter != '+')
                isAdding = false;
            j++;
			if (j < (int)modestring.size())
					modeLetter = modestring[j];
			else{
                sendClient(sd, ERR_NEEDMOREPARAMS(nick, "MODE"));
                return;
            }
        }	
        switch (modeLetter) {
            case 'i':
                if (isAdding && !channel->isInviteOnly) {
					channel->isInviteOnly = true;
                    channel->sendMessgeToAllUsers(MODE(nick, target, "+i", ""));
                } else if (!isAdding && channel->isInviteOnly) {
                    channel->isInviteOnly= false;
                    channel->sendMessgeToAllUsers(MODE(nick, target, "-i", ""));
                }
                break;
            case 't':
                if (isAdding && !channel->isTopicProtected) {
                    channel->isTopicProtected = true;
                    channel->sendMessgeToAllUsers(MODE(nick, target, "+t", ""));
                } else if (!isAdding && channel->isTopicProtected) {
                    channel->isTopicProtected = false;
                    channel->sendMessgeToAllUsers(MODE(nick, target, "-t", ""));
                }
                break;
            case 'l':
                if (isAdding && (i < nbArgs) && onlyDigits(modeArgs[i])) {
                        int userLimit = atoi(modeArgs[i].c_str());
                         i++;
                        channel->userLimit = userLimit;
                        channel->hasUserLimit = true;
                        channel->sendMessgeToAllUsers(MODE(nick, target, "+l", ""));
                    } else if (!isAdding && channel->hasUserLimit){
                        channel->hasUserLimit = false;
                        channel->sendMessgeToAllUsers(MODE(nick, target, "-l", ""));
                    } else if ((i < nbArgs) && !(onlyDigits(modeArgs[i])))
                        sendClient(sd, ERR_INVALIDMODEPARAM(nick, target, modeLetter, modeArgs[i], "parameter is not a number"));
                break;
            case 'o':
                if (nbArgs != 0 && (i < nbArgs)) {
                    std::string operatorNick = modeArgs[i];
                    i++;
                    if (channel->isUserInChannel(operatorNick)) {
                        if (isAdding && !channel->isUserOp(operatorNick)) {
                            channel->addOperator(operatorNick);
                            channel->sendMessgeToAllUsers(MODE(nick, target, "+o", operatorNick));
                        } else {
                            if (channel->isUserOp(operatorNick)) {
                                channel->removeOperator(operatorNick);
                                channel->sendMessgeToAllUsers(MODE(nick, target, "-o", operatorNick));
                            }
                        }
                    } else {
                        Server::sendClient(sd,ERR_USERNOTINCHANNEL(nick, operatorNick, target));
                    }
                } else {
                        std::string opNick = channel->getChanNick(nick);
                        channel->sendMessgeToAllUsers(MODE(nick, target, "+o", ""));
                    }
                break;
            case 'k':
                if (isAdding && (i < nbArgs)) {       
                    std::string pass = modeArgs[i];
                    i++;
                    channel->password = pass;
                    channel->hasPassword = true;
                    channel->sendMessgeToAllUsers(MODE(nick, target, "+k", ""));
                } else if (!isAdding && channel->hasPassword){
                    channel->hasPassword = false;
                    channel->sendMessgeToAllUsers(MODE(nick, target, "-k", ""));
                    }
                break;
            default:
                break;
            }
        }
    }

void	Server::modeCmd( Message msg, User *user ) {
	
	std::string nick = user->getNickName();
    int nbArgs = msg.nbParam();
	std::string target = msg.getParam(0);
    int sd = user->getSd();
    if (msg.getParam(0) == nick && msg.getParam(1) == "+i") {
        sendClient(sd, MODE(nick, nick, "+i", ""));
        return;
    }

    if(target[0] != '#'){
	 	sendClient(sd, ERR_UMODEUNKNOWNFLAG(nick));
        return;
    }
    if(_channels.find(target) == _channels.end()) {
		sendClient(sd, ERR_NOSUCHCHANNEL(nick, target));
		return;
	}
    if (_channels[target]->isUserInChannel(nick) == false) {
			sendClient(sd, ERR_NOTONCHANNEL(nick, target));
			return;
	}
	if (nbArgs == 1) {
        std::string opNick = _channels[target]->getChanNick(nick);
		sendClient(sd, RPL_CHANNELMODEIS(opNick, target, "+", _channels[target]->getCurrentModes()));
    }
	else {
        std::string modestring = msg.getParam(1);
        nbArgs = msg.nbParam() - 2;
	    std::string modeArgs[nbArgs];
	    for(int i = 0; i < nbArgs; i++)
		    modeArgs[i] = msg.getParam(2 + i);
	
		if (modestring[0] == 'b') 
            return;
        
        if (_channels[target]->isUserOp(nick) == false) {
			sendClient(sd, ERR_CHANOPRIVSNEEDED(nick, target));
			return;
		}
		else  
		 	parseMode(_channels[target], user, target,modestring, modeArgs, nbArgs);
	}
}
