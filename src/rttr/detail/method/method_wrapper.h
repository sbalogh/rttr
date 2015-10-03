/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014, 2015 Axel Menzel <info@axelmenzel.de>                       *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef RTTR_METHOD_WRAPPER_H_
#define RTTR_METHOD_WRAPPER_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/method/method_wrapper_base.h"
#include "rttr/detail/misc/function_traits.h"
#include "rttr/argument.h"
#include "rttr/instance.h"
#include "rttr/detail/type/accessor_type.h"
#include "rttr/detail/method/method_accessor.h"
#include "rttr/variant.h"
#include "rttr/detail/default_arguments/default_arguments.h"
#include "rttr/detail/default_arguments/invoke_with_defaults.h"


#include <functional>
#include <string>

namespace rttr
{
namespace detail
{

template<typename F, typename Policy, typename Default_Args>
class method_wrapper;

template<typename F, typename Policy>
class method_wrapper<F, Policy, default_args<>> : public method_wrapper_base
{
    public:
        method_wrapper(F func_acc)
        : m_func_acc(func_acc)
        { }

        bool is_static()                        const   { return method_accessor<F, Policy>::is_static();           }
        type get_return_type()                  const   { return method_accessor<F, Policy>::get_return_type();     }
        std::vector<bool> get_is_reference()    const   { return method_accessor<F, Policy>::get_is_reference();    }
        std::vector<bool> get_is_const()        const   { return method_accessor<F, Policy>::get_is_const();        }
        std::vector<type> get_parameter_types() const   { return method_accessor<F, Policy>::get_parameter_types(); }

        variant invoke(instance& object) const
        {
           return method_accessor<F, Policy>::invoke(m_func_acc, object);
        }
        variant invoke(instance& object, argument& arg1) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3, arg4);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return method_accessor<F, Policy>::invoke(m_func_acc, object, arg1, arg2, arg3, arg4, arg5, arg6);
        }

        variant invoke_variadic(instance& object, std::vector<argument>& args) const
        {
            return method_accessor<F, Policy>::invoke_variadic(m_func_acc, object, args);
        }

    private:
        F  m_func_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename Policy, typename...TArgs>
class method_wrapper<F, Policy, default_args<TArgs...> > : public method_wrapper_base
{
    using method_type = typename detail::method_type<F>::type;
    using arg_index_sequence = make_index_sequence< function_traits<F>::arg_count >;
    using invoker_class = method_invoker<F, Policy, method_type, arg_index_sequence>;
    using invoke_with_defaults = invoke_defaults_helper<invoker_class, F>;

    public:
        method_wrapper(F func_acc, default_args<TArgs...> default_args)
        : m_func_acc(func_acc), m_def_args(std::move(default_args))
        { }

        bool is_static()                        const   { return method_accessor<F, Policy>::is_static();           }
        type get_return_type()                  const   { return method_accessor<F, Policy>::get_return_type();     }
        std::vector<bool> get_is_reference()    const   { return method_accessor<F, Policy>::get_is_reference();    }
        std::vector<bool> get_is_const()        const   { return method_accessor<F, Policy>::get_is_const();        }
        std::vector<type> get_parameter_types() const   { return method_accessor<F, Policy>::get_parameter_types(); }

        variant invoke(instance& object) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args);
        }
        variant invoke(instance& object, argument& arg1) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3, arg4);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3, arg4, arg5);
        }
        variant invoke(instance& object, argument& arg1, argument& arg2, argument& arg3, argument& arg4, argument& arg5, argument& arg6) const
        {
            return invoke_with_defaults::invoke(m_func_acc, object, m_def_args.m_args, arg1, arg2, arg3, arg4, arg5, arg6);
        }

        variant invoke_variadic(instance& object, std::vector<argument>& args) const
        {
            if (args.size() <= function_traits<F>::arg_count)
                return invoke_variadic_helper<invoke_with_defaults, arg_index_sequence>::invoke(args, m_func_acc, object, m_def_args.m_args);
            else
                return variant();
        }

    private:
        F  m_func_acc;
        default_args<TArgs...> m_def_args;
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_METHOD_WRAPPER_H_