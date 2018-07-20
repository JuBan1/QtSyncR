#ifndef PROGRESSITEM_H
#define PROGRESSITEM_H

#include <QWidget>

namespace Ui {
class ProgressItem;
}

class ProgressItem : public QWidget
{
	Q_OBJECT

public:
	explicit ProgressItem(QWidget *parent = 0);
	~ProgressItem();

private:
	Ui::ProgressItem *ui;
};

#endif // PROGRESSITEM_H
