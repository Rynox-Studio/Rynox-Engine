#include "Common/SparseSet.h"

namespace Rynox::Common
{
	SparseSet::SparseSet()
	{
	}

	SparseSet::~SparseSet()
	{
		if (m_Sparse)
		{
			Traits<size_type>::deallocate(m_SparseAlloc, m_Sparse, m_SparseSize);
			m_Sparse = nullptr;
			m_SparseSize = 0;
		}

		if (m_Dense)
		{
			Traits<value_type>::deallocate(m_DenseAlloc, m_Dense, m_DenseCapacity);
			m_Dense = nullptr;
			m_DenseSize = 0;
			m_DenseCapacity = 0;
		}
	}

	SparseSet::value_type SparseSet::Insert(value_type id)
	{
		if (id >= m_SparseSize)
		{
			GrowSparse(id);
		}

		if (m_Sparse[id] == NULL_INDEX)
		{
			if (m_DenseSize == m_DenseCapacity)
			{
				GrowDense();
			}

			m_Dense[m_DenseSize] = id;
			m_Sparse[id] = m_DenseSize;
			m_DenseSize++;
		}
		return m_Sparse[id];
	}

	void SparseSet::Remove(value_type id)
	{
		if (Contains(id))
		{
			auto di = m_Sparse[id];
			if (di != m_Dense[m_DenseSize - 1])
			{
				std::swap(m_Dense[di], m_Dense[m_DenseSize - 1]);
			}
			m_Sparse[id] = NULL_INDEX;
			m_DenseSize--;
		}
	}

	bool SparseSet::Contains(value_type id) const
	{
		return (id < m_SparseSize &&
			m_Sparse[id] != NULL_INDEX &&
			m_Sparse[id] < m_DenseSize &&
			m_Dense[m_Sparse[id]] == id
			);
	}

	SparseSet::value_type SparseSet::GetIndex(value_type id) const
	{
		if (Contains(id))
		{
			return m_Sparse[id];
		}
		return NULL_INDEX;
	}

	void SparseSet::Clear()
	{
		if (m_Sparse)
		{
			Traits<value_type>::deallocate(m_SparseAlloc, m_Sparse, m_SparseSize);
			m_Sparse = nullptr;
			m_SparseSize = 0;
		}
	}

	void SparseSet::ReallocateSparse(value_type newSize)
	{
		if (m_SparseSize >= newSize) return;

		size_type* newSparse = Traits<size_type>::allocate(m_SparseAlloc, newSize);
		if (m_Sparse)
		{
			std::uninitialized_copy_n(m_Sparse, m_SparseSize, newSparse);
			Traits<size_type>::deallocate(m_SparseAlloc, m_Sparse, m_SparseSize);
		}

		std::uninitialized_fill_n(newSparse + m_SparseSize, newSize - m_SparseSize, NULL_INDEX);

		m_Sparse = newSparse;
		m_SparseSize = newSize;
	}

	void SparseSet::ReallocateDense(value_type newCapacity)
	{
		if (m_DenseCapacity >= newCapacity) return;

		value_type* newDense = Traits<value_type>::allocate(m_DenseAlloc, newCapacity);
		if (m_Dense)
		{
			std::uninitialized_copy_n(m_Dense, m_DenseSize, newDense);
			Traits<value_type>::deallocate(m_DenseAlloc, m_Dense, m_DenseCapacity);
		}

		m_Dense = newDense;
		m_DenseCapacity = newCapacity;
	}

	void SparseSet::GrowSparse(value_type desired)
	{
		ReallocateSparse(m_SparseSize == 0 ? 8 : (m_SparseSize * 2 > desired ? m_SparseSize * 2 : desired));
	}

	void SparseSet::GrowDense(value_type desired)
	{
		ReallocateDense(m_SparseSize == 0 ? 8 : (m_SparseSize * 2 > desired ? m_SparseSize * 2 : desired));
	}
}
