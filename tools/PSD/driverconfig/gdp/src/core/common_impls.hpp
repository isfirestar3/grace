#pragma once
#pragma execution_character_set("utf-8")
#include "os_util.hpp"
#include "log.h"
#include <QMultiMap>
#include <QApplication>
#include <vector>

namespace gdp
{
	namespace core
	{
		template <typename T>
		void qListFree(QList<T> &list)
		{
			QList<T> tmpList;
			list.swap(tmpList);
		}

		template <typename Key, typename Value>
		void qMapFree(QMap<Key, Value> &map)
		{
			QMap<Key, Value> tmpMap;
			map.swap(tmpMap);
		}

		template <typename Key, typename Value>
		void qMultiMapFree(QMultiMap<Key, Value> &map)
		{
			QMultiMap<Key, Value> tmpMap;
			map.swap(tmpMap);
		}

		template <typename T>
		void std_vector_free(std::vector<T> &vec)
		{
			std::vector<T> tmpVec;
			tmpVec.swap(vec);
		}

		template <typename T>
		T average(const std::vector<T> &vec)
		{
			T sum = 0;

			for (auto &v : vec)
			{
				sum += v;
			}

			return sum / (T)vec.size();
		}
	}
}