#include "server.h"


Server::Server (int MAX_X, int MAX_Y, int port, QObject *parent) : QObject(parent)
{
	this->socket=NULL;
	this->port=port;
	this->MAX_X=MAX_X;
	this->MAX_Y=MAX_Y;
}

bool Server::start ()//spusti se server mode
{
	this->tcpServer = new QTcpServer (this);//vytvoreni serveru
	tcpServer->setMaxPendingConnections(1);

	if (!tcpServer->listen (QHostAddress::Any, this->port)){//testovani jestli muzu poslouchat na danym portu
		QMessageBox::critical(NULL, "Tic Tac Toe Server", "Unable to start the server, this port isn't available.");
		dropConnection ();
		cout << "Unable to start the server, this port isn't available." << endl;
		return 1;
	}

	connect(tcpServer, SIGNAL (newConnection ()), this, SLOT (clientService ()));

	return 0;
}

void Server::clientService ()
{
	if (!socket){////////pokud je hra jiz vytvorena, nesmi se pripojit nikdo dalsi
		socket = tcpServer->nextPendingConnection();//ziskani socketu serveru
		connect(socket, SIGNAL (disconnected ()), this, SLOT (dropConnection (void)));
		connect(socket, SIGNAL (readyRead ()), this, SLOT (handleClient ()));

		//writeToClient("100 "+QString::number(MAX_X)+" "+QString::number(MAX_Y)+"\n");//zasle velikost sachovnice
		emit connectionStatus (1);
	}
}

void Server::handleClient ()
{
	readFromClient ();
	//cout << buffer.toLatin1().data() <<endl;

	if(!parseMessage ()){
		dropConnection ();
		start ();
	}
}

bool Server::parseMessage ()
{
	QStringList p = buffer.split(" ");

	if(p.length() != 3){//zajisteni dodrzeni protokolu
		QMessageBox::critical (NULL, "Tic Tac Toe Server", "Client don't use our protocol!");
		return false;
	}
	//aby se mohli posilat ruzny kody doplnit do additemu(nebo neceho jinyho) i signal na novou hru

	switch (p[0].toInt()){
	case 100:
		if (p[1].toInt()!=MAX_X || p[2].toInt()!=MAX_Y){//pro jistotu u obou aby jim vyskocila hlaska ze nemaji stejnou hraci plochu
			QMessageBox::critical(NULL, "Tic Tac Toe Server", "Server and Client don't have the same size of playboard");
			return false;
		}
		break;
	case 200:
		emit move (p[1].toInt (), p[2].toInt ());
		break;
	case 300:

		switch (p[1].toInt()){
		case 300:

			switch (MsgDialog ("Your opponent wants to reset game.")){
				case QMessageBox::Yes:
					writeToClient("300 1 1\n");
					emit reset_net ();
				   break;
				case QMessageBox::No:
				   writeToClient("300 0 0\n");
				   break;
			}
			break;
		case 1:
			emit reset_net ();
			emit NewGamePressed (1);
			emit buttonPressed (1);
			//QMessageBox::information(NULL, "Game reseted.", "Your opponent accepted your offeer.");
			statusChanged (8);
			break;
		case 0:
			emit NewGamePressed (1);
			emit buttonPressed (1);
			//QMessageBox::information(NULL, "Game wasn't reseted.", "Your opponent rejected your offer.");
			emit statusChanged (9);
			break;
		case 200:

			switch (MsgDialog ("Your opponent wants to take his move back.")){
				case QMessageBox::Yes:
					writeToClient("300 20 20\n");
					emit moveBack ();
				   break;
				case QMessageBox::No:
				   writeToClient("300 30 30\n");
				   break;
			}

			break;
		case 20:
			emit moveBack ();
			emit NewGamePressed (1);
			emit buttonPressed (1);
			//QMessageBox::information(NULL, "Game reseted.", "Your opponent accepted your offeer.");
			emit statusChanged (8);
			break;
		case 30:
			emit NewGamePressed (1);
			emit buttonPressed (1);
			//QMessageBox::information(NULL, "Game wasn't reseted.", "Your opponent rejected your offer.");
			emit statusChanged (9);
			break;

		}

		break;
	case 400:
		emit reset_net ();
		break;
	}
	return true;
}

bool Server::readFromClient () //cte z daneho sitoveho socketu
{
	QByteArray line=socket->readLine().trimmed();

	if(!line.isEmpty ()){
		buffer = QString (line);
		return true;
	}
	QMessageBox::critical (NULL, "Tic Tac Toe Server", "Invalid read");
	dropConnection ();
	start ();
	return false;
}

bool Server::writeToClient (QString output)
{
	if(!socket->isValid() || !socket || !socket->isOpen()){//kontrola otevrenosti a validity socketu
		return false;
	}

	if (socket){
		socket->write (output.toAscii());
		socket->flush ();
		return true;
	}else{
		//QMessageBox::critical(NULL, "Tic Tac Toe Server", "Nobody has connected yet!");
		return false;
	}
}

int Server::MsgDialog (QString message)
{
	QMessageBox msgBox;
	msgBox.setText(message);
	msgBox.setInformativeText("Do you want agree?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	return msgBox.exec();
}

void Server::dropConnection (void)
{
	emit connectionStatus (0);
	tcpServer->close ();
	if (socket){
		socket->close ();
	}
	socket=NULL;

	//start ();//reset daneho spojeni
}
