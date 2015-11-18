/*
* The MIT License (MIT)
*
* Copyright (c) 2015 Jesse Nicholson
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "DivertAddress.hpp"

namespace Divert
{
	namespace Net
	{
		Address::Address()
		{
			
		}

		Address::~Address()
		{
			this->!Address();
		}

		Address::!Address()
		{
			if (m_address != nullptr)
			{
				// This pointer is provided by WinDivert and is not ours to manage.
				m_address = nullptr;
			}
		}

		Address::Address(PWINDIVERT_ADDRESS address)
		{			
			#ifndef NDEBUG
			System::Diagnostics::Debug::Assert(address != nullptr, u8"In Address::Address(PWINDIVERT_ADDRESS) - nullptr provided to constructor expecting non-null pointer argument.");
			#endif

			m_address = address;
		}

		uint32_t Address::InterfaceIndex::get()
		{
			if (m_address != nullptr)
			{				
				return m_address->IfIdx;				
			}

			return 0;
		}

		void Address::InterfaceIndex::set(uint32_t value)
		{
			if (m_address != nullptr)
			{
				m_address->IfIdx = value;
			}
		}

		uint32_t Address::SubInterfaceIndex::get()
		{
			if (m_address != nullptr)
			{
				return m_address->SubIfIdx;
			}

			return 0;
		}

		void Address::SubInterfaceIndex::set(uint32_t value)
		{
			if (m_address != nullptr)
			{
				m_address->SubIfIdx = value;
			}
		}

		DivertDirection Address::Direction::get()
		{
			if (m_address != nullptr)
			{
				return static_cast<DivertDirection>(m_address->Direction);
			}

			return DivertDirection::Inbound;
		}

		void Address::Direction::set(DivertDirection value)
		{
			if (m_address != nullptr)
			{
				m_address->Direction = static_cast<uint8_t>(value);
			}
		}

		PWINDIVERT_ADDRESS Address::UnmanagedAddress::get()
		{
			return m_address;
		}

		void Address::UnmanagedAddress::set(PWINDIVERT_ADDRESS value)
		{
			m_address = value;
		}

	} /* namespace Net */
} /* namespace Divert  */
