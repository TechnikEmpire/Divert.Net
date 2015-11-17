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

		DivertIpHeader::DivertIpHeader()
		{
			m_sourceAddress = System::Net::IPAddress::Any;
			m_destinationAddress = System::Net::IPAddress::Any;
		}

		DivertIpHeader::~DivertIpHeader()
		{
			this->!DivertIpHeader();
		}

		DivertIpHeader::!DivertIpHeader()
		{
			if (m_ipHeader != nullptr)
			{
				delete m_ipHeader;
			}
		}

		DivertIpHeader::DivertIpHeader(PWINDIVERT_IPHDR ipheader)
		{
			m_ipHeader = ipheader;
		}

		System::Byte DivertIpHeader::HeaderLength::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->HdrLength;				
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::HeaderLength::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->HdrLength = value;
			}
		}

		System::Byte DivertIpHeader::Version::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Version;
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::Version::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Version = value;
			}
		}

		System::Byte DivertIpHeader::TOS::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->TOS;
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::TOS::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->TOS = value;
			}
		}

		uint16_t DivertIpHeader::Length::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Length;
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::Length::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Length = value;
			}
		}

		System::Byte DivertIpHeader::Id::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Id;
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::Id::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Id = value;
			}
		}

		uint16_t DivertIpHeader::FragOff::get()
		{
			if (m_ipHeader != nullptr)
			{
				return WINDIVERT_IPHDR_GET_FRAGOFF(m_ipHeader);
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::FragOff::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				WINDIVERT_IPHDR_SET_FRAGOFF(m_ipHeader, value);
			}
		}

		uint16_t DivertIpHeader::MF::get()
		{
			if (m_ipHeader != nullptr)
			{
				return WINDIVERT_IPHDR_GET_MF(m_ipHeader);
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::MF::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				WINDIVERT_IPHDR_SET_MF(m_ipHeader, value);
			}
		}

		uint16_t DivertIpHeader::DF::get()
		{
			if (m_ipHeader != nullptr)
			{
				return WINDIVERT_IPHDR_GET_DF(m_ipHeader);
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::DF::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				WINDIVERT_IPHDR_SET_DF(m_ipHeader, value);
			}
		}

		uint16_t DivertIpHeader::Reserved::get()
		{
			if (m_ipHeader != nullptr)
			{
				return WINDIVERT_IPHDR_GET_RESERVED(m_ipHeader);
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::Reserved::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				WINDIVERT_IPHDR_SET_RESERVED(m_ipHeader, value);
			}
		}

		System::Byte DivertIpHeader::TTL::get()
		{
			if (m_ipHeader != nullptr)
			{				
				return m_ipHeader->TTL;
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::TTL::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->TTL = value;
			}
		}

		System::Byte DivertIpHeader::Protocol::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Protocol;
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::Protocol::set(System::Byte value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Protocol = value;
			}
		}

		uint16_t DivertIpHeader::Checksum::get()
		{
			if (m_ipHeader != nullptr)
			{
				return m_ipHeader->Checksum;
			}
			else
			{
				return 0;
			}
		}

		void DivertIpHeader::Checksum::set(uint16_t value)
		{
			if (m_ipHeader != nullptr)
			{
				m_ipHeader->Checksum = value;				
			}
		}

		System::Net::IPAddress^ DivertIpHeader::SourceAddress::get()
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

		void DivertIpHeader::SourceAddress::set(System::Net::IPAddress^ value)
		{
			UINT32 intAddress = System::BitConverter::ToInt32(value->GetAddressBytes(), 0);

			if (m_ipHeader != nullptr)		
			{
				
				m_ipHeader->SrcAddr = intAddress;				
			}

			m_sourceAddress = value;
			m_lastSrcAddr = intAddress;
		}

		System::Net::IPAddress^ DivertIpHeader::DestinationAddress::get()
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

		void DivertIpHeader::DestinationAddress::set(System::Net::IPAddress^ value)
		{
			UINT32 intAddress = System::BitConverter::ToInt32(value->GetAddressBytes(), 0);

			if (m_ipHeader != nullptr)
			{

				m_ipHeader->DstAddr = intAddress;
			}

			m_destinationAddress = value;
			m_lastDstAddr = intAddress;
		}

		PWINDIVERT_IPHDR DivertIpHeader::GetUnmanagedIpHeader()
		{
			return m_ipHeader;
		}

	} /* namespace Net */
} /* namespace Divert */
