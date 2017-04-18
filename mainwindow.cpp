#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(char *argv[], int argc, QWidget *parent) : QMainWindow (parent), ui(new Ui::MainWindow)
{
	this->ui->setupUi(this);

	this->circle=new QPixmap (":/images/white.png");
	this->cross=new QPixmap (":/images/black.png");
	this->empty=new QPixmap (":/images/empty.png");

	this->setMinimumSize(26*MAX_X+80,26*MAX_Y+100);
	this->array=new Board (argv, argc, empty, circle, cross, this);
	this->ui->gridLayout->addWidget(array,2,0);
	this->ui->gridLayout->setRowMinimumHeight (2, 26*MAX_Y);
	this->ui->gridLayout->setColumnMinimumWidth(0, 26*MAX_X);
	this->ui->label_turn->setFixedHeight(25);
	this->ui->label_color->setFixedHeight(25);
	this->ui->label_turn->setScaledContents(1);
	this->ui->label_color->setScaledContents(1);
	this->ui->pushButton->setMaximumWidth(100);

	this->ui->label_score->setAlignment(Qt::AlignCenter);
	this->ui->label_score_black->setAlignment(Qt::AlignCenter);
	this->ui->label_score_white->setAlignment(Qt::AlignCenter);

	this->ui->label_4->setPixmap(*circle);
	this->ui->label_5->setPixmap(*cross);

	//nastaveni titulku okna
	if ((argc==3) && (QString(argv[1])=="server")){
		this->setWindowTitle("Connect Five Game Server");
	}else if ((argc==4) && (QString(argv[1])=="client")){
		this->setWindowTitle("Connect Five Game Client");
	}else{
		this->setWindowTitle("Connect Five Game");
	}

	connect (array, SIGNAL (statusChanged (int)), this, SLOT (displayStatus (const int&)));
	connect (ui->pushButton, SIGNAL (clicked ()), array, SLOT (reset ()));
	connect (array, SIGNAL (NewGamePressed (int)), this, SLOT (buttonPressHandle (int)));
	connect (ui->pushButton_moveBack, SIGNAL (clicked ()), array, SLOT (moveBackClicked (void)));


	connect (ui->actionNew_Network_Game, SIGNAL (triggered ()), this, SLOT (startNetworkGame()));
	connect (ui->actionConnect_to_Game, SIGNAL (triggered ()), this, SLOT (ConnectToGame ()));

	if (this->array->server){
		this->setStatusBar (2);
	}else if (this->array->client){
		this->setStatusBar (2);
	}else{
		this->displayStatus(this->array->activeType);
	}

	//zobrazeni, kdo ma jakou barvu
	if (this->array->gameType==Board::TYPE_SERVER){
		this->ui->label_3->setText ("You are ");
		this->ui->label_color->setPixmap (*circle);
	}else if (this->array->gameType==Board::TYPE_CLIENT){
		this->ui->label_color->setPixmap (*cross);
		this->ui->label_3->setText ("You are ");
	}
}

void MainWindow::startNetworkGame(){

	int port =0;

	if (this->array->server){
		//this->array->server->dropConnection();
		delete this->array->server;
		this->array->server = NULL;
	}
	if (this->array->client){
		//this->array->client->dropConnection();
		delete this->array->client;
		this->array->client = NULL;
	}

	bool ok;
	port = QInputDialog::getInt(this, tr("Server port number"),
								tr("Server port number:"), 4000, 0, 65535 ,1, &ok);

	if (ok == false){
		return;
	}


	this->array->server=new Server (MAX_X, MAX_Y, port, this->array);

	if (this->array->server->start() == 1){
		return;
	}

	this->array->gameType=Board::TYPE_LOCAL;
	this->array->reset();
	this->setStatusBar (2);
	this->array->gameType=Board::TYPE_SERVER;
	this->array->game=0;

	connect (this->array->server, SIGNAL(move(int, int)), this->array, SLOT(addItem_net (int, int)));
	connect (this->array->server, SIGNAL(connectionStatus (int)), this->array, SLOT(setGame (int)));
	connect (this->array->server, SIGNAL(NewGamePressed (int)), this->array, SLOT(setGame (int)));
	connect (this->array->server, SIGNAL(reset_net ()), this->array, SLOT(reset_net()));
	connect (this->array->server, SIGNAL(moveBack ()), this->array, SLOT(moveBack()));

	connect (this->array->server, SIGNAL (connectionStatus (int)), this, SLOT (setStatusBar (int)));//potrebuju ukazat na statusbaru ze se nekdo pripojil/odpojil atd
	connect (this->array->server, SIGNAL (buttonPressed (int)), this, SLOT (buttonPressHandle (int)));
	connect (this->array->server, SIGNAL (statusChanged (int)), this, SLOT (displayStatus (int)));

}

void MainWindow::ConnectToGame(){
	QHostAddress hostname;
	bool ok1, ok2;
	int port =0;

	if (this->array->client){
		//this->array->client->dropConnection();
		delete this->array->client;
		this->array->client = NULL;
	}
	if (this->array->server){
		//this->array->server->dropConnection();
		delete this->array->server;
		this->array->server = NULL;
	}

	QString address = QInputDialog::getText(this, tr("Server IP address"),
											tr("Server IP address:"), QLineEdit::Normal,
											"127.0.0.1", &ok1);
	port = QInputDialog::getInt(this, tr("Server port number"),
									tr("Server port number:"), 4000, 0, 65535, 1, &ok2);

	if (ok1 && ok2 && !address.isEmpty() && port!=0){
		if(!hostname.setAddress (address)){
			QMessageBox::critical(NULL, "Invalid IP address", "You've entered an invalid IP address!");
			cout << "Unable to start the client, you've entered an invalid IP address!." << endl;
			exit (0);
		}

		this->array->client=new Client (MAX_X, MAX_Y, hostname, port, this->array);
		if (this->array->client->start() == 1){
			return;
		}

		this->array->gameType=Board::TYPE_LOCAL;
		this->array->reset();
		this->array->gameType=Board::TYPE_CLIENT;
		this->array->game=0;
		this->setStatusBar (2);


		connect (this->array->client, SIGNAL(move(int, int)), this->array, SLOT(addItem_net (int, int)));
		connect (this->array->client, SIGNAL(connectionStatus (int)), this->array, SLOT(setGame (int)));
		connect (this->array->client, SIGNAL(NewGamePressed (int)), this->array, SLOT(setGame (int)));
		connect (this->array->client, SIGNAL(reset_net ()), this->array, SLOT(reset_net()));
		connect (this->array->client, SIGNAL(moveBack ()), this->array, SLOT(moveBack ()));

		connect (this->array->client, SIGNAL (connectionStatus (int)), this, SLOT (setStatusBar (int)));//potrebuju ukazat na statusbaru ze se nekdo pripojil/odpojil atd
		connect (this->array->client, SIGNAL (buttonPressed (int)), this, SLOT (buttonPressHandle (int)));
		connect (this->array->client, SIGNAL (statusChanged (int)), this, SLOT (displayStatus (int)));
	}
}



void MainWindow::setStatusBar (const int &x)
{
	switch (x){
	case 0:
		this->ui->statusBar->showMessage ("Peer disconnected");
		break;

	case 1:
		this->ui->statusBar->showMessage ("Peer connected");

		if (this->array->activeType==Item::TYPE_CIRCLE){
			this->ui->label_turn->setPixmap (*circle);
		}else{
			this->ui->label_turn->setPixmap (*cross);
		}
		this->ui->label_2->setText (" plays");

		break;
	case 2:
		this->ui->statusBar->showMessage ("Waiting for peer");
		break;

	case 3:
		this->ui->statusBar->showMessage ("White won");
		this->ui->label_score_white->setText(QString::number (array->score[0]));
		//QMessageBox::information(NULL, "White won", "White won");
		break;

	case 4:
		this->ui->statusBar->showMessage ("Black won");
		this->ui->label_score_black->setText (QString::number (array->score[1]));
		//QMessageBox::information(NULL, "Black won", "Black won");
		break;

	case 5:
		this->ui->statusBar->showMessage ("Draw");
		this->ui->label_score_white->setText (QString::number (array->score[0]));
		this->ui->label_score_black->setText (QString::number (array->score[1]));
		//QMessageBox::information(NULL, "Draw", "Draw");
		break;
	case 6:
		this->ui->statusBar->showMessage ("Draw");
		this->ui->label_score_white->setText (QString::number (array->score[0]));
		this->ui->label_score_black->setText (QString::number (array->score[1]));
		//QMessageBox::information(NULL, "Draw", "Draw");
		break;
	}
}


void MainWindow::displayStatus (const int &event)
{
	switch (event){
	case 1:
		this->ui->label_turn->setPixmap (*circle);
		this->ui->label_2->setText (" plays");
		break;
	case 2:
		this->ui->label_turn->setPixmap (*cross);
		this->ui->label_2->setText (" plays");
		break;
	case 7:
		this->ui->statusBar->showMessage ("");
		break;
	case 8:
		this->ui->statusBar->showMessage ("Your opponent accepted your offeer.");
		break;
	case 9:
		this->ui->statusBar->showMessage ("Your opponent rejected your offer.");
		break;
	case 10:
		this->ui->statusBar->showMessage ("Asking your opponent...");
		break;
	case 11:
		this->ui->statusBar->showMessage ("You can't take back your move if the game is over.");
		break;
	default:
		//ui->label->setPixmap(empty);
		this->ui->label_turn->setText ("Game Over");
		this->ui->label_2->clear ();
		//ui->label_3->clear();
		this->setStatusBar (event);
	}
}

void MainWindow::buttonPressHandle (const int &x)
{
	this->ui->pushButton->setEnabled ((bool)x);
	this->ui->pushButton_moveBack->setEnabled ((bool)x);
}

MainWindow::~MainWindow ()
{
	delete ui;
}

