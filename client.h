#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QMessageBox>
#include <QStringList>
#include <iostream>
#include <QString>
#include <QHostAddress>
using namespace std;

class Client : public QObject
{
Q_OBJECT
public:

	/*!
	 *
	 *	\fn Client::Client (int MAX_X, int MAX_Y, const QHostAddress hostname, int port, QObject *parent=NULL)
	 *  \brief Konstruktor
	 *  \param[in] MAX_X je X rozměr hracího pole
	 *  \param[in] MAX_Y je Y rozměr hracího pole
	 *  \param[in] hostname je IP adresa serveru, ke kterému se klient připojí
	 *  \param[in] port je port, ke kterému se klient připojí
	 *  \param[in] parent je proto, aby se Qt samo postaralo o dealokaci objektu při ničení nadřezené třídy
	 *
	 *  Nastaví potřebné proměnné
	 *
	 *
	 *  \fn Client::writeToServer (QString)
	 *  \brief Zapíše do socketu jednu řádku
	 *  \param[in] output řádka, která se ma zapsat
	 *  \return bool
	 *
	 *  \fn Client::start ()
	 *  \brief Vytvoří spojení se serverem
	 *
	 *
	 */

	explicit Client (const int &MAX_X, const int &MAX_Y, const QHostAddress &hostname, const int &port, QObject *parent=NULL);
	bool start (void);
	bool writeToServer (QString);

signals:

	/*!
	 *
	 *  \fn Client::move (int, int)
	 *  \brief Zavolá se se pokud po síti došel tah
	 *  \param[in] x - X souřadnice políčka
	 *  \param[in] y - Y souřadnice políčka
	 *
	 *  \fn Client::connectionStatus (int)
	 *  \brief Indikuje připojení/odpojení
	 *  \param[in] x - odpojeno/připojeno
	 *
	 *  \fn Client::reset_net (void)
	 *  \brief Indikuje příkaz serveru na reset hry
	 *
	 *  \fn Board::NewGamePressed (int)
	 *  \brief Vyvolán při stisknutí tlačítka New Game
	 *  \param[in] x zakazuje/povoluje tlačítka v okně
	 *
	 *  \fn Client::moveBack (void)
	 *  \brief Indikuje příkaz serveru na vrácení tahu
	 *
	 *  \fn Client::statusChanged (int)
	 *  \brief Signalizuje změnu statusu po kazdém tahu/při výhře/remíze
	 *  \param[in] event signalizuje status
	 *
	 *  \fn Client::buttonPressed (int)
	 *  \brief Signalizuje stisk jakéhokoli tlačítka
	 *  \param[in] x zakázat stisk/povolit stisk
	 *
	 */

	void move (const int&, const int&);
	void connectionStatus (const int&);
	void reset_net (void);
	void NewGamePressed (const int&);
	void moveBack (void);
	void statusChanged (const int&);
	void buttonPressed (const int&);

public slots:

	/*!
	 *
	 *  \fn Client::serverService (void)
	 *  \brief Obslouží signály serveru, zahájí komunikaci
	 *
	 *  \fn Client::handleServer (void)
	 *  \brief Obslouží požadavky serveru
	 *
	 *  \fn Client::dropConnection (void)
	 *  \brief Zavře aktuální spojení
	 *
	 */


	void serverService (void);
	void handleServer (void);
	void dropConnection (void);

private:

	/*!
	 *
	 *  \fn Client::readFromServer ()
	 *  \brief Přečte ze socketu jednu řádku
	 *  \return bool
	 *
	 *  \fn Client::parseMessage ()
	 *  \brief Stará se o parsování a zpracování zprávy ze sítě
	 *  \return bool
	 *
	 *  \fn Client::MsgDialog (QString)
	 *  \brief Vyvolá dialogové okno
	 *  \param[in] message je zpráva, která se v okně zobrazí
	 *
	 *  \var Client::code
	 *  \brief Typ síťového příkazu
	 *  - 100 rozměry šachovnice
	 *  - 200 tah
	 *  - 300 dotaz soupeře na reset hry/vrácení tahu
	 *  - 400 tvrdý reset hrací plochy
	 *
	 *  \var Client::buffer
	 *  \brief Jedna řádka síťového příkazu
	 *
	 *  \var Client::port
	 *  \brief Číslo portu na serveru
	 *
	 *  \var Client::MAX_X
	 *  \brief X rozměr hrací plochy
	 *
	 *  \var Client::MAX_Y
	 *  \brief Y rozměr hrací plochy
	 *
	 *  \var Client::hostname
	 *  \brief IP adresa serveru
	 *
	 *  \var Client::socket
	 *  \brief Ukazatel na socket
	 *
	 */
	bool readFromServer (void);
	bool parseMessage (void);

	int MsgDialog (QString);

	int code;
	QString buffer;

	int port;
	int MAX_X;
	int MAX_Y;
	QHostAddress hostname;
	QTcpSocket *socket;


};

#endif // CLIENT_H
