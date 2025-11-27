#pragma once
#include <type_traits>
#include <array>
#include <map>

#include "lambda_traits.h"
#include "function_info.h"
#include "hooking_result.h"

struct hook_info
{
	uintptr_t offset;
	uintptr_t address;
	uintptr_t original;
	uintptr_t detour;
	uintptr_t thunk;
	uintptr_t hook;
	void(*deleter)(uintptr_t);
};

class hooking
{
public:
	hooking_result init();
	~hooking();

	template<known_function_id ID, typename Fn>
	hooking_result add_hook(Fn&& cb)
	{
		return add_hook<ID>(known_function_offsets[ID], std::forward<Fn>(cb));
	}

	template<known_function_id ID, typename Fn>
	hooking_result add_hook(uintptr_t offset, Fn&& cb)
	{
		hook_info& hi = m_hooks[ID];
		hi.offset = offset;
		hi.address = m_base + offset;

		using cb_t = std::decay_t<Fn>;
		hi.hook = (uintptr_t)(new cb_t(std::forward<Fn>(cb)));
		hi.deleter = [](uintptr_t p) { delete reinterpret_cast<cb_t*>(p); };

		set_hook_info<ID>(&hi);

		using traits = lambda_traits<cb_t>;
		using ret = typename traits::ret;
		using all_args = typename traits::args;

		using orig_fn = std::tuple_element_t<0, all_args>;
		using remaining = typename tuple_tail<all_args>::type;

		hi.detour = make_detour<ID, cb_t, ret, orig_fn, remaining>();

		return install(hi);
	}

private:
	void deinit();
	hooking_result install(hook_info& hi);

	template<known_function_id ID>
	static hook_info*& get_hook_info()
	{
		static hook_info* info = nullptr;
		return info;
	}

	template<known_function_id ID>
	static void set_hook_info(hook_info* hi)
	{
		get_hook_info<ID>() = hi;
	}

	template<known_function_id ID, typename Lambda, typename Ret, typename OrigFn, typename Tuple>
	static uintptr_t make_detour()
	{
		return make_detour_impl<ID, Lambda, Ret, OrigFn>(
			(Tuple*)nullptr
		);
	}

	template<known_function_id ID, typename Lambda, typename Ret, typename OrigFn, typename... Args>
	static uintptr_t make_detour_impl(std::tuple<Args...>*)
	{
		return reinterpret_cast<uintptr_t>(
			&detour_stub<ID, Lambda, Ret, OrigFn, Args...>
			);
	}

	template<known_function_id ID, typename Lambda, typename Ret, typename OrigFn, typename... Args>
	static Ret __fastcall detour_stub(Args... args)
	{
		using cb_t = std::decay_t<Lambda>;

		hook_info* hi = hooking::get_hook_info<ID>();
		cb_t* callback = reinterpret_cast<cb_t*>(hi->hook);

		auto original = reinterpret_cast<OrigFn>(hi->original);

		return (*callback)(original, args...);
	}

private:
	uintptr_t m_base;
	std::map<uint32_t, hook_info> m_hooks;
	bool m_initialized = false;
};
