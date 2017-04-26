#include "server.h"


Server::Server (const int &MAX_X, const int &MAX_Y, const int &port, QObject *parent) : QObject(parent)
{
	this->socket=NULL;
	this->port=port;
	this->MAX_X=MAX_X;
	this->MAX_Y=MAX_Y;
}

bool Server::start ()//spusti se server mode
{
	this->tcpServer = new QTcpServer (this);//vytvoreni serveru
	this->tcpServer->setMaxPendingConnections(1);

	if (!this->tcpServer->listen (QHostAddress::Any, this->port)){//testovani jestli muzu poslouchat na danym portu
		QMessageBox::critical(NULL, "Tic Tac Toe Server", "Unable to start the server, this port isn't available.");
		this->dropConnection ();
		cout << "Unable to start the server, this port isn't available." << endl;
		return 1;
	}

	connect(this->tcpServer, SIGNAL (newConnection ()), this, SLOT (clientService ()));

	return 0;
}

void Server::clientService ()
{
	if (!this->socket){////////pokud je hra jiz vytvorena, nesmi se pripojit nikdo dalsi
		this->socket = this->tcpServer->nextPendingConnection();//ziskani socketu serveru
		connect(this->socket, SIGNAL (disconnected ()), this, SLOT (dropConnection (void)));
		connect (this->socket, SIGNAL (readyRead ()), this, SLOT (handleClient ()));

		//writeToClient("100 "+QString::number(MAX_X)+" "+QString::number(MAX_Y)+"\n");//zasle velikost sachovnice
		emit this->connectionStatus (1);
	}
}

void Server::handleClient ()
{
	this->readFromClient ();
	//cout << buffer.toLatin1().data() <<endl;

	if(!this->parseMessage ()){
		this->dropConnection ();
		this->start ();
	}
}

bool Server::parseMessage ()
{
	QStringList p = this->buffer.split(" ");

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
		emit this->move (p[1].toInt (), p[2].toInt ());
		break;
	case 300:

		switch (p[1].toInt()){
		case 300:

			switch (MsgDialog ("Your opponent wants to reset game.")){
				case QMessageBox::Yes:
					this->writeToClient("300 1 1\n");
					emit reset_net ();
				   break;
				case QMessageBox::No:
				   this->writeToClient("300 0 0\n");
				   break;
			}
			break;
		case 1:
			emit this->reset_net ();
			emit this->NewGamePressed (1);
			emit this->buttonPressed (1);
			//QMessageBox::information(NULL, "Game reseted.", "Your opponent accepted your offeer.");
			this->statusChanged (8);
			break;
		case 0:
			emit this->NewGamePressed (1);
			emit this->buttonPressed (1);
			//QMessageBox::information(NULL, "Game wasn't reseted.", "Your opponent rejected your offer.");
			emit this->statusChanged (9);
			break;
		case 200:

			switch (MsgDialog ("Your opponent wants to take his move back.")){
				case QMessageBox::Yes:
					this->writeToClient("300 20 20\n");
					emit this->moveBack ();
				   break;
				case QMessageBox::No:
				   this->writeToClient("300 30 30\n");
				   break;
			}

			break;
		case 20:
			emit this->moveBack ();
			emit this->NewGamePressed (1);
			emit this->buttonPressed (1);
			//QMessageBox::information(NULL, "Game reseted.", "Your opponent accepted your offeer.");
			emit this->statusChanged (8);
			break;
		case 30:
			emit this->NewGamePressed (1);
			emit this->buttonPressed (1);
			//QMessageBox::information(NULL, "Game wasn't reseted.", "Your opponent rejected your offer.");
			emit this->statusChanged (9);
			break;

		}

		break;
	case 400:
		emit this->reset_net ();
		break;
	}
	return true;
}

bool Server::readFromClient () //cte z daneho sitoveho socketu
{
	QByteArray line=this->socket->readLine().trimmed();

	if(!line.isEmpty ()){
		this->buffer = QString (line);
		return true;
	}
	QMessageBox::critical (NULL, "Tic Tac Toe Server", "Invalid read");
	this->dropConnection ();
	this->start ();
	return false;
}

bool Server::writeToClient (const QString &output)
{
	if(this->socket != NULL && !this->socket->isValid() || !this->socket || !this->socket->isOpen()){//kontrola otevrenosti a validity socketu
		return false;
	}

	if (this->socket){
		this->socket->write (output.toAscii());
		this->socket->flush ();
		return true;
	}else{
		//QMessageBox::critical(NULL, "Tic Tac Toe Server", "Nobody has connected yet!");
		return false;
	}
}

int Server::MsgDialog (const QString &message)
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
