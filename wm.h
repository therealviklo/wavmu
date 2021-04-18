#pragma once
#include <stack>
#include <memory>
#include "window.h"

class WindowStack
{
private:
	std::stack<std::unique_ptr<Window>> stack;
public:
	void push(std::unique_ptr<Window> w);
	void pop() noexcept;

	bool empty() const noexcept { return stack.empty(); }
	Window& top() noexcept { return *stack.top(); }
};