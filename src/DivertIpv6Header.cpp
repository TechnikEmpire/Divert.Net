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

#include "DivertIpv6Header.hpp"

namespace Divert
{
	namespace Net
	{

		Ipv6Header::Ipv6Header()
		{
			Init();
		}

		Ipv6Header::~Ipv6Header()
		{
			this->!Ipv6Header();
		}

		Ipv6Header::!Ipv6Header()
		{
			if (m_lastDstAddr != nullptr)
			{
				delete[] m_lastDstAddr;
			}

			if (m_lastSrcAddr != nullptr)
			{
				delete[] m_lastSrcAddr;
			}
		}

		Ipv6Header::Ipv6Header(PWINDIVERT_IPV6HDR ipv6Header)
		{
			#ifndef NDEBUG
			System::Diagnostics::Debug::Assert(ipv6Header != nullptr, u8"In Ipv6Header::Ipv6Header(PWINDIVERT_IPV6HDR) - nullptr provided to constructor expecting non-null pointer argument.");
			#endif

			Init();
			m_ipv6Header = ipv6Header;
		}

		uint32_t Ipv6Header::Version::get()
		{
			if (m_ipv6Header != nullptr)
			{
				return m_ipv6Header->Version;
			}

			return 0;
		}

		void Ipv6Header::Version::set(uint32_t value)
		{
			if (m_ipv6Header != nullptr)
			{
				m_ipv6Header->Version = value;
			}
		}

		uint32_t Ipv6Header::TrafficClass::get()
		{
			if (m_ipv6Header != nullptr)
			{
				return WINDIVERT_IPV6HDR_GET_TRAFFICCLASS(m_ipv6Header);
			}

			return 0;
		}

		void Ipv6Header::TrafficClass::set(uint32_t value)
		{
			if (m_ipv6Header != nullptr)
			{
				WINDIVERT_IPV6HDR_SET_TRAFFICCLASS(m_ipv6Header, value);
			}
		}

		uint32_t Ipv6Header::FlowLabel::get()
		{
			if (m_ipv6Header != nullptr)
			{
				return WINDIVERT_IPV6HDR_GET_FLOWLABEL(m_ipv6Header);
			}

			return 0;
		}

		void Ipv6Header::FlowLabel::set(uint32_t value)
		{
			if (m_ipv6Header != nullptr)
			{
				WINDIVERT_IPV6HDR_SET_FLOWLABEL(m_ipv6Header, value);
			}
		}

		System::Byte Ipv6Header::NextHeader::get()
		{
			if (m_ipv6Header != nullptr)
			{
				return m_ipv6Header->NextHdr;
			}

			return 0;
		}

		void Ipv6Header::NextHeader::set(System::Byte value)
		{
			if (m_ipv6Header != nullptr)
			{
				m_ipv6Header->NextHdr = value;
			}
		}

		System::Byte Ipv6Header::HopLimit::get()
		{
			if (m_ipv6Header != nullptr)
			{
				return m_ipv6Header->HopLimit;
			}

			return 0;
		}

		void Ipv6Header::HopLimit::set(System::Byte value)
		{
			if (m_ipv6Header != nullptr)
			{
				m_ipv6Header->HopLimit = value;
			}
		}

		System::Net::IPAddress^ Ipv6Header::SourceAddress::get()
		{
			if (m_ipv6Header != nullptr)
			{
				if (m_lastSrcAddr[0] != m_ipv6Header->SrcAddr[0] ||
					m_lastSrcAddr[1] != m_ipv6Header->SrcAddr[1] ||
					m_lastSrcAddr[2] != m_ipv6Header->SrcAddr[2] ||
					m_lastSrcAddr[3] != m_ipv6Header->SrcAddr[3]
					)
				{
					array<System::Byte>^ ipArray = gcnew array<System::Byte>(4);
					ipArray[0] = m_ipv6Header->SrcAddr[0];
					ipArray[1] = m_ipv6Header->SrcAddr[1];
					ipArray[2] = m_ipv6Header->SrcAddr[2];
					ipArray[3] = m_ipv6Header->SrcAddr[3];

					m_sourceAddress = gcnew System::Net::IPAddress(ipArray);

					m_lastSrcAddr[0] = m_ipv6Header->SrcAddr[0];
					m_lastSrcAddr[1] = m_ipv6Header->SrcAddr[1];
					m_lastSrcAddr[2] = m_ipv6Header->SrcAddr[2];
					m_lastSrcAddr[3] = m_ipv6Header->SrcAddr[3];
				}
			}

			return m_sourceAddress;
		}

		void Ipv6Header::SourceAddress::set(System::Net::IPAddress^ value)
		{
			array<System::Byte>^ ipArray = value->GetAddressBytes();

			m_lastSrcAddr[0] = ipArray[0];
			m_lastSrcAddr[1] = ipArray[1];
			m_lastSrcAddr[2] = ipArray[2];
			m_lastSrcAddr[3] = ipArray[3];

			if (m_ipv6Header != nullptr)
			{
				m_ipv6Header->SrcAddr[0] = m_lastSrcAddr[0];
				m_ipv6Header->SrcAddr[1] = m_lastSrcAddr[1];
				m_ipv6Header->SrcAddr[2] = m_lastSrcAddr[2];
				m_ipv6Header->SrcAddr[3] = m_lastSrcAddr[3];
			}			

			m_sourceAddress = value;
		}

		System::Net::IPAddress^ Ipv6Header::DestinationAddress::get()
		{
			if (m_ipv6Header != nullptr)
			{
				if (m_lastDstAddr[0] != m_ipv6Header->DstAddr[0] ||
					m_lastDstAddr[1] != m_ipv6Header->DstAddr[1] ||
					m_lastDstAddr[2] != m_ipv6Header->DstAddr[2] ||
					m_lastDstAddr[3] != m_ipv6Header->DstAddr[3]
					)
				{
					array<System::Byte>^ ipArray = gcnew array<System::Byte>(4);
					ipArray[0] = m_ipv6Header->DstAddr[0];
					ipArray[1] = m_ipv6Header->DstAddr[1];
					ipArray[2] = m_ipv6Header->DstAddr[2];
					ipArray[3] = m_ipv6Header->DstAddr[3];

					m_destinationAddress = gcnew System::Net::IPAddress(ipArray);

					m_lastDstAddr[0] = m_ipv6Header->DstAddr[0];
					m_lastDstAddr[1] = m_ipv6Header->DstAddr[1];
					m_lastDstAddr[2] = m_ipv6Header->DstAddr[2];
					m_lastDstAddr[3] = m_ipv6Header->DstAddr[3];
				}
			}

			return m_destinationAddress;
		}

		void Ipv6Header::DestinationAddress::set(System::Net::IPAddress^ value)
		{
			array<System::Byte>^ ipArray = value->GetAddressBytes();

			m_lastDstAddr[0] = ipArray[0];
			m_lastDstAddr[1] = ipArray[1];
			m_lastDstAddr[2] = ipArray[2];
			m_lastDstAddr[3] = ipArray[3];

			if (m_ipv6Header != nullptr)
			{
				m_ipv6Header->DstAddr[0] = m_lastDstAddr[0];
				m_ipv6Header->DstAddr[1] = m_lastDstAddr[1];
				m_ipv6Header->DstAddr[2] = m_lastDstAddr[2];
				m_ipv6Header->DstAddr[3] = m_lastDstAddr[3];
			}

			m_destinationAddress = value;
		}

		PWINDIVERT_IPV6HDR Ipv6Header::GetUnmanagedIpv6Header()
		{
			return m_ipv6Header;
		}

		void Ipv6Header::Init()
		{
			m_lastDstAddr = new UINT32[4];
			m_lastSrcAddr = new UINT32[4];

			if (m_lastDstAddr == nullptr)
			{
				System::Exception^ err = gcnew System::Exception(u8"In Ipv6Header::Init() - Failed to allocate new array UINT32[4] for member m_lastDstAddr.");
				throw err;
			}

			if (m_lastSrcAddr == nullptr)
			{
				System::Exception^ err = gcnew System::Exception(u8"In Ipv6Header::Init() - Failed to allocate new array UINT32[4] for member m_lastSrcAddr.");
				throw err;
			}

			m_sourceAddress = System::Net::IPAddress::Any;
			m_destinationAddress = System::Net::IPAddress::Any;
		}

	} /* namespace Net */
} /* namespace Divert */
