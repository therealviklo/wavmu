#include "wm.h"

void WindowStack::push(std::unique_ptr<Window> w)
{
	if (!stack.empty())
	{
		ShowWindow(*stack.top(), SW_HIDE);
	}
	stack.emplace(std::move(w));
	ShowWindow(*stack.top(), SW_SHOW);
}

void WindowStack::pop() noexcept
{
	ShowWindow(*stack.top(), SW_HIDE);
	stack.pop();
	if (!stack.empty())
	{
		ShowWindow(*stack.top(), SW_SHOW);
	}
}