#ifndef _H_QueueContainer
#define _H_QueueContainer

/* ****************************************************************************
*
* FILE                     QueueContainer.h
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/

#include "QueueViewer.h"

#include <QtGui>
#include <vector>
#include <string>

namespace samson {
        
class QueueContainer: public QWidget
{
        Q_OBJECT
        
        QHBoxLayout* filterLayout;
        QLabel* filterLabel;
        QLineEdit* filterValue;
        
        QLabel* noInputLabel;
        QLabel* noOutputLabel;
        QLabel* noTotalLabel;
        QGridLayout* inputLayout;
        QGridLayout* outputLayout;
        QGridLayout* totalLayout;
        
        std::vector<QueueData*> queuesData;
        //std::vector<QueueViewer::QueueData> queuesData;
        //std::vector<std::string> current_queues;
        std::vector<QueueViewer*> in_queues;
        std::vector<QueueViewer*> out_queues;
        QueueViewer* totalQueues;

        QGroupBox* inputBox;
        QGroupBox* outputBox;
        QGroupBox* totalBox;
                
    public:
        
        QueueContainer(QWidget* parent = 0);
        
        void setData(std::vector<QueueData*> queuesData);

    private:
        
        std::vector<QueueViewer*> quequeViewers;
        QVBoxLayout* mainLayout;
        QueueViewer* findQueue(std::vector<QueueViewer*>& list, std::string name_id);
        QueueData* findQueue(std::vector<QueueData*>& list, std::string name_id);
        std::vector<QueueViewer*> getDeletedQueues(std::vector<QueueData*>& QueuesData);
        void regenerateQueues();

    public slots:
        void onQueueDetailsClicked();
        void onFilterEdited();
    
    signals:
    void queueDetailsClicked(QueueViewer* queue);
    void queueHasChanged(QueueViewer* queue, QueueData* data);
    void queueDeleted(QueueViewer* queue);

};

} //namespace

#endif //_H_QueueContainer
