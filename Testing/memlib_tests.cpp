#include "nq_vector.h"
#include "nq_shared.h"
#include "nq_unique.h"
#include "nq_new.h"

#include "domains.h"

struct Test
{
private:
	int num_;
	int whatever_;
	int break_;
public:
	Test()
		: num_(55),
		whatever_(33),
		break_(123)
	{
		std::cout << "Test()" << std::endl;
	}
	Test(int i, int j, int h)
		: num_(i),
		whatever_(j),
		break_(h)
	{
		std::cout << "Test(" << num_ << ", " << whatever_
			<< ", " << break_ << ")" << std::endl;
	}
	~Test()
	{
		std::cout << "~Test(" << num_ << ", " << whatever_
			<< ", " << break_ << ")" << std::endl;
	}
	Test(const Test& o)
		: num_(o.num_),
		whatever_(o.whatever_),
		break_(o.break_)
	{
		std::cout << "Test(Test&" << o.num_ << ", " << o.whatever_
			<< ", " << o.break_ << ")" << std::endl;
	}

	void print()
	{
		std::cout << num_ << ", " << whatever_ << ", " << break_ << std::endl;
	}
};

void tests()
{
    auto f = nq::make_unique<int, DomainSpace>(4);
    
    /*Test *ho = nq::memlib::New<Test, DomainEarth>(3, 5, 1);

    nq::memlib::Delete<Test, DomainEarth>(ho);
    */
    {
        /*
        Test *dom_new = NQ_NEW(DomainSpace) Test(77, 100, 43);
        delete dom_new;
        */
        /*
        int *salut = nq::memlib::New_array<int, DomainSpace>(123, {42, 66, 321});
        for (int i = 0; i < 123; ++i)
        {
            std::cout << salut[i] << " -- ";
        }
        std::cout << std::endl;
        nq::memlib::Delete_array<int, DomainSpace>(salut);
        */
        //int *jay = new int[3] {3, 45, 123};
        //delete[] jay;
        //auto nq_un = nq::make_unique<Test[], DomainSpace>(3);
        //nq_un[0] = Test(3, 5, 1);
        
        //auto j = nq::make_shared<Test>(2, 3, 5); 44

        nq::shared_ptr<Test> h(NQ_NEW(DomainSpace) Test(3, 5, 6));

        //auto f = nq::new_shared<Test, DomainSpace>(44, 3030, 190);

        
        //nq::unique_ptr<Test[]> j(NQ_NEW(DomainEarth) Test[3]);

        /*
           nq::weak_ptr<int> n_weak;
           {
           nq::shared_ptr<int> n_shar(NQ_NEW(DomainEarth) int(3));
           n_weak = n_shar;
           }
           */
        nq::vector<Test, DomainSpace> vec{Test(3, 521, 900), Test(3, 5, 6)};
        vec.push_back(Test(3, 5123, 87));
        //nq::unique_ptr<Test> j(NQ_NEW(DomainEarth) Test(1, 3, 5));
        /*
        nq::unique_ptr<Test, DomainSpace> lol =
                        nq::make_unique<Test, DomainSpace>(3, 4, 5);
        nq::unique_ptr<Test>folk(lol.release());
        */

        int *array_n = NQ_NEW(DomainEarth) int[3]{3, 6, 3};
        nq::log::print(std::cout, "salute");
	}
    nq::log::print();
}

