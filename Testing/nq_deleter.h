#ifndef NQ_DELETER_H_
# define NQ_DELETER_H_

#include <type_traits>

#include "nq_allocator.h"

/* 
** The deleter class is used in RAII classes to know the way a given
** pointer needs to be deleted.
** It's the operator() associated with this class that define the
** method to follow
*/

namespace nq
{
	template <typename T,
		typename Domain = UnknownDomain,
		class AllocStrat = DefaultAlloc>
	struct deleter
	{
		deleter()
		{ // default construct
		}

		template <class U,
			typename U_Domain,
			class U_AllocStrat>
		deleter(const deleter<U, U_Domain, U_AllocStrat>&,
			    typename std::enable_if<
                  std::is_convertible<U*, T*>::value>::type* = nullptr)
		{ // construct from another deleter
		}

		void operator()(T *ptr) const
		{ //delete the ptr with allocstrat
			allocator<T, Domain, AllocStrat>().deallocate(ptr, 0);
		}
	};

	
	template <typename T,
		typename Domain,
		class AllocStrat>
	struct deleter <T[], Domain, AllocStrat>
	{
		deleter()
		{ // default construct
		}

		template <typename U>
		void operator()(U *ptr) const = delete;

		void operator()(T *ptr) const
		{
            memlib::Delete_array<T, Domain, AllocStrat>(ptr);
		}
	};

    /* new_deleter used by default by nq::shared_ptr to delete a newed ptr */
	template <typename T>
	struct new_deleter
	{
		new_deleter()
		{ // default construct
		}

		template <class U>
		new_deleter(const new_deleter<U>&,
			    typename std::enable_if<
                  std::is_convertible<U*, T*>::value>::type* = nullptr)
		{ // construct from another deleter
		}

		void operator()(T *ptr) const
		{ //delete the ptr with allocstrat
            delete ptr;
		}
	};

	template <typename T>
	struct new_deleter <T[]>
	{
		new_deleter()
		{ // default construct
		}

		template <typename U>
		void operator()(U *ptr) const = delete;

		void operator()(T *ptr) const
		{
            delete[](ptr);
		}
	};
}

#endif // !NQ_DELETER_H_
