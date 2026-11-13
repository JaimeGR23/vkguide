#pragma once

#include <Types.h>

#include <functional>

// stores cleanup callbacks and runs them in reverse order, so objects die in the opposite order they were created
class DeletionQueue
{
public:
	DeletionQueue();

	void push_function(std::function<void()>&& function);
	void flush();

private:
	vector<std::function<void()>> deletors;
};