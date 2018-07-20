#include "progressitem.h"
#include "ui_progressitem.h"

ProgressItem::ProgressItem(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ProgressItem)
{
	ui->setupUi(this);
}

ProgressItem::~ProgressItem()
{
	delete ui;
}
