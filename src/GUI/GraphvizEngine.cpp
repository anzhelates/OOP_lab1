#include "GraphvizEngine.h"
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <QDebug>
#include <vector>

static const double GV_TO_QT_SCALE = 1.0;

GraphvizEngine::GraphvizEngine() {
    m_gvc = gvContext();
    if (!m_gvc) {
        qWarning() << "Failed to initialize Graphviz context.";
    }
}

GraphvizEngine::~GraphvizEngine() {
    if (m_gvc) {
        gvFreeContext(m_gvc);
    }
}

std::string GraphvizEngine::getLayoutAlgoString(LayoutAlgorithm algo) {
    switch (algo) {
        case LayoutAlgorithm::DOT:   return "dot";
        case LayoutAlgorithm::NEATO: return "neato";
    }
    return "dot";
}

GraphvizEngine::LayoutAlgorithm GraphvizEngine::getDefaultLayoutAlgorithm(bool directed) {
    return directed ? LayoutAlgorithm::DOT : LayoutAlgorithm::NEATO;
}

Agraph_t* GraphvizEngine::createGvGraph(GVGraph* graph) {
    if (!graph) return nullptr;

    Agraph_t* g = agopen(const_cast<char*>("G"),
                         graph->isDirected() ? Agdirected : Agundirected,
                         nullptr);

    agattr(g, AGNODE, const_cast<char*>("shape"), const_cast<char*>("circle"));
    agattr(g, AGNODE, const_cast<char*>("fixedsize"), const_cast<char*>("true"));
    agattr(g, AGNODE, const_cast<char*>("width"), const_cast<char*>("0.5"));
    agattr(g, AGNODE, const_cast<char*>("style"), const_cast<char*>("filled"));
    agattr(g, AGNODE, const_cast<char*>("fillcolor"), const_cast<char*>("cyan"));

    agattr(g, AGEDGE, const_cast<char*>("color"), const_cast<char*>("black"));

    QMap<int, Agnode_t*> vertexMap;

    for (Vertex* v : graph->getVertices()) {
        if (v && v->isActive()) {
            std::string s_id = std::to_string(v->getId());
            Agnode_t* n = agnode(g, const_cast<char*>(s_id.c_str()), 1);

            std::string s_name = v->getName();
            agsafeset(n, const_cast<char*>("label"), const_cast<char*>(s_name.c_str()), const_cast<char*>(""));

            vertexMap[v->getId()] = n;
        }
    }

    for (Edge* e : graph->getEdges()) {
        if (e && e->isActive()) {
            Agnode_t* src = vertexMap.value(e->getFrom(), nullptr);
            Agnode_t* dst = vertexMap.value(e->getTo(), nullptr);

            if (src && dst) {
                Agedge_t* edge = agedge(g, src, dst, nullptr, 1);

                if (graph->isWeighted()) {
                    std::string s_weight = QString::number(e->getWeight(), 'f', 1).toStdString();
                    agsafeset(edge, const_cast<char*>("label"), const_cast<char*>(s_weight.c_str()), const_cast<char*>(""));
                }
            }
        }
    }
    return g;
}

QMap<int, QPointF> GraphvizEngine::layoutGraph(GVGraph* graph, LayoutAlgorithm algo) {
    QMap<int, QPointF> positions;
    if (!m_gvc || !graph) return positions;

    Agraph_t* g = createGvGraph(graph);
    if (!g) return positions;

    std::string algoStr = getLayoutAlgoString(algo);

    if (gvLayout(m_gvc, g, algoStr.c_str()) != 0) {
        qWarning() << "Graphviz layout failed.";
        agclose(g);
        return positions;
    }

    for (Agnode_t* n = agfstnode(g); n; n = agnxtnode(g, n)) {
        try {
            double x = ND_coord(n).x;
            double y = ND_coord(n).y;
            QPointF pos(x * GV_TO_QT_SCALE, -y * GV_TO_QT_SCALE);

            int id = std::stoi(agnameof(n));
            positions[id] = pos;
        } catch (...) {
        }
    }

    gvFreeLayout(m_gvc, g);
    agclose(g);
    return positions;
}

bool GraphvizEngine::exportToFile(GVGraph* graph, const QString& filePath, const QString& format, LayoutAlgorithm algo) {
    if (!m_gvc || !graph) return false;

    Agraph_t* g = createGvGraph(graph);
    if (!g) return false;

    std::string algoStr = getLayoutAlgoString(algo);

    if (gvLayout(m_gvc, g, algoStr.c_str()) != 0) {
        agclose(g);
        return false;
    }

    int res = gvRenderFilename(m_gvc, g, format.toLatin1().constData(), filePath.toUtf8().constData());

    gvFreeLayout(m_gvc, g);
    agclose(g);

    return (res == 0);
}