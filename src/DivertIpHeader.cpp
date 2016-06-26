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

#include "DivertIPHeader.hpp"

namespace Divert
{
	namespace Net
	{

		IPHeader::IPHeader()
		{
			Init();
		}

		IPHeader::~IPHeader()
		{
			this->!IPHeader();
		}

		IPHeader::!IPHeader()
		{
			if (m_ipHeader != nullptr)
			{
				// This pointer is provided by WinDivert and is not ours to manage.
				m_ipHeader = nullptr;
			}
		}

		IPHeader::IPHeader(PWINDIVERT_IPHDR ipHeader)
		{
			#ifndef NDEBUG
			System::Diagnostics::Debug::Assert(ipHeader != nullptr, u8"In IPHeader::IPHeader(PWINDIVERT_IPHDR) - nullptr provided to constructor expecting non-null pointer argument.");
			#endif

			Init();

			m_ipHeader = ipHeader;
		}

		System::Byte IPHeader::HeaderLength::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->HdrLength;				
			}

			return 0;
		}

		void IPHeader::HeaderLength::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->HdrLength = value;
			}
		}

		System::Byte IPHeader::Version::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Version;
			}

			return 0;
		}

		void IPHeader::Version::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Version = value;
			}
		}

		System::Byte IPHeader::TOS::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->TOS;
			}

			return 0;
		}

		void IPHeader::TOS::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->TOS = value;
			}
		}

		uint16_t IPHeader::Length::get()
		{
			if (m_ipHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(m_ipHeader->Length));				
			}

			return 0;
		}

		void IPHeader::Length::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Length = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(value));
			}
		}

		uint16_t IPHeader::Id::get()
		{
			if (m_ipHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(m_ipHeader->Id));				
			}

			return 0;
		}

		void IPHeader::Id::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Id = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(value));
			}
		}

		uint16_t IPHeader::FragOff::get()
		{
			if (m_ipHeader != nullptr)
			{
				auto value = WINDIVERT_IPHDR_GET_FRAGOFF(m_ipHeader);
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(value));
			}

			return 0;
		}

		void IPHeader::FragOff::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				auto modifiedValue = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(value));
				WINDIVERT_IPHDR_SET_FRAGOFF(m_ipHeader, modifiedValue);
			}
		}

		uint16_t IPHeader::MF::get()
		{
			if (m_ipHeader != nullptr)
			{
				auto value = WINDIVERT_IPHDR_GET_MF(m_ipHeader);
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(value));
			}

			return 0;
		}

		void IPHeader::MF::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				auto modifiedValue = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(value));
				WINDIVERT_IPHDR_SET_MF(m_ipHeader, modifiedValue);
			}
		}

		uint16_t IPHeader::DF::get()
		{
			if (m_ipHeader != nullptr)
			{
				auto value = WINDIVERT_IPHDR_GET_DF(m_ipHeader);
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(value));				
			}

			return 0;
		}

		void IPHeader::DF::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				auto modifiedValue = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(value));
				WINDIVERT_IPHDR_SET_DF(m_ipHeader, modifiedValue);
			}
		}

		uint16_t IPHeader::Reserved::get()
		{
			if (m_ipHeader != nullptr)
			{
				auto value = WINDIVERT_IPHDR_GET_RESERVED(m_ipHeader);
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(value));				
			}

			return 0;
		}

		void IPHeader::Reserved::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				auto modifiedValue = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(value));
				WINDIVERT_IPHDR_SET_RESERVED(m_ipHeader, modifiedValue);
			}
		}

		System::Byte IPHeader::TTL::get()
		{
			if (m_ipHeader != nullptr)
			{				
				return m_ipHeader->TTL;
			}

			return 0;
		}

		void IPHeader::TTL::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->TTL = value;
			}
		}

		System::Byte IPHeader::Protocol::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Protocol;
			}

			return 0;
		}

		void IPHeader::Protocol::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Protocol = value;
			}
		}

		uint16_t IPHeader::Checksum::get()
		{
			if (m_ipHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(m_ipHeader->Checksum));				
			}

			return 0;
		}

		void IPHeader::Checksum::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Checksum = static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(value));
			}
		}

		System::Net::IPAddress^ IPHeader::SourceAddress::get()
		{
			if (m_ipHeader != nullptr)
			{
				if (m_ipHeader->SrcAddr != m_lastSrcAddr && ((m_ipHeader->SrcAddr > 0 && m_ipHeader->SrcAddr <= 0x00000000FFFFFFFF)))
				{
					m_sourceAddress = gcnew System::Net::IPAddress(static_cast<long long>(m_ipHeader->SrcAddr));
					m_lastSrcAddr = m_ipHeader->SrcAddr;
				}
			}

			return m_sourceAddress;
		}

		void IPHeader::SourceAddress::set(System::Net::IPAddress^ value)
		{
			UINT32 intAddress = System::BitConverter::ToInt32(value->GetAddressBytes(), 0);

			if (m_ipHeader != nullptr)		
			{
				
				m_ipHeader->SrcAddr = intAddress;				
			}

			m_sourceAddress = value;
			m_lastSrcAddr = intAddress;
		}

		System::Net::IPAddress^ IPHeader::DestinationAddress::get()
		{
			if (m_ipHeader != nullptr)
			{
				if (m_ipHeader->DstAddr != m_lastDstAddr && ((m_ipHeader->SrcAddr > 0 && m_ipHeader->SrcAddr <= 0x00000000FFFFFFFF)))
				{
					m_destinationAddress = gcnew System::Net::IPAddress(static_cast<long long>(m_ipHeader->DstAddr));
					m_lastDstAddr = m_ipHeader->DstAddr;
				}
			}

			return m_destinationAddress;
		}

		void IPHeader::DestinationAddress::set(System::Net::IPAddress^ value)
		{
			UINT32 intAddress = System::BitConverter::ToInt32(value->GetAddressBytes(), 0);

			if (m_ipHeader != nullptr)
			{

				m_ipHeader->DstAddr = intAddress;
			}

			m_destinationAddress = value;
			m_lastDstAddr = intAddress;
		}

		bool IPHeader::Valid::get()
		{
			return UnmanagedHeader != nullptr;
		}

		PWINDIVERT_IPHDR IPHeader::UnmanagedHeader::get()
		{
			return m_ipHeader;
		}

		void IPHeader::UnmanagedHeader::set(PWINDIVERT_IPHDR value)
		{
			m_ipHeader = value;
		}

		void IPHeader::Init()
		{
			m_sourceAddress = System::Net::IPAddress::Any;
			m_destinationAddress = System::Net::IPAddress::Any;
		}

	} /* namespace Net */
} /* namespace Divert */
