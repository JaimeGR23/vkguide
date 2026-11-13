#include <DeletionQueue.h>

DeletionQueue::DeletionQueue() :
	deletors()
{

}

void DeletionQueue::push_function(std::function<void()>&& function)
{
	deletors.push_back(std::move(function));
}

void DeletionQueue::flush()
{
	// Last created, first destroyed
	for (auto it = deletors.rbegin(); it != deletors.rend(); ++it)
	{
		(*it)();
	}
	deletors.clear();
}