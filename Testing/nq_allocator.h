#ifndef NQ_ALLOCATOR_H_
# define NQ_ALLOCATOR_H_

# include <cassert>
# include <cstdlib>
# include <cstddef>
# include <limits>
# include <iostream>

# include "domains.h"
# include "alloc_strat.h"

# undef max

/* The alloc_strat could return the pointer with a significative BIT for wether or not it allocated */

namespace nq
{
	template<typename T,
		class Domain = UnknownDomain,
		class AllocStrat = DefaultAlloc>
	struct allocator
	{
		static_assert(sizeof(T) > 0, "Can't allocate an incomplete type");

		/* Member types */
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		typedef AllocStrat allocator_strategy;

		/* Allocator convertor from T to U */
		template<class U>
		struct rebind
		{
			typedef allocator<U, Domain, AllocStrat> other;
		};

		/* constructors */
		allocator() {};
		allocator(const allocator& other) {};

		template <typename U,
			typename U_Domain,
			class U_AllocStrat>
			allocator(const allocator<U, U_Domain, U_AllocStrat>& other) {}

		template<typename U,
			typename U_Domain,
			class U_AllocStrat>
		allocator& operator=(const allocator<U, U_Domain, U_AllocStrat>& other) { return *this; }
			allocator<T>& operator=(const allocator& other) { return *this; }

		~allocator() {};

		/* Adress */
		pointer adress(reference r) const
		{
			return &r;
		}
		const_pointer adress(const_reference r) const
		{
			return &r;
		}

		/* Memory allocation */
		pointer allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
		{
			if (n == 0)
				return nullptr;
			char *internal_ptr = static_cast<char *>(allocator_strategy().allocate(n * sizeof(T) + Domain::header_size));
			if (internal_ptr == nullptr)
				throw std::bad_alloc();
			Domain::getInstance().add(internal_ptr, n * sizeof(T));
			pointer usr_ptr = reinterpret_cast<pointer>(internal_ptr + Domain::header_size);
			return usr_ptr;
		}

		void deallocate(pointer usr_ptr, size_type n)
		{
			if (usr_ptr != nullptr)
			{
				void *internal_ptr = (reinterpret_cast<char *>(usr_ptr)-DomainEarth::header_size);
				Domain::getInstance().remove(internal_ptr);
				allocator_strategy().deallocate(internal_ptr);
			}
			/* size_type ? */
		}

		size_type max_size() const
		{
			return std::numeric_limits<size_type>::max();
		}

		void construct(pointer p, const_reference val)
		{
			new (static_cast<void*>(p)) T(val);
		}

		template <class U,
			class ... Args>
		void construct(U* p, Args&&... args)
		{
			 ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
		}

		/* destroy(pointer p) and destroy(U* p) are the same.*/
		template <class U>
		void destroy(U* p)
		{
			p->~U();
		}
	};


	template<class T1,
		typename Domain1,
		class T2,
		typename Domain2>
	bool operator==(const allocator<T1, Domain1>& lhs, const allocator<T2, Domain2>& rhs)
	{
		assert(!"Implement the operator== for allocator");
		return true;
	}

	template<class T1,
		typename Domain1,
		class T2,
		typename Domain2>
	bool operator!=(const allocator<T1, Domain1>& lhs, const allocator<T2, Domain2>& rhs)
	{
		return !operator==(lhs, rhs);
	}
}

#endif // !NQ_ALLOCATOR_H_