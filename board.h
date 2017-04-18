#ifndef BOARD_H
#define BOARD_H

/*! \class Board
 *  \brief Board
 *
 *  Vytvoří hrací plochu
 */



#include <QWidget>
#include <QGridLayout>
#include <iostream>
#include "item.h"
#include "server.h"
#include "client.h"

using namespace std;

/** Rozměry hrací plochy */
#define MAX_X 20
#define MAX_Y 18
#define MAX_ITEMS MAX_Y*MAX_X




class Board : public QWidget
{
	friend class Item;
	Q_OBJECT //kvuli slotum (connect je metoda QObjectu)
public:

	/*!
	 *
	 *  \enum gameType
	 *  \brief Značí úlohu, kterou ve hře bude program zastávat
	 *
	 *  hra 2 hráčů na PC/server/klient
	 *
	 *	\fn Board::Board (QWidget  *parent = NULL)
	 *  \brief Konstruktor
	 *
	 *  Inicializuje hrací plochu, nastaví potřebné proměnné
	 *
	 *  \param[in] parent je proto, aby se Qt samo postaralo o dealokaci objektu při ničení nadřezené třídy
	 *
	 *  \fn Board::~Board ()
	 *  \brief destruktor
	 *
	 *  \var Board::gameType
	 *  \brief Značí úlohu, kterou ve hře bude program zastávat
	 *
	 *  \var Board::activeType
	 *  \brief Který hráč je na právě tahu
	 *
	 *  \var Board::server
	 *  \brief Ukazatel na server
	 *
	 *  \var Board::client
	 *  \brief Ukazatel na klienta
	 *
	 *  \var Board::score
	 *  \brief Pole s informacemi o skóre
	 */

	enum gameType{
		TYPE_LOCAL=0,
		TYPE_SERVER=1,
		TYPE_CLIENT=2
	};

	explicit Board (char *argv[], int argc, QPixmap *, QPixmap *, QPixmap *, QWidget  *parent = NULL);
	~Board ();

	bool game;
	gameType gameType;
	Item::Type activeType;
	Server *server;
	Client *client;

	int score[2];

signals:

	/*!
	 *  \fn Board::statusChanged (int)
	 *  \brief Signalizuje změnu statusu po kazdém tahu/při výhře/remíze
	 *  \param[in] event signalizuje status
	 *
	 *  \fn Board::NewGamePressed (int)
	 *  \brief Vyvolán při stisknutí tlačítka New Game
	 *  \param[in] x zakazuje/povoluje tlačítka v okně
	 *
	 */
	void statusChanged (int);
	void NewGamePressed (int);

public slots:
	/*!
	 *  \fn void Board::reset (void)
	 *  \brief Resetuje celé hrací pole a připraví ho k nové hře
	 *
	 *  \fn void Board::reset_net (void)
	 *  \brief Resetuje celé hrací pole, volá se pouze ze sítě
	 *
	 *  \fn Board::addItem_net (int, int)
	 *  \brief Volána ze sítě, volá funkci <EM>addItem</EM>
	 *  \param[in] x souřadnice daného políčka
	 *  \param[in] y souřadnice daného políčka
	 *
	 *  \fn void Board::setGame (int)
	 *  \brief Nastavuje proměnnou <EM>game</EM>
	 *
	 *  \fn void Board::moveBack (void)
	 *  \brief Vrací tah zpět
	 *
	 *  \fn void Board::moveBackClicked (void)
	 *  \brief Požádá protihráče o vrácení tahu, nebo rovnou zavolá funkci <EM>moveBack</EM>
	 *
	 */
	void reset (void);
	void reset_net (void);
	void addItem_net (const int &, const int &);
	void setGame (int);
	void moveBack (void);
	void moveBackClicked (void);

private:

	/*!
	 *
	 *  \fn Board::testwin (int x, int y)
	 *  \brief Zjistí, jestli někdo nevyhrál
	 *  \param[in] x souřadnice daného políčka
	 *  \param[in] y souřadnice daného políčka
	 *
	 *  \fn Board::addItem (int, int)
	 *  \brief Vloží do políčka zvýrazněný křížek/kolečko
	 *  \param[in] x souřadnice daného políčka
	 *  \param[in] y souřadnice daného políčka
	 *
	 *  \fn Board::unHighlight (void)
	 *  \brief Znevýrazní přdposlední tah
	 *
	 *
	 *  \var Board::game
	 *  \brief Signalizuje, jestli se může hrát
	 *
	 *  \var Board::win
	 *  \brief Kdo vyhrál
	 *
	 *  \var Board::firstPlayer
	 *  \brief Kdo v daném kole hrál jako první
	 *
	 *  \var Board::moves
	 *  \brief Seznam všech tahů
	 *
	 *  \var Board::pentad
	 *  \brief Seznam tahů, které se při výhře zvýrazní
	 *
	 *  \var Board::items
	 *  \brief Dynamicky alokované 2D pole
	 *
	 *  Pointer na dynamicky alokované 2D pole, pointer v každém políčku ukazuje na dynamicky alokovaný Item
	 *
	 *  \var Board::circle
	 *  \brief Ukazatel na obrázek kolečka
	 *
	 *  \var Board::circleHighlighted
	 *  \brief Zvýrazněné kolečko
	 *
	 *  \var Board::cross
	 *  \brief Ukazatel na obrázek křížku
	 *
	 *  \var Board::crossHighlighted
	 *  \brief Zvýrazněný křížek
	 *
	 * \var Board::empty
	 * \brief Ukazatel na obrázek prázdného pole
	 */

	bool testwin (int, int);
	void addItem (int, int);
	void unHighlight (void);


	Item::Type win;
	Item::Type firstPlayer;

	QList <QPair <int, int> > moves;
	QList <QPair <int, int> > pentad;

	Item *** items;

	QPixmap *circle;
	QPixmap circleHighlighted;
	QPixmap *cross;
	QPixmap crossHighlighted;
	QPixmap *empty;
};

#endif // BOARD_H
