#include "MemManager.h"
#include "FastList.h"
#include <algorithm>
#include <cassert>

namespace htm {

extern MemManager mem_manager;

FastListTray::FastListTray()
: pointer(nullptr), prev(nullptr), next(nullptr)
{
}

void FastListTray::Initialize()
{
	pointer = nullptr;
	prev = nullptr;
	next = nullptr;
}

FastList::FastList()
{
	Initialize();
}

void FastList::Initialize()
{
	count = 0;
	iterator_cur = nullptr;
	first = nullptr;
	last = nullptr;
}

void FastList::Clear()
{
	FastListTray *cur;

	// Release all trays in the list
	while ((cur = first) != nullptr)
	{
		// Record pointer to next tray in list
		first = cur->next;

		// Release the current tray
		mem_manager.ReleaseObject(cur);
	}

	// Reset variables
	last = nullptr;
	count = 0;
	iterator_cur = nullptr;
}

void FastList::InsertAtStart(void *_new)
{
	assert(_new != nullptr);

	// Get new tray to represent the new item
	FastListTray *new_tray = (FastListTray*)(mem_manager.GetObject(MOT_FAST_LIST_TRAY));
	new_tray->pointer = _new;

	// Insert the new tray at the start of the list
	new_tray->next = first;
	new_tray->prev = nullptr;

	if (first != nullptr) {
		first->prev = new_tray;
	}

	first = new_tray;

	if (last == nullptr) {
		last = new_tray;
	}

	// Increment the count
	count++;
}

void FastList::InsertAtEnd(void *_new)
{
	assert(_new != nullptr);

	// Get new tray to represent the new item
	FastListTray *new_tray = (FastListTray*)(mem_manager.GetObject(MOT_FAST_LIST_TRAY));
	new_tray->pointer = _new;

	// Insert the new tray at the end of the list
	new_tray->next = nullptr;
	new_tray->prev = last;

	if (last != nullptr) {
		last->next = new_tray;
	}

	last = new_tray;

	if (first == nullptr) {
		first = new_tray;
	}

	// Increment the count
	count++;
}

void* FastList::RemoveFirst()
{
	// If there are no items in the list, return nullptr.
	if (first == nullptr) {
		return nullptr;
	}

	// Record pointer to removed tray
	FastListTray *removed_tray = first;

	// Record pointer to removed object
	void *removed_item = removed_tray->pointer;

	// Advance the first pointer
	first = first->next;

	if (first == nullptr) {
		last = nullptr;
	} else {
		first->prev = nullptr;
	}

	// Release the removd tray
	mem_manager.ReleaseObject(removed_tray);

	// Decrease the count
	count--;

	return removed_item;
}

void FastList::Remove(void *_item_to_remove, bool _multiple)
{
	FastListIter iter(this);
	iter.Reset();

	while (iter.Get() != nullptr)
	{
		if (iter.Get() == _item_to_remove) 
		{
			iter.Remove();

			if (!_multiple) {
				return;
			}
		} 
		else 
		{
			iter.Advance();
		}
	}
}

void FastList::TransferContentsTo(FastList &_destination_list)
{
	// The given _destinaton_list must be empty.
	assert(_destination_list.Count() == 0);

	// Transfer this list's member values to the given _destination_list.
	_destination_list.count = count;
	_destination_list.first = first;
	_destination_list.last  = last;
	_destination_list.iterator_cur = iterator_cur;

	// Clear this list's members.
	count = 0;
	first = last = nullptr;
	iterator_cur = nullptr;
}

void FastList::CopyContentsTo(FastList &_destination_list)
{
	// Copy the contents of this list to the given _destination_list.
	FastListIter iter(this);
	for (void *cur = iter.Reset(); cur != nullptr; cur = iter.Advance()) {
		_destination_list.InsertAtEnd(cur);
	}
}

void *FastList::GetFirst()
{
	return (first == nullptr) ? nullptr : first->pointer;
}

void *FastList::GetLast()
{
	return (last == nullptr) ? nullptr : last->pointer;
}

void *FastList::GetByIndex(int _index)
{	
	FastListTray *cur;

	// Iterate through the trays, counting down _index, until _index is 0.
	for (cur = first; _index > 0; _index--)
	{
		// If there is no current tray, return nullptr.
		if (cur == nullptr) {
			return nullptr;
		}

		// Advance to the next tray.
		cur = cur->next;
	}

	// Return the current tray's pointer, or nullptr if there is no current tray.
	return (cur == nullptr) ? nullptr : cur->pointer;
}

bool FastList::IsInList(void *_item)
{
	FastListIter iter(this);
	for (void *cur = iter.Reset(); cur != nullptr; cur = iter.Advance())
	{
		if (cur == _item) {
			return true;
		}
	}

	return false;
}

bool FastList::ListsAreIdentical(FastList &_list_A, FastList &_list_B)
{
	// If the two lists don't have an identical number of items, return false.
	if (_list_A.Count() != _list_B.Count()) {
		return false;
	}

	FastListIter iter_A(&_list_A), iter_B(&_list_B);

	iter_A.Reset();
	iter_B.Reset();

	// Iterate through all corresponding items in both lists...
	while (iter_A.Get() != nullptr)
	{
		// If the current pair of corresponding items are not identical, return false.
		if (iter_A.Get() != iter_B.Get()) {
			return false;
		}

		// Advance both iterators.
		iter_A.Advance();
		iter_B.Advance();
	}

	// The two lists are identical; return true.
	return true;
}

FastListIter::FastListIter(FastList *_list)
: list(_list)
{
}

FastListIter::FastListIter(FastList &_list)
{
	list = &_list;
}

void* FastListIter::Reset()
{
	assert(list != nullptr);

	// Reset the iterator to point to the first item in the list
	iterator_cur = list->first;

	return (iterator_cur == nullptr) ? nullptr : iterator_cur->pointer;
}

void* FastListIter::Reset_Reverse()
{
	assert(list != nullptr);

	// Reset the iterator to point to the last item in the list
	iterator_cur = list->last;

	return (iterator_cur == nullptr) ? nullptr : iterator_cur->pointer;
}

void* FastListIter::Prev()
{
	assert(list != nullptr);

	if (iterator_cur == nullptr) {
		return nullptr;
	}

	// Advance the iterator to the previous item in the list
	iterator_cur = iterator_cur->prev;

  return (iterator_cur == nullptr) ? nullptr : iterator_cur->pointer;
}

void* FastListIter::Advance()
{
	assert(list != nullptr);

	if (iterator_cur == nullptr) {
		return nullptr;
	}

	// Advance the iterator to the next item in the list
	iterator_cur = iterator_cur->next;

  return (iterator_cur == nullptr) ? nullptr : iterator_cur->pointer;
}

void* FastListIter::Get()
{
	assert(list != nullptr);

	return (iterator_cur == nullptr) ? nullptr : iterator_cur->pointer;
}

void* FastListIter::Duplicate(FastListIter &_original)
{
	assert(list == _original.list);

	iterator_cur = _original.iterator_cur;

	return (iterator_cur == nullptr) ? nullptr : iterator_cur->pointer;
}

bool FastListIter::IsFirst()
{
	assert(list != nullptr);

	return (iterator_cur == nullptr) ? false : (iterator_cur == list->first);
}

bool FastListIter::IsLast()
{
	assert(list != nullptr);

	return (iterator_cur == nullptr) ? false : (iterator_cur == list->last);
}

int FastListIter::GetIndex()
{
	assert(list != nullptr);

	if (iterator_cur == nullptr) {
		return -1;
	}

	int index = 0;

	for (FastListTray *cur = list->first; cur != nullptr; cur = cur->next)
	{
		// If the current tray is this iterator's current tray, return the current index.
		if (cur == iterator_cur) {
			return index;
		}

		// Increment the index.
		index++;
	}

	assert(false); // This point should never be reached.
	return -1;
}

void FastListIter::Insert(void *_new)
{
	assert(list != nullptr);

	// If the iterator isn't valid, insert at the end of the list.
	if (iterator_cur == nullptr) 
	{
    list->InsertAtEnd(_new);
		return;
	}

	// Insert the given item at the current position of the iterator.

	// Get new tray to represent the new item
	FastListTray *new_tray = (FastListTray*)(mem_manager.GetObject(MOT_FAST_LIST_TRAY));
	new_tray->pointer = _new;

	new_tray->prev = iterator_cur->prev;
	new_tray->next = iterator_cur;

	if (new_tray->prev == nullptr) {
		list->first = new_tray; 
	} else {
		new_tray->prev->next = new_tray;
	}

	iterator_cur->prev = new_tray;

	// Increment the count
	list->count++;
}

void FastListIter::Remove()
{
	assert(list != nullptr);

	// If the iterator isn't valid, do nothing -- just return.
	if (iterator_cur == nullptr) {
		return;
	}

	if (iterator_cur->prev == nullptr) {
		list->first = iterator_cur->next; 
	} else {
		iterator_cur->prev->next = iterator_cur->next;
	}

	if (iterator_cur->next == nullptr) {
		list->last = iterator_cur->prev; 
	} else {
		iterator_cur->next->prev = iterator_cur->prev;
	}

	// Record pointer to removed tray
	FastListTray *removed_tray = iterator_cur;

	// Advance the iterator to the next tray
	iterator_cur = iterator_cur->next;

	// Release the removed tray
	mem_manager.ReleaseObject(removed_tray);

	// Decrease the count
	list->count--;
}

void FastListIter::SetList(FastList &_list)
{
	list = &_list;

	if (list != nullptr) {
		Reset();
	}
}

void FastListIter::SetList(FastList *_list)
{
	list = _list;

	if (list != nullptr) {
		Reset();
	}
}

};

