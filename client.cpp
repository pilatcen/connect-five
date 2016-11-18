#include "client.h"


Client::Client (int MAX_X, int MAX_Y, const QHostAddress hostname, int port, QObject *parent) : QObject(parent)
{
	this->socket = NULL;
	this->port=port;
	this->hostname = hostname;
	this->MAX_X=MAX_X;
	this->MAX_Y=MAX_Y;
}

void Client::start ()
{
	this->socket = new QTcpSocket(this);
	socket->connectToHost (hostname, this->port);
	connect(socket, SIGNAL(connected()), this, SLOT(serverService()));
}

void Client::serverService ()
{
	connect(socket, SIGNAL (readyRead ()), this, SLOT (handleServer ()));
	connect(socket, SIGNAL (disconnected ()), this, SLOT (dropConnection ()));
	//writeToServer ("100 "+QString::number(MAX_X)+" "+QString::number(MAX_Y)+"\n");
	writeToServer ("400 400 400\n");//reset
	emit connectionStatus (1);
}

void Client::handleServer ()
{
	readFromServer ();
	//cout << buffer.toLatin1().data() <<endl;

	if(!parseMessage ()){
		dropConnection ();
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
		emit move (p[1].toInt(), p[2].toInt());
		break;
	case 300:

		switch (p[1].toInt()){
		case 300:

			switch (MsgDialog ("Your opponent wants to reset game.")){
				case QMessageBox::Yes:
					writeToServer("300 1 1\n");
					emit reset_net ();
				   break;
				case QMessageBox::No:
				   writeToServer("300 0 0\n");
				   break;
			default:
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
			statusChanged (9);
			break;
		case 200:

			switch (MsgDialog ("Your opponent wants to take his move back.")){
				case QMessageBox::Yes:
					writeToServer("300 20 20\n");
					emit moveBack ();
				   break;
				case QMessageBox::No:
				   writeToServer("300 30 30\n");
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

bool Client::readFromServer ()
{
	if (socket){
		QByteArray line;

		if (!(line = socket->readLine().trimmed()).isEmpty()) {
			buffer = QString(line);
			return true;
		}else{
			dropConnection ();
			return false;
		}
	}
	return false;
}

bool Client::writeToServer (QString output)
{
	if(!socket->isValid() || !socket || !socket->isOpen()){//kontrola otevrenosti a validity socketu
		return false;
	}
	socket->write (output.toAscii());
	socket->flush ();
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
	if (socket){
		socket->close ();
	}
	socket=NULL;
}
