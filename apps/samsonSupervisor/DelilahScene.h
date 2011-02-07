#ifndef DELILAH_SCENE_H
#define DELILAH_SCENE_H

/* ****************************************************************************
*
* FILE                     DelilahScene.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QObject>
#include <QGraphicsScene>

class QGraphicsItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QGraphicsSceneContextMenuEvent;
class QAction;
class DelilahQueue;



/* ****************************************************************************
*
* DelilahScene - 
*/
class DelilahScene : public QGraphicsScene
{
	Q_OBJECT

public:
	enum Mode { InsertItem, InsertLine, InsertText, MoveItem };

	DelilahScene(QObject *parent = 0);

	void           qCreate(void);
	void           source(void);
	void           result(void);
	void           connection(void);
	DelilahQueue*  lookup(QGraphicsItem* gItemP);
	void           setCursor(const char* cursor);

	QAction* disableAction;
private:
	QAction* removeAction;
	QAction* chainRemoveAction;
	QAction* renameAction;
	QAction* configAction;
	QAction* bindAction;
	QAction* checkAction;
	QAction* executeAction;
	QAction* emptyAction;
	QAction* clearAction;
	QAction* queueAddAction;
	QAction* aboutAction;

	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent);
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* contextMenuEvent);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent);

public slots:
	void remove(void);
	void chainRemove(void);
	void removeFromMenu(void);
	void config(void);
	void bind();
	void check();
	void execute();
	void disable();
	void empty();
	void rename();
	void clear(void);
	void queueAdd(void);
	void about(void);
};

#endif
