#include "rope.h"
#include <sstream>

namespace ip2
{
	struct RopeNode
	{
		std::string data; ///< Non-empty only for leaf nodes.
		size_t weight;	  ///< Leaf: data.size().  Internal: length of left subtree.
		RopeNode *left;
		RopeNode *right;

		/** Construct a leaf node from a string fragment. */
		explicit RopeNode(const std::string &str)
			: data(str), weight(str.size()), left(nullptr), right(nullptr)
		{
		}

		/** Construct an internal node with the given children and left-subtree weight. */
		RopeNode(RopeNode *l, RopeNode *r, size_t leftWeight)
			: data(), weight(leftWeight), left(l), right(r)
		{
		}

		bool isLeaf() const
		{
			return left == nullptr && right == nullptr;
		}
	};

	class Rope::Impl
	{
	public:
		static size_t objectCount;			   ///< Number of live Impl (and therefore Rope) objects.
		static const size_t MAX_LEAF = 16; ///< Maximum characters stored in a single leaf node.

		RopeNode *root; ///< Root of the binary tree; nullptr for an empty rope.

		Impl()
			: root(nullptr)
		{
			++objectCount;
		}

		explicit Impl(const std::string &str)
			: root(str.empty() ? nullptr : build(str, 0, str.size()))
		{
			++objectCount;
		}

		Impl(const Impl &other)
			: root(deepCopy(other.root))
		{
			++objectCount;
		}

		~Impl()
		{
			destroyTree(root);
			--objectCount;
		}

		/**
		 * Build a balanced rope tree from the substring s[start, end).
		 * Recursively halves the range until each fragment fits in a leaf.
		 */
		static RopeNode *build(const std::string &s, size_t start, size_t end)
		{
			size_t len = end - start;
			if (len == 0)
				return nullptr;
			if (len <= MAX_LEAF)
				return new RopeNode(s.substr(start, len));

			size_t mid = start + len / 2;
			RopeNode *l = build(s, start, mid);
			RopeNode *r = build(s, mid, end);
			return new RopeNode(l, r, mid - start);
		}

		/** Deep-copy a subtree; returns nullptr for nullptr input. */
		static RopeNode *deepCopy(const RopeNode *node)
		{
			if (!node)
				return nullptr;
			if (node->isLeaf())
				return new RopeNode(node->data);

			RopeNode *n = new RopeNode(nullptr, nullptr, node->weight);
			n->left = deepCopy(node->left);
			n->right = deepCopy(node->right);
			return n;
		}

		/** Post-order delete of an entire subtree. */
		static void destroyTree(RopeNode *node)
		{
			if (!node)
				return;
			destroyTree(node->left);
			destroyTree(node->right);
			delete node;
		}

		/**
		 * Compute the total character count of a subtree.
		 * For a leaf: weight == data.size(), so return weight directly.
		 * For an internal node: weight == left-subtree length,
		 * so total = weight + treeLength(right).
		 * Traverses only the rightmost spine: O(log n) on a balanced tree.
		 */
		static size_t treeLength(const RopeNode *node)
		{
			if (!node)
				return 0;
			if (node->isLeaf())
				return node->weight;
			return node->weight + treeLength(node->right);
		}

		/** Append all leaf data left-to-right into out. */
		static void flattenInto(const RopeNode *node, std::string &out)
		{
			if (!node)
				return;
			if (node->isLeaf())
			{
				out += node->data;
				return;
			}
			flattenInto(node->left, out);
			flattenInto(node->right, out);
		}

		/** Return the full logical string by concatenating all leaves left-to-right. */
		std::string flatten() const
		{
			std::string result;
			result.reserve(treeLength(root));
			flattenInto(root, result);
			return result;
		}

		/** Destroy the current tree and rebuild a balanced one from s. */
		void rebuild(const std::string &s)
		{
			destroyTree(root);
			root = s.empty() ? nullptr : build(s, 0, s.size());
		}

		/** Return the total number of nodes in a subtree. */
		static int countNodes(const RopeNode *node)
		{
			if (node)
				return 1 + countNodes(node->left) + countNodes(node->right);
			else
				return 0;
		}
	};

	size_t Rope::Impl::objectCount = 0;

	Rope::Rope()
		: pImpl(new Impl())
	{
	}

	Rope::Rope(const std::string &str)
		: pImpl(new Impl(str))
	{
	}

	Rope::Rope(const Rope &other)
		: pImpl(new Impl(*other.pImpl))
	{
	}

	Rope::~Rope()
	{
		delete pImpl;
	}

	size_t Rope::getObjectCount()
	{
		return static_cast<size_t>(Rope::Impl::objectCount);
	}

	Rope &Rope::operator=(const Rope &other)
	{
		if (this != &other)
		{
			delete pImpl;
			pImpl = new Impl(*other.pImpl);
		}
		return *this;
	}

	Rope &Rope::operator+=(const std::string &str)
	{
		std::string s = pImpl->flatten();
		s += str;
		pImpl->rebuild(s);
		return *this;
	}

	Rope &Rope::operator+=(const Rope &other)
	{
		return *this += other.flatten();
	}

	Rope &Rope::operator-=(const std::pair<size_t, size_t> &range)
	{
		size_t pos = range.first;
		size_t len = range.second;
		size_t total = length();

		if (pos > total)
		{
			throw std::out_of_range(
				"Rope::operator-=: position " + std::to_string(pos) +
				" exceeds length " + std::to_string(total));
		}
		if (pos + len > total)
		{
			throw std::out_of_range(
				"Rope::operator-=: range [" + std::to_string(pos) + ", " +
				std::to_string(pos + len) + ") exceeds length " +
				std::to_string(total));
		}

		std::string s = pImpl->flatten();
		s.erase(pos, len);
		pImpl->rebuild(s);
		return *this;
	}

	Rope &Rope::operator%=(const std::pair<size_t, std::string> &edit)
	{
		size_t pos = edit.first;
		const std::string &replacement = edit.second;
		size_t total = length();

		if (empty())
		{
			throw RopeException("Rope::operator%=: cannot edit an empty Rope");
		}
		if (pos >= total)
		{
			throw std::out_of_range(
				"Rope::operator%=: position " + std::to_string(pos) +
				" out of range (length=" + std::to_string(total) + ")");
		}
		if (pos + replacement.size() > total)
		{
			throw RopeException(
				"Rope::operator%=: replacement of length " +
				std::to_string(replacement.size()) + " starting at " +
				std::to_string(pos) + " would exceed rope length " +
				std::to_string(total));
		}

		std::string s = pImpl->flatten();
		s.replace(pos, replacement.size(), replacement);
		pImpl->rebuild(s);
		return *this;
	}

	Rope Rope::operator+(const Rope &other) const
	{
		Rope result(*this);
		result += other;
		return result;
	}

	Rope Rope::operator+(const std::string &str) const
	{
		Rope result(*this);
		result += str;
		return result;
	}

	bool Rope::operator==(const Rope &other) const
	{
		return flatten() == other.flatten();
	}

	bool Rope::operator!=(const Rope &other) const
	{
		return !(*this == other);
	}

	bool Rope::operator<(const Rope &other) const
	{
		return flatten() < other.flatten();
	}

	bool Rope::operator<=(const Rope &other) const
	{
		return !(*this > other);
	}

	bool Rope::operator>(const Rope &other) const
	{
		return other < *this;
	}

	bool Rope::operator>=(const Rope &other) const
	{
		return !(*this < other);
	}

	Rope &Rope::operator!()
	{
		pImpl->rebuild("");
		return *this;
	}

	int Rope::operator[](const std::string &value) const
	{
		if (value.empty())
		{
			throw std::invalid_argument("Rope::operator[]: search value must not be empty");
		}

		std::string s = flatten();
		size_t pos = s.find(value);
		return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
	}

	std::string Rope::flatten() const
	{
		return pImpl->flatten();
	}

	std::string Rope::toString() const
	{
		static const size_t MAX_PREVIEW = 30; ///< Maximum characters shown in the preview.

		size_t len = length();
		int nodes = Impl::countNodes(pImpl->root);
		std::string full = pImpl->flatten();

		std::string preview;
		size_t limit = (full.size() < MAX_PREVIEW) ? full.size() : MAX_PREVIEW;
		for (size_t i = 0; i < limit; ++i)
		{
			if (full[i] == '\n')
				preview += "\\n";
			else
				preview += full[i];
		}
		if (full.size() > MAX_PREVIEW)
			preview += "...";

		std::ostringstream oss;
		oss << nodes << "," << len << ":" << preview;
		return oss.str();
	}

	size_t Rope::length() const
	{
		return Impl::treeLength(pImpl->root);
	}

	bool Rope::empty() const
	{
		return pImpl->root == nullptr;
	}

} // namespace ip2