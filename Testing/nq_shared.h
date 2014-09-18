#ifndef NQ_SHARED_H_
# define NQ_SHARED_H_

# include <memory>
# include <atomic>
# include <type_traits>

# include "nq_allocator.h"
# include "nq_new.h"
# include "nq_unique.h"
# include "nq_deleter.h"
# include "domains.h"
# include "alloc_strat.h"

/* weak allocation ? where how ? weak ref count */

namespace nq
{
	template<class T>
	class shared_ptr : public std::shared_ptr<T>
	{
	public:
		/* Allocator used to allocate the ref_count of the shared_ptr */
		typedef nq::allocator<T, SharedPtrRefCountDomain, DefaultAlloc> count_alloc;
		typedef nq::deleter<T, UnknownDomain, DefaultAlloc> deleter;

		/*** Constructors for a nullptr ***/

		shared_ptr() noexcept
			: std::shared_ptr<T>(nullptr, deleter{}, count_alloc{})
		{ // construct with nullptr deleter{} and alloc{} 
		}

		shared_ptr(std::nullptr_t) noexcept
			:std::shared_ptr<T>(nullptr, deleter{}, count_alloc{})
		{ // construct with nullptr, deleter{}, count_alloc{}
		}

		template<class Deleter>
		shared_ptr(std::nullptr_t, Deleter del) noexcept
			: std::shared_ptr<T>(nullptr, del, count_alloc{})
		{ // construct with nullptr, del and count_alloc{}
		}

		template<class Deleter,
			class Allocator>
		shared_ptr(std::nullptr_t, Deleter del, Allocator alloc) noexcept
			: std::shared_ptr<T>(nullptr, del, alloc)
		{ // construct with nullptr, del and alloc
		}

		/*** Constructors for an already allocated ptr ***/

		template<class Y,
			class Deleter>
		shared_ptr(Y *ptr, Deleter del) noexcept
			: std::shared_ptr<T>(ptr, del, count_alloc{})
		{ // construct with ptr, del and count_alloc{}
		}

		template<class Y,
			class Deleter,
			class Allocator>
		shared_ptr(Y *ptr, Deleter del, Allocator alloc) noexcept
			: std::shared_ptr<T>(ptr, del, alloc)
		{ // construct with ptr, del and alloc
		}

		/*** Copy constructors ***/

		template<class Y>
		shared_ptr(const shared_ptr<Y>& other, T *ptr) noexcept
			: std::shared_ptr<T>(other, ptr)
		{ // construct shared_ptr object that aliases ptr
		}

		shared_ptr(const shared_ptr& other) noexcept
			: std::shared_ptr<T>(other)
		{ // construct a shared_ptr object that owns same resource as other
		}

		template<class Y,
			class = class std::enable_if<std::is_convertible<Y *, T *>::value,
				void>::type>
		shared_ptr(const shared_ptr<Y>& other) noexcept
			: std::shared_ptr<T>(other)
		{ // construct a shared_ptr object that owns same resource as other
		}

		/*** Move contructors ***/

		shared_ptr(shared_ptr&& other) noexcept
			: std::shared_ptr<T>(std::move(other))
		{ // construct shared_ptr that takes resource from other
		}

		template<class Y,
			class = class std::enable_if<std::is_convertible<Y *, T *>::value,
				void>::type>
		shared_ptr(shared_ptr<Y>&& other) noexcept
			: std::shared_ptr<T>(std::move(other))
		{ // construct shared_ptr that takes resource from other
		}

		/* FIXME change std::weak_ptr with nq::weak_ptr when it'll be handled */
		template <class Y>
		explicit shared_ptr(const std::weak_ptr<Y>& other) noexcept
			: std::shared_ptr<T>(other)
		{ // construct shared_ptr object that owns resource *other
		}

		template <class Y,
			class Del>
		shared_ptr(nq::unique_ptr<Y, Del>&& other) noexcept
			: std::shared_ptr<T>(std::move(other))
		{ // construct from unique_ptr
		}

        /*** Assignement operator ***/

        shared_ptr& operator=(const shared_ptr& r) noexcept = default;

        template<class Y>
        shared_ptr& operator=(const shared_ptr<Y>& r) noexcept
        { // copy assignement from shared_ptr<Y>
            this->std::shared_ptr<T>::operator=(r);
            return *this;
        }

        shared_ptr& operator=(shared_ptr&& r) noexcept
        { // move assignement from shared_ptr
            this->std::shared_ptr<T>::operator=(std::move(r));
            return *this;
        }

        template<class Y>
        shared_ptr& operator=(shared_ptr<Y>&& r) noexcept
        { // move assignement from shared_ptr<Y>
            this->std::shared_ptr<T>::operator=(std::move(r));
            return *this;
        }

        template<class Y,
            class Del>
        shared_ptr& operator=(std::unique_ptr<Y, Del>&& other)
        { // move assignement from unique_ptr
            this->std::shared_ptr<T>::operator=(std::move(other));
            return *this;
        }
    public:
        // convert to std from nq
        shared_ptr(std::shared_ptr<T>&& other) noexcept
            : std::shared_ptr<T>(std::move(other))
        {}

        //convert to nq from str
        operator std::shared_ptr<T>()
        {
            return *this;
        }
	private:
		/*
		** We don't want to take the risk that a misinformed user write somthing like:
		** nq::shared<T>(new (Domain, AllocStrat) T());
		** The pointer allocated in Domain with AllocStrat will be, by default,
		** deleted in Unknown Domain with DefaultAlloc
		*/
		template<class Y>
		explicit shared_ptr(Y *ptr) = delete;
	};

	/*** Non member functions ***/

	template<class T,
		class Domain = UnknownDomain,
		class AllocStrat = DefaultAlloc,
		class... Args>
	shared_ptr<T> new_shared(Args&&... args)
	{ // make a shared_ptr with two allocation (as the shared_ptr constructor do)
		typedef nq::allocator<T, SharedPtrRefCountDomain, DefaultAlloc> count_alloc;
		typedef nq::deleter<T, Domain, AllocStrat> deleter;

		return shared_ptr<T>(nq::New<T, Domain, AllocStrat>(std::forward<Args>(args)...), deleter{}, count_alloc{});
	}

	template<class T,
		class Domain = UnknownDomain,
		class AllocStrat = DefaultAlloc,
		class... Args>
	shared_ptr<T> make_shared(Args&&... args)
	{ // make a shared_ptr with a single allocation
		typedef nq::allocator<T, Domain, AllocStrat> alloc;

		return std::allocate_shared<T>(alloc{}, std::forward<Args>(args)...);
	}

	template<class T,
		class Allocator,
		class... Args>
	shared_ptr<T> allocate_shared(const Allocator& alloc, Args&&... args)
	{ // make a shared_ptr with a single allocation
		return std::allocate_shared<T>(alloc, std::forward<Args>(args)...);
	}
}

// Provide partial specialization of std fonctions for nq::shared_ptr
namespace std
{
    /* have to copy paste the stl tp make this works... */
    template<typename T>
    struct owner_less<nq::shared_ptr<T>>
    : public _Sp_owner_less<shared_ptr<T>, weak_ptr<T>>
    { };
}

#endif // !NQ_SHARED_H_
