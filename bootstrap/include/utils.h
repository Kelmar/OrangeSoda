/*************************************************************************/
/*************************************************************************/

#ifndef OS_UTILS_H__
#define OS_UTILS_H__

/*************************************************************************/

template <class F>
class deferred_action
{
private:
    F m_call;

public:
    explicit deferred_action(F call) noexcept
        : m_call(call)
    {
    }

    deferred_action(deferred_action &&other) noexcept
        : m_call(std::move(other.m_call))
    {
    }

    // Prevent copy
    deferred_action(const deferred_action &) = delete;
    deferred_action &operator =(const deferred_action &) = delete;

    ~deferred_action() noexcept
    {
        m_call();
    }
};

template <class F>
inline deferred_action<F> defer(const F &f) noexcept
{
    return deferred_action(f);
}

template <class F>
inline deferred_action<F> defer(F &&f) noexcept
{
    return deferred_action(f);
}

/*************************************************************************/

#endif /* OS_UTILS_H__ */

/*************************************************************************/
