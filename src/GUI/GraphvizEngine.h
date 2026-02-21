#pragma once

#include "Graph.h"
#include "Vertex.h"
#include "Edge.h"

#include <QMap>
#include <QPointF>
#include <QString>
#include <string>

struct Agraph_s;
struct GVC_s;
typedef struct Agraph_s Agraph_t;
typedef struct GVC_s GVC_t;

using GVGraph = Graph<Vertex, Edge>;

class GraphvizEngine {
public:
    enum class LayoutAlgorithm {
        DOT,
        NEATO
    };

    GraphvizEngine();
    ~GraphvizEngine();

    GraphvizEngine(const GraphvizEngine&) = delete;
    GraphvizEngine& operator=(const GraphvizEngine&) = delete;

    QMap<int, QPointF> layoutGraph(GVGraph* graph, LayoutAlgorithm algo);
    bool exportToFile(GVGraph* graph, const QString& filePath, const QString& format, LayoutAlgorithm algo);

    static LayoutAlgorithm getDefaultLayoutAlgorithm(bool directed);

private:
    Agraph_t* createGvGraph(GVGraph* graph);
    std::string getLayoutAlgoString(LayoutAlgorithm algo);
    GVC_t* m_gvc;
};