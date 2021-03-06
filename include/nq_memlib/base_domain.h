#ifndef BASE_DOMAIN_H_
# define BASE_DOMAIN_H_

# include <cassert>
# include <cstddef>
# include <utility>

# include <iostream>

# include <mutex>

# include "env_maccro.h"
# include "tree.h"

/*
** Domains log memory allocation through different cases so we can know
** what kind of part of the app take more memory or allocate the most.
** They are singletons inheriting from BaseDomain (which is an interface).
** Domains are double linked lists of Headers.
** Headers are placed (by the allocator!) before all the allocated memory
** so we can recover them with trivial pointer arithmetic.
** Placing them there avoid costy extra allocation for every logging.
*/

namespace nq { namespace memlib {
    void remove_header_operator_delete(void *ptr);
}} // nq::memlib

# ifdef WITH_NQ_MEMLOG
class BaseDomain : public slwn::BaseTree<int, int>
{
private:
    // TODO remove that
    // operator delete function only have to know about Header Structure.
    friend void nq::memlib::remove_header_operator_delete(void *ptr);

    typedef slwn::BaseTree<int, int> Super;
    
    class Header
    {
    private:
        Header *prev_;
        Header *next_;
        /* We use a size_t instead of others int (uint**_t, etc..) to get
        * a 32 and 64 bits adaptability */
        const size_t size_;

        /* The padding is here for allignement */
        /*
        ** !It's also used to log wheter the Header is a SubHeader or not;
        ** it avoids dynamic_cast for a static_cast (less costy)
        */
        const size_t padding_;
    public:
        Header(size_t size, size_t padding = 0,
                Header *prev = nullptr, Header *next = nullptr)
            : prev_(prev),
            next_(next),
            size_(size),
            padding_(padding)
        {}
        void add(Header *next);
        void remove();

        /* remove_begin()/remove_end() return a Header* so the Domain can put
         * his begin_/end_ pointers up-to-date */
        Header* remove_begin();
        Header* remove_end();

        inline size_t size() const { return size_; }

        /* print the Header datas in the stream */
        void
        print(std::ostream&, size_t) const;
    };

private:
    /*
    ** SubHeader is the Header specific to operator new and delete so we can:
    ** -log the file and line they have been allocated in
    ** -log the Domain* used by new so it can be recovered later by delete.
    */
    class SubHeader : public Header
    {
    private:
        const char* file_;
        size_t line_;
        BaseDomain *dom_;
        size_t nothing_;
    public:
        SubHeader(size_t size, const char* file, size_t line, BaseDomain *dom)
            : Header(size, 1, nullptr, nullptr),
            file_(file),
            line_(line),
            dom_(dom),
            nothing_(0)
        {}

        inline const char* get_file() const { return file_; }
        inline size_t get_line() const { return line_; }
        inline BaseDomain *get_domain() const { return dom_; }
    };
private:
    mutable std::mutex mutex_; // protect internal datas
private:
    size_t count_ = 0; // The number of non freed allocation in the domain
    size_t size_ = 0; // The total size in bytes of all the allocations
private:
    Header *begin_ = nullptr;
    Header *end_ = nullptr;
public:
    inline size_t get_count() const { return count_; }
    inline size_t get_size() const { return size_; }

public:
    enum HSENUM { header_size = sizeof(Header),
        sub_header_size = sizeof(SubHeader)};

    /* Add the Header constructed with size at the ptr location to the
     * current Domain's list */

    void add(void *internal_ptr, size_t size);

    void add(void* internal_ptr, std::size_t size,
        const char* file, size_t line, BaseDomain *dom);

    /* Remove from the current Domain's list the Header associated with
     * the allocated ptr send */
    void remove(void *internal_ptr);

    virtual void virtual_remove(void *internal_ptr) = 0;

protected:
    /* BaseDomain is an interface  it's constructor can't be called */
    BaseDomain()
    {}
private:
    BaseDomain(const BaseDomain&) {}
    BaseDomain& operator=(const BaseDomain&) { return *this; };

    /* return a string of the domain name for the printer */
    virtual const char* domain_name() const
    { assert(!"How did you got here!?"); return "never_reached"; };

public:
    /* printer for the debug, will probably to change */
    virtual void
    print(std::ostream& = std::cout, size_t = 0) const override;

private:
    virtual std::tuple<int, int>
    get_node_infos() const override
    {
        return std::tuple<int, int>(count_, size_);
    }

# ifdef NQ_ENV_32
        static_assert(sizeof(Header) == 16,
                "Header don't take 16 bytes in 32 bits");
# else // NQ_ENV_32
        static_assert(sizeof(Header) == 32,
                "Header don't take 32 bytes in 64 bits");
# endif // !NQ_ENV_32
};

# else // WITH_NQ_MEMLOG
/*
** If not logging, BaseDomain is an empty class in which all methods are
** empty
*/
class BaseDomain : public slwn::BaseTree<int, int>
{
public:
    enum HSENUM { header_size = 0,
        sub_header_size = 0 };

    inline void add(void*, size_t) {}

    inline void add(void*, std::size_t,
        const char*, size_t, BaseDomain*) {}

    inline void remove(void*) {}

    inline virtual
    void print(std::ostream& = std::cout, size_t = 0) const override {}

    inline size_t get_count() const { return 0; }

    virtual std::tuple<int, int>
    get_node_infos() const override
    { return std::tuple<int, int>(0, 0); }
protected:
    BaseDomain() {}
};
# endif // WITH_NQ_MEMLOG

/*** Non-member functions ***/
namespace nq { namespace memlib {
    /* These non-members are (way) easier to pass as function pointers */
    template<class Domain>
    void remove_elem_domain(void *ptr)
    {
        Domain::getInstance().remove(ptr);
    }

    template<class Domain,
        class... Args>
    void add_elem_domain(Args... args)
    {
        Domain::getInstance().add(std::forward<Args>(args)...);
    }
}} // nq::memlib

class AllDomains : public BaseDomain
{
public:
    static AllDomains& getInstance()
    {
        static AllDomains instance;
        return instance;
    }
# ifdef WITH_NQ_MEMLOG
    virtual void virtual_remove(void *internal_ptr) override
    {
        this->remove(internal_ptr);
    }
# endif // !WITH_NQ_MEMLOG
private:
    virtual const char* domain_name() const { return "AllDomains"; }
};

# ifdef WITH_NQ_MEMLOG

/* Generic declaration of a Domain to avoid copy paste at every
 * new domain creation */
#  define NQ_DOMAIN(new_domain, parent_domain) \
class new_domain : public BaseDomain           \
{                                              \
public:                                        \
    static new_domain& getInstance()           \
    {                                          \
        static new_domain instance;            \
        parent_domain::getInstance().add_son(&instance);\
        return instance;                       \
    }                                          \
    virtual void virtual_remove(void *internal_ptr) override\
    {                                          \
        this->remove(internal_ptr);            \
    }                                          \
private:                                       \
    virtual const char* domain_name() const { return #new_domain; } \
};

# else //WITH_NQ_MEMLOG
/*
** If not logging all the Domains type are merged to one type (AllDomains)
** to avoid code duplication in templates.
** This results in smaller binary file
*/
# define NQ_DOMAIN(new_domain, unused_param) \
    typedef AllDomains new_domain;

# endif // !WITH_NQ_MEMLOG

#endif // !BASE_DOMAIN_H_
