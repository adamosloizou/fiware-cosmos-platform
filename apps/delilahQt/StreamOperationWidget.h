#include <QtGui>
#include <sstream>

#include "au/RRT.h"
#include "au/string.h"

#include <KDChartChart>
#include <KDChartBarDiagram>
#include <KDChartLineDiagram>
#include <KDChartPieDiagram>

#include "Plot.h"
#include "UpdateElements.h"  // delilaQTElement

#include "WorkerDataController.h"

#include "ui_StreamOperationWidget.h"


class StreamOperationWidget : public QWidget , public Ui_StreamOperationWidget , DelilahQTElement
{
    
    std::string queue_name;                         // Name of the queue we are monitoring
    WorkerDataController worker_data_controller; // Controller to keep updated values from all workers
    
    // Plot memory-disk
    QStandardItemModel m_model_disk_memory;
    KDChart::Chart m_chart_disk_memory;
    
    Plot *plot_size;
    Plot *plot_kvs;
    Plot *plot_num_operations;
    
    Q_OBJECT
    
public:
    
    StreamOperationWidget(QWidget *parent = 0);

	// Update with the top level element in XML monitorization
	void update( au::TreeItem *item , bool complete_update );

    
    private slots:
    
	void change_queue();
    
    
private:
    
    void setup();
    
};

