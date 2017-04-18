#ifndef ITEM_H
#define ITEM_H

/*! \class Item
 *  \brief Item
 *
 *  Vytvoří jedno políčko
 */

#include <QLabel>

//dopredna deklarace
class Board;

class Item : public QLabel
{
	friend class Board;
	Q_OBJECT //kvuli slotum (connect je metoda QObjectu)
public:

	/*!
	*  \enum Type
	*  \brief Typ políčka
	*
	*  prázdné/křížek/kolečko
	*
	*  \fn Item::Item(int y, int x, Board *parent = NULL)
	*  \brief Konstruktor
	*
	*  Inicializuje jedno políčko, nastaví potřebné proměnné
	*
	*  \param[in] y je souřadnice daného políčka
	*  \param[in] x je souřadnice daného políčka
	*  \param[in] parent nastaven proto, aby se Qt samo postaralo o dealokaci objektu při ničení nadřezené třídy
	*/

	enum Type{
		TYPE_EMPTY=0,
		TYPE_CIRCLE=1,
		TYPE_CROSS=2
	};

	explicit Item (const int &, const int &, QPixmap *, Board *parent = NULL);

signals:
public slots:
private:

	/*!
	 *  \fn Item::mousePressEvent (QMouseEvent *)
	 *  \brief Přetížená metoda, která odchytí kliknutí myši
	 *  \param[in] Stisk jakéhokoli tlačítka myši nad políčkem
	 *
	 *  \fn Item::clear (void)
	 *  \brief Resetuje políčko
	 *
	 *  \var Item::x, Item::y
	 *  \brief X a Y souřadnice každého políčka
	 *
	 *  \var Item::type
	 *  \brief Typ políčka
	 *  - prázdné
	 *  - kolečko
	 *  - křížek
	 *
	 *  \var Item::parentPtr
	 *  \brief Odkaz na rodiče
	 *
	 *  \var Item::empty
	 *  \brief Ukazatel na obrázek (prázdné políčko)
	 *
	 */

	void mousePressEvent (const QMouseEvent *);
	void clear (void);

	int x,y;
	Type type;
	Board* parentPtr;
	QPixmap * empty;
	

};

#endif // ITEM_H
