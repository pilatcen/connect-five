#include "item.h"
#include "board.h"

Item::Item(int y, int x, QPixmap *empty, Board *parent)
{

	setPalette(QPalette (QColor ("#d0a343")));
	setAutoFillBackground(true);

	this->x=x;
	this->y=y;
	this->empty=empty;
	this->type=TYPE_EMPTY;
	this->parentPtr=parent;
	this->setPixmap(*empty);
	//this->setScaledContents(1);
	this->setMinimumSize ((*empty).width(), (*empty).height());

}

void Item::mousePressEvent (QMouseEvent *)
{
	if ((this->type==TYPE_EMPTY) && parentPtr->game){


		switch (parentPtr->gameType){
		case Board::TYPE_LOCAL:
			parentPtr->addItem (this->x, this->y);
			break;

		case Board::TYPE_SERVER:
			if ((int)parentPtr->activeType==(int)Board::TYPE_SERVER){
				parentPtr->server->writeToClient("200 "+QString::number(this->x)+" "+QString::number(this->y)+"\n");
				parentPtr->addItem (this->x, this->y);
			}
			break;

		case Board::TYPE_CLIENT:
			if ((int)parentPtr->activeType==(int)Board::TYPE_CLIENT){
				parentPtr->client->writeToServer ("200 "+QString::number(this->x)+" "+QString::number(this->y)+"\n");
				parentPtr->addItem (this->x, this->y);
			}
			break;
		}
	}
}

void Item::clear (void){
	this->type=TYPE_EMPTY;
	this->setPixmap(*empty);
}