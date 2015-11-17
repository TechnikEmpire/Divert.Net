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

#include "DivertIpHeader.hpp"

namespace Divert
{
	namespace Net
	{

		IpHeader::IpHeader()
		{
			Init();
		}

		IpHeader::~IpHeader()
		{
			this->!IpHeader();
		}

		IpHeader::!IpHeader()
		{
			if (m_ipHeader != nullptr)
			{
				// This pointer is provided by WinDivert and is not ours to manage.
				m_ipHeader = nullptr;
			}
		}

		IpHeader::IpHeader(PWINDIVERT_IPHDR ipHeader)
		{
			#ifndef NDEBUG
			System::Diagnostics::Debug::Assert(ipHeader != nullptr, u8"In IpHeader::IpHeader(PWINDIVERT_IPHDR) - nullptr provided to constructor expecting non-null pointer argument.");
			#endif

			Init();

			m_ipHeader = ipHeader;
		}

		System::Byte IpHeader::HeaderLength::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->HdrLength;				
			}

			return 0;
		}

		void IpHeader::HeaderLength::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->HdrLength = value;
			}
		}

		System::Byte IpHeader::Version::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Version;
			}

			return 0;
		}

		void IpHeader::Version::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Version = value;
			}
		}

		System::Byte IpHeader::TOS::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->TOS;
			}

			return 0;
		}

		void IpHeader::TOS::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->TOS = value;
			}
		}

		uint16_t IpHeader::Length::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Length;
			}

			return 0;
		}

		void IpHeader::Length::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Length = value;
			}
		}

		System::Byte IpHeader::Id::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Id;
			}

			return 0;
		}

		void IpHeader::Id::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Id = value;
			}
		}

		uint16_t IpHeader::FragOff::get()
		{
			if (m_ipHeader != nullptr)
			{
				return WINDIVERT_IPHDR_GET_FRAGOFF(m_ipHeader);
			}

			return 0;
		}

		void IpHeader::FragOff::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				WINDIVERT_IPHDR_SET_FRAGOFF(m_ipHeader, value);
			}
		}

		uint16_t IpHeader::MF::get()
		{
			if (m_ipHeader != nullptr)
			{
				return WINDIVERT_IPHDR_GET_MF(m_ipHeader);
			}

			return 0;
		}

		void IpHeader::MF::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				WINDIVERT_IPHDR_SET_MF(m_ipHeader, value);
			}
		}

		uint16_t IpHeader::DF::get()
		{
			if (m_ipHeader != nullptr)
			{
				return WINDIVERT_IPHDR_GET_DF(m_ipHeader);
			}

			return 0;
		}

		void IpHeader::DF::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				WINDIVERT_IPHDR_SET_DF(m_ipHeader, value);
			}
		}

		uint16_t IpHeader::Reserved::get()
		{
			if (m_ipHeader != nullptr)
			{
				return WINDIVERT_IPHDR_GET_RESERVED(m_ipHeader);
			}

			return 0;
		}

		void IpHeader::Reserved::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				WINDIVERT_IPHDR_SET_RESERVED(m_ipHeader, value);
			}
		}

		System::Byte IpHeader::TTL::get()
		{
			if (m_ipHeader != nullptr)
			{				
				return m_ipHeader->TTL;
			}

			return 0;
		}

		void IpHeader::TTL::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->TTL = value;
			}
		}

		System::Byte IpHeader::Protocol::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Protocol;
			}

			return 0;
		}

		void IpHeader::Protocol::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Protocol = value;
			}
		}

		uint16_t IpHeader::Checksum::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Checksum;
			}

			return 0;
		}

		void IpHeader::Checksum::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Checksum = value;				
			}
		}

		System::Net::IPAddress^ IpHeader::SourceAddress::get()
		{
			if (m_ipHeader != nullptr)
			{
				if (m_ipHeader->SrcAddr != m_lastSrcAddr)
				{
					m_sourceAddress = gcnew System::Net::IPAddress(m_ipHeader->SrcAddr);
					m_lastSrcAddr = m_ipHeader->SrcAddr;
				}
			}

			return m_sourceAddress;
		}

		void IpHeader::SourceAddress::set(System::Net::IPAddress^ value)
		{
			UINT32 intAddress = System::BitConverter::ToInt32(value->GetAddressBytes(), 0);

			if (m_ipHeader != nullptr)		
			{
				
				m_ipHeader->SrcAddr = intAddress;				
			}

			m_sourceAddress = value;
			m_lastSrcAddr = intAddress;
		}

		System::Net::IPAddress^ IpHeader::DestinationAddress::get()
		{
			if (m_ipHeader != nullptr)
			{
				if (m_ipHeader->DstAddr != m_lastDstAddr)
				{
					m_destinationAddress = gcnew System::Net::IPAddress(m_ipHeader->DstAddr);
					m_lastDstAddr = m_ipHeader->DstAddr;
				}
			}

			return m_destinationAddress;
		}

		void IpHeader::DestinationAddress::set(System::Net::IPAddress^ value)
		{
			UINT32 intAddress = System::BitConverter::ToInt32(value->GetAddressBytes(), 0);

			if (m_ipHeader != nullptr)
			{

				m_ipHeader->DstAddr = intAddress;
			}

			m_destinationAddress = value;
			m_lastDstAddr = intAddress;
		}

		PWINDIVERT_IPHDR IpHeader::GetUnmanagedIpHeader()
		{
			return m_ipHeader;
		}

		void IpHeader::Init()
		{
			m_sourceAddress = System::Net::IPAddress::Any;
			m_destinationAddress = System::Net::IPAddress::Any;
		}

	} /* namespace Net */
} /* namespace Divert */
