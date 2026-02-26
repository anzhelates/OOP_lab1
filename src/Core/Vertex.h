#pragma once
#include <string>

namespace Core {

    class Vertex {
    private:
        int m_id = -1;
        std::string m_name;
        bool m_active = true;

    public:
        Vertex() = default;
        explicit Vertex(const std::string& name) : m_name(name) {}
        virtual ~Vertex() = default;

        int getId() const { return m_id; }
        void setId(int id) { m_id = id; }

        const std::string& getName() const { return m_name; }
        void setName(const std::string& name) { m_name = name; }

        bool isActive() const { return m_active; }
        void markInactive() { m_active = false; }
        void markActive() { m_active = true; }
    };
}