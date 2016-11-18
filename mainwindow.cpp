#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(char *argv[], int argc, QWidget *parent) : QMainWindow (parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	circle=new QPixmap (":/images/white.png");
	cross=new QPixmap (":/images/black.png");
	empty=new QPixmap (":/images/empty.png");

	this->setMinimumSize(26*MAX_X+80,26*MAX_Y+100);
	array=new Board (argv, argc, empty, circle, cross, this);
	ui->gridLayout->addWidget(array,2,0);
	ui->gridLayout->setRowMinimumHeight (2, 26*MAX_Y);
	ui->gridLayout->setColumnMinimumWidth(0, 26*MAX_X);
	ui->label_turn->setFixedHeight(25);
	ui->label_color->setFixedHeight(25);
	ui->label_turn->setScaledContents(1);
	ui->label_color->setScaledContents(1);
	ui->pushButton->setMaximumWidth(100);

	ui->label_score->setAlignment(Qt::AlignCenter);
	ui->label_score_black->setAlignment(Qt::AlignCenter);
	ui->label_score_white->setAlignment(Qt::AlignCenter);

	ui->label_4->setPixmap(*circle);
	ui->label_5->setPixmap(*cross);

	//nastaveni titulku okna
	if ((argc==3) && (QString(argv[1])=="server")){
		this->setWindowTitle("Connect Five Game Server");
	}else if ((argc==4) && (QString(argv[1])=="client")){
		this->setWindowTitle("Connect Five Game Client");
	}else{
		this->setWindowTitle("Connect Five Game");
	}

	connect (array, SIGNAL (statusChanged (int)), this, SLOT (displayStatus (int)));
	connect (ui->pushButton, SIGNAL (clicked ()), array, SLOT (reset ()));
	connect (array, SIGNAL (NewGamePressed (int)), this, SLOT (buttonPressHandle (int)));
	connect (ui->pushButton_moveBack, SIGNAL (clicked ()), array, SLOT (moveBackClicked (void)));

	if (array->server){
		connect (array->server, SIGNAL (connectionStatus (int)), this, SLOT (setStatusBar (int)));//potrebuju ukazat na statusbaru ze se nekdo pripojil/odpojil atd
		connect (array->server, SIGNAL (buttonPressed (int)), this, SLOT (buttonPressHandle (int)));
		connect (array->server, SIGNAL (statusChanged (int)), this, SLOT (displayStatus (int)));
		setStatusBar (2);
	}else if (array->client){
		connect (array->client, SIGNAL (connectionStatus (int)), this, SLOT (setStatusBar (int)));//potrebuju ukazat na statusbaru ze se nekdo pripojil/odpojil atd
		connect (array->client, SIGNAL (buttonPressed (int)), this, SLOT (buttonPressHandle (int)));
		connect (array->client, SIGNAL (statusChanged (int)), this, SLOT (displayStatus (int)));
		setStatusBar (2);
	}else{
		displayStatus(array->activeType);
	}

	//zobrazeni, kdo ma jakou barvu
	if (array->gameType==Board::TYPE_SERVER){
		ui->label_3->setText ("You are ");
		ui->label_color->setPixmap (*circle);
	}else if (array->gameType==Board::TYPE_CLIENT){
		ui->label_color->setPixmap (*cross);
		ui->label_3->setText ("You are ");
	}
}

void MainWindow::setStatusBar (int x)
{
	switch (x){
	case 0:
		ui->statusBar->showMessage ("Peer disconnected");
		break;

	case 1:
		ui->statusBar->showMessage ("Peer connected");

		if (array->activeType==Item::TYPE_CIRCLE){
			ui->label_turn->setPixmap (*circle);
		}else{
			ui->label_turn->setPixmap (*cross);
		}
		ui->label_2->setText (" plays");

		break;
	case 2:
		ui->statusBar->showMessage ("Waiting for peer");
		break;

	case 3:
		ui->statusBar->showMessage ("White won");
		ui->label_score_white->setText(QString::number (array->score[0]));
		//QMessageBox::information(NULL, "White won", "White won");
		break;

	case 4:
		ui->statusBar->showMessage ("Black won");
		ui->label_score_black->setText (QString::number (array->score[1]));
		//QMessageBox::information(NULL, "Black won", "Black won");
		break;

	case 5:
		ui->statusBar->showMessage ("Draw");
		ui->label_score_white->setText (QString::number (array->score[0]));
		ui->label_score_black->setText (QString::number (array->score[1]));
		//QMessageBox::information(NULL, "Draw", "Draw");
		break;
	case 6:
		ui->statusBar->showMessage ("Draw");
		ui->label_score_white->setText (QString::number (array->score[0]));
		ui->label_score_black->setText (QString::number (array->score[1]));
		//QMessageBox::information(NULL, "Draw", "Draw");
		break;
	}
}


void MainWindow::displayStatus (int event)
{
	switch (event){
	case 1:
		ui->label_turn->setPixmap (*circle);
		ui->label_2->setText (" plays");
		break;
	case 2:
		ui->label_turn->setPixmap (*cross);
		ui->label_2->setText (" plays");
		break;
	case 7:
		ui->statusBar->showMessage ("");
		break;
	case 8:
		ui->statusBar->showMessage ("Your opponent accepted your offeer.");
		break;
	case 9:
		ui->statusBar->showMessage ("Your opponent rejected your offer.");
		break;
	case 10:
		ui->statusBar->showMessage ("Asking your opponent...");
		break;
	case 11:
		ui->statusBar->showMessage ("You can't take back your move if the game is over.");
		break;
	default:
		//ui->label->setPixmap(empty);
		ui->label_turn->setText ("Game Over");
		ui->label_2->clear ();
		//ui->label_3->clear();
		setStatusBar (event);
	}
}

void MainWindow::buttonPressHandle (int x)
{
	ui->pushButton->setEnabled ((bool)x);
	ui->pushButton_moveBack->setEnabled ((bool)x);
}

MainWindow::~MainWindow ()
{
	delete ui;
}

