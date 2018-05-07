#include <algorithm>
#include <cstdlib>
#include <forward_list>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <range/v3/all.hpp>
#include <vector>

std::vector<int>
generate_random_int_array (int const size) {
    std::vector<int> v;
    v.reserve (size);

    std::random_device dev;
    std::mt19937 engine (dev ());
    std::uniform_int_distribution<> dist (1, 100);

    std::generate_n (back_inserter (v), size, [&]() { return dist (engine); });
    std::sort (begin (v), end (v));

    return v;
}

void
print_array (std::vector<int> const& v) {
    std::cout << "[";
    if (!v.empty ()) {
        auto i = begin (v);
        std::cout << std::setw (3) << *i++;
        while (i != end (v)) {
            std::cout << ", " << std::setw (3) << *i++;
        }
    }
    std::cout << "]\n";
}


template <typename Iterator>
void
print_subarrays (Iterator b, Iterator e) {
    using namespace ranges;

    using row_type = typename std::iterator_traits<Iterator>::value_type;
    using const_row_iterator = typename row_type::const_iterator;
    using row_view           = v3::iterator_range<const_row_iterator>;

    // Create views for non-empty rows and put them in to a heap vector
    std::vector<row_view> heap =
        v3::make_iterator_range (b, e) |
        v3::view::remove_if ([](auto& row) { return row.empty (); }) |
        v3::view::transform (
            [](auto& row) { return row_view (begin (row), end (row)); });

    auto greater_head = [](auto& a, auto& b) {
        return *begin (a) > *begin (b);
    };

    // Heapify
    std::make_heap (begin (heap), end (heap), greater_head);
    std::cout << "[";

    if (!heap.empty ()) {
        std::pop_heap (begin (heap), end (heap), greater_head);
        auto top = std::move (heap.back ());
        heap.pop_back ();

        while (true) {
            auto& head = *begin (top);
            auto tail  = row_view (std::next (begin (top)), end (top));

            std::cout << head;

            if (heap.empty ()) {
                for (auto& x : tail) {
                    std::cout << ", " << x;
                }
                break;
            } else {
                std::pop_heap (begin (heap), end (heap), greater_head);
                top = std::move (heap.back ());

                std::cout << ", ";

                if (begin (tail) != end (tail)) {
                    auto& next_head = *begin (heap.back ());

                    auto tail_it = begin (tail);
                    while ((tail_it != end (tail)) && (*tail_it <= next_head)) {
                        std::cout << *tail_it << ", ";
                        ++tail_it;
                    }

                    tail = row_view (tail_it, end (tail));

                    if (begin (tail) != end (tail)) {
                        heap.back () = std::move (tail);
                        std::push_heap (begin (heap), end (heap), greater_head);
                        continue;
                    }
                }

                heap.pop_back ();
            }
        }
    }

    std::cout << "]\n";
}


int
main (int argc, char* argv[]) {
    std::ios::sync_with_stdio (false);
    std::vector<std::vector<int>> array;
    int n;
    int k;

    std::cout << "We're going to create k sorted arrays each containing n "
                 "integers between 1 and 100 (inclusive)\n\n";

    std::cout << "k = ";
    std::cin >> k;
    if (!std::cin) {
        return EXIT_FAILURE;
    }

    std::cout << "n = ";
    std::cin >> n;
    if (!std::cin) {
        return EXIT_FAILURE;
    }

    k = std::max (1, k);
    n = std::max (1, n);

    array.reserve (k);
    std::generate_n (back_inserter (array), k, [n]() {
        return generate_random_int_array (n);
    });

    std::cout << "\n";

    int i = 0;
    for (auto& subarray : array) {
        std::cout << "in[" << i++ << "] = ";
        print_array (subarray);
    }

    std::cout << "\nout[] = ";

    // There's no reason to copy in to nested std::forward_lists here except
    // to check that the print_subarray() function works with ForwardIterators
    std::forward_list<std::forward_list<int>> list;
    for (auto& v : array) {
        list.emplace_front (begin (v), end (v));
    }

    print_subarrays (begin (list), end (list));
    return EXIT_SUCCESS;
}
