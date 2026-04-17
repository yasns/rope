/**
 * @brief Rope ADT
 * @author Matas Jasiūnas
 *
 * A Rope is a binary tree whose leaves hold fixed string fragments.
 * @see https://en.wikipedia.org/wiki/Rope_(data_structure)
 *
 * In theory, concatenation, splitting, and indexing are O(log n). However, 
 * this implementation uses a flatten/rebuild approach for simplicity, so 
 * modifying operations are O(n) in practice.
 *
 * Implementation developed with assistance from AI tools (Claude Sonnet 4.6).
 *
 * Namespace: ip2
 */

#ifndef ROPE_H
#define ROPE_H

#include <string>
#include <stdexcept>
#include <utility>

namespace Ip2
{

    /**
     * Thrown for Rope-specific error conditions (e.g. editing an empty rope, 
     * replacement overflow)
     */
    class RopeException : public std::runtime_error
    {
    public:
        explicit RopeException(const std::string &message);
    };

    class Rope
    {
    private:
        class Impl;  ///< Forward declaration of the pImpl implementation class.
        Impl *pImpl; ///< Pointer to internal implementation (pImpl idiom).

    public:
        /** Construct an empty Rope. */
        Rope();

        /** Construct a Rope pre-loaded with str. */
        explicit Rope(const std::string &str);

        /** Deep-copy constructor. */
        Rope(const Rope &other);

        /** Deep-copy assignment. */
        Rope &operator=(const Rope &other);

        /** Destroys the internal tree and releases all memory. */
        ~Rope();

        /** Returns the number of currently live Rope objects. Should be 0 at program end. */
        static size_t getObjectCount();

        /** Append str to the end of this rope. */
        Rope &operator+=(const std::string &str);

        /** Append another rope to the end of this rope. */
        Rope &operator+=(const Rope &other);

        /**
         * Delete characters in range [range.first, range.first + range.second).
         * @throws std::out_of_range  if position or derived end exceeds length().
         */
        Rope &operator-=(const std::pair<size_t, size_t> &range);

        /**
         * Replace edit.second.size() characters starting at edit.first
         * with the string edit.second (same-length in-place edit).
         * @throws RopeException        if the rope is empty.
         * @throws std::out_of_range    if edit.first >= length().
         * @throws RopeException        if the replacement would extend beyond length().
         */
        Rope &operator%=(const std::pair<size_t, std::string> &edit);

        /** Return a new Rope that is the concatenation of *this and other. */
        Rope operator+(const Rope &other) const;

        /** Return a new Rope that is the concatenation of *this and str. */
        Rope operator+(const std::string &str) const;

        bool operator==(const Rope &other) const; ///< Lexicographic equality.
        bool operator!=(const Rope &other) const; ///< Lexicographic inequality.
        bool operator<(const Rope &other) const;  ///< Lexicographic less-than.
        bool operator<=(const Rope &other) const; ///< Lexicographic less-than-or-equal.
        bool operator>(const Rope &other) const;  ///< Lexicographic greater-than.
        bool operator>=(const Rope &other) const; ///< Lexicographic greater-than-or-equal.

        /**
         * Clear all content and return *this.
         * @warning Returns Rope&, not bool. Do not use in a boolean context —
         *          the result is always a non-null reference, which evaluates to true.
         */
        Rope &operator!();

        /**
         * Return the zero-based index of the first occurrence of value,
         * or -1 if value is not found.
         * @throws std::invalid_argument  if value is empty.
         */
        int operator[](const std::string &value) const;

        /** Concatenate all leaves left-to-right into the full logical string. */
        std::string flatten() const;

        /** Summary: nodes,length:prefix...}. */
        std::string toString() const;

        /** Return the total number of characters stored in this rope. */
        size_t length() const;

        /** Return true if this rope contains no characters. */
        bool empty() const;
    };

} // namespace ip2

#endif