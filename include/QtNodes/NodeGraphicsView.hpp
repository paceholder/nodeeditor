#pragma once

#include <QMenu>
#include "QtNodes/GraphicsView"
#include "QtNodes/BasicGraphicsScene"
#include "QtNodes/DataFlowGraphModel"

namespace nitro {

    class NodeGraphicsView : public QtNodes::GraphicsView {
    public:

        NodeGraphicsView(QtNodes::BasicGraphicsScene *scene,
                         QtNodes::DataFlowGraphModel *model,
                         QWidget *parent);


        virtual QMenu *initNodeMenu() = 0;

    public:
        [[nodiscard]] QtNodes::DataFlowGraphModel *getDataModel() const;

        [[nodiscard]] QtNodes::BasicGraphicsScene *getScene() const;

        QMenu *getNodeMenu();

    protected:
        QtNodes::DataFlowGraphModel *dataModel_;
        QtNodes::BasicGraphicsScene *scene_ = nullptr;


    private:
        QMenu *nodeMenu_ = nullptr;
        void spawnNodeMenu();
    };

} // nitro
