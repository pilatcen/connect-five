#include "board.h"



Board::Board (char *argv[], int argc, QPixmap *empty, QPixmap *circle, QPixmap *cross, QWidget *parent) : QWidget(parent)
{
	int i, j;
	this->server=NULL;
	this->client=NULL;

	if (argc==3 && (QString (argv[1])=="server")){
		gameType=TYPE_SERVER;
		game=0;

		server=new Server (MAX_X, MAX_Y, QString (argv[2]).toInt(), this);

		connect (server, SIGNAL(move(const int &, const int &)), this, SLOT(addItem_net (const int &, const int &)));
		connect (server, SIGNAL(connectionStatus (int)), this, SLOT(setGame (int)));
		connect (server, SIGNAL(NewGamePressed (int)), this, SLOT(setGame (int)));
		connect (server, SIGNAL(reset_net ()), this, SLOT(reset_net()));
		connect (server, SIGNAL(moveBack ()), this, SLOT(moveBack()));

		server->start();


		connect (this->server, SIGNAL (connectionStatus (int)), this, SLOT (setStatusBar (int)));//potrebuju ukazat na statusbaru ze se nekdo pripojil/odpojil atd
		connect (this->server, SIGNAL (buttonPressed (int)), this, SLOT (buttonPressHandle (int)));
		connect (this->server, SIGNAL (statusChanged (int)), this, SLOT (displayStatus (int)));



	}else if (argc==4 && (QString (argv[1])=="client")){

		QHostAddress hostname;

		if(!hostname.setAddress (QString (argv[2]))){
			QMessageBox::critical(NULL, "Invalid IP address", "You've entered an invalid IP address!");
			cout << "Unable to start the client, you've entered an invalid IP address!." << endl;
			exit (0);
		}

		gameType=TYPE_CLIENT;
		game=0;

		client=new Client (MAX_X, MAX_Y, hostname, QString (argv[3]).toInt(), this);

		connect (client, SIGNAL(move(const int &, const int &)), this, SLOT(addItem_net (const int &, const int &)));
		connect (client, SIGNAL(connectionStatus (int)), this, SLOT(setGame (int)));
		connect (client, SIGNAL(NewGamePressed (int)), this, SLOT(setGame (int)));
		connect (client, SIGNAL(reset_net ()), this, SLOT(reset_net()));
		connect (client, SIGNAL(moveBack ()), this, SLOT(moveBack ()));
		client->start();


		connect (this->client, SIGNAL (connectionStatus (int)), this, SLOT (setStatusBar (int)));//potrebuju ukazat na statusbaru ze se nekdo pripojil/odpojil atd
		connect (this->client, SIGNAL (buttonPressed (int)), this, SLOT (buttonPressHandle (int)));
		connect (this->client, SIGNAL (statusChanged (int)), this, SLOT (displayStatus (int)));

	}else{
		gameType=TYPE_LOCAL;
		game=1;
	}

	QGridLayout *layout=new QGridLayout (this);
	layout->setSpacing (0);//vzdalenost 1 pixel mezi policky

	items=new Item**[MAX_Y];//dynamicka alokace dvorozmerneho pole ve kterem jsou pointry (kvuli addWidget) na policka
	for (i=0;i<MAX_Y;i++){//vytvari matici
		layout->setRowMinimumHeight (i, 26);
		items[i]=new Item*[MAX_X];
		for (j=0;j<MAX_X;j++){
			items[i][j]=new Item (i, j, empty, this);
			layout->setColumnMinimumWidth(j, 26);
			layout->addWidget (items[i][j], i, j);
		}
	}
	this->activeType=Item::TYPE_CIRCLE;
	this->firstPlayer=Item::TYPE_CIRCLE;
	this->win=Item::TYPE_EMPTY;

	this->empty=empty;
	this->circle=circle;
	this->cross=cross;
	this->score[0]=0;
	this->score[1]=0;

	circleHighlighted.load(":/images/white_highlighted.png");
	crossHighlighted.load(":/images/black_highlighted.png");
}

void Board::unHighlight (void)//stara se o zruseni zvyrazneni posledniho tahu
{
	if (moves.size()>0){
		if (this->activeType==Item::TYPE_CIRCLE){
			items[moves[moves.size()-1].second][moves[moves.size()-1].first]->setPixmap(*cross);
		}else if (this->activeType==Item::TYPE_CROSS){
			items[moves[moves.size()-1].second][moves[moves.size()-1].first]->setPixmap(*circle);
		}
	}
}

void Board::addItem_net (const int &x, const int &y)
{
	if (x<MAX_X && y<MAX_Y && x>-1 && y>-1){
		if (server && ((int)items[y][x]->type==(int)Item::TYPE_EMPTY) && game && ((int)this->activeType==(int)TYPE_CLIENT)){
			addItem (x, y);
		}else if (client && ((int)items[y][x]->type==(int)Item::TYPE_EMPTY) && game && ((int)this->activeType==(int)TYPE_SERVER)){
			addItem (x, y);
		}
	}
}

void Board::addItem (int x, int y)//udelat dalsi metodu pro posilani souradnice ze site
{
	if (x<MAX_X && y<MAX_Y && x>-1 && y>-1){
		int i;
		Item::Type t=this->activeType;
		items[y][x]->type=t;

		unHighlight ();
		moves.append(QPair <int, int>(x, y));

		if (t==Item::TYPE_CIRCLE){
			items[y][x]->setPixmap(circleHighlighted);
			this->activeType=Item::TYPE_CROSS;
		}else{
			items[y][x]->setPixmap(crossHighlighted);
			this->activeType=Item::TYPE_CIRCLE;
		}
		
		emit statusChanged (activeType);

		//otestuje vyhru
		if (testwin (x, y) || moves.size()==MAX_ITEMS){
			game=0;
			win=t;
			score[(int)win-1]++;

			//zvyrazni petici
			if (win==Item::TYPE_CIRCLE){
				for (i=0;i<5;i++){
					items[pentad[i].second][pentad[i].first]->setPixmap(circleHighlighted);
				}
			}else{
				for (i=0;i<5;i++){
					items[pentad[i].second][pentad[i].first]->setPixmap(crossHighlighted);
				}
			}
			emit statusChanged(t+2);
		}

		//otestuje remizu
		if (moves.size()==MAX_ITEMS){
			game=0;
			win=t;
			score[0]++;
			score[1]++;
			emit statusChanged(t+4);
		}
	}
}

void Board::moveBack (void)
{
	if (!win){
		if (moves.size()>1){
			items[moves[moves.size()-1].second][moves[moves.size()-1].first]->setPixmap (*empty);
			items[moves[moves.size()-1].second][moves[moves.size()-1].first]->type=Item::TYPE_EMPTY;
			moves.removeLast ();

			if (activeType==Item::TYPE_CIRCLE){
				items[moves[moves.size()-1].second][moves[moves.size()-1].first]->setPixmap (circleHighlighted);
				this->activeType=Item::TYPE_CROSS;
			}else{
				items[moves[moves.size()-1].second][moves[moves.size()-1].first]->setPixmap (crossHighlighted);
				this->activeType=Item::TYPE_CIRCLE;
			}

			emit statusChanged (activeType);
			emit statusChanged (7);
		}
	}
}

void Board::moveBackClicked (void)
{
	if (win){
		emit statusChanged (11);
	}else{
		switch (gameType){

		case Board::TYPE_LOCAL:
			moveBack ();
			game=1;
			break;

		case Board::TYPE_SERVER:
				emit NewGamePressed (0);
				game=0;
				server->writeToClient ("300 200 200\n");//pozadavek na reset
				//QMessageBox::information (NULL, "Asking oponent.", "You asked your opponent to reset this game");
				emit statusChanged(10);
				return;
			break;

		case Board::TYPE_CLIENT:
				emit NewGamePressed (0);
				game=0;
				client->writeToServer ("300 200 200\n");//pozadavek na reset
				emit statusChanged(10);
				//QMessageBox::information (NULL, "Asking oponent.", "You asked your opponent to reset this game");
				return;
			break;
		}
	}
}

void Board::reset_net(void)
{
	for (int i=0; i< MAX_Y;i++){
		for (int j=0;j<MAX_X;j++){
			items[i][j]->clear ();
		}
	}
	game=1;
	if (win!=Item::TYPE_EMPTY){
		activeType=(Item::Type)(3-(int)this->firstPlayer);// 3 je pocet prvku v typename Item::Type
	}else{
		activeType=Item::TYPE_CIRCLE;
	}
	this->firstPlayer=activeType;
	this->win=Item::TYPE_EMPTY;
	moves.clear ();
	pentad.clear ();
	emit statusChanged(activeType);
	emit statusChanged(7);
}

void Board::reset (void)
{
	switch (gameType){

	case Board::TYPE_LOCAL:
		game=1;//protoze hra je ihned pripravena, nezavisi na serveru/klientu
		//emit statusChanged(activeType);
		break;

	case Board::TYPE_SERVER:
		if (win){
			game=1;//pri vyhre se game vynuluje..
			server->writeToClient ("400 400 400\n");//reset
		}else{
			emit NewGamePressed (0);
			game=0;
			server->writeToClient ("300 300 300\n");//pozadavek na reset
			emit statusChanged(10);
			//QMessageBox::information (NULL, "Asking oponent.", "You asked your opponent to reset this game");
			return;
		}
		break;

	case Board::TYPE_CLIENT:
		if (win){
			game=1;
			client->writeToServer ("400 400 400\n");//reset
		}else{
			emit NewGamePressed (0);
			game=0;
			client->writeToServer ("300 300 300\n");//pozadavek na reset
			emit statusChanged(7);
			//QMessageBox::information (NULL, "Asking oponent.", "You asked your opponent to reset this game");
			return;
		}
		break;
	}

	for (int i=0; i< MAX_Y;i++){
		for (int j=0;j<MAX_X;j++){
			items[i][j]->clear ();
		}
	}

	if (win!=Item::TYPE_EMPTY){//zajisteni stridani hracu po vyhre
		activeType=(Item::Type)(3-(int)this->firstPlayer);
	}else{
		activeType=Item::TYPE_CIRCLE;
	}

	if (game){
		emit statusChanged(activeType);
	}

	this->firstPlayer=activeType;
	this->win=Item::TYPE_EMPTY;
	moves.clear ();
	pentad.clear ();
	emit statusChanged(7);
}

void Board::setGame (int x)
{
	this->game=x;
}

bool Board::testwin (int x, int y)//kontroluje vyhru
{
	int i, count, itemRemainBottom, tmpX, tmpY;
	Item::Type searched=items[y][x]->type;

	//horizontalne
	for (i=0;i<MAX_X;i++){
		if (items[y][i]->type==searched){
			pentad.append(QPair <int, int>(i, y));
			if (pentad.size ()>=5){
				return 1;
			}
		}else{
			pentad.clear ();
		}
	}
	pentad.clear();

	//vertikalne
	for (i=0;i<MAX_Y;i++){
		if (items[i][x]->type==searched){
			pentad.append(QPair <int, int>(x, i));
			if (pentad.size ()>=5){
				return 1;
			}
		}else{
			pentad.clear ();
		}
	}
	pentad.clear();

	//diagonalne smerem doleva nahoru
	itemRemainBottom=((MAX_X-1-x) <= (MAX_Y-1-y)) ? (MAX_X-1-x) : (MAX_Y-1-y);//kolik policek je k nejblizsimu kraji sachovnice
	tmpX=x+itemRemainBottom;
	tmpY=y+itemRemainBottom;

	while ((tmpX>=0) && (tmpY>=0)){
		if (items[tmpY][tmpX]->type==searched){
			pentad.append(QPair <int, int>(tmpX, tmpY));
			if (pentad.size ()>=5){
				return 1;
			}
		}else{
			pentad.clear ();
		}
		tmpX--;
		tmpY--;
	}
	pentad.clear();

	//diagonalne smerem doprava nahoru
	itemRemainBottom=((x) <= (MAX_Y-1-y)) ? (x) : (MAX_Y-1-y);
	tmpX=x-itemRemainBottom;
	tmpY=y+itemRemainBottom;
	count=0;

	while ((tmpX<MAX_X) && (tmpY>=0)){
		if (items[tmpY][tmpX]->type==searched){
			pentad.append(QPair <int, int>(tmpX, tmpY));
			if (pentad.size ()>=5){
				return 1;
			}
		}else{
			pentad.clear ();
		}
		tmpX++;
		tmpY--;
	}
	pentad.clear();

	//nikdo nevyhral
	return 0;
}


Board::~Board ()
{
	int i;
	for (i=0;i<MAX_Y;i++){
		delete [] items[i];
	}
	delete [] items;
}
