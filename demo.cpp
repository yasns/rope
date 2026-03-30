#include <cassert>
#include <iostream>
#include "rope.h"

static void section(const std::string &title)
{
    std::cout << "\n--- " << title << " ---\n";
}

static void show(const std::string &label, const ip2::Rope &r)
{
    std::cout << "  " << label << " => " << r.toString() << "\n";
}

int main()
{
    try
    {
        std::cout << "Starting demo...\n";

        section("1. Construction");

        ip2::Rope empty;
        show("empty Rope", empty);

        ip2::Rope r1("Hello, ");
        show("r1 from string", r1);

        ip2::Rope r2("world!");
        show("r2 from string", r2);


        section("2. Copy constructor & copy assignment");

        ip2::Rope r3(r1); // copy-ctor
        show("r3 (copy of r1)", r3);

        ip2::Rope r4;
        r4 = r2; // copy-assignment
        show("r4 (assigned from r2)", r4);

        // modify original, copy must be unchanged
        r1 += "modified";
        show("r1 after += \"modified\"", r1);
        show("r3 unchanged", r3);


        section("3. operator+=");

        ip2::Rope sentence("The quick brown");
        show("initial", sentence);

        sentence += " fox";
        show("after += \" fox\"", sentence);

        ip2::Rope tail(" jumps over the lazy dog");
        sentence += tail;
        show("after += tail Rope", sentence);


        section("4. operator-=  (delete range)");

        ip2::Rope del("Hello, cruel world!");
        show("before delete", del);

        del -= {7, 6}; // remove "cruel " -> "Hello, world!"
        show("after -= {7, 6}", del);


        section("5. operator%=  (in-place edit)");

        ip2::Rope edit("Hello, world!");
        show("before edit", edit);

        edit %= {7, "WORLD"}; // replace 5 chars at index 7 -> "Hello, WORLD!"
        show("after %= {7, \"WORLD\"}", edit);


        section("6. operator+  (returns new Rope)");

        ip2::Rope a("foo");
        ip2::Rope b("bar");
        ip2::Rope c = a + b;
        show("a", a);
        show("b", b);
        show("c = a + b", c);

        ip2::Rope d = a + std::string("baz");
        show("d = a + \"baz\"", d);


        section("7. Comparison operators  (lexicographic)");

        ip2::Rope lo("apple");
        ip2::Rope hi("banana");
        ip2::Rope eq("apple");

        std::cout << "  lo=\"" << lo.flatten() << "\"  hi=\"" << hi.flatten() << "\"  eq=\"" << eq.flatten() << "\"\n";
        std::cout << "  lo == eq  : " << (lo == eq ? "true" : "false") << "\n";
        std::cout << "  lo != hi  : " << (lo != hi ? "true" : "false") << "\n";
        std::cout << "  lo <  hi  : " << (lo < hi ? "true" : "false") << "\n";
        std::cout << "  lo <= eq  : " << (lo <= eq ? "true" : "false") << "\n";
        std::cout << "  hi >  lo  : " << (hi > lo ? "true" : "false") << "\n";
        std::cout << "  hi >= hi  : " << (hi >= hi ? "true" : "false") << "\n";
        std::cout << "  lo >  hi  : " << (lo > hi ? "true" : "false") << "\n";


        section("8. operator!  (clear)");

        ip2::Rope toClear("Please erase me");
        show("before clear", toClear);

        !toClear;
        show("after !toClear", toClear);
        std::cout << "  empty(): " << (toClear.empty() ? "true" : "false") << "\n";


        section("9. operator[]  (search)");

        ip2::Rope text("the quick brown fox");
        show("text", text);

        int pos = text["quick"];
        std::cout << "  [\"quick\"] = " << pos << "  (expected 4)\n";

        pos = text["fox"];
        std::cout << "  [\"fox\"]   = " << pos << "  (expected 16)\n";

        pos = text["cat"];
        std::cout << "  [\"cat\"]   = " << pos << "  (expected -1)\n";


        section("10. flatten() and length()");

        ip2::Rope full("Hello, rope world!");
        std::cout << "  flatten(): \"" << full.flatten() << "\"\n";
        std::cout << "  length() : " << full.length() << "\n";


        section("11. toString()");

        ip2::Rope ts("A somewhat longer string to show toString diagnostics.");
        std::cout << "  " << ts.toString() << "\n";


        section("12. Exception handling");

        try
        {
            ip2::Rope x("short");
            x -= {0, 100};
        }
        catch (const std::out_of_range &e)
        {
            std::cout << "  Caught out_of_range (delete): " << e.what() << "\n";
        }

        try
        {
            ip2::Rope x;
            x %= {0, "oops"};
        }
        catch (const ip2::RopeException &e)
        {
            std::cout << "  Caught RopeException (edit empty): " << e.what() << "\n";
        }

        try
        {
            ip2::Rope x("Hi");
            x %= {1, "TOOLONG"};
        }
        catch (const ip2::RopeException &e)
        {
            std::cout << "  Caught RopeException (overflow): " << e.what() << "\n";
        }

        try
        {
            ip2::Rope x("test");
            x[""];
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "  Caught invalid_argument (empty key): " << e.what() << "\n";
        }

        std::cout << "\nEnd of Demo.\n";
    }
    catch (...)
    {
        std::cerr << "\n[FATAL] Unexpected exception — demo aborted.\n";
        return 1;
    }

    assert(ip2::Rope::getObjectCount() == 0);

    return 0;
}