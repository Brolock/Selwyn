
#include "tracer.h"
#include "nq_allocator.h"
#include "nq_new.h"
#include "nq_vector.h"
#include "nq_set.h"
#include <memory>

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
		: num_(o.num_)
	{
		std::cout << "Test(Test&" << o.num_ << ", " << o.whatever_<< ")" << std::endl;
	}
};


int main()
{
    nq::set<int, DomainSpace> slt;
    {
        auto test_un = nq::make_unique<Test>(2, 3);

        nq::vector<int, DomainEarth> vec_test = {1, 3, 5, 7};

        std::cout << vec_test[3] << std::endl;

        nq::vector<int, DomainEarth> vec_swap({3, 3});

        std::swap(vec_swap, vec_test);

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
    /*
	nq::vector<Test, DomainSpace> x;
	x.push_back(Test(4));
	x.push_back(Test(5));
	x.push_back(Test(7));
	x.push_back(Test(1));
	x.push_back(Test(345));
	x.push_back(Test(345));
	x.push_back(Test(345));
	nq::vector<Test, DomainEarth, DefaultAlloc> v;
	v.push_back(Test(1));
	v.push_back(Test(2));
	v.push_back(Test(3));
	v.push_back(Test(4));
	v.push_back(Test(1214));

	DomainSpace::getInstance().print();
	DomainEarth::getInstance().print();
    */
}
