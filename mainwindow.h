#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*! \class MainWindow
 *  \brief MainWindow
 *
 *  Slouží k vykreslení všech widgetů v okně a okna samotného
 */


#include <QMainWindow>
#include "item.h"
#include "board.h"
#include <QString>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT //kvuli slotum (connect je metoda QObjectu)

public:

	/*!
	*  \fn MainWindow::MainWindow(QWidget *parent = NULL);
	*  \brief Konstruktor
	*
	*  Inicializuje okno
	*
	*  \param[in] parent nastaven proto, aby se Qt samo postaralo o dealokaci objektu při ničení nadřezené třídy
	*
	*  \fn ~MainWindow()
	*  \brief Destruktor
	*/


	explicit MainWindow(char *argv[], int argc, QWidget *parent = NULL);//explicit zabrani implicitnimu pretypovani
	~MainWindow();

public slots:

	/*!
	 *  \fn void MainWindow::displayStatus (int)
	 *  \brief Stará se o zobrazení různých událostí ve hře
	 *  \param[in] event signalizuje událost

	 *  \fn void MainWindow::setStatusBar (int)
	 *  \brief Stará se o zobrazení různých událostí ve hře pomocí statusbaru
	 *  \param[in] event signalizuje událost

	 *  \fn void MainWindow::buttonPressHandle (int)
	 *  \brief Zakazuje/povoluje tlačítka v okně
	 *  \param[in] x povolit/zakázat


	 */
	void displayStatus (int);
	void setStatusBar (int);
	void buttonPressHandle (int);

private:

	/*!
	 *
	 *  \var MainWindow::ui
	 *  \brief Ukazatel na grafické rozhraní aplikace
	 *
	 *  \var MainWindow::array
	 *  \brief Pointer na dynamicky alokovanou hraci plochu
	 *
	 *  \var MainWindow::circle
	 *  \brief Ukazatel na obrázek (kolečko)
	 *
	 *  \var MainWindow::cross
	 *  \brief Ukazatel na obrázek (křížek)
	 *
	 *  \var MainWindow::empty
	 *  \brief Ukazatel na obrázek (prázdné pole)
	 *
	 */

	Ui::MainWindow *ui;
	Board * array;

	QPixmap *circle;
	QPixmap *cross;
	QPixmap *empty;


};

#endif // MAINWINDOW_H
