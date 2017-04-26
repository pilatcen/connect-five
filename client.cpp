#include "client.h"


Client::Client (const int &MAX_X, const int &MAX_Y, const QHostAddress &hostname, const int &port, QObject *parent) : QObject(parent)
{
	this->socket = NULL;
	this->port=port;
	this->hostname = hostname;
	this->MAX_X=MAX_X;
	this->MAX_Y=MAX_Y;
}

bool Client::start()
{
	this->socket = new QTcpSocket(this);
	this->socket->connectToHost (hostname, this->port);

	if (!this->socket->isValid() || !this->socket->isOpen()){
		return 1;
	}

	connect(socket, SIGNAL(connected()), this, SLOT(serverService()));

	return 0;
}

void Client::serverService ()
{
	connect(socket, SIGNAL (readyRead ()), this, SLOT (handleServer ()));
	connect(socket, SIGNAL (disconnected ()), this, SLOT (dropConnection ()));
	//writeToServer ("100 "+QString::number(MAX_X)+" "+QString::number(MAX_Y)+"\n");
	this->writeToServer ("400 400 400\n");//reset
	emit this->connectionStatus (1);
}

void Client::handleServer ()
{
	this->readFromServer ();
	//cout << buffer.toLatin1().data() <<endl;

	if(!this->parseMessage ()){
		this->dropConnection ();
	}
}

bool Client::parseMessage ()
{
	QStringList p = buffer.split(" ");

	if(p.length() != 3){//zajisteni dodrzeni protokolu
		QMessageBox::critical (NULL, "Tic Tac Toe Server", "Server don't use our protocol!");
		return false;
	}

	switch (p[0].toInt()){
	case 100:
		if (p[1].toInt()!=MAX_X || p[2].toInt()!=MAX_Y){//pro jistotu u obou aby jim vyskocila hlaska ze nemaji stejnou hraci plochu
			QMessageBox::critical(NULL, "Tic Tac Toe Client", "Server and Client don't have the same size of playboard");
			return false;
		}
		break;
	case 200:
		emit this->move (p[1].toInt(), p[2].toInt());
		break;
	case 300:

		switch (p[1].toInt()){
		case 300:

			switch (MsgDialog ("Your opponent wants to reset game.")){
				case QMessageBox::Yes:
					this->writeToServer("300 1 1\n");
					emit this->reset_net ();
				   break;
				case QMessageBox::No:
				   this->writeToServer("300 0 0\n");
				   break;
			default:
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
			this->statusChanged (9);
			break;
		case 200:

			switch (MsgDialog ("Your opponent wants to take his move back.")){
				case QMessageBox::Yes:
					this->writeToServer("300 20 20\n");
					emit this->moveBack ();
				   break;
				case QMessageBox::No:
				   this->writeToServer("300 30 30\n");
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

bool Client::readFromServer ()
{
	if (this->socket){
		QByteArray line;

		if (!(line = this->socket->readLine().trimmed()).isEmpty()) {
			this->buffer = QString(line);
			return true;
		}else{
			this->dropConnection ();
			return false;
		}
	}
	return false;
}

bool Client::writeToServer (QString output)
{
	if(this->socket != NULL && !this->socket->isValid() || !this->socket || !this->socket->isOpen()){//kontrola otevrenosti a validity socketu
		return false;
	}
	this->socket->write (output.toAscii());
	this->socket->flush ();
	return true;
}

int Client::MsgDialog (QString message)
{
	QMessageBox msgBox;
	msgBox.setText(message);
	msgBox.setInformativeText("Do you want agree?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	return msgBox.exec();
}

void Client::dropConnection (void)
{
	emit connectionStatus (0);
	if (this->socket){
		this->socket->close ();
	}
	this->socket=NULL;
}
