#include <cassert>
#include <iostream>
#include "rope.h"

static void section(const std::string &title)
{
    std::cout << "\n--- " << title << " ---\n";
}

static void show(const std::string &label, const Ip2::Rope &r)
{
    std::cout << "  " << label << " => " << r.toString() << "\n";
}

int main()
{
    try
    {
        std::cout << "Starting demo...\n";

        section("1. Construction");

        Ip2::Rope empty;
        show("empty Rope", empty);

        Ip2::Rope r1("Hello, ");
        show("r1 from string", r1);

        Ip2::Rope r2("world!");
        show("r2 from string", r2);


        section("2. Copy constructor & copy assignment");

        Ip2::Rope r3(r1); // copy-ctor
        show("r3 (copy of r1)", r3);

        Ip2::Rope r4;
        r4 = r2; // copy-assignment
        show("r4 (assigned from r2)", r4);

        // modify original, copy must be unchanged
        r1 += "modified";
        show("r1 after += \"modified\"", r1);
        show("r3 unchanged", r3);


        section("3. operator+=");

        Ip2::Rope sentence("The quick brown");
        show("initial", sentence);

        sentence += " fox";
        show("after += \" fox\"", sentence);

        Ip2::Rope tail(" jumps over the lazy dog");
        sentence += tail;
        show("after += tail Rope", sentence);


        section("4. operator-=  (delete range)");

        Ip2::Rope del("Hello, cruel world!");
        show("before delete", del);

        del -= {7, 6}; // remove "cruel " -> "Hello, world!"
        show("after -= {7, 6}", del);


        section("5. operator%=  (in-place edit)");

        Ip2::Rope edit("Hello, world!");
        show("before edit", edit);

        edit %= {7, "WORLD"}; // replace 5 chars at index 7 -> "Hello, WORLD!"
        show("after %= {7, \"WORLD\"}", edit);


        section("6. operator+  (returns new Rope)");

        Ip2::Rope a("foo");
        Ip2::Rope b("bar");
        Ip2::Rope c = a + b;
        show("a", a);
        show("b", b);
        show("c = a + b", c);

        Ip2::Rope d = a + std::string("baz");
        show("d = a + \"baz\"", d);


        section("7. Comparison operators  (lexicographic)");

        Ip2::Rope lo("apple");
        Ip2::Rope hi("banana");
        Ip2::Rope eq("apple");

        std::cout << "  lo=\"" << lo.flatten() << "\"  hi=\"" << hi.flatten() << "\"  eq=\"" << eq.flatten() << "\"\n";
        std::cout << "  lo == eq  : " << (lo == eq ? "true" : "false") << "\n";
        std::cout << "  lo != hi  : " << (lo != hi ? "true" : "false") << "\n";
        std::cout << "  lo <  hi  : " << (lo < hi ? "true" : "false") << "\n";
        std::cout << "  lo <= eq  : " << (lo <= eq ? "true" : "false") << "\n";
        std::cout << "  hi >  lo  : " << (hi > lo ? "true" : "false") << "\n";
        std::cout << "  hi >= hi  : " << (hi >= hi ? "true" : "false") << "\n";
        std::cout << "  lo >  hi  : " << (lo > hi ? "true" : "false") << "\n";


        section("8. operator!  (clear)");

        Ip2::Rope toClear("Please erase me");
        show("before clear", toClear);

        !toClear;
        show("after !toClear", toClear);
        std::cout << "  empty(): " << (toClear.empty() ? "true" : "false") << "\n";


        section("9. operator[]  (search)");

        Ip2::Rope text("the quick brown fox");
        show("text", text);

        int pos = text["quick"];
        std::cout << "  [\"quick\"] = " << pos << "\n";

        pos = text["fox"];
        std::cout << "  [\"fox\"]   = " << pos << "\n";

        pos = text["cat"];
        std::cout << "  [\"cat\"]   = " << pos << "\n";


        section("10. flatten() and length()");

        Ip2::Rope full("Hello, rope world!");
        std::cout << "  flatten(): \"" << full.flatten() << "\"\n";
        std::cout << "  length() : " << full.length() << "\n";


        section("11. toString()");

        Ip2::Rope ts("A somewhat longer string to show toString diagnostics.");
        std::cout << "  " << ts.toString() << "\n";


        section("12. Exception handling");

        try
        {
            Ip2::Rope x("short");
            x -= {0, 100};
        }
        catch (const std::out_of_range &e)
        {
            std::cout << "  Caught out_of_range (delete): " << e.what() << "\n";
        }

        try
        {
            Ip2::Rope x;
            x %= {0, "oops"};
        }
        catch (const Ip2::RopeException &e)
        {
            std::cout << "  Caught RopeException (edit empty): " << e.what() << "\n";
        }

        try
        {
            Ip2::Rope x("Hi");
            x %= {1, "TOOLONG"};
        }
        catch (const Ip2::RopeException &e)
        {
            std::cout << "  Caught RopeException (overflow): " << e.what() << "\n";
        }

        try
        {
            Ip2::Rope x("test");
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

    assert(Ip2::Rope::getObjectCount() == 0);

    return 0;
}