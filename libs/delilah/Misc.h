/*
 * FILE:		Misc.h
 *
 * AUTHOR:		ania
 *
 * DESCRIPTION:
 *
 */

#ifndef MISC_H_
#define MISC_H_

#include <QAction>

class ActionWithPos : public QAction
{
	Q_OBJECT

public:
	ActionWithPos(QObject* parent) : QAction(parent)
		{ connect(this, SIGNAL(triggered()), this, SLOT(triggerAtPos()));};
	ActionWithPos(const QString & text, QObject* parent) : QAction(text, parent)
		{ connect(this, SIGNAL(triggered()), this, SLOT(triggerAtPos())); };
	~ActionWithPos() {};

	void setPosition(QPointF p) { pos=p; };
	QPointF position() { return pos; };

protected slots:
	void triggerAtPos() { emit(triggered(pos)); };

signals:
	void triggered(QPointF);

private:
	QPointF pos;
};


#endif /* MISC_H_ */
