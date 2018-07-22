#include <iostream>
#include <tuple>
#include <utility>
#include <string>
#include <vector>
#include <sstream>
template <typename T> 
void print( T t) {
    std::cout << t << std::endl;
}

class tfn_print{
    public:
    template <typename... Args>
    auto operator()(Args&& ... args) const -> decltype( print(std::forward<Args>(args)...)) {
        return print(std::forward<Args>(args)...);
    }
};

// class universal_functor {
//     public : 
//     template <typename ... Args> 
//     auto operator()(Args&&... args) const -> decltype ( global_func(std::forward<Args>(args)...)) {
//         return global_func(std::forward<Args>(args)...);
//     }
// }
// #define make_citizen(X) class tfn_##X { public: template <typename... Args> auto operator()(Args&&... args) const ->decltype(X(std::forward<Args>(args)...))  { return X(std::forward<Args>(args)...); } }
#define define_functor_type(func_name) class tfn_##func_name {\
    public: template <typename ... Args> auto operator()(Args&&... args) const -> decltype(func_name(std::forward<Args>(args)...)) {\
    return func_name(std::forward<Args>(args)...);  }};

template<typename F, size_t ... I , typename ... Args>
inline auto tuple_apply_impl(const F & f, const std::index_sequence<I...> &, const std::tuple<Args...> &tp) {
    return f (std::get<I>(tp) ...);
}

template<typename F, typename ... Args> 
inline auto tuple_apply(const F& f, const std::tuple<Args...> &tp) -> decltype(f(std::declval<Args>() ...)) {
    return tuple_apply_impl(f, std::make_index_sequence<sizeof ... (Args)>{}, tp );

}


template <typename T, class F > 
auto  operator| ( T && param, const F & f) -> decltype(f(std::forward<T>(param) )) {
    return  f(std::forward<T>(param));
}


int add(int a, int b) {
    return a + b;
}
int add_one(int a) {
    return 1 + a;
}

template <typename F, typename Before = std::tuple<> , typename After = std::tuple<>>
class curry_functor {
    private:
        F f_;
        Before before_;
        After  after_;
    public:
        curry_functor(F && f) : f_(std::forward<F>(f)), before_(std::tuple<>()), after_(std::tuple<>()) {}
        curry_functor(const F &f, const Before &before, const After &after): f_(f), before_(before), after_(after) {}
        template <typename ... Args>
        auto operator()(Args ... args) const -> decltype(tuple_apply(f_, std::tuple_cat(before_, std::make_tuple(args...), after_))) {
            return tuple_apply(f_, std::tuple_cat(before_, std::make_tuple(std::forward<Args>(args)...), after_));
        }

        template <typename T> 
        auto curry_before( T && param) const {
            using RealBefore = decltype( std::tuple_cat(before_, std::make_tuple(param)));
            return curry_functor<F, RealBefore, After>(f_, std::tuple_cat(before_, std::make_tuple(std::forward<T>(param))), after_);
        }

        template<typename T> 
        auto curry_after( T && param) const {
            using RealAfter = decltype ( std::tuple_cat(after_, std::make_tuple(param)));
            return curry_functor<F, Before, RealAfter>(f_, before_, std::tuple_cat(after_, std::make_tuple(std::forward<T>(param))));
        }
};


// template<typename F, typename Before = std::tuple<>, typename After = std::tuple<>>
// class curry_functor {
// private:
// 	F f_;                            ///< main functor
// 	Before before_;             ///< curryed arguments
// 	After after_;               ///< curryed arguments
	
// public:

// 	curry_functor(F && f) : f_(std::forward<F>(f)), before_(std::tuple<>()), after_(std::tuple<>()) {}
// 	curry_functor(const F & f, const Before & before, const After & after) : f_(f), before_(before), after_(after) {}

// 	template <typename... Args>
// 	auto operator()(Args... args) const -> decltype(tuple_apply(f_, std::tuple_cat(before_, make_tuple(args...), after_)))
// 	{
// 		// execute via tuple
// 		return tuple_apply(f_, std::tuple_cat(before_, make_tuple(std::forward<Args>(args)...), after_));
// 	}

// 	// curry
// 	template <typename T>
// 	auto curry_before(T && param) const
// 	{
// 		using RealBefore = decltype(std::tuple_cat(before_, std::make_tuple(param)));
// 		return curry_functor<F, RealBefore, After>(f_, std::tuple_cat(before_, std::make_tuple(std::forward<T>(param))), after_);
// 	}

// 	template <typename T>
// 	auto curry_after(T && param) const
// 	{
// 		using RealAfter = decltype(std::tuple_cat(after_, std::make_tuple(param)));
// 		return curry_functor<F, Before, RealAfter>(f_, before_, std::tuple_cat(after_, std::make_tuple(std::forward<T>(param))));
// 	}
// };



template <typename F> 
auto fn_to_curry_functor(F && f) {
    return curry_functor<F>(std::forward<F>(f));
}

template<typename UF, typename Arg>
auto operator<<( const UF &f, Arg && arg)  -> decltype (f.template curry_before<Arg>(std::forward<Arg>(arg))) {
    return f.template curry_before<Arg>(std::forward<Arg>(arg));
}

template<typename UF, typename Arg>
auto operator>>( const UF &f, Arg && arg) -> decltype(f.template curry_after<Arg>(std::forward<Arg>(arg))) {
    return f.template curry_after<Arg>(std::forward<Arg>(arg));
}


template <typename ... FNs>
class fn_chain {
    private:
        const std::tuple<FNs ...> functions_;
        const static size_t TUPLE_SIZE = sizeof ... (FNs);

        template<typename Arg, std::size_t I> 
        auto call_impl(Arg&& arg, const std::index_sequence<I> &) const -> decltype ( std::get<I>(functions_)(std::forward<Arg>(arg))) {
            return std::get<I>(functions_)(std::forward<Arg>(arg));
        }
        template<typename Arg, std::size_t I, std::size_t ...Is > 
        auto call_impl(Arg&& arg, const std::index_sequence<I, Is...> &) const -> decltype(call_impl(std::get<I>(functions_)(std::forward<Arg>(arg)), std::index_sequence<Is...>{})) {
            return call_impl(std::get<I>(functions_)(std::forward<Arg>(arg)), std::index_sequence<Is...>{});
        }

        template <typename Arg>
        auto call(Arg && arg) const ->decltype(call_impl(std::forward<Arg>(arg), std::make_index_sequence<sizeof ... (FNs)>{})) {
            return call_impl(std::forward<Arg>(arg), std::make_index_sequence<sizeof ... (FNs)>{});
        }
    public:
        fn_chain() : functions_(std::tuple<>()) {}
        fn_chain(std::tuple<FNs...> functions) : functions_(functions){}
        template< typename F> 
        inline auto add (const F & f) const {
            return fn_chain<FNs..., F> (std::tuple_cat(functions_, std::make_tuple(f)));
        }

        template< typename Arg> 
        inline auto operator() ( Arg && arg)const -> decltype(call(std::forward<Arg>(arg))) {
            return call (std::forward<Arg>(arg));
        }
};

template <typename... FNs, typename F> 
inline auto operator|(fn_chain<FNs...> && chain, F &&f ) {
    return chain.add(std::forward<F>(f));
}


template <typename T, typename ... TArgs, template <typename...> class C, typename F>
auto fn_map( const C<T,TArgs...> & container, const F &f) -> C<decltype(f(std::declval<T>()))> {
    using resultType = decltype( f (std::declval<T>()));
    C<resultType> result;
    for (const auto & item : container) {
        result.push_back(f(item));
    }
    return result;
}

template <typename TResult, typename T, typename ... TArgs, template <typename...> class C, typename F>
TResult fn_reduce( const C<T, TArgs...> & container, const TResult &startValue, const F &f) {
    TResult result = startValue;
    for (const auto &item : container) {
        result = f (result, item);
    }
    return result;
}

// template <typename TResult, typename T, typename... TArgs, template <typename...>class C, typename F>
// TResult fn_reduce(const C<T, TArgs...>& container, const TResult& startValue, const F& f)
// {
// 	TResult result = startValue;
// 	for (const auto& item : container)
// 		result = f(result, item);
// 	return result;
// }

template <typename T, typename ... TArgs, template < typename ...> class C, typename F> 
C<T, TArgs...> fn_filter(const C<T, TArgs...> &container, const F &f) {
    C<T, TArgs...> result;
    for (const auto &item : container) {
        if (f(item)) {
            result.push_back(item);
        }
    }
    return result;
}

#define make_globle_curry_functor(NAME, F) define_functor_type(F); const auto NAME = fn_to_curry_functor(tfn_##F());

make_globle_curry_functor(map, fn_map);
make_globle_curry_functor(reduce, fn_reduce);
make_globle_curry_functor(filter, fn_filter);
// define_functor_type(add);
// define_functor_type(add_one);

// #define make_global_functor(NAME,F) const auto NAME = define_functor_type(F);
// #define make_globle_functor(NAME, F)  const auto NAME = define_functor_type(F); 
// make_citizen(print);
// make_globle_functor(tfn_add, add);
// make_globle_functor(fn_add_one, add_one);
// make_global_functor(fn_add, add);
// make_global_functor(fn_add_one, add_one);

void test_curry() {
    auto f = [](int x, int y, int z) { return x + y - z; };
    auto fn = fn_to_curry_functor(f);
    auto result = fn.curry_before(1)(2, 3); //0
    result = fn.curry_before(1).curry_before(2)(3); //0
    print(result);
    result = fn.curry_before(1).curry_before(2).curry_before(3)(); //0
    print(result);

    result = fn.curry_before(1).curry_after(2).curry_before(3)(); //2
    print(result);

    result = fn.curry_after(1).curry_after(2).curry_before(2)();  //1
    print(result);

}
#define tfn_chain fn_chain<>()
void test_pipe1()
{
    //test map reduce
    std::vector<std::string> slist = { "one", "two", "three" };

    slist | (map >> [](auto s) { return s.size(); })
        | (reduce >> 0 >> [](auto a, auto b) { return a + b; })
        | [](auto a) { std::cout << a << std::endl; };

    //test chain, lazy eval
    auto chain = tfn_chain | (map >> [](auto s) { return s.size(); })
        | (reduce >> 0 >> [](auto a, auto b) { return a + b; })
        | ([](int a) { std::cout << a << std::endl; });

    slist | chain;
}

void test_currying()
{
    auto f = [](int x, int y, int z) { return x + y - z; };
    auto fn = fn_to_curry_functor(f);

    auto result = (fn << 1)(2, 3); //0
    result = (fn << 1 << 2)(3); //0
    result = (fn << 1 << 2 << 3)(); //0

    result = (fn << 1 >> 2 << 3)(); //2
    result = (fn >> 1 >> 2 << 3)(); //1
}
void test_pipe(){
    auto f1 = [](int x) { return x + 3; };
    auto f2 = [](int x) { return x * 2; };
    auto f3 = [](int x) { return (double)x / 2.0; };
    auto f4 = [](double x) { std::stringstream ss; ss << x; return ss.str(); };
    auto f5 = [](std::string s) { return "Result: " + s; };
    auto result = 2|f1|f2|f3|f4|f5; //Result: 5
}
int main(int argc, char *argv[] ) {
    test_curry();
    test_pipe();
    // tfn_print print;

    // auto f = [](int x, int y, int z) {
    //     return x + y + z;
    // } ;
    // auto params = std::make_tuple(1, 2, 3);
    // auto result = tuple_apply(f, params);
    // print(result);


    // auto add_one = [] (auto a){return a + 1;};
    // auto result1 = 2 | add_one ;
    // print(result1);
    // tfn_print print;
    // print(5);
    // print("Hello, World");
    // tfn_add add_function;
    // auto result = add_function(1, 2);
    // print(result);
    // tfn_add_one add_one_function;
    // auto result1 = add_one_function(1);
    // print(result1);

    // auto result = fn_add(1, 2);
    // print(result);
    return 0;
}