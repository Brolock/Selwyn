
#include "tracer.h"
#include "nq_allocator.h"
#include "nq_new.h"
#include "nq_vector.h"
#include "nq_set.h"
#include "nq_unordered_set.h"
#include "nq_unordered_map.h"
#include <memory>
#include <unordered_set>

#include <vector>
#include "alloc_strat.h"
#include "type_to_string.h"
#include "nq_shared.h"
#include "nq_unique.h"

#include <type_traits>

#include <algorithm>
#include <iostream>

#include "Header.h"
#include "nq_deleter.h"

nq::Tracer NewTrace;

struct Test
{
private:
	int num_;
	int whatever_;
public:
	Test()
		: num_(55),
        whatever_(33)
	{
		std::cout << "Test()" << std::endl;
	}
	Test(int i, int j)
		: num_(i),
		whatever_(j)
	{
		std::cout << "Test(" << num_ << ", " << whatever_ << ")" << std::endl;
	}
	~Test()
	{
		std::cout << "~Test(" << num_ << ", " << whatever_ << ")" << std::endl;
	}
	Test(const Test& o)
		: num_(o.num_),
          whatever_(o.whatever_)
	{
		std::cout << "Test(Test&" << o.num_ << ", " << o.whatever_<< ")" << std::endl;
	}
};


int main()
{
    nq::unordered_map<int, Test> tmp{std::make_pair<int, Test>(1, Test(1, 2)),
    std::make_pair<int, Test>(61, Test(4, 5)),
    std::make_pair<int, Test>(3, Test(3, 22))};

    {
        DomainEarth::getInstance().print();
        DomainSpace::getInstance().print();
        UnknownDomain::getInstance().print();
        SharedPtrRefCountDomain::getInstance().print();

        std::cout << "=================" << std::endl;
    }

    DomainEarth::getInstance().print();
    DomainSpace::getInstance().print();
    UnknownDomain::getInstance().print();
    SharedPtrRefCountDomain::getInstance().print();
}
