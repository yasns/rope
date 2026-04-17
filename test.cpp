#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "rope.h"

/** Outputs to stdout and a log file simultaneously */
class Logger
{
public:
    explicit Logger(const std::string &filename)
        : file(filename)
    {
    }

    template <typename T>
    Logger &operator<<(const T &value)
    {
        std::cout << value;
        file << value;
        return *this;
    }

    /** Support stream manipulators such as std::endl */
    Logger &operator<<(std::ostream &(*manip)(std::ostream &))
    {
        manip(std::cout);
        manip(file);
        return *this;
    }

private:
    std::ofstream file;
};

static int passed = 0;
static int failed = 0;

static void report(Logger &log, const std::string &name, bool condition, const std::string &detail)
{
    std::string verdict = condition ? "PASS" : "FAIL";
    log << "  [" << verdict << "] " << name << "\n";
    log << "         " << detail << "\n\n";
    if (condition)
        ++passed;
    else
        ++failed;
}

static void runTests(Logger &log)
{
    log << "Starting tests...\n\n";

    {
        Ip2::Rope r;
        report(log, "T01  Default construction",
               r.empty() && r.length() == 0,
               "empty()=true, length()=0");
    }

    {
        Ip2::Rope r("hello");
        report(log, "T02  String construction",
               r.flatten() == "hello" && r.length() == 5,
               "flatten()=\"hello\", length()=5");
    }

    {
        Ip2::Rope original("abc");
        Ip2::Rope copy(original);
        original += "XYZ";
        report(log, "T03  Copy constructor — deep copy",
               copy.flatten() == "abc",
               "copy.flatten()=\"abc\" after original modified");
    }

    {
        Ip2::Rope src("hello");
        Ip2::Rope dst;
        dst = src;
        src += "!";
        report(log, "T04  Copy assignment — deep copy",
               dst.flatten() == "hello",
               "dst.flatten()=\"hello\" after src modified");
    }

    {
        Ip2::Rope r("foo");
        r += "bar";
        report(log, "T05  operator+= (string append)",
               r.flatten() == "foobar",
               "\"foo\" += \"bar\" => \"foobar\"");
    }

    {
        Ip2::Rope r("Hello");
        Ip2::Rope tail(", world");
        r += tail;
        report(log, "T06  operator+= (Rope append)",
               r.flatten() == "Hello, world",
               "\"Hello\" += \", world\" => \"Hello, world\"");
    }

    {
        Ip2::Rope r("Hello, cruel world!");
        r -= {7, 6}; // remove "cruel "
        report(log, "T07  operator-= (delete range {7,6})",
               r.flatten() == "Hello, world!",
               "result: \"Hello, world!\"");
    }

    {
        Ip2::Rope r("Hello, world!");
        r %= {7, "WORLD"};
        report(log, "T08  operator%= (edit at pos 7)",
               r.flatten() == "Hello, WORLD!",
               "result: \"Hello, WORLD!\"");
    }

    {
        Ip2::Rope a("foo");
        Ip2::Rope b("bar");
        Ip2::Rope c = a + b;
        report(log, "T09  operator+ (Rope + Rope)",
               c.flatten() == "foobar" &&
                   a.flatten() == "foo" &&
                   b.flatten() == "bar",
               "c=\"foobar\", a and b unchanged");
    }

    {
        Ip2::Rope a("Hello");
        Ip2::Rope b = a + std::string(" world");
        report(log, "T10  operator+ (Rope + string)",
               b.flatten() == "Hello world" && a.flatten() == "Hello",
               "b=\"Hello world\", a unchanged");
    }

    {
        Ip2::Rope x("abc"), y("abc"), z("xyz");
        report(log, "T11  operator== and !=",
               (x == y) && (x != z) && !(x == z),
               "x==y true, x!=z true, x==z false");
    }

    {
        Ip2::Rope lo("apple"), hi("banana");
        report(log, "T12  operator< and >",
               (lo < hi) && (hi > lo) && !(lo > hi),
               "apple < banana, banana > apple");
    }

    {
        Ip2::Rope a("apple"), b("apple"), c("banana");
        report(log, "T13  operator<= and >=",
               (a <= b) && (a >= b) && (a <= c) && (c >= a),
               "a<=b (equal), a<=c (less), c>=a");
    }

    {
        Ip2::Rope p("cat"), q("dog");
        bool neqConsistent = (p != q) == !(p == q);
        bool leqConsistent = (p <= q) == !(p > q);
        bool geqConsistent = (q >= p) == !(q < p);
        report(log, "T14  Comparison alignment",
               neqConsistent && leqConsistent && geqConsistent,
               "!= ≡ !(==),  <= ≡ !(>),  >= ≡ !(< )");
    }

    {
        Ip2::Rope r("erase me");
        !r;
        report(log, "T15  operator! (clear)",
               r.empty() && r.length() == 0,
               "after !r: empty()=true, length()=0");
    }

    {
        Ip2::Rope r("the quick brown fox");
        int p1 = r["quick"];
        int p2 = r["fox"];
        report(log, "T16  operator[] — found",
               p1 == 4 && p2 == 16,
               "[\"quick\"]=4, [\"fox\"]=16");
    }

    {
        Ip2::Rope r("hello world");
        report(log, "T17  operator[] — not found",
               r["xyz"] == -1,
               "[\"xyz\"] = -1");
    }

    {
        bool caught = false;
        try
        {
            Ip2::Rope r("hi");
            r -= {0, 100};
        }
        catch (const std::out_of_range &)
        {
            caught = true;
        }
        report(log, "T18  Exception out_of_range (operator-= overflow)",
               caught, "std::out_of_range thrown for range [0, 100) on len=2");
    }

    {
        bool caught = false;
        try
        {
            Ip2::Rope r;
            r %= {0, "x"};
        }
        catch (const Ip2::RopeException &)
        {
            caught = true;
        }
        report(log, "T19  Exception RopeException (edit empty rope)",
               caught, "Ip2::RopeException thrown when editing empty Rope");
    }

    {
        bool caught = false;
        try
        {
            Ip2::Rope r("Hi");
            r %= {1, "TOOLONG"};
        }
        catch (const Ip2::RopeException &)
        {
            caught = true;
        }
        report(log, "T20  Exception RopeException (replacement overflow)",
               caught, "Ip2::RopeException thrown for 7-char replacement at pos 1 on len=2");
    }

    {
        bool caught = false;
        try
        {
            Ip2::Rope r("test");
            r[""];
        }
        catch (const std::invalid_argument &)
        {
            caught = true;
        }
        report(log, "T21  Exception invalid_argument (empty search key)",
               caught, "std::invalid_argument thrown for operator[](\"\")");
    }

    {
        Ip2::Rope r("Hello");
        std::string s = r.toString();
        bool hasNodes = s.find("1,") != std::string::npos;
        bool hasLen = s.find(",5:") != std::string::npos;
        bool hasQuote = s.find(":Hello") != std::string::npos;
        report(log, "T22  toString() contains nodes, len, preview",
               hasNodes && hasLen && hasQuote,
               "toString()=" + s);
    }

    log << "Results: " << passed << " passed, " << failed << " failed\n";
}

int main()
{
    try
    {
        Logger log("log.txt");
        runTests(log);
    }
    catch (...)
    {
        std::cerr << "[FATAL] Unexpected exception in test runner.\n";
        return 1;
    }

    assert(Ip2::Rope::getObjectCount() == 0);

    return (failed == 0) ? 0 : 1;
}