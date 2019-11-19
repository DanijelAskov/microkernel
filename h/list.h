/*
 * Copyright (C) 2017  Danijel Askov
 *
 * This file is part of MicroKernel.
 *
 * MicroKernel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MicroKernel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _LIST_H_
#define _LIST_H_

template<class T>
class LinkedList {
	struct Node {
		T *data;
		Node *next, *previous;
		Node(T *data = 0, Node *next = 0, Node *previous = 0) :
				data(data), next(next), previous(previous) {
		}
	};

	Node *head, *tail;
	unsigned sz;

public:
	class Iterator {
		Node *node;
	public:
		Iterator(Node *node = 0) :
				node(node) {
		}

		Iterator& operator++() {
			node = node->next;
			return *this;
		}

		Iterator operator++(int) {
			Iterator temp(*this);
			node = node->next;
			return temp;
		}

		int operator==(const Iterator &other) const {
			return node == other.node;
		}

		int operator!=(const Iterator &other) const {
			return node != other.node;
		}

		T& operator*() const {
			return *(node->data);
		}

		T* operator->() const {
			return node->data;
		}

		friend class LinkedList<T> ;
	};

	LinkedList() :
			head(new Node()), tail(head), sz(0) {
	}

	~LinkedList();

	int empty() const {
		return sz == 0;
	}

	unsigned size() const {
		return sz;
	}

	Iterator begin() {
		return Iterator(head);
	}

	Iterator end() {
		return Iterator(tail);
	}

	Iterator insert(const Iterator &posititon, T *data) {
		if (posititon == Iterator())
			return Iterator();
		Iterator previous(posititon.node->previous);
		Node *n;
		if (previous != Iterator()) {
			n = new Node(data, posititon.node, previous.node);
			previous.node->next = n;
		} else {
			n = new Node(data, posititon.node);
			head = n;
		}
		posititon.node->previous = n;
		sz++;
		return Iterator(n);
	}

	Iterator erase(const Iterator &posititon) {
		if (posititon == Iterator() || posititon == end())
			return Iterator();
		Iterator previous(posititon.node->previous);
		if (previous != Iterator()) {
			previous.node->next = posititon.node->next;
		} else {
			head = posititon.node->next;
		}
		posititon.node->next->previous = previous.node;
		Iterator result(posititon.node->next);
		delete posititon.node;
		sz--;
		return result;
	}

	Iterator f();
};

template<class T>
LinkedList<T>::~LinkedList() {
	Node *old = 0;
	while (head != 0) {
		old = head;
		head = head->next;
		delete old;
	}
	tail = 0;
	sz = 0;
}

#endif // _LIST_H_