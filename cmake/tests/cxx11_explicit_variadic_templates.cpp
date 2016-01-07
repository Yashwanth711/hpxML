//  Copyright (C) 2016 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

template <typename  ... Ts>
struct tuple
{
    tuple() {}
};

struct tag1 {};
struct tag2 {};
struct tag3 {};

template <typename T>
struct identity
{
    typedef T type;
};

template <typename Tag, typename T>
struct tagged_type
{
    typedef typename identity<Tag(T)>::type type;
};

template <typename ... Tags, typename ... Ts>
tuple<typename tagged_type<Tags, Ts>::type...> foo(Ts && ...)
{
    return tuple<typename tagged_type<Tags, Ts>::type...>();
}

int main()
{
    auto t = foo<tag1, tag2, tag3>(42, 43, 44);
}
