#pragma once

template <typename T>
T Max(T const &a, T const &b)
{
	return a;
}

//--------------------------------------------------------------------
template <typename T, typename ...ARGS>
T Max(T const &a, T const &b, ARGS ...args)
{
	return Max(a, Max(b, args...));
}