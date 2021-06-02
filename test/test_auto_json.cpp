// Using an anonymous lambda doesn't work as the compiler won't allow the assignment from the generic lambda parameter
template <typename T>
struct Extractor {
    constexpr Extractor(T** t, int target)
    : _t{t}
    , _target{target} {}

    // The !std::is_const_v<S> is not strictly required but helps avoid subtle bugs where the caller uses a const object
    // resulting in this template resolving as S = const T or const T&, resulting in this function being called instead
    // of the specialization on our target type T, without the compiler telling us.
    template <typename S, typename = std::enable_if_t<!std::is_const_v<S>>>
    constexpr void operator()(const char* name, S& member) {
        _count++;
    }

    template <>
    constexpr void operator()<T>(const char* name, T& member) {
        if (_count == _target) {
            *_t = &member;
        }
        _count++;
    }
    T** _t;
    int _target{};
    int _count{};
};
