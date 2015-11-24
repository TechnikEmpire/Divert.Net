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

		/// <summary>
		/// Flag summaries copied from:
		/// https://reqrypt.org/windivert-doc.html#divert_helper_parse_packet
		/// </summary>
		public enum class FilterFlags : System::UInt64
		{
			/// <summary>
			/// By default WinDivert ensures that each diverted packet has a valid checksum. If the
			/// checksum is missing (e.g. with TCP checksum offloading), WinDivert will calculate it
			/// before passing the packet to the user application. This flag disables this behavior.
			/// </summary>
			[System::Obsolete(u8"As of WinDivert 1.2, the NoChecksum attribute is deprecated, because the default behavior of the library is now to no longer automatically calculate checksums.")]
			NoChecksum = 0,
			
			/// <summary>
			/// This flag opens the WinDivert handle in packet sniffing mode. In packet sniffing
			/// mode the original packet is not dropped-and-diverted (the default) but
			/// copied-and-diverted. This mode is useful for implementing packet sniffing tools
			/// similar to those applications that currently use Winpcap.
			/// </summary>
			Sniff = 1,

			/// <summary>
			/// This flag indicates that the user application does not intend to read matching
			/// packets with WinDivertRecv(), instead the packets should be silently dropped. This
			/// is useful for implementing simple packet filters using the WinDivert filter
			/// language.
			/// </summary>
			Drop = 2
		};

		/// <summary>
		/// Flag summaries copied from:
		/// https://reqrypt.org/windivert-doc.html#divert_helper_calc_checksums
		/// </summary>
		public enum class ChecksumCalculationFlags : System::UInt64
		{
			/// <summary>
			/// Do not calculate the IPv4 checksum.
			/// </summary>
			NoIpChecksum = 1,

			/// <summary>
			/// Do not calculate the ICMP checksum.
			/// </summary>
			NoIcmpChecksum = 2,

			/// <summary>
			/// Do not calculate the ICMPv6 checksum.
			/// </summary>
			NoIcmpV6Checksum = 4,

			/// <summary>
			///  Do not calculate the TCP checksum.
			/// </summary>
			NoTcpChecksum = 8,

			/// <summary>
			/// Do not calculate the UDP checksum.
			/// </summary>
			NoUdpChecksum = 16,

			/// <summary>
			/// Non-zero checksum fields should not be replaced.
			/// </summary>
			NoReplace = 2048
		};
	
		/// <summary>
		/// https://reqrypt.org/windivert-doc.html#divert_set_param
		/// </summary>
		public enum class DivertParam
		{
			/// <summary>
			/// Sets the maximum length of the packet queue for WinDivertRecv(). Currently the
			/// default value is 512, the minimum is 1, and the maximum is 8192.
			/// </summary>
			QueueLength = 0,

			/// <summary>
			/// Sets the minimum time, in milliseconds, a packet can be queued before it is
			/// automatically dropped. Packets cannot be queued indefinitely, and ideally, packets
			/// should be processed by the application as soon as is possible. Note that this sets
			/// the minimum time a packet can be queued before it can be dropped. The actual time
			/// may be exceed this value. Currently the default value is 512, the minimum is 128,
			/// and the maximum is 2048.
			/// </summary>
			QueueTime = 1
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
			static Diversion^ Open(System::String^ filter, DivertLayer layer, int16_t priority, FilterFlags flags);

			/// <summary>
			/// Checks the given filter string to ensure its format is correct and does not contain
			/// any invalid data.
			/// </summary>
			/// <param name="filter">
			/// The filter string to check. 
			/// </param>
			/// <param name="layer">
			/// Layer information. 
			/// </param>
			/// <param name="errorDetails">
			/// If the return value is false, an error string detailing the problem.
			/// </param>
			/// <returns>
			/// True if the supplied filter string is valid, false otherwise. 
			/// </returns>
			static bool ValidateFilter(System::String^ filter, DivertLayer layer, System::String^% errorDetails);

			/// <summary>
			/// Evaluates a filter against a given packet and its corresponding data to see if the
			/// filter applies.
			/// </summary>
			/// <param name="filter">
			/// The filter string to check. 
			/// </param>
			/// <param name="layer">
			/// The layer information for the supplied packet. 
			/// </param>
			/// <param name="packetBuffer">
			/// The packet data to check the filter against. 
			/// </param>
			/// <param name="packetLength">
			/// The length of valid packet data inside the supplied buffer. 
			/// </param>
			/// <param name="address">
			/// The address information for the supplied packet. 
			/// </param>
			/// <returns>
			/// True if the filter applies, false otherwise. 
			/// </returns>
			static bool EvaluateFilter(System::String^ filter, DivertLayer layer, array<System::Byte>^ packetBuffer, uint32_t packetLength, Address^ address);

			static void GetPacketProcess(Address^ address, TCPHeader^ tcpHeader, IPHeader^ ipv4Header, ULONG% processId, [System::Runtime::InteropServices::Out] System::String^% processName);

			static void GetPacketProcess(Address^ address, TCPHeader^ tcpHeader, IPv6Header^ ipv6Header, ULONG% processId, [System::Runtime::InteropServices::Out] System::String^% processName);

			static void GetPacketProcess(Address^ address, UDPHeader^ udpHeader, IPHeader^ ipv4Header, ULONG% processId, [System::Runtime::InteropServices::Out] System::String^% processName);

			static void GetPacketProcess(Address^ address, UDPHeader^ udpHeader, IPv6Header^ ipv6Header, ULONG% processId, [System::Runtime::InteropServices::Out] System::String^% processName);

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
			/// True if the operation succeeded and a packet was captured, false otherwise.
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
			/// More information here: https://reqrypt.org/windivert-doc.html#divert_recv_ex
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
			/// Injects a packet into the network stack. The injected packet may be one received
			/// from WinDivertRecv(), or a modified version, or a completely new packet. More
			/// information here: https://reqrypt.org/windivert-doc.html#divert_send
			/// </summary>
			/// <param name="packetBuffer">
			/// A valid array allocated with a length greater than zero. 
			/// </param>
			/// <param name="packetLength">
			/// The length of valid data in the buffer.
			/// </param>
			/// <param name="address">
			/// A Address instance. The Address instance will hold information about the origin and
			/// direction of the intercepted packet.
			/// </param>
			/// <param name="sendLength">
			/// The amount of data injected from the buffer. This is must be supplied as a ref
			/// parameter, will be set internally.
			/// </param>
			/// <returns>
			/// True if the supplied packet was successfully injected, false otherwise.
			/// </returns>
			bool Send(array<System::Byte>^ packetBuffer, uint32_t packetLength, Address^ address, uint32_t% sendLength);

			/// <summary>
			/// Injects a packet into the network stack. The injected packet may be one received
			/// from WinDivertRecv(), or a modified version, or a completely new packet.
			/// 
			/// This async function version will return immediately, always. If the return value is
			/// true, then a packet was injected immediately on calling, and the sendLength
			/// parameter will be set correctly.
			/// 
			/// However, if the function returns false, the packet was not injected immediately and
			/// the operation may be pending. In such an event, the supplied sendLength variable
			/// will not be updated, as the operation must be completed outside the scope of this
			/// function. If a valid DivertAsyncResult object was supplied as a parameter, then the
			/// user can call the DivertAsyncResult.Get(uint timeout) method to wait for the async
			/// operation to finished, then user the other members of DivertAsyncResult to determine
			/// if the operation was ultimately a success or not. If the operation was a success,
			/// then the sendLength will be available from the DivertAsyncResult.Length member.
			/// 
			/// If DivertAsyncResult.Get(uint timeout) returns false, then the async IO failed
			/// completely.
			/// 
			/// It is recommended that unless you truly care whether or not a packet injection
			/// failed, always use this method and do not supply a DivertAsyncResult object. Simply
			/// fire and forget. More information here:
			/// https://reqrypt.org/windivert-doc.html#divert_send_ex
			/// </summary>
			/// <param name="packetBuffer">
			/// A valid array allocated with a length greater than zero. 
			/// </param>
			/// <param name="packetLength">
			/// The length of valid data in the buffer.
			/// </param>
			/// <param name="address">
			/// A Address instance. The Address instance will hold information about the origin and
			/// direction of the intercepted packet.
			/// </param>
			/// <param name="sendLength">
			/// The amount of data injected from the buffer. This is must be supplied as a ref
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
			/// </returns>
			bool SendAsync(array<System::Byte>^ packetBuffer, uint32_t packetLength, Address^ address, uint32_t% sendLength, [System::Runtime::InteropServices::Optional]DivertAsyncResult^ asyncResult);

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

			/// <summary>
			/// Sets a WinDivert parameter. More information here:
			/// https://reqrypt.org/windivert-doc.html#divert_get_param
			/// </summary>
			/// <param name="divertParam">
			/// The DivertParam to set the value of. See comments on DivertParam enum members for
			/// specific parameter information.
			/// </param>
			/// <param name="value">
			/// The value to set for the supplied parameter. 
			/// </param>
			/// <returns>
			/// True if successful, false if an error occurred. Use Marshal.GetLastWin32Error() to
			/// get the reason for the error.
			/// </returns>
			bool SetParam(DivertParam divertParam, uint64_t value);

			/// <summary>
			/// Gets a WinDivert parameter value. More information here:
			/// https://reqrypt.org/windivert-doc.html#divert_set_param
			/// </summary>
			/// <param name="divertParam">
			/// The DivertParam to get the value of. See comments on DivertParam enum members for
			/// specific parameter information.
			/// </param>
			/// <param name="value">
			/// The value reference to populate with the actual value of the supplied parameter. 
			/// </param>
			/// <returns>
			/// True if successful, false if an error occurred. Use Marshal.GetLastWin32Error() to
			/// get the reason for the error.
			/// </returns>
			bool GetParam(DivertParam divertParam, uint64_t% value);

			/// <summary>
			/// Parses a raw packet (e.g. from WinDivertRecv()) into the various packet headers
			/// and/or payloads that may or may not be present. More information here:
			/// https://reqrypt.org/windivert-doc.html#divert_helper_parse_packet
			/// </summary>
			/// <param name="packetBuffer">
			/// A valid array populated with packet data from a previously successful call to one of
			/// the Receive methods.
			/// </param>
			/// <param name="ipHeader">
			/// The IPHeader object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="ipv6Header">
			/// The IPv6Header object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="icmpHeader">
			/// The ICMPHeader object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="icmpv6Header">
			/// The ICMPv6Header object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="tcpHeader">
			/// The TCPHeader object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="udpHeader">
			/// The UDPHeader object to populate. Optional, pass null if not desired.
			/// </param>
			/// <returns>
			/// True if all expected (non-NULL) outputs were present, false otherwise. Note that
			/// FALSE may sometimes be a legitimate return value, e.g., when both ppIpHdr and
			/// ppIpv6Hdr are non-NULL.
			/// </returns>
			bool ParsePacket(array<System::Byte>^ packetBuffer, uint32_t packetLength, IPHeader^ ipHeader, IPv6Header^ ipv6Header, ICMPHeader^ icmpHeader, ICMPv6Header^ icmpv6Header, TCPHeader^ tcpHeader, UDPHeader^ udpHeader);

			/// <summary>
			/// Parses a raw packet (e.g. from WinDivertRecv()) into the various packet headers
			/// and/or payloads that may or may not be present. More information here:
			/// https://reqrypt.org/windivert-doc.html#divert_helper_parse_packet
			/// 
			/// This overload provides the ability to fetch an array populated with just the data
			/// portion of the captured packet.
			/// </summary>
			/// <param name="packetBuffer">
			/// A valid array populated with packet data from a previously successful call to one of
			/// the Receive methods.
			/// </param>
			/// <param name="packetLength">
			/// The length of valid data in the buffer.
			/// </param>
			/// <param name="ipHeader">
			/// The IPHeader object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="ipv6Header">
			/// The IPv6Header object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="icmpHeader">
			/// The ICMPHeader object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="icmpv6Header">
			/// The ICMPv6Header object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="tcpHeader">
			/// The TCPHeader object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="udpHeader">
			/// The UDPHeader object to populate. Optional, pass null if not desired.
			/// </param>
			/// <param name="packetData">
			/// A reference to populate to the copied packet payload.
			/// </param>
			/// <returns>
			/// True if all expected (non-NULL) outputs were present, false otherwise. Note that
			/// FALSE may sometimes be a legitimate return value, e.g., when both ppIpHdr and
			/// ppIpv6Hdr are non-NULL.
			/// </returns>
			bool ParsePacket(array<System::Byte>^ packetBuffer, uint32_t packetLength, IPHeader^ ipHeader, IPv6Header^ ipv6Header, ICMPHeader^ icmpHeader, ICMPv6Header^ icmpv6Header, TCPHeader^ tcpHeader, UDPHeader^ udpHeader, [System::Runtime::InteropServices::Out]array<System::Byte>^% packetData);

			/// <summary>
			/// (Re)calculates the checksum for any IPv4/ICMP/ICMPv6/TCP/UDP checksum present in the
			/// given packet. Individual checksum calculations may be disabled via the appropriate
			/// flag. Typically this function should be invoked on a modified packet before it is
			/// injected with WinDivertSend(). More information here:
			/// https://reqrypt.org/windivert-doc.html#divert_helper_calc_checksums
			/// </summary>
			/// <param name="packetBuffer">
			/// An array containing the packet data to calculate the checksums for. 
			/// </param>
			/// <param name="packetLength">
			/// The length of valid data in the buffer.
			/// </param>
			/// <param name="flags">
			/// Checksum calculation flags. Adjust which headers have the checksums calculated for
			/// with these flags.
			/// </param>
			/// <returns>
			/// The number of checksums calculated.
			/// </returns>
			uint32_t CalculateChecksums(array<System::Byte>^ packetBuffer, uint32_t packetLength, ChecksumCalculationFlags flags);

		private:

			/// <summary>
			/// Default constructor is private because instances must be created through the
			/// provided static interface.
			/// </summary>
			Diversion();

			/// <summary>
			/// Allow construction from an existing handle.
			/// </summary>
			/// <param name="handle">
			/// A valid WinDivert handle.
			/// </param>
			Diversion(DivertHandle^ handle);

			/// <summary>
			/// A reference to the WinDivert handle that this diversion object was successfully
			/// created with.
			/// </summary>
			DivertHandle^ m_winDivertHandle;

			/// <summary>
			/// Utility for getting the full path to the binary including the binary name for the
			/// supplied process ID.
			/// </summary>
			/// <param name="processId">
			/// A valid process ID (Must not be equal to 0 or 4, as these values imply SYSTEM). 
			/// </param>
			/// <returns>
			/// If a valid PID was supplied, the full path to the binary including the binary name.
			/// If an invalid PID was supplied, an empty string.
			/// </returns>
			static std::string GetProcessName(const ULONG processId);

		};

	} /* namespace Net */
} /* namespace Divert */
