/**
* @file Vertex.h
 * @brief Represents a node in a graph.
 */

#pragma once
#include <string>

namespace Core {

    /**
     * @brief Represents a node in a graph.
     */
    class Vertex {
    private:
        int m_id = -1;           ///< The unique identifier of the vertex.
        std::string m_name;      ///< The display name or label of the vertex.
        bool m_active = true;    ///< Flag indicating whether the vertex is active (exists in the graph).

    public:
        /**
         * @brief Default constructor.
         */
        Vertex() = default;

        /**
         * @brief Constructs a vertex with a specific name.
         * @param name The name of the vertex.
         */
        explicit Vertex(const std::string& name) : m_name(name) {}

        /**
         * @brief Virtual destructor.
         */
        virtual ~Vertex() = default;

        /**
         * @brief Gets the ID of the vertex.
         * @return The unique identifier.
         */
        int getId() const { return m_id; }

        /**
         * @brief Sets the ID of the vertex.
         * @param id The new identifier to set.
         */
        void setId(int id) { m_id = id; }

        /**
         * @brief Gets the name of the vertex.
         * @return A constant reference to the name string.
         */
        const std::string& getName() const { return m_name; }

        /**
         * @brief Sets the name of the vertex.
         * @param name The new name to set.
         */
        void setName(const std::string& name) { m_name = name; }

        /**
         * @brief Checks if the vertex is active.
         * @return True if active, false otherwise.
         */
        bool isActive() const { return m_active; }

        /**
         * @brief Marks the vertex as inactive (logically removed).
         */
        void markInactive() { m_active = false; }

        /**
         * @brief Marks the vertex as active.
         */
        void markActive() { m_active = true; }
    };
}