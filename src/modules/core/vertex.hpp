//
// Created by dte on 10/22/2025.
//

#pragma once
#include <list>
#include <memory>
#include <string>
#include <chrono>

/**
 * @namespace geoma::core
 * @brief Fundamental building blocks of geoma: vertices, search steps,
 *        the navigator and its budget counters.
 */
namespace geoma::core
{
    /** @brief Alias kept for readability; the project's string type. */
    using string = std::string;

    /**
     * @brief A single point in 2D space that can be linked to other vertices.
     *
     * A vertex is both a geometric point and a node of a graph: the
     * @p neighbors list holds every vertex it is connected to. A collection
     * of vertices with their connections is referred to throughout the
     * project as a "world" (`std::list<std::shared_ptr<vertex>>`).
     *
     * Vertices are always handled through @c std::shared_ptr; use the static
     * helpers below instead of touching neighbors directly.
     */
    struct vertex
    {
        /** @brief Horizontal coordinate (longitude in geographic data). */
        double x;
        /** @brief Vertical coordinate (latitude in geographic data). */
        double y;
        // double z;

        /** @brief Vertices connected to this one (outgoing edges). */
        std::list<std::shared_ptr<vertex>> neighbors;

        /**
         * @brief Converts the vertex to a human-readable string.
         * @return A string of the form @c "{ x = <x>, y = <y> }" with full
         *         precision (14 significant decimals).
         */
        explicit operator string() const;

        std::string to_simple_string() const;

        /**
         * @brief Computes the Euclidean distance between two vertices.
         * @param va First vertex.
         * @param vb Second vertex.
         * @return The straight-line distance @f$\sqrt{(x_a-x_b)^2 + (y_a-y_b)^2}@f$.
         */
        static double distance(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb);

        /**
         * @brief Factory: allocates a new vertex on the heap.
         * @param x Horizontal coordinate.
         * @param y Vertical coordinate.
         * @return A shared pointer to the freshly created vertex.
         */
        static std::shared_ptr<vertex> make(double x, double y);

        /**
         * @brief Connects two vertices by adding @p vb to @p va's neighbors.
         *
         * @note The link is <b>one-directional</b>. To create a road-like,
         *       two-way edge, call @c join(a, b) and @c join(b, a).
         *       No duplicate check is performed; use @c decouple() to clean
         *       up repeated links.
         *
         * @param va Source vertex (its @p neighbors list grows).
         * @param vb Destination vertex.
         */
        static void join(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb);

        /**
         * @brief Checks whether two vertices are directly connected.
         * @param va Suspected source vertex.
         * @param vb Suspected destination vertex.
         * @return True if @p vb appears in @p va's neighbors list.
         */
        static bool joined(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb);

        /**
         * @brief Compares two vertices by their coordinates.
         * @param va First vertex.
         * @param vb Second vertex.
         * @return True if both @p x and @p y are exactly equal.
         * @note This is an exact floating-point comparison, not an
         *       epsilon-based one; vertices built from different sources
         *       may look "the same" without comparing equal.
         */
        static bool equals(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb);

        /**
         * @brief Loose "less-than" ordering between two vertices.
         * @param va Left-hand side.
         * @param vb Right-hand side.
         * @return True if @p va's @p x is smaller, or its @p y is smaller.
         * @note This is not a strict weak ordering (it compares either
         *       coordinate); suitable for ad-hoc sorting only.
         */
        static bool lower(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb);

        /**
         * @brief Loose "greater-than" ordering between two vertices.
         * @param va Left-hand side.
         * @param vb Right-hand side.
         * @return True if @p va's @p x is greater, or its @p y is greater.
         * @note See the caveat on @c lower().
         */
        static bool greater(const std::shared_ptr<vertex>& va, const std::shared_ptr<vertex>& vb);

        /**
         * @brief Finds the vertex of @p world nearest to @p v.
         * @param v Reference vertex.
         * @param world Candidate vertices.
         * @return The closest vertex, or @c nullptr when @p world is empty.
         * @note Performs a linear scan — O(n) per call.
         */
        static std::shared_ptr<vertex> closest(const std::shared_ptr<vertex>& v,
                                               const std::list<std::shared_ptr<vertex>>& world);
    };
}