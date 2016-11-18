#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QString>
#include <iostream>
#include <QtNetwork>
using namespace std;


class Server : public QObject
{
Q_OBJECT
public:

	/*!
	 *
	 *	\fn Server::Server (int MAX_X, int MAX_Y, int port, QObject *parent=NULL)
	 *  \brief Konstruktor
	 *  \param[in] MAX_X je X rozměr hracího pole
	 *  \param[in] MAX_Y je Y rozměr hracího pole
	 *  \param[in] port je port, ke kterému se klient připojí
	 *  \param[in] parent je proto, aby se Qt samo postaralo o dealokaci objektu při ničení nadřezené třídy
	 *
	 *  Nastaví potřebné proměnné
	 *
	 *
	 *
	 *  \fn Server::start ()
	 *  \brief Vytvoří spojení se klientem
	 *
	 *  \fn Server::writeToClient (QString)
	 *  \brief Zapíše do socketu jednu řádku
	 *  \param[in] output řádka, která se ma zapsat
	 *  \return bool
	 *
	 *
	 */

	explicit Server (int MAX_X, int MAX_Y, int port, QObject *parent = NULL);
	bool writeToClient (QString output);
	void start ();

signals:

	/*!
	 *
	 *  \fn Server::move (int, int)
	 *  \brief Zavolá se se pokud po síti došel tah
	 *  \param[in] x - X souřadnice políčka
	 *  \param[in] y - Y souřadnice políčka
	 *
	 *  \fn Server::connectionStatus (int)
	 *  \brief Indikuje připojení/odpojení
	 *  \param[in] x - odpojeno/připojeno
	 *
	 *  \fn Server::reset_net (void)
	 *  \brief Indikuje příkaz klientu na reset hry
	 *
	 *  \fn Board::NewGamePressed (int)
	 *  \brief Vyvolán při stisknutí tlačítka New Game
	 *  \param[in] x zakazuje/povoluje tlačítka v okně
	 *
	 *  \fn Server::moveBack (void)
	 *  \brief Indikuje příkaz klientu na vrácení tahu
	 *
	 *  \fn Server::statusChanged (int)
	 *  \brief Signalizuje změnu statusu po kazdém tahu/při výhře/remíze
	 *  \param[in] event signalizuje status
	 *
	 *  \fn Server::buttonPressed (int)
	 *  \brief Signalizuje stisk jakéhokoli tlačítka
	 *  \param[in] x zakázat stisk/povolit stisk
	 *
	 */

	void move (int, int);
	void connectionStatus (int);
	void reset_net (void);
	void NewGamePressed (int);
	void moveBack ();
	void statusChanged (int);
	void buttonPressed (int);

public slots:

	/*!
	 *
	 *  \fn Server::clientService (void)
	 *  \brief Obslouží signály klienta, zahájí komunikaci
	 *
	 *  \fn Server::handleClient (void)
	 *  \brief Obslouží požadavky klienta
	 *
	 *  \fn Server::dropConnection (void)
	 *  \brief Zavře aktuální spojení
	 *
	 */

	void clientService();
	void handleClient();
	void dropConnection (void);


private:


	/*!
	 *
	 *  \fn Server::parseMessage ()
	 *  \brief Stará se o parsování a zpracování zprávy ze sítě
	 *  \return bool
	 *
	 *  \fn Server::readFromClient ()
	 *  \brief Přečte ze socketu jednu řádku
	 *  \return bool
	 *
	 *  \fn Server::MsgDialog (QString)
	 *  \brief Vyvolá dialogové okno
	 *  \param[in] message je zpráva, která se v okně zobrazí
	 *
	 *  \var Server::code
	 *  \brief Typ síťového příkazu
	 *  - 100 rozměry šachovnice
	 *  - 200 tah
	 *  - 300 dotaz soupeře na reset hry/vrácení tahu
	 *  - 400 tvrdý reset hrací plochy
	 *
	 *  \var Server::buffer
	 *  \brief Jedna řádka síťového příkazu
	 *
	 *  \var Server::port
	 *  \brief Číslo portu na klienta
	 *
	 *  \var Server::MAX_X
	 *  \brief X rozměr hrací plochy
	 *
	 *  \var Server::MAX_Y
	 *  \brief Y rozměr hrací plochy
	 *
	 *  \var Server::socket
	 *  \brief ukazatel na socket
	 *
	 *  \var Server::tcpServer
	 *  \brief ukazatel na server
	 *
	 */
	bool readFromClient ();
	bool parseMessage ();

	int MsgDialog (QString);

	int code;
	QString buffer;

	int port;
	int MAX_X;
	int MAX_Y;
	QTcpSocket *socket;
	QTcpServer *tcpServer;


};

#endif // SERVER_H
