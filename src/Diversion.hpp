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

#pragma once

#include "DivertAddress.hpp"
#include "DivertHandle.hpp"
#include "DivertIpHeader.hpp"
#include "DivertIpv6Header.hpp"
#include "DivertICMPHeader.hpp"
#include "DivertICMPv6Header.hpp"
#include "DivertTCPHeader.hpp"
#include "DivertUDPHeader.hpp"
#include "DivertAsyncResult.hpp"

#using <mscorlib.dll>

#pragma comment(lib, "WinDivert.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "psapi.lib")

namespace Divert
{
	namespace Net
	{
		public enum class DivertLayer
		{		
			Network = 0,
			NetworkForward = 1
		};

		[System::Flags]
		public enum class FilterFlags : uint64_t
		{
			[System::Obsolete(u8"As of WinDivert 1.2, the NoChecksum attribute is deprecated, because the default behavior of the library is now to no longer automatically calculate checksums.")]
			NoChecksum = 0,
			Sniff = 1,
			Drop = 2
		};

		public ref class Diversion
		{

		public:
			
			/// <summary>
			/// Create a new Diversion instance. A Diversion instance will be created according to
			/// the supplied arguments. This function is not guaranteed to succeed and will throw if
			/// incorrect arguments are supplied, or if an external error occurred in the driver or
			/// native library during execution.
			/// 
			/// If an error occurs which this library is aware of internally, a correctly formatted
			/// System::Exception will be thrown with detailed information about the error. However,
			/// if an unknown exception occurs, use Marshal.GetLastWin32Error to check the error
			/// code, from which more detailed information about the exception can be extrapolated.
			/// Absolutely do not use .NET provided GetLastError() function, you must use the
			/// Marshal version to get accurate results.
			/// </summary>
			/// <param name="filter">
			/// The filtering string that defines what type of traffic should be captured. More
			/// information here: https://reqrypt.org/windivert-doc.html#filter_language
			/// </param>
			/// <param name="layer">
			/// The network layer to operate on. 
			/// </param>
			/// <param name="priority">
			/// The priority of the filter. Packets are diverted to higher priority handles before
			/// lower priority handles. More information here:
			/// https://reqrypt.org/windivert-doc.html#divert_open
			/// </param>
			/// <param name="flags">
			/// Flags that modify the filtering mode. 
			/// </param>
			/// <returns>
			/// If the operation succeeds, a valid Diversion instance with an open handle. If the
			/// operation fails, this function will throw, so nothing will be returned.
			/// </returns>
			static Diversion^ Open(System::String^ filter, DivertLayer layer, uint16_t priority, FilterFlags^ flags);

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~Diversion();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!Diversion();

			/// <summary>
			/// The handle used for hooking into the driver and receiving packets according to the
			/// filter supplied when this instance was constructed through the static Open(...)
			/// method. This handle is made available in a read-only state so that its validity can
			/// be checked. This handle is generated and managed internally.
			/// </summary>
			property DivertHandle^ Handle
			{
				DivertHandle^ get();	
			private:
				void set(DivertHandle^ value);
			}

			/// <summary>
			/// Receives a diverted packet that matched the filter passed to WinDivertOpen(). The
			/// received packet is guaranteed to match the filter. An application should call
			/// WinDivertRecv() as soon as possible after a successful call to WinDivertOpen(). More
			/// information here: https://reqrypt.org/windivert-doc.html#divert_recv
			/// </summary>
			/// <param name="packetBuffer">
			/// A valid array allocated with a length greater than zero. 
			/// </param>
			/// <param name="address">
			/// A Address instance. The Address instance will hold information about the origin and
			/// direction of the intercepted packet.
			/// </param>
			/// <param name="receiveLength">
			/// The amount of data read into the buffer. This is must be supplied as an ref
			/// parameter, will be set internally.
			/// </param>
			/// <returns>
			/// </returns>
			bool Receive(array<System::Byte>^ packetBuffer, Address^ address, uint32_t% receiveLength);

			/// <summary>
			/// Receives a diverted packet that matched the filter passed to WinDivertOpen().
			/// 
			/// This async function version will return immediately, always. If the return value is
			/// true, then a packet was intercepted immediately on calling, and the length parameter
			/// will be set correctly, and the data will be available in the supplied buffer.
			/// 
			/// However, if the function returns false, the buffer will not contain valid data and
			/// the supplied length parameter will not be set either. If a valid DivertAsyncResult
			/// object was supplied as a parameter, then the buffer with contain valid data and the
			/// proper read length will be found in the DivertAsyncResult.Length property after the
			/// DivertAsyncResult.Get(uint timeout) method has returned successfully.
			/// 
			/// If DivertAsyncResult.Get(uint timeout) returns false, then the async IO failed
			/// completely.
			/// 
			/// The received packet is guaranteed to match the filter. An application should call
			/// WinDivertRecvEx() as soon as possible after a successful call to WinDivertOpen().
			/// More information here: https://reqrypt.org/windivert-doc.html#divert_recv
			/// </summary>
			/// <param name="packetBuffer">
			/// A valid array allocated with a length greater than zero. 
			/// </param>
			/// <param name="address">
			/// A Address instance. The Address instance will hold information about the origin and
			/// direction of the intercepted packet.
			/// </param>
			/// <param name="receiveLength">
			/// The amount of data read into the buffer. This is must be supplied as a ref
			/// parameter, will be set internally.
			/// </param>
			/// <param name="asyncResult">
			/// An optional DivertAsyncResult instance. If a valid instance is supplied, and the
			/// function does not intercept a packet immediately (returning true), then the
			/// DivertAsyncResult will be configured so that, at a later time, the user may call
			/// DivertAsyncResult.Get(uint timeout) to attempt to fetch the results of the
			/// asynchronous I/O operation.
			/// 
			/// The DivertAsyncResult object will be configured even in the event of an error, and
			/// the user can use the provided members of DivertAsyncResult to determine if an error
			/// occurred, what the error code was, etc.
			/// </param>
			/// <returns>
			/// True if the function intercepted a packet immediately, false if not. A false return
			/// value may or may not indicate that an error occurred. If a valid DivertAsyncResult
			/// instance was supplied, it will contain information about any errors that did occur
			/// in the event of a false return value.
			/// </returns>
			bool ReceiveAsync(array<System::Byte>^ packetBuffer, Address^ address, uint32_t% receiveLength, [System::Runtime::InteropServices::Optional]DivertAsyncResult^ asyncResult);

			/// <summary>
			/// Close the handle. This can be called explicitly by the user or, if the user disposes
			/// of the Diversion instance, the open handle will be closed automatically as the
			/// object is cleaned up.
			/// 
			/// Note that once you call Close, this object will no longer be able to receive or send
			/// any packets, as communication with the driver is lost. The helper methods for
			/// parsing packets and such will still function, but nothing beyond that. Only call
			/// this method when finished with this object.
			/// </summary>
			/// <returns>
			/// True if successful, false if an error occurred. Use Marshal.GetLastWin32Error() to
			/// get the error code following a false return value.
			/// </returns>
			bool Close();

		private:

			Diversion();

			Diversion(DivertHandle^ handle);

			DivertHandle^ m_handle;

		};

	} /* namespace Net */
} /* namespace Divert */
