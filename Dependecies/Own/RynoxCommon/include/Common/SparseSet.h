#pragma once

#include <memory>
#include <limits>

namespace Rynox::Common
{
	class SparseSet
	{
	public:
		using value_type = uint32_t;
		using size_type = uint32_t;

		static constexpr size_type NULL_INDEX = std::numeric_limits<value_type>::max();

		SparseSet();
		~SparseSet();

		value_type Insert(value_type id);
		void Remove(value_type id);
		bool Contains(value_type id) const;

		value_type GetIndex(value_type id) const;

		void Clear();
		
		value_type* begin() { return m_Dense; }
		const value_type* begin() const { return m_Dense; }
		const value_type* cbegin() const { return m_Dense; }

		value_type* end() { return m_Dense + m_DenseSize; }
		const value_type* end() const { return m_Dense + m_DenseSize; }
		const value_type* cend() const { return m_Dense + m_DenseSize; }

	private:
		template<typename T>
		using Alloc = std::allocator<T>;

		template<typename T>
		using Traits = std::allocator_traits<Alloc<T>>;

		void ReallocateSparse(value_type newSize);
		void ReallocateDense(value_type newCapacity);

		void GrowSparse(value_type desired = 0);
		void GrowDense(value_type desired = 0);

	private:
		size_type* m_Sparse = nullptr;
		size_type m_SparseSize = 0;

		value_type* m_Dense = nullptr;
		size_type m_DenseSize = 0;
		size_type m_DenseCapacity = 0;

		Alloc<size_type> m_SparseAlloc;
		Alloc<value_type> m_DenseAlloc;
	};
}
